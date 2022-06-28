/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "update_client.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <tuple>
#include <unistd.h>
#include <uv.h>
#include <vector>

#include "client_helper.h"
#include "iupdate_service.h"
#include "misc_info/misc_info.h"
#include "napi_util.h"
#include "node_api.h"
#include "node_api_types.h"
#include "package/package.h"
#include "securec.h"
#include "update_helper.h"
#include "update_service_kits.h"
#include "update_session.h"

using namespace std;

namespace OHOS {
namespace UpdateEngine {
const std::string MISC_FILE = "/dev/block/by-name/misc";
const std::string CMD_WIPE_DATA = "--user_wipe_data";
const std::string UPDATER_PKG_NAME = "/data/ota_package/updater.zip";

napi_value UpdateClient::Napi::NapiOn(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("UpdateClient::Napi::NapiOn");
    UpdateClient* updater = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, updater != nullptr, return nullptr, "Error get UpdateClient");
    return updater->On(env, info);
}

napi_value UpdateClient::Napi::NapiOff(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("UpdateClient::Napi::NapiOff");
    UpdateClient* updater = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, updater != nullptr, return nullptr, "Error get UpdateClient");
    return updater->Off(env, info);
}

UpdateClient::UpdateClient(napi_env env, napi_value thisVar)
{
    napi_ref thisReference = nullptr;
    napi_create_reference(env, thisVar, 1, &thisReference);
    sessionsMgr_ = std::make_shared<SessionManager>(env, thisReference);
}

UpdateClient::~UpdateClient()
{
    if (sessionsMgr_ != nullptr) {
        sessionsMgr_->Clear();
    }
}

napi_value UpdateClient::GetOnlineUpdater(napi_env env, napi_callback_info info)
{
    napi_value result;
    napi_create_int32(env, 0, &result);
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");
    PARAM_CHECK_NAPI_CALL(env, argc >= 1, return nullptr, "Invalid param");
    PARAM_CHECK_NAPI_CALL(env, !isInit, return result, "Has been init");

    ClientStatus ret = ClientHelper::GetUpgradeInfoFromArg(env, args[0], upgradeInfo_);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Failed to get upgradeInfo param");

    UpdateCallbackInfo callback {
        [=](const BusinessError &businessError, const CheckResultEx &checkResultEx) {
            NotifyCheckVersionDone(businessError, checkResultEx);
        },
        [=](const EventInfo &eventInfo) {
            NotifyEventInfo(eventInfo);
        }
    };
    UpdateServiceKits::GetInstance().RegisterUpdateCallback(upgradeInfo_, callback);
    isInit = true;
    return result;
}

napi_value UpdateClient::CheckNewVersion(napi_env env, napi_callback_info info)
{
    versionInfo_.status = SYSTEM_ERROR;
    SessionParams sessionParams(SessionType::SESSION_CHECK_VERSION, CALLBACK_POSITION_ONE, false, true);
    napi_value ret = StartSession(env, info, sessionParams, [=](SessionType type, void *context) -> int {
        return UpdateServiceKits::GetInstance().CheckNewVersion(upgradeInfo_);
    });
    PARAM_CHECK(ret != nullptr, return nullptr, "Failed to start worker.");
    return ret;
}

napi_value UpdateClient::CancelUpgrade(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok && argc == 0, return nullptr, "Error get cb info");
    CLIENT_LOGI("CancelUpgrade");
    SessionParams sessionParams(SessionType::SESSION_CANCEL_UPGRADE, CALLBACK_POSITION_ONE, true);
    std::shared_ptr<UpdateSession> sess = nullptr;
    sess = std::make_shared<UpdateAsyncessionNoCallback>(this, sessionParams, argc);
    PARAM_CHECK_NAPI_CALL(env, sess != nullptr, return nullptr, "Failed to create update session");
    sessionsMgr_->AddSession(sess);
    napi_value retValue = sess->StartWork(
        env,
        args,
        [=](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
            return UpdateServiceKits::GetInstance().Cancel(upgradeInfo_, IUpdateService::DOWNLOAD, *businessError);
        },
        nullptr);
    PARAM_CHECK(retValue != nullptr, sessionsMgr_->RemoveSession(sess->GetSessionId());
        return nullptr, "Failed to start worker.");
    return retValue;
}

template <typename T>
ClientStatus UpdateClient::ParseUpgOptions(napi_env env, napi_callback_info info, VersionDigestInfo &versionDigestInfo,
    T &options)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return ClientStatus::CLIENT_INVALID_PARAM, "Error get cb info");

    ClientStatus ret = ClientHelper::GetVersionDigestInfoFromArg(env, args[0], versionDigestInfo);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return ClientStatus::CLIENT_INVALID_PARAM,
        "Failed to get versionDigestInfo param");

    ret = ClientHelper::GetOptionsFromArg(env, args[1], options);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return ClientStatus::CLIENT_INVALID_PARAM,
        "Failed to get Options param");
    return ClientStatus::CLIENT_SUCCESS;
}

napi_value UpdateClient::DownloadVersion(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");
    CLIENT_LOGI("DownloadVersion");

    ClientStatus ret = ParseUpgOptions(env, info, versionDigestInfo_, downloadOptions_);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Failed to get DownloadVersion param");

    SessionParams sessionParams(SessionType::SESSION_DOWNLOAD, CALLBACK_POSITION_THREE, true);
    napi_value retValue = StartSession(env, info, sessionParams, [=](SessionType type, void *context) -> int {
        BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
        return UpdateServiceKits::GetInstance().DownloadVersion(
            upgradeInfo_, versionDigestInfo_, downloadOptions_, *businessError);
    });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to start worker.");
    return retValue;
}

napi_value UpdateClient::PauseDownload(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");

    ClientStatus ret = ParseUpgOptions(env, info, versionDigestInfo_, pauseDownloadOptions_);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Failed to get pauseDownloadOptions param");

    SessionParams sessionParams(SessionType::SESSION_PAUSE_DOWNLOAD, CALLBACK_POSITION_THREE, true);
    napi_value retValue = StartSession(env, info, sessionParams, [=](SessionType type, void *context) -> int {
        BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
        return UpdateServiceKits::GetInstance().PauseDownload(
            upgradeInfo_, versionDigestInfo_, pauseDownloadOptions_, *businessError);
    });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to start worker.");
    return retValue;
}

napi_value UpdateClient::ResumeDownload(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");

    ClientStatus ret = ParseUpgOptions(env, info, versionDigestInfo_, resumeDownloadOptions_);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Failed to get resumeDownloadOptions param");

    SessionParams sessionParams(SessionType::SESSION_RESUME_DOWNLOAD, CALLBACK_POSITION_THREE, true);
    napi_value retValue = StartSession(env, info, sessionParams, [=](SessionType type, void *context) -> int {
        BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
        return UpdateServiceKits::GetInstance().ResumeDownload(
            upgradeInfo_, versionDigestInfo_, resumeDownloadOptions_, *businessError);
    });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to start worker.");
    return retValue;
}

napi_value UpdateClient::Upgrade(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");

    ClientStatus ret = ParseUpgOptions(env, info, versionDigestInfo_, upgradeOptions_);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Failed to get resumeDownloadOptions param");

    SessionParams sessionParams(SessionType::SESSION_UPGRADE, CALLBACK_POSITION_THREE, true);
    napi_value retValue = StartSession(env, info, sessionParams, [=](SessionType type, void *context) -> int {
#ifndef UPDATER_API_TEST
        BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
        return UpdateServiceKits::GetInstance().DoUpdate(
            upgradeInfo_, versionDigestInfo_, upgradeOptions_, *businessError);
#else
            return 0;
#endif
    });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to start worker.");
    return retValue;
}

napi_value UpdateClient::ClearError(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");

    ClientStatus ret = ParseUpgOptions(env, info, versionDigestInfo_, clearOptions_);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Failed to get clearOptions param");

    SessionParams sessionParams(SessionType::SESSION_CLEAR_ERROR, CALLBACK_POSITION_THREE, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError*>(context);
            return UpdateServiceKits::GetInstance().ClearError(
                upgradeInfo_, versionDigestInfo_, clearOptions_, *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to start worker.");
    return retValue;
}

napi_value UpdateClient::TerminateUpgrade(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");

    SessionParams sessionParams(SessionType::SESSION_TERMINATE_UPGRADE, CALLBACK_POSITION_ONE, true);
    napi_value retValue = StartSession(env, info, sessionParams, [=](SessionType type, void *context) -> int {
        BusinessError *businessError = reinterpret_cast<BusinessError *>(context);
        return UpdateServiceKits::GetInstance().TerminateUpgrade(upgradeInfo_, *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to start worker.");
    return retValue;
}

napi_value UpdateClient::SetUpdatePolicy(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");

    ClientStatus ret = ClientHelper::GetUpdatePolicyFromArg(env, args[0], updatePolicy_);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return nullptr, "Failed to get updatePolicy param");

    SessionParams sessionParams(SessionType::SESSION_SET_POLICY, CALLBACK_POSITION_TWO, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [&](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError*>(context);
            result_ = UpdateServiceKits::GetInstance().SetUpdatePolicy(upgradeInfo_, updatePolicy_, *businessError);
            return result_;
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to SetUpdatePolicy.");
    return retValue;
}

napi_value UpdateClient::GetUpdatePolicy(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_GET_POLICY, CALLBACK_POSITION_ONE, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError*>(context);
            return UpdateServiceKits::GetInstance().GetUpdatePolicy(upgradeInfo_, updatePolicy_, *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to UpgradeVersion.");
    return retValue;
}

napi_value UpdateClient::GetNewVersionInfo(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_GET_NEW_VERSION, CALLBACK_POSITION_ONE, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError*>(context);
            return UpdateServiceKits::GetInstance().GetNewVersion(upgradeInfo_, newVersionInfo_, *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to GetNewVersionInfo.");
    return retValue;
}

napi_value UpdateClient::GetCurrentVersionInfo(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_GET_CUR_VERSION, CALLBACK_POSITION_ONE, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError*>(context);
            return UpdateServiceKits::GetInstance().GetCurrentVersionInfo(upgradeInfo_, currentVersionInfo_,
                *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to GetCurrentVersionInfo.");
    return retValue;
}

napi_value UpdateClient::GetTaskInfo(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_GET_TASK_INFO, CALLBACK_POSITION_ONE, true);
    napi_value retValue = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError*>(context);
            return UpdateServiceKits::GetInstance().GetTaskInfo(upgradeInfo_, taskInfo_, *businessError);
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to GetTaskInfo.");
    return retValue;
}

napi_value UpdateClient::GetOtaStatus(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_GET_OTA_STATUS, CALLBACK_POSITION_ONE, true);
    napi_value ret = StartSession(env, info, sessionParams,
        [=](SessionType type, void *context) -> int {
            BusinessError *businessError = reinterpret_cast<BusinessError*>(context);
            return UpdateServiceKits::GetInstance().GetOtaStatus(upgradeInfo_, otaStatus_, *businessError);
        });
    PARAM_CHECK(ret != nullptr, return nullptr, "Failed to GetOtaStatus.");
    return ret;
}

napi_value UpdateClient::ApplyNewVersion(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_APPLY_NEW_VERSION, CALLBACK_POSITION_ONE);
    napi_value retValue = StartSession(env, info, sessionParams, [&](SessionType type, void *context) -> int {
#ifndef UPDATER_API_TEST
        result_ = UpdateServiceKits::GetInstance().RebootAndInstall(MISC_FILE, UPDATER_PKG_NAME);
#endif
        return result_;
    });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to GetNewVersionInfo.");
    return retValue;
}

napi_value UpdateClient::RebootAndClean(napi_env env, napi_callback_info info)
{
    SessionParams sessionParams(SessionType::SESSION_REBOOT_AND_CLEAN, CALLBACK_POSITION_ONE);
    napi_value retValue = StartSession(env, info, sessionParams,
        [&](SessionType type, void *context) -> int {
#ifndef UPDATER_API_TEST
            result_ = UpdateServiceKits::GetInstance().RebootAndClean(MISC_FILE, CMD_WIPE_DATA);
#endif
            return result_;
        });
    PARAM_CHECK(retValue != nullptr, return nullptr, "Failed to GetNewVersionInfo.");
    return retValue;
}

napi_value UpdateClient::VerifyUpdatePackage(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "Error get cb info");
    PARAM_CHECK_NAPI_CALL(env, argc >= ARG_NUM_TWO, return nullptr, "Error get cb info");

    int ret = NapiUtil::GetString(env, args[0], upgradeFile_);
    PARAM_CHECK_NAPI_CALL(env, ret == napi_ok, return nullptr, "Error get upgradeType");
    ret = NapiUtil::GetString(env, args[1], certsFile_);
    PARAM_CHECK_NAPI_CALL(env, ret == napi_ok, return nullptr, "Error get certsFile");

    CLIENT_LOGI("VerifyUpdatePackage");
    SessionParams sessionParams(SessionType::SESSION_VERIFY_PACKAGE, CALLBACK_POSITION_THREE);
    std::shared_ptr<UpdateSession> sess = nullptr;
    sess = std::make_shared<UpdateAsyncessionNoCallback>(this, sessionParams, argc);
    PARAM_CHECK_NAPI_CALL(env, sess != nullptr, return nullptr, "Fail to create update session");
    sessionsMgr_->AddSession(sess);
    napi_value retValue = sess->StartWork(env, args,
        [&](SessionType type, void *context) -> int {
            CLIENT_LOGI("StartWork VerifyUpdatePackage");
            result_ = VerifyPackageWithCallback(upgradeFile_, certsFile_,
                [&](int32_t result, uint32_t percent) { NotifyVerifyProgresss(result, percent); });
            return result_;
        },
        nullptr);
    PARAM_CHECK(retValue != nullptr, sessionsMgr_->RemoveSession(sess->GetSessionId()); return nullptr,
        "Failed to start worker.");
    return retValue;
}

void UpdateClient::NotifyVerifyProgresss(int32_t retCode, uint32_t percent)
{
    verifyProgress_.status = (retCode == 0) ? UPDATE_STATE_VERIFY_SUCCESS : UPDATE_STATE_VERIFY_FAIL;
    verifyProgress_.percent = percent;

    UpdateResult result;
    result.type = SessionType::SESSION_VERIFY_PACKAGE;
    result.result.progress = &verifyProgress_;
    result.buildJSObject = ClientHelper::BuildProgress;
    result.businessError.errorNum = CallResult::SUCCESS;
    sessionsMgr_->Emit("verifyProgress", result);
}

void UpdateClient::NotifyCheckVersionDone(const BusinessError &businessError, const CheckResultEx &checkResultEx)
{
    CLIENT_LOGI("NotifyCheckVersionDone businessError %{public}d", static_cast<int32_t> (businessError.errorNum));
    CLIENT_LOGI("NotifyCheckVersionDone isExistNewVersion %{public}d", checkResultEx.isExistNewVersion);
    checkResultEx_ = checkResultEx;
}

int32_t UpdateClient::GetUpdateResult(SessionType type, UpdateResult &result)
{
    CLIENT_LOGI("GetUpdateResult type %{public}d", type);
    result.type = type;
    switch (type) {
        case SessionType::SESSION_CHECK_VERSION: {
            result.result.checkResultEx = &checkResultEx_;
            result.buildJSObject = ClientHelper::BuildCheckResultEx;
            break;
        }
        case SessionType::SESSION_GET_NEW_VERSION: {
            result.result.newVersionInfo = &newVersionInfo_;
            result.buildJSObject = ClientHelper::BuildNewVersionInfo;
            break;
        }
        case SessionType::SESSION_GET_TASK_INFO: {
            result.result.taskInfo = &taskInfo_;
            result.buildJSObject = ClientHelper::BuildTaskInfo;
            break;
        }
        case SessionType::SESSION_GET_CUR_VERSION: {
            result.result.currentVersionInfo = &currentVersionInfo_;
            result.buildJSObject = ClientHelper::BuildCurrentVersionInfo;
            break;
        }
        case SessionType::SESSION_DOWNLOAD: {
            result.result.progress = &progress_;
            result.buildJSObject = ClientHelper::BuildProgress;
            break;
        }
        case SessionType::SESSION_UPGRADE: {
            result.result.progress = &progress_;
            result.buildJSObject = ClientHelper::BuildProgress;
            break;
        }
        case SessionType::SESSION_VERIFY_PACKAGE: {
            result.result.progress = &verifyProgress_;
            result.buildJSObject = ClientHelper::BuildProgress;
            break;
        }
        case SessionType::SESSION_GET_POLICY: {
            result.result.updatePolicy = &updatePolicy_;
            result.buildJSObject = ClientHelper::BuildUpdatePolicy;
            break;
        }
        case SessionType::SESSION_GET_OTA_STATUS: {
            result.result.otaStatus = &otaStatus_;
            result.buildJSObject = ClientHelper::BuildOtaStatus;
            break;
        }
        case SessionType::SESSION_SET_POLICY: {
            result.result.status = result_;
            result.buildJSObject = ClientHelper::BuildInt32Status;
            break;
        }
        default: {
            result.result.status = result_;
            result.buildJSObject = ClientHelper::BuildVoidStatus;
            break;
        }
    }
    return napi_ok;
}
} // namespace UpdateEngine
} // namespace OHOS