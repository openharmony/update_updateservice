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

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <algorithm>
#include <functional>
#include <string>

#include "nlohmann/json.hpp"

#include "update_define.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {

enum class JsonParseError {
    ERR_OK = 0,
    COMMOM_ERROR,
    MISSING_PROP,
    TYPE_ERROR
};

class JsonUtils {
public:
    template <typename T>
    static int32_t GetValueAndSetTo(const nlohmann::json &jsonObject, const std::string &key, T &value)
    {
        if (jsonObject.find(key) == jsonObject.end()) {
            ENGINE_LOGI("parse error, missing prop : %{public}s ", key.c_str());
            return CAST_INT(JsonParseError::MISSING_PROP);
        }
        if (!CheckType(jsonObject, key, value)) {
            ENGINE_LOGI("type is error : %{public}s ", key.c_str());
            return CAST_INT(JsonParseError::TYPE_ERROR);
        }
        jsonObject.at(key).get_to(value);
        return CAST_INT(JsonParseError::ERR_OK);
    };
private:
    static bool CheckType(const nlohmann::json &jsonObject, const std::string &key, std::string &value)
    {
        return jsonObject.at(key).is_string();
    }
    static bool CheckType(const nlohmann::json &jsonObject, const std::string &key, int32_t &value)
    {
        return jsonObject.at(key).is_number();
    }
    static bool CheckType(const nlohmann::json &jsonObject, const std::string &key, uint32_t &value)
    {
        return jsonObject.at(key).is_number();
    }
    static bool CheckType(const nlohmann::json &jsonObject, const std::string &key, uint64_t &value)
    {
        return jsonObject.at(key).is_number();
    }
    static bool CheckType(const nlohmann::json &jsonObject, const std::string &key, bool &value)
    {
        return jsonObject.at(key).is_boolean();
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // JSON_UTILS_H