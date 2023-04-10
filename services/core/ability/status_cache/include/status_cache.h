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

#ifndef STATUS_CACHE_H
#define STATUS_CACHE_H

#include <cstdint>

#include "time_utils_proxy.h"

namespace OHOS {
namespace UpdateEngine {
class StatusCache {
public:
    StatusCache(std::shared_ptr<TimeUtilsProxy> timeUtilsProxy) : timeUtilsProxy_(timeUtilsProxy) {}
    ~StatusCache() = default;

    bool IsChecking();
    void SetIsChecking(bool isChecking);

private:
    int64_t GetCurrentTime();

private:
    std::shared_ptr<TimeUtilsProxy> timeUtilsProxy_;
    bool isChecking_ = false;
    int64_t lastCheckTime_ = -1;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // STATUS_CACHE_H