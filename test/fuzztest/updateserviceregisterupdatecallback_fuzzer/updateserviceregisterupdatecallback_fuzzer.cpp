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

#include "updateserviceregisterupdatecallback_fuzzer.h"

using namespace OHOS::update_engine;

namespace OHOS {
    int32_t FuzzUpdateServiceRegisterUpdateCallbackImpl(void)
    {
        return UpdateServiceKits::GetInstance().RegisterUpdateCallback(FtBuildUpdateContext(),
            FtBuildUpdateCallbackInfo());
    }

    bool FuzzUpdateServiceRegisterUpdateCallback(const uint8_t* data, size_t size)
    {
        if (size < FUZZ_DATA_LEN) {
            return false;
        }
        if (memcpy_s(g_data, sizeof(g_data), data, FUZZ_DATA_LEN) != EOK) {
            return false;
        }
        return !FuzzUpdateServiceRegisterUpdateCallbackImpl();
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzUpdateServiceRegisterUpdateCallback(data, size);
    return 0;
}
