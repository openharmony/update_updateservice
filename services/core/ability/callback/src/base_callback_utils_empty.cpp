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

#include "base_callback_utils.h"

#include "iupdate_callback.h"
#include "update_define.h"
#include "update_helper.h"
#include "update_log.h"
#include "update_service.h"
#include "update_service_cache.h"
#include "update_service_util.h"

namespace OHOS {
namespace UpdateEngine {
void BaseCallbackUtils::NotifyEvent(const std::string &versionDigestInfo, EventId eventId, UpgradeStatus status,
    const ErrorMessage &errorMessage, const std::vector<VersionComponent> &versionComponents)
{
    ENGINE_LOGI("BaseCallbackUtils NotifyEvent");
}

void BaseCallbackUtils::ProgressCallback(const std::string &versionDigestInfo, const Progress &progress)
{
    ENGINE_LOGI("BaseCallbackUtils ProgressCallback");
}

void BaseCallbackUtils::CallbackToHap(EventInfo &eventInfo)
{
    ENGINE_LOGI("BaseCallbackUtils CallbackToHap");
}

sptr<IUpdateCallback> BaseCallbackUtils::GetUpgradeCallback(const UpgradeInfo &upgradeInfo)
{
    return nullptr;
}

void BaseCallbackUtils::NotifyToHap(EventInfo &info)
{
    ENGINE_LOGI("BaseCallbackUtils NotifyToHap");
}
} // namespace UpdateEngine
} // namespace OHOS
