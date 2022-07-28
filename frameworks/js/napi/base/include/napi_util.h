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

#ifndef UPDATE_NAPI_UTIL_H
#define UPDATE_NAPI_UTIL_H

#include <string>
#include "client_helper.h"
#include "js_native_api.h"
#include "js_native_api_types.h"

namespace OHOS {
namespace UpdateEngine {
enum ARG_NUM {
    ARG_NUM_ONE = 1,
    ARG_NUM_TWO,
    ARG_NUM_THREE,
    ARG_NUM_FOUR,
    MAX_ARGC
};

enum CALLBACK_POSITION {
    CALLBACK_POSITION_ONE = 1,
    CALLBACK_POSITION_TWO,
    CALLBACK_POSITION_THREE,
    CALLBACK_POSITION_FOUR,
    CALLBACK_MAX_POSITION
};

template<typename T>
T *UnwrapJsObject(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    T *nativeObject = nullptr;
    napi_unwrap(env, thisVar, (void**)&nativeObject);
    return nativeObject;
}

class NapiUtil {
public:
    static int32_t GetInt32(napi_env env, napi_value arg, const std::string &attrName, int32_t &intValue);
    static int32_t GetBool(napi_env env, napi_value arg, const std::string &attrName, bool &value);

    static int32_t GetString(napi_env env, napi_value arg, const std::string &attrName, std::string &strValue);
    static int32_t GetString(napi_env env, napi_value arg, std::string &strValue);
    static int32_t SetString(napi_env env, napi_value arg, const std::string &attrName, const std::string &string);

    static int32_t SetInt32(napi_env env, napi_value arg, const std::string &attrName, int32_t intValue);
    static int32_t SetInt64(napi_env env, napi_value arg, const std::string &attrName, int64_t intValue);
    static int32_t SetBool(napi_env env, napi_value arg, const std::string &attrName, bool value);

    static ClientStatus IsTypeOf(napi_env env, napi_value arg, napi_valuetype type);

    static ClientStatus CreateReference(napi_env env, napi_value arg, uint32_t refcount, napi_ref &reference);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_NAPI_UTIL_H