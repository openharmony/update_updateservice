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
#include "restorer.h"
#include "napi_util.h"
#include "update_service_kits.h"

namespace OHOS {
namespace UpdateEngine {
napi_value Restorer::Napi::FactoryReset(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("Restorer::Napi::FactoryReset");
    Restorer* restorer = UnwrapJsObject<Restorer>(env, info);
    PARAM_CHECK_NAPI_CALL(env, restorer != nullptr, return nullptr, "Error get restorer");
    return restorer->FactoryReset(env, info);
}

Restorer::Restorer(napi_env env, napi_value thisVar)
{
    napi_ref thisReference = nullptr;
    napi_create_reference(env, thisVar, 1, &thisReference);
    sessionsMgr_ = std::make_shared<SessionManager>(env, thisReference);
    CLIENT_LOGI("Restorer::Restorer");
}

napi_value Restorer::FactoryReset(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_FACTORY_RESET, CALLBACK_POSITION_ONE, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
            return UpdateServiceKits::GetInstance().FactoryReset(*businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to FactoryReset.");
    return retValue;
}
} // namespace UpdateEngine
} // namespace OHOS
