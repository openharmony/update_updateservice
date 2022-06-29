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
const std::string MISC_PATH = "/misc";
const std::string CMD_WIPE_DATA = "--user_wipe_data";
const std::string MISC_FILE = "/dev/block/by-name/misc";

Restorer::Restorer(napi_env env, napi_value thisVar)
{
    napi_ref thisReference = nullptr;
    napi_create_reference(env, thisVar, 1, &thisReference);
    sessionsMgr_ = std::make_shared<SessionManager>(env, thisReference);
    CLIENT_LOGI("Restorer::Restorer");
}

napi_value Restorer::FactoryReset(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_REBOOT_AND_CLEAN, CALLBACK_POSITION_ONE);
    napi_value retValue = StartSession(env, info, sessionParams,
        [&](SessionType type, void *context) -> int {
            CLIENT_LOGI("RebootAndClean::misc path : %{public}s", MISC_FILE.c_str());
            result_ = UpdateServiceKits::GetInstance().RebootAndClean(MISC_FILE, CMD_WIPE_DATA);
            return result_;
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to GetNewVersionInfo.");
    return retValue;
}

int32_t Restorer::GetUpdateResult(SessionType type, UpdateResult &result)
{
    businessError_.errorNum = CallResult::SUCCESS;
    result.businessError = businessError_;
    result.result.status = result_;
    result.buildJSObject = ClientHelper::BuildInt32Status;
    return napi_ok;
}
} // namespace UpdateEngine
} // namespace OHOS
