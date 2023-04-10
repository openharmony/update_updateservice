/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "update_service_util.h"

#include "firmware_combine_version_utils.h"
#include "firmware_constant.h"
#include "device_adapter.h"
#include "firmware_update_helper.h"
#include "preferences_utils.h"
#include "update_define.h"
#include "update_log.h"
#include "update_service.h"
#include "update_service_cache.h"
#include "update_system_event.h"

namespace OHOS {
namespace UpdateEngine {
void UpdateServiceUtil::BuildTaskBody(const Progress &progress, TaskBody &taskBody)
{
    taskBody.status = progress.status;
    taskBody.progress = static_cast<int32_t>(progress.percent);
    int errorCode = atoi(progress.endReason.c_str());
    if (errorCode != 0) {
        ErrorMessage errorMsg;
        errorMsg.errorCode = errorCode;
        errorMsg.errorMessage = progress.endReason;
        taskBody.errorMessages.push_back(errorMsg);
    }
}

sptr<IUpdateCallback> UpdateServiceUtil::GetUpgradeCallback(const UpgradeInfo &upgradeInfo)
{
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service == nullptr) {
        ENGINE_LOGI("GetUpgradeCallback no instance");
        return nullptr;
    }
    return service->GetUpgradeCallback(upgradeInfo);
}

void UpdateServiceUtil::SearchCallback(const UpgradeInfo &upgradeInfo, BusinessError &businessError,
    CheckResult &checkResult)
{
    ENGINE_LOGI("SearchCallback isExistNewVersion %{public}d", checkResult.isExistNewVersion);
    auto upgradeCallback = GetUpgradeCallback(upgradeInfo);
    ENGINE_CHECK(upgradeCallback != nullptr, return, "SearchCallback upgradeCallback is null");
    upgradeCallback->OnCheckVersionDone(businessError, checkResult);
}
} // namespace UpdateEngine
} // namespace OHOS