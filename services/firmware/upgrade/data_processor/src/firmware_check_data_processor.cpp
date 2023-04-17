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

#include "firmware_check_data_processor.h"

#include "config_parse.h"
#include "constant.h"
#include "dupdate_inet_observer.h"
#ifdef "NETMANAGER_BASE_ENABLE"
#include "dupdate_net_observer.h"
#endif
#include "firmware_changelog_utils.h"
#include "firmware_combine_version_utils.h"
#include "firmware_common.h"
#include "firmware_component_operator.h"
#include "firmware_constant.h"
#include "firmware_callback_utils.h"
#include "firmware_log.h"
#include "firmware_task_operator.h"
#include "device_adapter.h"
#include "firmware_update_helper.h"
#include "string_utils.h"
#include "time_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareCheckDataProcessor::FirmwareCheckDataProcessor()
{
    FIRMWARE_LOGD("FirmwareCheckDataProcessor::FirmwareCheckDataProcessor");
}

FirmwareCheckDataProcessor::~FirmwareCheckDataProcessor()
{
    FIRMWARE_LOGD("FirmwareCheckDataProcessor::~FirmwareCheckDataProcessor");
}

void FirmwareCheckDataProcessor::SetCheckResult(
    CheckStatus status, const Duration &duration, const std::vector<FirmwareComponent> &componentList)
{
    FIRMWARE_LOGI("FirmwareCheckDataProcessor::SetCheckResult status %{public}d duration %{public}d",
        static_cast<uint32_t>(status), duration.duration);
    status_ = status;
    duration_ = duration;
    componentList_ = componentList;
    HandleCheckResult();
}

bool FirmwareCheckDataProcessor::IsCheckFailed()
{
    return status_ == CheckStatus::CHECK_FAIL ||
           status_ == CheckStatus::FILE_LIST_REQUEST_FAIL ||
           status_ == CheckStatus::CHANGELOG_REQUEST_FAIL;
}

bool FirmwareCheckDataProcessor::HasNewVersion()
{
    return !componentList_.empty();
}

bool FirmwareCheckDataProcessor::IsSameWithDb()
{
    return isSameWithDb_;
}

void FirmwareCheckDataProcessor::HandleCheckResult()
{
    FIRMWARE_LOGI("FirmwareCheckDataProcessor HandleCheckResult status %{public}d size: %{public}d",
        CAST_INT(status_), CAST_INT(componentList_.size()));
    if (IsCheckFailed()) {
        // 搜包失败或者获取fileList失败
        FIRMWARE_LOGI("FirmwareCheckDataProcessor HandleCheckResult fail");
        return;
    }

    RefreshPollingCycle();
    if (HasUndoneTask()) {
        // 下载中、安装中，不处理
        FIRMWARE_LOGI("FirmwareCheckDataProcessor HasUndoneTask");
        HandleUndoneEvent();
        return;
    }

    if (HasNewVersion()) {
        HandleNewVersion();
    } else {
        HandleNoNewVersion();
    }
}

bool FirmwareCheckDataProcessor::HasUndoneTask()
{
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGI("HasUndoneTask no task");
        return false;
    }

    FIRMWARE_LOGI("HasUndoneTask status %{public}d", task.status);
    UpgradeStatus taskStatus = task.status;
    return taskStatus == UpgradeStatus::DOWNLOADING ||
           taskStatus == UpgradeStatus::DOWNLOAD_PAUSE ||
           taskStatus == UpgradeStatus::VERIFYING ||
           taskStatus == UpgradeStatus::INSTALLING ||
           taskStatus == UpgradeStatus::UPDATING;
}

bool FirmwareCheckDataProcessor::IsUpdateOnStatus()
{
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGI("IsUpdateOnStatus no task");
        return false;
    }

    FIRMWARE_LOGI("IsUpdateOnStatus status %{public}d", task.status);
    return task.status == UpgradeStatus::UPDATING;
}

void FirmwareCheckDataProcessor::HandleNewVersion()
{
    FIRMWARE_LOGI("FirmwareCheckDataProcessor::HandleNewVersion");
    isSameWithDb_ = IsVersionSameWithDb();
    if (isSameWithDb_) {
        UpdateFirmwareComponent();
        FIRMWARE_LOGI("FirmwareCheckDataProcessor::HandleNewVersion version is same with db");
        return;
    }

    // 删除数据库和本地下载文件
    FirmwareUpdateHelper::ClearFirmwareInfo();

    BuildComponentSPath();
    FirmwareComponentOperator().Insert(componentList_);
    FirmwareTask task;
    task.taskId = FirmwareUpdateHelper::BuildTaskId(componentList_);
    task.status = UpgradeStatus::CHECK_VERSION_SUCCESS;
    task.combinationType = GetCombinationType();
    FirmwareTaskOperator().Insert(task);
}

void FirmwareCheckDataProcessor::BuildComponentSPath()
{
    for (auto &component : componentList_) {
        component.spath = Firmware::PACKAGE_PATH + "/" + component.fileName;
    }
}

bool FirmwareCheckDataProcessor::IsVersionSameWithDb()
{
    // 查询当前数据库保存数据
    std::vector<FirmwareComponent> dbComponentList;
    FirmwareComponentOperator().QueryAll(dbComponentList);

    FIRMWARE_LOGI("FirmwareCheckDataProcessor::IsVersionSameWithDb db size %{public}d server size %{public}d",
        CAST_INT(dbComponentList.size()), CAST_INT(componentList_.size()));
    if (dbComponentList.size() != componentList_.size()) {
        return false;
    }

    for (const auto &serverResult : componentList_) {
        std::string versionId = serverResult.versionId;
        FirmwareComponent dbResult;
        bool hasResult = FirmwareComponentOperator().QueryByVersionId(versionId, dbResult);
        if (!hasResult || dbResult.size != serverResult.size || dbResult.verifyInfo != serverResult.verifyInfo) {
            FIRMWARE_LOGI("FirmwareCheckDataProcessor::IsVersionSameWithDb versionId %{public}s not in db or data "
                "is updated size %{public}s verifyInfo %{public}s",
                versionId.c_str(), std::to_string(dbResult.size).c_str(), dbResult.verifyInfo.c_str());
            return false;
        }
    }
    FIRMWARE_LOGI("FirmwareCheckDataProcessor::IsVersionSameWithDb");
    return true;
}

void FirmwareCheckDataProcessor::UpdateFirmwareComponent()
{
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGE("UpdateFirmwareComponent no task");
        return;
    }
    if (task.status != UpgradeStatus::CHECK_VERSION_SUCCESS) {
        FIRMWARE_LOGI("UpdateFirmwareComponent status not check version success");
        return;
    }
    for (const auto &serverResult : componentList_) {
        FirmwareComponentOperator().UpdateUrlByVersionId(serverResult.versionId, serverResult.url);
    }
}

void FirmwareCheckDataProcessor::HandleNoNewVersion()
{
    FIRMWARE_LOGI("FirmwareCheckDataProcessor::HandleNoNewVersion");
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (task.isExistTask) {
        // 搜包从有到无，推送task_cancel
        DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
            task.taskId, EventId::EVENT_TASK_CANCEL, UpgradeStatus::INIT);
    }
    FirmwareUpdateHelper::ClearFirmwareInfo();
}

CombinationType FirmwareCheckDataProcessor::GetCombinationType()
{
    uint32_t combinationType = CAST_UINT(CombinationType::INVALLID_TYPE);
    for (const auto &component : componentList_) {
        if (component.blVersionType == CAST_INT(BlType::HOTA_TYPE)) {
            combinationType |= CAST_UINT(CombinationType::HOTA);
        }
    }

    FIRMWARE_LOGI("CombinationType::%{public}u", combinationType);
    return static_cast<CombinationType>(combinationType);
}

void FirmwareCheckDataProcessor::RefreshPollingCycle()
{
    FIRMWARE_LOGI("FirmwareCheckDataProcessor::RefreshPollingCycle");
}

void FirmwareCheckDataProcessor::BuildCheckResult(CheckResult &checkResult)
{
    if (IsCheckFailed()) {
        FIRMWARE_LOGE("BuildCheckResult check error");
        return;
    }

    // 搜包结果返回数据库结果，避免下载中，搜到新包，返回新包结果，此时数据库中保存的是老包
    std::vector<FirmwareComponent> dbComponentList;
    FirmwareComponentOperator().QueryAll(dbComponentList);
    checkResult.isExistNewVersion = (dbComponentList.size() != 0);
    FIRMWARE_LOGI("BuildCheckResult status %{public}s", StringUtils::GetBoolStr(checkResult.isExistNewVersion).c_str());
    if (checkResult.isExistNewVersion) {
        BuildVersionDigest(checkResult.newVersionInfo, dbComponentList);
        FirmwareUpdateHelper::BuildNewVersionInfo(dbComponentList, checkResult.newVersionInfo.versionComponents);
    }
}

void FirmwareCheckDataProcessor::BuildVersionDigest(
    NewVersionInfo &newVersionInfo, const std::vector<FirmwareComponent> &componentList)
{
    newVersionInfo.versionDigestInfo.versionDigest = FirmwareUpdateHelper::BuildTaskId(componentList);
}
} // namespace UpdateEngine
} // namespace OHOS