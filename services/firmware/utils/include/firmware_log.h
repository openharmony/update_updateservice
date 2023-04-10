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

#ifndef FIRMWARE_LOG_H
#define FIRMWARE_LOG_H

#include "hilog/log.h"

#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
static constexpr OHOS::HiviewDFX::HiLogLabel FIRMWARE_UPDATE_LABEL = {LOG_CORE, 0xD002E00, "UPDATE_FIRMWARE"};

#define FIRMWARE_LOGD(fmt, ...) PRINT_LOGD(FIRMWARE_UPDATE_LABEL, fmt, ##__VA_ARGS__)
#define FIRMWARE_LOGI(fmt, ...) PRINT_LOGI(FIRMWARE_UPDATE_LABEL, fmt, ##__VA_ARGS__)
#define FIRMWARE_LOGE(fmt, ...) PRINT_LOGE(FIRMWARE_UPDATE_LABEL, fmt, ##__VA_ARGS__)

#define FIRMWARE_LONG_LOGD(fmt, args) PRINT_LONG_LOGD(FIRMWARE_UPDATE_LABEL, fmt, args)
#define FIRMWARE_LONG_LOGI(fmt, args) PRINT_LONG_LOGI(FIRMWARE_UPDATE_LABEL, fmt, args)
#define FIRMWARE_LONG_LOGE(fmt, args) PRINT_LONG_LOGE(FIRMWARE_UPDATE_LABEL, fmt, args)
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_LOG_H