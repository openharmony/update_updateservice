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

#ifndef UPDATE_DEFINE_H
#define UPDATE_DEFINE_H

#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))
#define CAST_INT(enumClass) (static_cast<int32_t>(enumClass))
#define CAST_UINT(enumClass) (static_cast<uint32_t>(enumClass))

#define ENGINE_CHECK(retCode, exper, ...)     \
    do {                                      \
        if (!(retCode)) {                     \
            ENGINE_LOGE(__VA_ARGS__);         \
            exper;                            \
        }                                     \
    } while (0)
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_DEFINE_H