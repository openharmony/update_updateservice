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

#ifndef DEFINE_PROPERTY_H
#define DEFINE_PROPERTY_H

#include <string>
#include <vector>

#include "js_native_api.h"
#include "js_native_api_types.h"

namespace OHOS {
namespace UpdateEngine {
class DefineProperty {
public:
    static void DefineProperties(napi_env env, napi_value exports);

private:
    static void DefineCallResult(napi_env env, napi_value exports);
    static void DefineUpgradeStatus(napi_env env, napi_value exports);
    static void DefineComponentType(napi_env env, napi_value exports);
    static void DefineEffectiveMode(napi_env env, napi_value exports);
    static void DefineNetType(napi_env env, napi_value exports);
    static void DefineOrder(napi_env env, napi_value exports);
    static void DefineEventClassify(napi_env env, napi_value exports);
    static void DefineBusinessSubType(napi_env env, napi_value exports);
    static void DefineDescriptionType(napi_env env, napi_value exports);
    static void DefineDescriptionFormat(napi_env env, napi_value exports);
    static void DefineEventId(napi_env env, napi_value exports);
    static void DefineUpgradeAction(napi_env env, napi_value exports);
    static void DefineBusinessVendor(napi_env env, napi_value exports);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // DEFINE_PROPERTY_H