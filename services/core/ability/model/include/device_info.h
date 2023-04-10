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

#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <string>

#include "nlohmann/json.hpp"

#include "encrypt_utils.h"

namespace OHOS {
namespace UpdateEngine {
struct DeviceInfo {
public:
    std::string udid;
    std::string deviceId;

    nlohmann::ordered_json ToJson(bool isPrint)
    {
        nlohmann::ordered_json json = nlohmann::ordered_json::object();
        json["udid"] = isPrint ? EncryptUtils::EncryptString(udid) : udid;
        json["deviceId"] = isPrint ? EncryptUtils::EncryptString(deviceId) : deviceId;
        return json;
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // DEVICE_INFO_H