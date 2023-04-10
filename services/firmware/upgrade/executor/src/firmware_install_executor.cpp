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

#include "firmware_install_executor.h"

#include <thread>

#include "firmware_callback_utils.h"
#include "firmware_component_operator.h"
#include "firmware_constant.h"
#include "firmware_install_factory.h"
#include "firmware_log.h"
#include "firmware_task_operator.h"
#include "firmware_update_helper.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
void FirmwareInstallExecutor::Execute()
{
    FIRMWARE_LOGI("FirmwareInstallExecutor::Execute");
    std::thread installThread(&FirmwareInstallExecutor::DoInstall, this);
    installThread.detach();
}

void FirmwareInstallExecutor::DoInstall()
{
    FirmwareComponentOperator().QueryAll(components_);
    FIRMWARE_LOGI("FirmwareInstallExecutor DoInstall installType: %{public}d, component num: %{public}d",
        CAST_INT(installType_), CAST_INT(components_.size()));
    if (components_.size() == 0) {
        Progress progress;
        progress.status = UpgradeStatus::UPDATE_FAIL;
        progress.endReason = "no task";
        installCallbackInfo_.progress = progress;
        installCallback_.installCallback(installCallbackInfo_);
        return;
    }

    GetTask();
    Progress progress;
    if (installType_ == InstallType::SYS_INSTALLER) {
        progress.status = UpgradeStatus::INSTALLING;
    } else if (installType_ == InstallType::UPDATER) {
        progress.status = UpgradeStatus::UPDATING;
    } else {
        FIRMWARE_LOGI("installType:%{public}d is illegal", CAST_INT(installType_));
    }

    FirmwareTaskOperator().UpdateProgressByTaskId(tasks_.taskId, progress.status, progress.percent);
    for (FirmwareComponent &component : components_) {
        FirmwareComponentOperator().UpdateProgressByUrl(component.url, progress.status, progress.percent);
    }

    FirmwareInstallCallback cb {[=](const FirmwareComponent &component) {
                                    Progress progress;
                                    progress.status = component.status;
                                    progress.percent = component.progress;
                                    HandleInstallProgress(component, progress);
                                },
        [=](const bool result, const ErrMsg &errMsg) {
            HandleInstallResult(result, errMsg);
        },
        [=](const UpgradeStatus &status) {
            FIRMWARE_LOGI("update start status :%{public}d", CAST_INT(status));
            DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(tasks_.taskId,
                EventId::EVENT_UPGRADE_START, status);
        }};

    std::shared_ptr<FirmwareInstall> executor = InstallFactory::GetInstance(installType_);
    if (executor == nullptr) {
        FIRMWARE_LOGE("get install pointer fail");
        return;
    }
    executor->StartInstall(components_, cb);
}

void FirmwareInstallExecutor::GetTask()
{
    if (!tasks_.isExistTask) {
        FirmwareTaskOperator().QueryTask(tasks_);
    }
}

void FirmwareInstallExecutor::HandleInstallProgress(const FirmwareComponent &component, const Progress &progress)
{
    FIRMWARE_LOGI("UpdateCallback versionId %{public}s status %{public}d progress %{public}d",
        component.versionId.c_str(), progress.status, progress.percent);
    FirmwareComponentOperator().UpdateProgressByUrl(component.url, progress.status, progress.percent);

    taskProgress_.status = progress.status;
    taskProgress_.percent = progress.percent;

    // 整体进度插入到 task 表
    FirmwareTaskOperator().UpdateProgressByTaskId(tasks_.taskId, taskProgress_.status, taskProgress_.percent);
    installCallbackInfo_.progress = taskProgress_;
    installCallback_.installCallback(installCallbackInfo_);
}

void FirmwareInstallExecutor::HandleInstallResult(const bool result, const ErrMsg &errMsg)
{
    FIRMWARE_LOGI("FirmwareInstallExecutor::HandleInstallResult, result =%{public}d", result);
    if (result) {
        taskProgress_.status = UpgradeStatus::INSTALL_SUCCESS;
        taskProgress_.percent = Firmware::ONE_HUNDRED;
    } else {
        if (installType_ == InstallType::SYS_INSTALLER) {
            taskProgress_.status = UpgradeStatus::INSTALL_FAIL;
        } else if (installType_ == InstallType::UPDATER) {
            taskProgress_.status = UpgradeStatus::UPDATE_FAIL;
        }
    }
    // 整体进度插入到 task 表
    FirmwareTaskOperator().UpdateProgressByTaskId(tasks_.taskId, taskProgress_.status, taskProgress_.percent);
    installCallbackInfo_.progress = taskProgress_;
    installCallbackInfo_.errorMessage.errorCode = errMsg.errorCode;
    installCallbackInfo_.errorMessage.errorMessage = errMsg.errorMsg;
    installCallback_.installCallback(installCallbackInfo_);
}
} // namespace UpdateEngine
} // namespace OHOS
