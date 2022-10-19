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

#include "napi_util.h"

#include <string>

namespace OHOS {
namespace UpdateEngine {
const int32_t STRING_MAX_LENGTH = 200;

int32_t NapiUtil::GetInt32(napi_env env, napi_value arg, const std::string &attrName, int32_t &intValue)
{
    bool result = false;
    napi_status status = napi_has_named_property(env, arg, attrName.c_str(), &result);
    if (result && (status == napi_ok)) {
        napi_value value = nullptr;
        napi_get_named_property(env, arg, attrName.c_str(), &value);
        napi_get_value_int32(env, value, &intValue);
    }
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t NapiUtil::GetBool(napi_env env, napi_value arg, const std::string &attrName, bool &value)
{
    bool result = false;
    napi_status status = napi_has_named_property(env, arg, attrName.c_str(), &result);
    if (result && (status == napi_ok)) {
        napi_value obj = nullptr;
        napi_get_named_property(env, arg, attrName.c_str(), &obj);
        napi_get_value_bool(env, obj, &value);
    }
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t NapiUtil::GetString(napi_env env, napi_value arg, const std::string &attrName, std::string &strValue)
{
    bool result = false;
    napi_status status = napi_has_named_property(env, arg, attrName.c_str(), &result);
    if (result && (status == napi_ok)) {
        napi_value value = nullptr;
        napi_get_named_property(env, arg, attrName.c_str(), &value);
        return GetString(env, value, strValue);
    }
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t NapiUtil::GetString(napi_env env, napi_value arg, std::string &strValue)
{
    napi_valuetype valuetype;
    napi_status status = napi_typeof(env, arg, &valuetype);
    PARAM_CHECK(status == napi_ok, return CAST_INT(ClientStatus::CLIENT_FAIL), "Failed to napi_typeof");
    PARAM_CHECK(valuetype == napi_string, return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE), "Invalid type");

    std::vector<char> buff(STRING_MAX_LENGTH);
    size_t copied;
    status = napi_get_value_string_utf8(env, arg, static_cast<char*>(buff.data()), STRING_MAX_LENGTH, &copied);
    PARAM_CHECK(status == napi_ok, return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE), "Error get string");
    strValue.assign(buff.data(), copied);
    return CAST_INT(ClientStatus::CLIENT_SUCCESS);
}

int32_t NapiUtil::SetString(napi_env env, napi_value arg, const std::string &attrName, const std::string &string)
{
    napi_value value = nullptr;
    napi_create_string_utf8(env, string.c_str(), string.length(), &value);
    napi_set_named_property(env, arg, attrName.c_str(), value);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t NapiUtil::SetInt32(napi_env env, napi_value arg, const std::string &attrName, int32_t intValue)
{
    napi_value infoStatus = nullptr;
    napi_create_int32(env, intValue, &infoStatus);
    napi_set_named_property(env, arg, attrName.c_str(), infoStatus);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t NapiUtil::SetInt64(napi_env env, napi_value arg, const std::string &attrName, int64_t intValue)
{
    napi_value infoStatus = nullptr;
    napi_create_int64(env, intValue, &infoStatus);
    napi_set_named_property(env, arg, attrName.c_str(), infoStatus);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t NapiUtil::SetBool(napi_env env, napi_value arg, const std::string &attrName, bool value)
{
    napi_value infoStatus = nullptr;
    napi_create_int32(env, value, &infoStatus);
    napi_set_named_property(env, arg, attrName.c_str(), infoStatus);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

ClientStatus NapiUtil::IsTypeOf(napi_env env, napi_value arg, napi_valuetype type)
{
    napi_valuetype valueType;
    napi_status status = napi_typeof(env, arg, &valueType);
    PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_FAIL, "Failed to napi_typeof");
    PARAM_CHECK(valueType == type, return ClientStatus::CLIENT_INVALID_TYPE, "Not same napi type");
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus NapiUtil::CreateReference(napi_env env, napi_value arg, uint32_t refcount, napi_ref &reference)
{
    napi_status status = napi_create_reference(env, arg, refcount, &reference);
    PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_FAIL, "Failed to create reference");
    return ClientStatus::CLIENT_SUCCESS;
}
} // namespace UpdateEngine
} // namespace OHOS