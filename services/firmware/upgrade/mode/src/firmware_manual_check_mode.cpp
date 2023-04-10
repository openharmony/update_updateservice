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

#include "firmware_manual_check_mode.h"

#include "dupdate_errno.h"
#include "dupdate_net_manager.h"
#include "firmware_callback_utils.h"
#include "firmware_common.h"
#include "firmware_constant.h"
#include "firmware_log.h"
#include "firmware_manager.h"
#include "firmware_preferences_utils.h"
#include "firmware_status_cache.h"
#include "firmware_task.h"
#include "firmware_task_operator.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareStep FirmwareManualCheckMode::GetNextStep(FirmwareStep step)
{
    FIRMWARE_LOGI("GetNextStep %{public}d", static_cast<uint32_t>(step));
    FirmwareStep nextStep = FirmwareStep::COMPLETE;
    switch (step) {
        case FirmwareStep::INIT:
            nextStep = GetStepAfterInit();
            break;
        case FirmwareStep::CHECK_STEP:
            nextStep = GetStepAfterCheck();
            break;
        default:
            break;
    }
    return nextStep;
}

FirmwareStep FirmwareManualCheckMode::GetStepAfterInit()
{
    if (DelayedSingleton<FirmwareStatusCache>::GetInstance()->IsChecking()) {
        FIRMWARE_LOGI("IsNeedBlockCheck system busy");
        businessError_.Build(CallResult::FAIL, "system busy!");
        businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_SYSTEM_BUSY_ON_CHECK), "checkNewVersion is checking!");
        return FirmwareStep::COMPLETE;
    }
    if (checkDataProcessor_.IsUpdateOnStatus()) {
        FIRMWARE_LOGI("IsNeedBlockCheck system busy on update");
        businessError_.Build(CallResult::FAIL, "system busy!");
        businessError_.AddErrorMessage(CAST_INT(DUPDATE_ERR_SYSTEM_BUSY_ON_INSTALL), "system is updating!");
        return FirmwareStep::COMPLETE;
    }

    if (!DelayedSingleton<NetManager>::GetInstance()->IsNetAvailable()) {
        FIRMWARE_LOGI("IsNeedBlockCheck no network");
        businessError_.Build(CallResult::FAIL, "CheckNewVersionService no network!");
        businessError_.AddErrorMessage(CAST_INT(SearchStatus::NET_ERROR), "CheckNewVersionService no network !");
        return FirmwareStep::COMPLETE;
    }
    return FirmwareStep::CHECK_STEP;
}

FirmwareStep FirmwareManualCheckMode::GetStepAfterCheck()
{
    if (checkDataProcessor_.IsCheckFailed() || checkDataProcessor_.IsSameWithDb() ||
        !checkDataProcessor_.HasNewVersion()) {
        return FirmwareStep::COMPLETE;
    }

    // 手动搜包，搜到新版本并且与之前结果不一致，并且当前状态为搜包成功，推送task_receive事件
    FirmwareTask task;
    FirmwareTaskOperator().QueryTask(task);
    FIRMWARE_LOGI("FirmwareManualCheckMode::status %{public}d", static_cast<uint32_t>(task.status));
    if (task.status == UpgradeStatus::CHECK_VERSION_SUCCESS) {
        DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
            task.taskId, EventId::EVENT_TASK_RECEIVE, UpgradeStatus::CHECK_VERSION_SUCCESS);
        std::shared_ptr<FirmwarePreferencesUtil> utils = DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
        if (!utils->ObtainBool(Firmware::AUTO_DOWNLOAD_SWITCH, false)) {
            DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
                task.taskId, EventId::EVENT_DOWNLOAD_WAIT, UpgradeStatus::CHECK_VERSION_SUCCESS);
        }
    }
    return FirmwareStep::COMPLETE;
}

void FirmwareManualCheckMode::SetCheckResult(
    CheckStatus status, const Duration &duration, const std::vector<FirmwareComponent> &componentList,
    const CheckAndAuthInfo &checkAndAuthInfo)
{
    FIRMWARE_LOGI("FirmwareManualCheckMode::SetCheckResult %{public}d", static_cast<uint32_t>(status));
    checkDataProcessor_.SetCheckResult(status, duration, componentList);
}

void FirmwareManualCheckMode::HandleComplete()
{
    FIRMWARE_LOGI("FirmwareManualCheckMode::HandleComplete");

    // 回调搜包结果
    CheckResult checkResult;
    checkDataProcessor_.BuildCheckResult(checkResult);
    onCheckComplete_(businessError_, checkResult);
}
} // namespace UpdateEngine
} // namespace OHOS