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

#ifndef CLIENT_DEFINE_H
#define CLIENT_DEFINE_H

#include "hilog/log.h"

#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
static constexpr OHOS::HiviewDFX::HiLogLabel UPDATE_CLIENT = {LOG_CORE, 0xD002E00, "UPDATE_CLIENT"};

#define CLIENT_LOGI(format, ...)  \
    OHOS::HiviewDFX::HiLog::Info(UPDATE_CLIENT, "[%{public}s(%{public}d)] " format, \
    UpdateLog::GetBriefFileName(std::string(__FILE__)).c_str(), __LINE__, ##__VA_ARGS__)

#define CLIENT_LOGE(format, ...)  \
    OHOS::HiviewDFX::HiLog::Error(UPDATE_CLIENT, "[%{public}s(%{public}d)] " format, \
    UpdateLog::GetBriefFileName(std::string(__FILE__)).c_str(), __LINE__, ##__VA_ARGS__)

#define PARAM_CHECK(validCheck, exper, ...)  \
    if (!(validCheck)) {                     \
        CLIENT_LOGE(__VA_ARGS__);            \
        exper;                               \
    }

#define PARAM_CHECK_NAPI_CALL(env, assertion, exper, message) \
    if (!(assertion)) {                     \
        CLIENT_LOGE(message);               \
        exper;                              \
    }

#define INDEX(x) ((x) - 1)
} // namespace UpdateEngine
} // namespace OHOS
#endif // CLIENT_DEFINE_H