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

#include "firmware_install_data_processor.h"

#include "firmware_callback_utils.h"
#include "firmware_component_operator.h"
#include "firmware_constant.h"
#include "firmware_file_utils.h"
#include "firmware_iexecutor.h"
#include "firmware_log.h"
#include "firmware_task_operator.h"
#include "firmware_update_helper.h"
#include "string_utils.h"

namespace OHOS {
namespace UpdateEngine {
constexpr int64_t SPACE_OFFSET = 100 *1024 *1024;

bool FirmwareInstallDataProcessor::IsSpaceEnough(int64_t &requireTotalSize)
{
    std::vector<FirmwareComponent> components;
    FirmwareComponentOperator().QueryAll(components);
    for (const auto &component : components) {
        requireTotalSize = requireTotalSize + component.size;
    }
    bool isSpaceEnough = FirmwareFileUtils::IsSpaceEnough(requireTotalSize * Firmware::COMPRESSION_RATIO
        + SPACE_OFFSET);
    FIRMWARE_LOGI("IsSpaceEnough %{public}s", StringUtils::GetBoolStr(isSpaceEnough).c_str());
    return isSpaceEnough;
}

bool FirmwareInstallDataProcessor::HasInstallSuccess()
{
    // 实时取当前状态
    FirmwareTaskOperator().QueryTask(tasks_);
    FIRMWARE_LOGI("GetTaskStatus status: %{public}d", tasks_.status);
    return tasks_.status == UpgradeStatus::INSTALL_SUCCESS;
}

bool FirmwareInstallDataProcessor::HasUpdateSuccess()
{
    // 实时取当前状态
    FirmwareTaskOperator().QueryTask(tasks_);
    FIRMWARE_LOGI("GetTaskStatus status: %{public}d", tasks_.status);
    return tasks_.status == UpgradeStatus::UPDATE_SUCCESS;
}

void FirmwareInstallDataProcessor::GetTask()
{
    if (!tasks_.isExistTask) {
        FirmwareTaskOperator().QueryTask(tasks_);
        FIRMWARE_LOGI("GetTask has task %{public}s", StringUtils::GetBoolStr(tasks_.isExistTask).c_str());
    }
}

void FirmwareInstallDataProcessor::SetInstallResult(const InstallCallbackInfo &installCallbackInfo)
{
    Progress progress = installCallbackInfo.progress;
    FIRMWARE_LOGI("SetInstallResult status: %{public}d progress: %{public}d", progress.status, progress.percent);
    GetTask();
    if (progress.status == UpgradeStatus::UPDATE_FAIL || progress.status == UpgradeStatus::INSTALL_FAIL) {
        FIRMWARE_LOGI("SetInstallResult errorCode: %{public}d errorMessage: %{public}s",
            installCallbackInfo.errorMessage.errorCode, installCallbackInfo.errorMessage.errorMessage.c_str());
        DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(tasks_.taskId, EventId::EVENT_UPGRADE_FAIL,
            progress.status, installCallbackInfo.errorMessage);
        return;
    }

    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->ProgressCallback(tasks_.taskId, progress);
}

bool FirmwareInstallDataProcessor::IsUpgradeFileCheckSuccess()
{
    std::vector<FirmwareComponent> components;
    FirmwareComponentOperator().QueryAll(components);
    return FirmwareUpdateHelper::IsUpgradePackagesReady(components);
}
} // namespace UpdateEngine
} // namespace OHOS
