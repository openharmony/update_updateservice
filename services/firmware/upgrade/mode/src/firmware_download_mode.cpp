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

#include "firmware_download_mode.h"

#include "dupdate_errno.h"
#include "dupdate_inet_observer.h"
#include "dupdate_net_manager.h"
#include "firmware_callback_utils.h"
#include "firmware_common.h"
#include "firmware_constant.h"
#include "firmware_log.h"
#include "firmware_preferences_utils.h"
#include "firmware_status_cache.h"
#include "firmware_task_operator.h"
#include "firmware_update_helper.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareStep FirmwareDownloadMode::GetNextStep(FirmwareStep step)
{
    FIRMWARE_LOGI("GetNextStep %{public}d", static_cast<uint32_t>(step));
    FirmwareStep nextStep = FirmwareStep::COMPLETE;
    switch (step) {
        case FirmwareStep::INIT:
            nextStep = GetStepAfterInit();
            break;
        case FirmwareStep::DOWNLOAD_STEP:
            nextStep = GetStepAfterDownload();
            break;
        default:
            break;
    }
    return nextStep;
}

FirmwareStep FirmwareDownloadMode::GetStepAfterInit()
{
    GetTask();
    if (!tasks_.isExistTask) {
        FIRMWARE_LOGI("GetStepAfterInit no task");
        businessError_.Build(CallResult::FAIL, "no task!");
        return FirmwareStep::COMPLETE;
    }

    FIRMWARE_LOGI("GetStepAfterInit status %{public}d", static_cast<int32_t>(tasks_.status));
    UpgradeStatus taskStatus = tasks_.status;
    if (taskStatus != UpgradeStatus::CHECK_VERSION_SUCCESS &&
        taskStatus != UpgradeStatus::DOWNLOAD_FAIL &&
        taskStatus != UpgradeStatus::DOWNLOAD_PAUSE) {
        businessError_.Build(CallResult::FAIL, "status error!");
        return FirmwareStep::COMPLETE;
    }

    if (taskStatus == UpgradeStatus::DOWNLOADING) {
        FIRMWARE_LOGI("GetStepAfterInit system busy");
        businessError_.Build(CallResult::FAIL, "system busy!");
        businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_SYSTEM_BUSY_ON_DOWNLOAD), "busy on Downloading!");
        return FirmwareStep::COMPLETE;
    }

    if (!DelayedSingleton<NetManager>::GetInstance()->IsNetAvailable()) {
        FIRMWARE_LOGI("GetStepAfterInit IsNeedBlockCheck no network, nettype is %{public}d",
            DelayedSingleton<NetManager>::GetInstance()->GetNetType());
        businessError_.Build(CallResult::FAIL, "download no network!");
        businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_DLOAD_REQUEST_FAIL), "download no network!");
        return FirmwareStep::COMPLETE;
    }

    int64_t requireTotalSize = 0;
    bool isSpaceEnough = downloadDataProcessor_.IsSpaceEnough(requireTotalSize);
    if (!isSpaceEnough) {
        FIRMWARE_LOGE("GetStepAfterInit isSpaceEnough false");
        businessError_.Build(CallResult::FAIL, "download condition fail!");
        businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_NO_ENOUGH_MEMORY), std::to_string(requireTotalSize));
        return FirmwareStep::COMPLETE;
    }
    return FirmwareStep::DOWNLOAD_STEP;
}

FirmwareStep FirmwareDownloadMode::GetStepAfterDownload()
{
    FIRMWARE_LOGI("GetStepAfterDownload downloadOptions %{public}d", CAST_INT(downloadOptions_.order));
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    FIRMWARE_LOGI("GetStepAfterDownload task status %{public}d", task.status);
    if (!task.isExistTask) {
        FIRMWARE_LOGE("GetStepAfterDownload task not exist");
        return FirmwareStep::COMPLETE;
    }
    ErrorMessage errorMessage;
    if (!downloadDataProcessor_.GetDownloadProgress().endReason.empty()) {
        errorMessage.errorCode = stoi(downloadDataProcessor_.GetDownloadProgress().endReason);
    }
    FirmwareStep nextStep = FirmwareStep::COMPLETE;
    switch (task.status) {
        case UpgradeStatus::DOWNLOAD_PAUSE:
            DownloadPauseProcess(task, errorMessage);
            nextStep = FirmwareStep::COMPLETE;
            break;
        case UpgradeStatus::DOWNLOAD_FAIL:
            DownloadFailProcess(task, errorMessage);
            nextStep = FirmwareStep::COMPLETE;
            break;
        case UpgradeStatus::DOWNLOAD_CANCEL:
            DownloadCancelProcess(task, errorMessage);
            nextStep = FirmwareStep::COMPLETE;
            break;
        case UpgradeStatus::DOWNLOAD_SUCCESS:
            DownloadSucessProcess(task, errorMessage);
            nextStep = FirmwareStep::COMPLETE;
            break;
        default:
            break;
    }
    return nextStep;
}

void FirmwareDownloadMode::DownloadPauseProcess(const FirmwareTask &task, const ErrorMessage &errorMessage)
{
    FIRMWARE_LOGI("GetStepAfterDownload download pause");
    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
        task.taskId, EventId::EVENT_DOWNLOAD_PAUSE, UpgradeStatus::DOWNLOAD_PAUSE, errorMessage);
}

void FirmwareDownloadMode::DownloadFailProcess(const FirmwareTask &task, const ErrorMessage &errorMessage)
{
    FIRMWARE_LOGI("GetStepAfterDownload download fail");
    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
        task.taskId, EventId::EVENT_DOWNLOAD_FAIL, UpgradeStatus::DOWNLOAD_FAIL, errorMessage);
    FirmwareUpdateHelper::ClearFirmwareInfo();
}

void FirmwareDownloadMode::DownloadCancelProcess(const FirmwareTask &task, const ErrorMessage &errorMessage)
{
    FIRMWARE_LOGI("GetStepAfterDownload download cancel");
    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
        task.taskId, EventId::EVENT_DOWNLOAD_CANCEL, UpgradeStatus::DOWNLOAD_CANCEL, errorMessage);
    FirmwareTaskOperator().UpdateProgressByTaskId(task.taskId, UpgradeStatus::CHECK_VERSION_SUCCESS, 0);
    std::vector<FirmwareComponent> firmwareComponentList;
    FirmwareComponentOperator firmwareComponentOperator;
    firmwareComponentOperator.QueryAll(firmwareComponentList);
    for (const FirmwareComponent &component : firmwareComponentList) {
        firmwareComponentOperator.UpdateProgressByUrl(
            component.url, UpgradeStatus::CHECK_VERSION_SUCCESS, 0);
    }
    FirmwareTaskOperator().UpdateDownloadTaskIdByTaskId(task.taskId, "");
    FirmwareFileUtils::DeleteDownloadFiles();
}

void FirmwareDownloadMode::DownloadSucessProcess(const FirmwareTask &task, const ErrorMessage &errorMessage)
{
    FIRMWARE_LOGI("GetStepAfterDownload download success");
    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
        task.taskId, EventId::EVENT_UPGRADE_WAIT, UpgradeStatus::DOWNLOAD_SUCCESS);
}

void FirmwareDownloadMode::GetTask()
{
    if (!tasks_.isExistTask) {
        FirmwareTaskOperator().QueryTask(tasks_);
    }
}

void FirmwareDownloadMode::HandleComplete()
{
    FIRMWARE_LOGI("FirmwareDownloadMode::HandleComplete");
    onExecuteFinishCallback_();
}

DownloadOptions FirmwareDownloadMode::GetDownloadOptions()
{
    return downloadOptions_;
}

void FirmwareDownloadMode::SetDownloadProgress(const Progress &progress)
{
    downloadDataProcessor_.SetDownloadProgress(progress);
}
} // namespace UpdateEngine
} // namespace OHOS