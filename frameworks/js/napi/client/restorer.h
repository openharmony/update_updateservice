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
#ifndef UPDATE_CLIENT_RESTORER_H
#define UPDATE_CLIENT_RESTORER_H

#include "iupdater.h"
#include "js_native_api_types.h"

namespace OHOS {
namespace UpdateEngine {
class Restorer : public IUpdater {
public:
    class Napi {
    public:
        static constexpr const char *FUNCTION_FACTORY_RESET = "factoryReset";

        static napi_value FactoryReset(napi_env env, napi_callback_info info);
    };

    Restorer(napi_env env, napi_value thisVar);

    napi_value FactoryReset(napi_env env, napi_callback_info info);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_CLIENT_RESTORER_H
