/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "firmware_manager.h"

#include <cstdlib>
#include <ohos_types.h>
#include <thread>

#include "config_parse.h"
#include "dupdate_errno.h"
#include "dupdate_upgrade_helper.h"
#include "dupdate_net_manager.h"
#include "file_utils.h"
#include "firmware_callback_utils.h"
#include "firmware_changelog_utils.h"
#include "firmware_common.h"
#include "firmware_constant.h"
#include "firmware_database.h"
#include "firmware_event_listener.h"
#include "firmware_flow_manager.h"
#include "firmware_iexecute_mode.h"
#include "firmware_log.h"
#include "firmware_manual_check_mode.h"
#include "firmware_download_mode.h"
#include "firmware_install_apply_mode.h"
#include "firmware_status_cache.h"
#include "firmware_task_operator.h"
#include "firmware_update_helper.h"
#ionclude "progress_thread.h"
#include "schedule_task.h"
#include "startup_schedule.h"
#include "string_utils.h"
#include "time_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
constexpr int32_t INIT_DELAY_TIME = 5; // 进程启动延时时间5秒，为了安装重启之后可以看到版本号及时刷新
constexpr int32_t PROCESS_EXIT_DELAY_TIME = 1; // 进程退出等待时间，单位：秒
constexpr uint64_t PULLUP_AFTER_TERMINATE_INTERVAL = 5; // 终止升级后SA拉起间隔

FirmwareManager::FirmwareManager() {}

FirmwareManager::~FirmwareManager()
{
    DelayedSingleton<FirmwareEventListener>::GetInstance()->UnregisterNetChangedListener();
}

void FirmwareManager::Init(StartupReason startupReason)
{
    DelayedSingleton<ConfigParse>::GetInstance()->LoadConfigInfo(); // 启动读取配置信息
    FIRMWARE_LOGI("FirmwareManager Init");

    // 当StartupReason为PROCESS_ENV_RESET时, 会进行以下处理：
    // 1. 删除Firmware目录（FIRMWARE_DIR_INFOS）中的所有允许删除的目录中的所有文件及子目录
    // 2. 清空Firmware SP（FirmwarePreferencesUtil）中所有记录
    // 3. 删除数据库（FirmwareDatabase/DownloadDatabase）
    if (startupReason == StartupReason::PROCESS_ENV_RESET) {
        FileUtils::DestroyBaseDirectory(FIRMWARE_DIR_INFOS);
        preferencesUtil_->RemoveAll();
        DelayedSingleton<FirmwareDatabase>::GetInstance()->DeleteDbStore();
    }
    FileUtils::InitAndCreateBaseDirs(FIRMWARE_DIR_INFOS);

    std::thread initThread(&FirmwareManager::DelayInit, this, startupReason);
    initThread.detach();
}

void FirmwareManager::DelayInit(StartupReason startupReason)
{
    FIRMWARE_LOGI("FirmwareManager DelayInit startupReason %{public}d", startupReason);
    RestoreUpdate();
    DelayedSingleton<FirmwareEventListener>::GetInstance()->RegisterNetChangedListener();

    auto eventType = CommonEventType::PROCESS_INIT;
    if (startupReason == StartupReason::DEVICE_REBOOT) {
        // 重启启动，延时5秒，等待系统初始化完再恢复周期提醒alarm | 执行升级结果判断  避免出现通知hap拉不起的问题
        sleep(INIT_DELAY_TIME);
        eventType = CommonEventType::BOOT_COMPLETE;
    }

    // 以下两种情况会向OUC发送初始启动消息：
    // 1. DUE启动原因为StartupReason::PROCESS_ENV_RESET，DUE缓存数据清空
    // 2. DUE首次启动，还未向OUC发送过初始启动消息
    NotifyInitEvent();

    // 处理 设备重启/进程初始化 事件
    HandleEvent(eventType);
}

void FirmwareManager::RestoreUpdate()
{
    FirmwareTask firmwareTask;
    FirmwareTaskOperator().QueryTask(firmwareTask);
    FIRMWARE_LOGI("RestoreUpdate status: %{public}d", firmwareTask.status);
    if (firmwareTask.status == UpgradeStatus::DOWNLOAD_PAUSE) {
        HandleBootDownloadPauseStatusProcess(firmwareTask);
        return;
    }
    if (firmwareTask.status == UpgradeStatus::DOWNLOADING) {
        HandleBootDownloadOnStatusProcess(firmwareTask);
        return;
    }
    if (firmwareTask.status == UpgradeStatus::DOWNLOAD_SUCCESS) {
        HandleBootDownloadedStatusProcess(firmwareTask);
        return;
    }

    if (firmwareTask.status == UpgradeStatus::INSTALLING || firmwareTask.status == UpgradeStatus::INSTALL_SUCCESS) {
        HandleBootInstallOnStatusProcess(firmwareTask);
        return;
    }
}

bool FirmwareManager::IsIdle()
{
    if (DelayedSingleton<FirmwareStatusCache>::GetInstance()->IsChecking()) {
        FIRMWARE_LOGI("FirmwareManager IsIdle checking");
        return false;
    }

    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    bool isIdle = !task.isExistTask;
    FIRMWARE_LOGI("FirmwareManager IsIdle:%{public}s", StringUtils::GetBoolStr(isIdle).c_str());
    return isIdle;
}

std::vector<ScheduleTask> FirmwareManager::GetScheduleTasks()
{
    ScheduleTask scheduleTask;
    return {scheduleTask};
}

bool FirmwareManager::Exit()
{
    FIRMWARE_LOGI("FirmwareManager Exit");
    return true;
}

void FirmwareManager::DoCancelDownload(BusinessError &businessError)
{
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGI("DoCancelDownload no task");
        businessError.Build(CallResult::FAIL, "no download task to cancel!");
        businessError.AddErrorMessage(CAST_INT(DUPDATE_ERR_DOWNLOAD_COMMON_ERROR), "no download task to cancel!");
        return;
    }
    ProgressThread::isCancel_ = true;
    return;
}

void FirmwareManager::DoTerminateUpgrade(BusinessError &businessError)
{
    ScheduleTask scheduleTask;
    scheduleTask.startupReason = StartupReason::PROCESS_ENV_RESET;
    scheduleTask.minDelayTime = PULLUP_AFTER_TERMINATE_INTERVAL;
    if (!DelayedSingleton<StartupSchedule>::GetInstance()->Schedule(scheduleTask)) {
        FIRMWARE_LOGE("DoTerminateUpgrade fail");
        businessError.AddErrorMessage(CAST_INT(DUPDATE_ERR_INTERNAL_ERROR), "terminate upgrade error!");
        return;
    }

    // 主线程拉起子线程之后向OUC返回回调结果，子线程sleep 1秒之后，DUE进程退出
    std::thread th = std::thread([this]() {
        sleep(PROCESS_EXIT_DELAY_TIME);
        _Exit(0);
    });
    th.detach();
}

void FirmwareManager::DoCheck(OnCheckComplete onCheckComplete)
{
    auto *flowManager = new FirmwareFlowManager();
    std::shared_ptr<FirmwareIExecuteMode> executeMode =
        std::make_shared<FirmwareManualCheckMode>([=](BusinessError &businessError, CheckResult &checkResult) {
            FIRMWARE_LOGI("FirmwareManager DoCheck newVersionInfoResult");
            onCheckComplete(businessError, checkResult);
            delete flowManager;
        });
    flowManager->SetExecuteMode(executeMode);
    flowManager->Start();
}

void FirmwareManager::DoDownload(const DownloadOptions &downloadOptions, BusinessError &businessError)
{
    auto *flowManager = new FirmwareFlowManager();
    std::shared_ptr<FirmwareIExecuteMode> executeMode =
        std::make_shared<FirmwareDownloadMode>(downloadOptions, businessError, [=]() {
            FIRMWARE_LOGI("FirmwareManager DoDownload finish");
            delete flowManager;
        });
    flowManager->SetExecuteMode(executeMode);
    flowManager->Start();
}

bool FirmwareManager::CanInstall(void)
{
    FirmwareTask task;
    FirmwareTaskOperator firmwareTaskOperator;
    firmwareTaskOperator.QueryTask(task);
    FIRMWARE_LOGI("check can install task.status:%{public}d", CAST_INT(task.status));
    if (FirmwareUpdateHelper::GetInstallType() == InstallType::SYS_INSTALLER) {
        return task.status == UpgradeStatus::DOWNLOAD_SUCCESS || task.status == UpgradeStatus::INSTALL_SUCCESS;
    } else {
        return task.status == UpgradeStatus::DOWNLOAD_SUCCESS;
    }
}

void FirmwareManager::DoInstall(const UpgradeOptions &upgradeOptions, BusinessError &businessError,
    InstallType installType)
{
    FIRMWARE_LOGI("FirmwareManager DoInstall upgradeOptions order:%{public}d, installType = %{public}d",
        CAST_INT(upgradeOptions.order), CAST_INT(installType));
    if (!CanInstall()) {
        FIRMWARE_LOGI("status can not Install");
        return;
    }
    FirmwareFlowManager *flowManager = new FirmwareFlowManager();
    std::shared_ptr<FirmwareIExecuteMode> executeMode =
        std::make_shared<FirmwareInstallApplyMode>(upgradeOptions, businessError, installType, [=]() {
            FIRMWARE_LOGI("FirmwareManager DoInstall finish");
        });
    flowManager->SetExecuteMode(executeMode);
    flowManager->Start();
}

void FirmwareManager::DoAutoDownloadSwitchChanged(bool isDownloadSwitchOn)
{
    FIRMWARE_LOGI("DoAutoDownloadSwitchChanged isDownloadSwitchOn %{public}s", isDownloadSwitchOn ? "on" : "off");
    preferencesUtil_->SaveBool(Firmware::AUTO_DOWNLOAD_SWITCH, isDownloadSwitchOn);
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGI("DoAutoDownloadSwitchChanged no task");
        return;
    }
    FIRMWARE_LOGI("DoAutoDownloadSwitchChanged current task status %{public}d", task.status);
    // 当前是否有搜索完待下载的包
    if (task.status != UpgradeStatus::CHECK_VERSION_SUCCESS) {
        return;
    }
    if (!isDownloadSwitchOn) {
        // 开关关，发送新版本通知
        DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
            task.taskId, EventId::EVENT_DOWNLOAD_WAIT, UpgradeStatus::CHECK_VERSION_SUCCESS);
    }
}

void FirmwareManager::DoClearError(BusinessError &businessError)
{
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGI("DoClearError has no task");
        return;
    }
    if (!UpdateHelper::IsUpgradeFailed(task.status)) {
        businessError.Build(CallResult::FAIL, "please check status before clear error");
        businessError.AddErrorMessage(CAST_INT(DUPDATE_ERR_UPDATE_COMMON_ERROR),
            "please check status before clear error");
        return;
    }
    FirmwareUpdateHelper::ClearFirmwareInfo();
}

void FirmwareManager::HandleEvent(CommonEventType event)
{
    FIRMWARE_LOGI("handleEvent event %{public}d", static_cast<uint32_t>(event));
    switch (event) {
        case CommonEventType::NET_CHANGED:
            HandleNetChanged();
            break;
        case CommonEventType::BOOT_COMPLETE:
            HandleBootComplete();
            break;
        default:
            break;
    }
}

void FirmwareManager::HandleBootComplete()
{
    FIRMWARE_LOGI("HandleBootComplete");
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGI("HandleBootComplete has no task");
        return;
    }

    FIRMWARE_LOGI("HandleBootComplete status %{public}d", CAST_INT(task.status));
    if (task.status == UpgradeStatus::UPDATING) {
        HandleBootUpdateOnStatusProcess(task);
        return;
    }
}

void FirmwareManager::HandleNetChanged()
{
    FIRMWARE_LOGI("HandleNetChanged");
    if (!DelayedSingleton<NetManager>::GetInstance()->IsNetAvailable()) {
        FIRMWARE_LOGE("HandleNetChanged network not available.");
        ProgressThread::isNoNet_ = true;
        return;
    }

    ProgressThread::isNoNet_ = false;
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    FIRMWARE_LOGI("HandleNetChanged status %{public}d", task.status);

    if (!DelayedSingleton<NetManager>::GetInstance()->IsNetAvailable(NetType::NOT_METERED_WIFI)) {
        return;
    }

    if (task.status == UpgradeStatus::DOWNLOAD_PAUSE) {
        HandleResumeDownload(task);
        return;
    }

    if (task.status == UpgradeStatus::CHECK_VERSION_SUCCESS) {
        bool isDownloadSwitchOn = preferencesUtil_->ObtainBool(Firmware::AUTO_DOWNLOAD_SWITCH, false);
        FIRMWARE_LOGI("HandleNetChanged isDownloadSwitchOn %{public}s",
            StringUtils::GetBoolStr(isDownloadSwitchOn).c_str());
        if (isDownloadSwitchOn) {
            DoAutoDownload(task);
        }
    }
}

// updater调用后正常启动
void FirmwareManager::HandleBootUpdateOnStatusProcess(const FirmwareTask &task)
{
    FIRMWARE_LOGI("HandleBootUpdateOnStatusProcess");
    FirmwareResultProcess resultProcess;
    std::map<std::string, UpdateResult> resultMap;
    std::vector<FirmwareComponent> components;
    FirmwareComponentOperator().QueryAll(components);
    switch (resultProcess.GetUpdaterResult(components, resultMap)) {
        case UpdateResultCode::SUCCESS:
            HandleBootUpdateSuccess(task, resultMap);
            break;
        case UpdateResultCode::FAILURE:
        case UpdateResultCode::FILE_ERROR:
            HandleBootUpdateFail(task, resultMap);
            break;
        default:
            HandleAbInstallBootUpdateOnStatusProcess(task, resultMap);
            break;
    }
}

void FirmwareManager::HandleAbInstallBootUpdateOnStatusProcess(const FirmwareTask &task, const std::map<std::string,
    UpdateResult> &resultMap)
{
    if (task.status != UpgradeStatus::UPDATING) {
        return;
    }
    HandleBootUpdateSuccess(task, resultMap);
}

void FirmwareManager::HandleBootUpdateSuccess(const FirmwareTask &task,
    const std::map<std::string, UpdateResult> &resultMap)
{
    preferencesUtil_->SaveString(Firmware::UPDATE_ACTION, "upgrade");
    std::vector<FirmwareComponent> components;
    FirmwareComponentOperator().QueryAll(components);
    std::vector<VersionComponent> versionComponents;
    for (const auto &component : components) {
        VersionComponent versionComponent;
        versionComponent.componentType = CAST_INT(ComponentType::OTA);
        versionComponent.componentId = component.componentId;
        versionComponent.upgradeAction = UpgradeAction::UPGRADE;
        versionComponent.displayVersion = component.targetBlDisplayVersionNumber;
        versionComponent.innerVersion = component.targetBlVersionNumber;
        versionComponents.push_back(versionComponent);
    }

    DelayedSingleton<FirmwareChangelogUtils>::GetInstance()->SaveHotaCurrentVersionComponentId();
    if (task.combinationType == CombinationType::HOTA) {
        FIRMWARE_LOGI("notify upgrade success");
        DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(task.taskId, EventId::EVENT_UPGRADE_SUCCESS,
            UpgradeStatus::UPDATE_SUCCESS, ErrorMessage{}, versionComponents);
        FirmwareUpdateHelper::ClearFirmwareInfo();
        return;
    }
}

void FirmwareManager::HandleBootUpdateFail(const FirmwareTask &task,
    const std::map<std::string, UpdateResult> &resultMap)
{
    preferencesUtil_->SaveString(Firmware::UPDATE_ACTION, "recovery");
    std::vector<FirmwareComponent> components;
    FirmwareComponentOperator().QueryAll(components);
    std::vector<VersionComponent> versionComponents;
    for (const auto &component : components) {
        VersionComponent versionComponent;
        versionComponent.componentType = CAST_INT(ComponentType::OTA);
        versionComponent.componentId = component.componentId;
        versionComponent.upgradeAction = UpgradeAction::UPGRADE;
        versionComponent.displayVersion = component.targetBlDisplayVersionNumber;
        versionComponent.innerVersion = component.targetBlVersionNumber;
        versionComponents.push_back(versionComponent);
    }

    ErrorMessage errorMessage;
    for (const auto &result : resultMap) {
        if (result.second.result == UPDATER_RESULT_FAILURE) {
            errorMessage.errorCode = CAST_INT(result.second.GetUpdateResultCode());
            errorMessage.errorMessage = result.second.reason;
            break;
        }
    }

    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(task.taskId, EventId::EVENT_UPGRADE_FAIL,
        UpgradeStatus::UPDATE_FAIL, errorMessage, versionComponents);
    FIRMWARE_LOGI("upgrade fail");
    FirmwareUpdateHelper::ClearFirmwareInfo();
}

// sysInstaller安装中重启，进度回退为下载完成
void FirmwareManager::HandleBootInstallOnStatusProcess(FirmwareTask &task)
{
    FIRMWARE_LOGI("HandleBootInstallOnStatusProcess");
    FirmwareTaskOperator().UpdateProgressByTaskId(
        task.taskId, UpgradeStatus::DOWNLOAD_SUCCESS, Firmware::ONE_HUNDRED);
    std::vector<FirmwareComponent> firmwareComponentList;
    FirmwareComponentOperator firmwareComponentOperator;
    firmwareComponentOperator.QueryAll(firmwareComponentList);
    for (const FirmwareComponent &component : firmwareComponentList) {
        if (component.status == UpgradeStatus::INSTALLING) {
            firmwareComponentOperator.UpdateProgressByUrl(
                component.url, UpgradeStatus::DOWNLOAD_SUCCESS, Firmware::ONE_HUNDRED);
        }
    }
    BusinessError businessError;
    UpgradeOptions upgradeOptions;
    upgradeOptions.order = Order::INSTALL_AND_APPLY;
    DoInstall(upgradeOptions, businessError, InstallType::SYS_INSTALLER);
}

void FirmwareManager::HandleBootDownloadOnStatusProcess(FirmwareTask &task)
{
    // 下载中重启，清除记录和数据
    FIRMWARE_LOGI("HandleBootDownloadOnStatusProcess ClearFirmwareInfo");
    FirmwareUpdateHelper::ClearFirmwareInfo();
    return;
}

void FirmwareManager::HandleBootDownloadPauseStatusProcess(FirmwareTask &task)
{
    FirmwareUpdateHelper::ClearFirmwareInfo();
    return;
}

void FirmwareManager::HandleResumeDownload(FirmwareTask &task)
{
    FIRMWARE_LOGI("HandleResumeDownload");
    return;
}

void FirmwareManager::HandleBootDownloadedStatusProcess(FirmwareTask &task)
{
    FIRMWARE_LOGI("HandleBootDownloadedStatusProcess");
    return;
}

void FirmwareManager::DoAutoDownload(const FirmwareTask &task)
{
    FIRMWARE_LOGI("DoAutoDownload");
    return;
}

void FirmwareManager::NotifyInitEvent()
{
    bool dueInitFlag = preferencesUtil_->ObtainBool(Constant::DUE_INIT_FLAG, false);
    FIRMWARE_LOGI("DUE_INIT_FLAG is %{public}s", StringUtils::GetBoolStr(dueInitFlag).c_str());
    if (!dueInitFlag) {
        preferencesUtil_->SaveBool(Constant::DUE_INIT_FLAG, true);
        DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent("", EventId::EVENT_INITIALIZE,
            UpgradeStatus::INIT);
    }
}
} // namespace UpdateEngine
} // namespace OHOS
