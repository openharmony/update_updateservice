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

#ifndef TIME_UTILS_PROXY
#define TIME_UTILS_PROXY

#include <cstdint>
#include <string>

namespace OHOS {
namespace UpdateEngine {
class TimeUtilsProxy {
public:
    virtual ~TimeUtilsProxy() = default;

    virtual int64_t GetTimestamp();
    virtual int64_t GetRandTime(int32_t minTime, int32_t maxTime);
    virtual int64_t GetSystemBootDuration();
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // TIME_UTILS_PROXY