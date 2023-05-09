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

#include "firmware_install_apply_mode.h"

#include <utility>

#include "dupdate_errno.h"
#include "dupdate_net_manager.h"
#include "dupdate_upgrade_helper.h"
#include "firmware_callback_utils.h"
#include "firmware_changelog_utils.h"
#include "firmware_common.h"
#include "firmware_component_operator.h"
#include "firmware_constant.h"
#include "firmware_log.h"
#include "firmware_preferences_utils.h"
#include "firmware_status_cache.h"
#include "firmware_task_operator.h"
#include "firmware_update_helper.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareStep FirmwareInstallApplyMode::GetNextStep(FirmwareStep step)
{
    FIRMWARE_LOGI("GetNextStep %{public}d", static_cast<uint32_t>(step));
    FirmwareStep nextStep = FirmwareStep::COMPLETE;
    switch (step) {
        case FirmwareStep::INIT:
            nextStep = GetStepAfterInit();
            break;
        case FirmwareStep::INSTALL_STEP:
            nextStep = GetStepAfterInstall();
            break;
        case FirmwareStep::APPLY_STEP:
            nextStep = GetStepAfterApply();
            break;
        default:
            break;
    }
    return nextStep;
}

FirmwareStep FirmwareInstallApplyMode::GetStepAfterInit()
{
    GetTask();
    if (!tasks_.isExistTask) {
        FIRMWARE_LOGI("GetStepAfterInit no task");
        businessError_.Build(CallResult::FAIL, "no task!");
        return FirmwareStep::COMPLETE;
    }

    FIRMWARE_LOGI("GetStepAfterInit status %{public}d taskId = %{public}s, order = %{public}d",
        static_cast<int32_t>(tasks_.status), tasks_.taskId.c_str(), CAST_INT(upgradeOptions_.order));

    if (upgradeOptions_.order == Order::INSTALL || upgradeOptions_.order == Order::INSTALL_AND_APPLY) {
        return GetStepForInstallAndApplyOrder();
    }

    if (upgradeOptions_.order == Order::APPLY) {
        return FirmwareStep::APPLY_STEP;
    }

    businessError_.Build(CallResult::FAIL, "upgradeOptions error");
    return FirmwareStep::COMPLETE;
}

FirmwareStep FirmwareInstallApplyMode::GetStepForInstallAndApplyOrder()
{
    if (tasks_.status != UpgradeStatus::DOWNLOAD_SUCCESS) {
        businessError_.Build(CallResult::FAIL, "status error!");
        return FirmwareStep::COMPLETE;
    }

    if (!IsAllowInstall()) {
        return FirmwareStep::COMPLETE;
    }

    if (!IsUpgradeFilesReady()) {
        return FirmwareStep::COMPLETE;
    }

    return FirmwareStep::INSTALL_STEP;
}

bool FirmwareInstallApplyMode::IsAllowInstall()
{
    int64_t requireTotalSize = 0;
    if (!installStepDataProcessor_.IsSpaceEnough(requireTotalSize)) {
        FIRMWARE_LOGE("GetStepAfterInit isSpaceEnough false");
        businessError_.Build(CallResult::FAIL, "install condition fail!");
        businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_NO_ENOUGH_MEMORY), std::to_string(requireTotalSize));
        return false;
    }

    if (!FirmwareUpdateHelper::IsBatteryEnough(MANUAL_UPGRADE_BATTERY_LEVEL)) {
        FIRMWARE_LOGE("IsAllowInstall isBatteryEnough is false");
        businessError_.Build(CallResult::FAIL, "install condition fail!");
        businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_LOW_BATTERY_LEVEL), "battery is low");
        return false;
    }
    return true;
}

bool FirmwareInstallApplyMode::IsUpgradeFilesReady()
{
    if (installStepDataProcessor_.IsUpgradeFileCheckSuccess()) {
        return true;
    }

    FIRMWARE_LOGE("check component failed");
    // 手动触发安装场景，DUE通过接口回调返回错误信息给OUC
    businessError_.Build(CallResult::FAIL, "check component failed");
    businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_UPDATE_PRECHECK_FAIL), "check component failed");

    FirmwareUpdateHelper::ClearFirmwareInfo();
    return false;
}

FirmwareStep FirmwareInstallApplyMode::GetStepAfterInstall()
{
    FIRMWARE_LOGI("FirmwareInstallApplyMode installOptions %{public}d", CAST_INT(upgradeOptions_.order));
    if (!installStepDataProcessor_.HasInstallSuccess()) {
        FirmwareUpdateHelper::ClearFirmwareInfo();
    }

    return FirmwareStep::COMPLETE;
}

FirmwareStep FirmwareInstallApplyMode::GetStepAfterApply()
{
    return FirmwareStep::COMPLETE;
}

void FirmwareInstallApplyMode::GetTask()
{
    if (!tasks_.isExistTask) {
        FirmwareTaskOperator().QueryTask(tasks_);
        FIRMWARE_LOGI("FirmwareInstallApplyMode::GetTask status: %{public}d ", CAST_INT(tasks_.status));
    }
}

void FirmwareInstallApplyMode::HandleComplete()
{
    onExecuteFinishCallback_();
}

UpgradeOptions FirmwareInstallApplyMode::GetUpgradeOptions()
{
    return upgradeOptions_;
}

InstallType FirmwareInstallApplyMode::GetInstallType()
{
    return installType_;
}

void FirmwareInstallApplyMode::SetInstallResult(const InstallCallbackInfo &installCallbackInfo)
{
    installStepDataProcessor_.SetInstallResult(installCallbackInfo);
}

void FirmwareInstallApplyMode::SetApplyResult(bool isSuccess)
{
    FIRMWARE_LOGI("SetApplyResult isSuccess: %{public}s", isSuccess ? "success" : "fail");
}
} // namespace UpdateEngine
} // namespace OHOS