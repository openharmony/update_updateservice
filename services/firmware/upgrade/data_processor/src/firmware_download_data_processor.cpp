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

#include "firmware_download_data_processor.h"

#include "constant.h"
#include "firmware_callback_utils.h"
#include "firmware_component_operator.h"
#include "firmware_constant.h"
#include "firmware_file_utils.h"
#include "firmware_log.h"
#include "firmware_status_cache.h"
#include "firmware_task_operator.h"
#include "string_utils.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareDownloadDataProcessor::FirmwareDownloadDataProcessor()
{
    FIRMWARE_LOGD("FirmwareDownloadDataProcessor::FirmwareDownloadDataProcessor");
}

FirmwareDownloadDataProcessor::~FirmwareDownloadDataProcessor()
{
    FIRMWARE_LOGD("FirmwareDownloadDataProcessor::~FirmwareDownloadDataProcessor");
}

bool FirmwareDownloadDataProcessor::IsDownloadSuccess()
{
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    if (!task.isExistTask) {
        FIRMWARE_LOGI("IsDownloadSuccess no task");
        return false;
    }
    FIRMWARE_LOGI("IsDownloadSuccess task status %{public}d", task.status);
    return task.status == UpgradeStatus::DOWNLOAD_SUCCESS;
}

bool FirmwareDownloadDataProcessor::IsSpaceEnough(int64_t &requireTotalSize)
{
    std::vector<FirmwareComponent> components;
    FirmwareComponentOperator().QueryAll(components);
    for (FirmwareComponent &component : components) {
        requireTotalSize += (Firmware::ONE_HUNDRED - component.progress) * component.size / Firmware::ONE_HUNDRED;
    }
    bool isDownloadSpaceEnough = FirmwareFileUtils::IsSpaceEnough(requireTotalSize);
    FIRMWARE_LOGI("IsSpaceEnough %{public}s", StringUtils::GetBoolStr(isDownloadSpaceEnough).c_str());
    return isDownloadSpaceEnough;
}

void FirmwareDownloadDataProcessor::GetTask()
{
    if (!tasks_.isExistTask) {
        FirmwareTaskOperator().QueryTask(tasks_);
    }
}

void FirmwareDownloadDataProcessor::SetDownloadProgress(const Progress &progress)
{
    FIRMWARE_LOGI("SetDownloadProgress status: %{public}d progress: %{public}d", progress.status, progress.percent);
    downloadProgress_ = progress;
    GetTask();
    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->ProgressCallback(tasks_.taskId, progress);
}

Progress FirmwareDownloadDataProcessor::GetDownloadProgress()
{
    return downloadProgress_;
}
} // namespace UpdateEngine
} // namespace OHOS
