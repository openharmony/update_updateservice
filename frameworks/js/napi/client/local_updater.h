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
#ifndef LOCAL_UPDATER_H
#define LOCAL_UPDATER_H

#include "iupdater.h"
#include "node_api.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class LocalUpdater : public IUpdater {
public:
    class Napi {
    public:
        static constexpr const char *FUNCTION_VERIFY_UPGRADE_PACKAGE = "verifyUpgradePackage";
        static constexpr const char *FUNCTION_APPLY_NEW_VERSION = "applyNewVersion";
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_OFF = "off";

        static napi_value NapiVerifyUpgradePackage(napi_env env, napi_callback_info info);
        static napi_value NapiApplyNewVersion(napi_env env, napi_callback_info info);
        static napi_value NapiOn(napi_env env, napi_callback_info info);
        static napi_value NapiOff(napi_env env, napi_callback_info info);
    };

    LocalUpdater(napi_env env, napi_value thisVar);

    void Init();

    napi_value VerifyUpgradePackage(napi_env env, napi_callback_info info);

    napi_value ApplyNewVersion(napi_env env, napi_callback_info info);

private:
    bool isInit_ = false;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // LOCAL_UPDATER_H