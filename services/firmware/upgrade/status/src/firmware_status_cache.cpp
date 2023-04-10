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

#include "firmware_status_cache.h"

#include "firmware_log.h"
#include "time_utils_proxy.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareStatusCache::FirmwareStatusCache()
{
    FIRMWARE_LOGD("FirmwareStatusCache::FirmwareStatusCache");
    statusCache_ = std::make_shared<StatusCache>(std::make_shared<TimeUtilsProxy>());
}

FirmwareStatusCache::~FirmwareStatusCache()
{
    FIRMWARE_LOGD("FirmwareStatusCache::~FirmwareStatusCache");
}

bool FirmwareStatusCache::IsChecking()
{
    if (statusCache_ == nullptr) {
        return false;
    }
    return statusCache_->IsChecking();
}

void FirmwareStatusCache::SetIsChecking(bool isChecking)
{
    if (statusCache_ == nullptr) {
        return;
    }
    statusCache_->SetIsChecking(isChecking);
}
} // namespace UpdateEngine
} // namespace OHOS
