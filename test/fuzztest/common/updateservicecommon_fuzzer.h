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

#include "update_service_kits_impl.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

namespace OHOS {
namespace update_engine {
    int32_t FtBuildService(void);

    UpdateCallbackInfo FtBuildUpdateCallbackInfo(void);

    UpdateContext FtBuildUpdateContext(void);

    UpdatePolicy FtBuildUpdatePolicy(void);

    UpgradeInfo FtBuildUpgradeInfo(void);

    VersionInfo FtBuildVersionInfo(void);

    const uint32_t FUZZ_HEAD_DATA = 0;

    const uint32_t FUZZ_DATA_LEN = 500;

    uint8_t g_data[FUZZ_DATA_LEN];

    uint32_t g_index = FUZZ_HEAD_DATA;
} // namespace update_engine
} // namespace OHOS

#endif // UPDATE_SERVICE_COMMON_FUZZER_H
