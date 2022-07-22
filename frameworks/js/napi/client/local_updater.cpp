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
#include "local_updater.h"
#include "napi_util.h"
#include "update_service_kits.h"
#include "update_session.h"

namespace OHOS {
namespace UpdateEngine {
const std::string MISC_FILE = "/dev/block/by-name/misc";
napi_value LocalUpdater::Napi::NapiVerifyUpgradePackage(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("LocalUpdater::Napi::NapiVerifyUpgradePackage");
    LocalUpdater* localUpdater = UnwrapJsObject<LocalUpdater>(env, info);
    PARAM_CHECK_NAPI_CALL(env, localUpdater != nullptr, return nullptr, "Error get localUpdater");
    return localUpdater->VerifyUpgradePackage(env, info);
}

napi_value LocalUpdater::Napi::NapiApplyNewVersion(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("LocalUpdater::Napi::NapiApplyNewVersion");
    LocalUpdater* localUpdater = UnwrapJsObject<LocalUpdater>(env, info);
    PARAM_CHECK_NAPI_CALL(env, localUpdater != nullptr, return nullptr, "Error get localUpdater");
    return localUpdater->ApplyNewVersion(env, info);
}

napi_value LocalUpdater::Napi::NapiOn(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("LocalUpdater::Napi::NapiOn");
    LocalUpdater* localUpdater = UnwrapJsObject<LocalUpdater>(env, info);
    PARAM_CHECK_NAPI_CALL(env, localUpdater != nullptr, return nullptr, "Error get localUpdater");
    return localUpdater->On(env, info);
}

napi_value LocalUpdater::Napi::NapiOff(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("LocalUpdater::Napi::NapiOff");
    LocalUpdater* localUpdater = UnwrapJsObject<LocalUpdater>(env, info);
    PARAM_CHECK_NAPI_CALL(env, localUpdater != nullptr, return nullptr, "Error get localUpdater");
    return localUpdater->Off(env, info);
}

LocalUpdater::LocalUpdater(napi_env env, napi_value thisVar)
{
    napi_ref thisReference = nullptr;
    napi_create_reference(env, thisVar, 1, &thisReference);
    sessionsMgr_ = std::make_shared<SessionManager>(env, thisReference);
    CLIENT_LOGI("LocalUpdater::LocalUpdater");
}

void LocalUpdater::Init()
{
    PARAM_CHECK(!isInit_, return, "local updater has init.");
    UpdateCallbackInfo callback {
        [](const BusinessError &businessError, const CheckResultEx &checkResultEx) {},
        [this](const EventInfo &eventInfo) {
            NotifyEventInfo(eventInfo);
        },
    };
    CLIENT_LOGI("LocalUpdater::Init");
    UpgradeInfo upgradeInfo;
    upgradeInfo.upgradeApp = LOCAL_UPGRADE_INFO;
    UpdateServiceKits::GetInstance().RegisterUpdateCallback(upgradeInfo, callback);
    isInit_ = true;
}

napi_value LocalUpdater::VerifyUpgradePackage(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");
    PARAM_CHECK_NAPI_CALL(env, argc >= ARG_NUM_TWO, return nullptr, "Error get cb info");

    UpgradeFile upgradeFile;
    ClientStatus ret = ClientHelper::GetUpgradeFileFromArg(env, args[0], upgradeFile);
    if (ret != ClientStatus::CLIENT_SUCCESS) {
        CLIENT_LOGE("VerifyUpgradePackage error, get upgradeFile fail");
        return StartParamErrorSession(env, info, CALLBACK_POSITION_THREE);
    }

    std::string certsFile;
    int32_t result = NapiUtil::GetString(env, args[1], certsFile);
    if (result != CAST_INT(ClientStatus::CLIENT_SUCCESS)) {
        CLIENT_LOGE("VerifyUpgradePackage error, get certsFile fail");
        return StartParamErrorSession(env, info, CALLBACK_POSITION_THREE);
    }

    SessionParams sessionParams(SessionType::SESSION_VERIFY_PACKAGE, CALLBACK_POSITION_THREE, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [upgradeFile, certsFile](SessionType type, void *context) -> int {
            CLIENT_LOGI("VerifyUpdatePackage StartWork %s, %s", upgradeFile.filePath.c_str(), certsFile.c_str());
            BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
            return UpdateServiceKits::GetInstance().VerifyUpgradePackage(upgradeFile.filePath, certsFile,
                *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to VerifyUpgradePackage.");
    return retValue;
}

napi_value LocalUpdater::ApplyNewVersion(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok && argc >= ARG_NUM_ONE, return nullptr, "Error get cb info");

    std::vector<UpgradeFile> upgradeFiles;
    ClientStatus ret = ClientHelper::GetUpgradeFilesFromArg(env, args[0], upgradeFiles);
    if ((ret != ClientStatus::CLIENT_SUCCESS) || (upgradeFiles.size() == 0)) {
        CLIENT_LOGE("ApplyNewVersion error, get GetUpgradeFilesFromArg fail");
        return StartParamErrorSession(env, info, CALLBACK_POSITION_TWO);
    }

    SessionParams sessionParams(SessionType::SESSION_APPLY_NEW_VERSION, CALLBACK_POSITION_TWO, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [upgradeFiles](SessionType type, void *context) -> int {
            CLIENT_LOGI("ApplyNewVersion %s", upgradeFiles[0].filePath.c_str());
            BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
            UpgradeInfo upgradeInfo;
            upgradeInfo.upgradeApp = LOCAL_UPGRADE_INFO;
            return UpdateServiceKits::GetInstance().ApplyNewVersion(upgradeInfo, MISC_FILE, upgradeFiles[0].filePath,
                *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to ApplyNewVersion");
    return retValue;
}
} // namespace UpdateEngine
} // namespace OHOS
