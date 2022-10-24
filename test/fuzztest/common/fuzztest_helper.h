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

#ifndef UPDATE_SERVICE_COMMON_FUZZER_H
#define UPDATE_SERVICE_COMMON_FUZZER_H

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include "singleton.h"

#include "update_service_kits_impl.h"

namespace OHOS {
namespace UpdateEngine {
constexpr const int32_t FUZZ_DATA_LEN = 500;

class FuzztestHelper : public DelayedSingleton<FuzztestHelper> {
    DECLARE_DELAYED_SINGLETON(FuzztestHelper);

public:
    bool TrySetData(const uint8_t* data, size_t size);
    int32_t GetInt();
    BusinessType BuildBusinessType();
    UpdateCallbackInfo BuildUpdateCallbackInfo();
    UpgradeInfo BuildUpgradeInfo();
    UpgradePolicy BuildUpgradePolicy();
    VersionDigestInfo BuildVersionDigestInfo();

private:
    void GetCharArray(char *charArray, uint32_t arraySize);
    uint32_t GetUInt();

    uint32_t index_ = 0;
    uint8_t data_[FUZZ_DATA_LEN] = {0};
};
} // namespace UpdateEngine
} // namespace OHOS

#endif // UPDATE_SERVICE_COMMON_FUZZER_H
