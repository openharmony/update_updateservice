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
#include "package/package.h"
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
        [&](const EventInfo &eventInfo) {
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
    PARAM_CHECK_NAPI_CALL(env, ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Error get upgradeFile");

    std::string certsFile;
    PARAM_CHECK_NAPI_CALL(env, NapiUtil::GetString(env, args[1], certsFile) == CAST_INT(ClientStatus::CLIENT_SUCCESS),
        return nullptr, "Error get certsFile");
    CLIENT_LOGI("VerifyUpgradePackage certsFile:%s", certsFile.c_str());

    SessionParams sessionParams(SessionType::SESSION_VERIFY_PACKAGE, CALLBACK_POSITION_THREE);
    napi_value retValue = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            CLIENT_LOGI("StartWork VerifyUpdatePackage %s, %s", upgradeFile.filePath.c_str(), certsFile.c_str());
            result_ = VerifyPackageWithCallback(upgradeFile.filePath, certsFile,
                [&](int32_t result, uint32_t percent) { NotifyVerifyPackageResult(result, percent); });
            return result_;
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
    PARAM_CHECK_NAPI_CALL(env, ret == ClientStatus::CLIENT_SUCCESS && upgradeFiles.size() != 0,
        return nullptr, "Error get upgrade files");
    SessionParams sessionParams(SessionType::SESSION_APPLY_NEW_VERSION, CALLBACK_POSITION_TWO);
    napi_value retValue = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            CLIENT_LOGI("ApplyNewVersion %s", upgradeFiles[0].filePath.c_str());
            result_ = UpdateServiceKits::GetInstance().RebootAndInstall(MISC_FILE, upgradeFiles[0].filePath);
            return result_;
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to ApplyNewVersion");
    return retValue;
}

int32_t LocalUpdater::GetUpdateResult(SessionType type, UpdateResult &result)
{
    businessError_.errorNum = CallResult::SUCCESS;
    result.businessError = businessError_;
    result.result.status = result_;
    result.buildJSObject = ClientHelper::BuildInt32Status;
    return napi_ok;
}

void LocalUpdater::NotifyVerifyPackageResult(int32_t retCode, uint32_t percent)
{
    uint32_t PERCENT_ONE_HUNDRED = 100;
	// verify fail
    if (retCode != UPDATE_STATE_VERIFY_SUCCESS) {
        CLIENT_LOGE("NotifyVerifyPackageResult error, ret = %{public}d, percent = %{public}d", retCode, percent);
        UpdateResult result;
        result.businessError.errorNum = CallResult::FAIL;
        result.type = SessionType::SESSION_VERIFY_PACKAGE;
        result.result.status = 0;
        result.buildJSObject = ClientHelper::BuildInt32Status;
        sessionsMgr_->Emit("verifyProgress", result);
        return;
    }

    // verify not complete
    if (percent != PERCENT_ONE_HUNDRED) {
        CLIENT_LOGI("NotifyVerifyPackageResult, ret = %{public}d, percent = %{public}d", retCode, percent);
        return;
    }

    // verify success
    CLIENT_LOGI("NotifyVerifyPackageResult success");
    UpdateResult result;
    result.businessError.errorNum = CallResult::SUCCESS;
    result.type = SessionType::SESSION_VERIFY_PACKAGE;
    result.result.status = 1;
    result.buildJSObject = ClientHelper::BuildInt32Status;
    sessionsMgr_->Emit("verifyProgress", result);
}
} // namespace UpdateEngine
} // namespace OHOS
