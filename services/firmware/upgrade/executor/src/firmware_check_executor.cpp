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

#include "firmware_check_executor.h"

#include <thread>

#include "config_parse.h"
#include "firmware_common.h"
#include "firmware_constant.h"
#include "firmware_file_utils.h"
#include "firmware_icheck.h"
#include "firmware_log.h"
#include "firmware_status_cache.h"
#include "firmware_update_helper.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
void FirmwareCheckExecutor::Execute()
{
    FIRMWARE_LOGI("FirmwareCheckExecutor::Execute");
    DelayedSingleton<FirmwareStatusCache>::GetInstance()->SetIsChecking(true);
    std::thread checkThread(&FirmwareCheckExecutor::DoCheck, this);
    checkThread.detach();
}

void FirmwareCheckExecutor::Complete()
{
    FIRMWARE_LOGI("FirmwareCheckExecutor::complete");
    DelayedSingleton<FirmwareStatusCache>::GetInstance()->SetIsChecking(false);
    checkComponentCallback_.firmwareComponentCallback(status_, duration_, componentList_, checkAndAuthInfo_);
}

void FirmwareCheckExecutor::DoCheck()
{
    FIRMWARE_LOGI("FirmwareCheckExecutor::DoCheck");
    FirmwareCheckCallback cb{
        [=](CheckStatus status, const Duration &duration, const std::vector<FirmwareComponent> &firmwareCheckResultList,
            const CheckAndAuthInfo &checkAndAuthInfo) {
        status_ = status;
        duration_ = duration;
        componentList_ = firmwareCheckResultList;
        checkAndAuthInfo_ = checkAndAuthInfo;
        FIRMWARE_LOGI("CheckComplete status: %{public}d size: %{public}d", status_, CAST_INT(componentList_.size()));
        if (status_ == CheckStatus::CHECK_FAIL || componentList_.size() == 0) {
            Complete();
            return;
        }
        SetComponentCheckStatus();
        Complete();
    }};

    std::shared_ptr<FirmwareICheck> check = nullptr;
    check = std::make_shared<FirmwareICheck>(RequestType::CHECK);
    if (check == nullptr) {
        FIRMWARE_LOGE("check is nullptr");
        return;
    }
    check->DoAction(cb);
}

void FirmwareCheckExecutor::SetComponentCheckStatus()
{
    FIRMWARE_LOGI("FirmwareCheckExecutor::SetComponentCheckStatus");
    for (auto &component : componentList_) {
        component.status = UpgradeStatus::CHECK_VERSION_SUCCESS;
    }
}
} // namespace UpdateEngine
} // namespace OHOS
