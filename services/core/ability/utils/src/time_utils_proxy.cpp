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

#include "time_utils_proxy.h"

#include "time_utils.h"

namespace OHOS {
namespace UpdateEngine {
int64_t TimeUtilsProxy::GetTimestamp()
{
    return TimeUtils::GetTimestamp();
}

int64_t TimeUtilsProxy::GetRandTime(int32_t minTime, int32_t maxTime)
{
    return TimeUtils::GetRandTime(minTime, maxTime);
}

int64_t TimeUtilsProxy::GetSystemBootDuration()
{
    return TimeUtils::GetSystemBootDuration();
}
} // namespace UpdateEngine
} // namespace OHOS
