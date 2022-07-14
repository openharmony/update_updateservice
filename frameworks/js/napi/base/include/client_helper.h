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

#ifndef UPDATE_CLIENT_HELPER_H
#define UPDATE_CLIENT_HELPER_H

#include <string>
#include "node_api.h"
#include "update_define.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
// Update status
enum class ClientStatus {
    CLIENT_SUCCESS = 0,
    CLIENT_INVALID_PARAM = 1000,
    CLIENT_INVALID_TYPE,
    CLIENT_REPEAT_REQ,
    CLIENT_FAIL,
    CLIENT_CHECK_NEW_FIRST,
};

enum class SessionType {
    SESSION_CHECK_VERSION = 0,
    SESSION_DOWNLOAD,
    SESSION_PAUSE_DOWNLOAD,
    SESSION_RESUME_DOWNLOAD,
    SESSION_UPGRADE,
    SESSION_SET_POLICY,
    SESSION_GET_POLICY,
    SESSION_CLEAR_ERROR,
    SESSION_TERMINATE_UPGRADE,
    SESSION_GET_NEW_VERSION,
    SESSION_SUBSCRIBE,
    SESSION_UNSUBSCRIBE,
    SESSION_GET_UPDATER,
    SESSION_APPLY_NEW_VERSION,
    SESSION_REBOOT_AND_CLEAN,
    SESSION_VERIFY_PACKAGE,
    SESSION_CANCEL_UPGRADE,
    SESSION_GET_OTA_STATUS,
    SESSION_GET_CUR_VERSION,
    SESSION_GET_TASK_INFO,
    SESSION_MAX
};

struct SessionParams {
    SessionType type;
    size_t callbackStartIndex;
    bool isNeedBusinessError;
    bool isAsyncCompleteWork;

    SessionParams(SessionType typeValue = SessionType::SESSION_MAX, size_t callbackPosition = 1,
        bool isNeedBusinessErrorValue = false, bool isAsyncCompleteWorkValue = false)
        : type(typeValue), callbackStartIndex(INDEX(callbackPosition)), isNeedBusinessError(isNeedBusinessErrorValue),
        isAsyncCompleteWork(isAsyncCompleteWorkValue) {}
};

struct UpdateResult {
    using BuildJSObject = std::function<int(napi_env env, napi_value &obj, const UpdateResult &result)>;
    SessionType type;
    BusinessError businessError;
    union {
        UpdatePolicy *updatePolicy;
        Progress *progress;
        OtaStatus *otaStatus;
        NewVersionInfo *newVersionInfo;
        CheckResultEx *checkResultEx;
        CurrentVersionInfo *currentVersionInfo;
        TaskInfo *taskInfo;
        int32_t status;
    } result;

    BuildJSObject buildJSObject;

    void Release()
    {
        CLIENT_LOGI("UpdateResult Release");
        if (type == SessionType::SESSION_DOWNLOAD || type == SessionType::SESSION_UPGRADE) {
            delete result.progress;
            result.progress = nullptr;
        } else if (type == SessionType::SESSION_CHECK_VERSION) {
            delete result.checkResultEx;
            result.checkResultEx = nullptr;
        } else if (type == SessionType::SESSION_GET_NEW_VERSION) {
            delete result.newVersionInfo;
            result.newVersionInfo = nullptr;
        } else if (type == SessionType::SESSION_GET_CUR_VERSION) {
            delete result.currentVersionInfo;
            result.currentVersionInfo = nullptr;
        } else if (type == SessionType::SESSION_GET_POLICY) {
            delete result.updatePolicy;
            result.updatePolicy = nullptr;
        } else {
            CLIENT_LOGI("UpdateResult Release, unknow type");
        }
        CLIENT_LOGI("UpdateResult Release finish");
    }

    UpdateResult &operator=(const UpdateResult &updateResult)
    {
        if (&updateResult == this) {
            return *this;
        }

        buildJSObject = updateResult.buildJSObject;
        type = updateResult.type;
        businessError = updateResult.businessError;

        CLIENT_LOGI("UpdateResult operator type %{public}d", updateResult.type);
        if (type == SessionType::SESSION_DOWNLOAD || type == SessionType::SESSION_UPGRADE) {
            if (result.progress == nullptr) {
                result.progress = new (std::nothrow) Progress();
            }
            if ((result.progress != nullptr) && (updateResult.result.progress != nullptr)) {
                *(result.progress) = *(updateResult.result.progress);
            }
        } else if (type == SessionType::SESSION_CHECK_VERSION) {
            if (result.checkResultEx == nullptr) {
                result.checkResultEx = new (std::nothrow) CheckResultEx();
            }
            if ((result.checkResultEx != nullptr) && (updateResult.result.checkResultEx != nullptr)) {
                *(result.checkResultEx) = *(updateResult.result.checkResultEx);
            }
        } else if (type == SessionType::SESSION_GET_NEW_VERSION) {
            if (result.newVersionInfo == nullptr) {
                result.newVersionInfo = new (std::nothrow) NewVersionInfo();
            }
            if ((result.newVersionInfo != nullptr) && (updateResult.result.newVersionInfo != nullptr)) {
                *(result.newVersionInfo) = *(updateResult.result.newVersionInfo);
            }
        } else if (type == SessionType::SESSION_GET_CUR_VERSION) {
            if (result.currentVersionInfo == nullptr) {
                result.currentVersionInfo = new (std::nothrow) CurrentVersionInfo();
            }
            if ((result.currentVersionInfo != nullptr) && (updateResult.result.currentVersionInfo != nullptr)) {
                *(result.currentVersionInfo) = *(updateResult.result.currentVersionInfo);
            }
        } else if (type == SessionType::SESSION_GET_POLICY) {
            if (result.updatePolicy == nullptr) {
                result.updatePolicy = new (std::nothrow) UpdatePolicy();
            }
            if ((result.updatePolicy != nullptr) && (updateResult.result.updatePolicy != nullptr)) {
                *(result.updatePolicy) = *(updateResult.result.updatePolicy);
            }
        } else if (type == SessionType::SESSION_VERIFY_PACKAGE) {
            result.status = updateResult.result.status;
        } else {
            CLIENT_LOGI("UpdateResult unknow type");
        }
        return *this;
    }
};

class ClientHelper {
public:
    static bool CheckUpgradeFile(const std::string &upgradeFile);
    static bool CheckUpgradeType(const std::string &type);

    static int32_t BuildCheckResultEx(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildNewVersionInfo(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildProgress(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildUpdatePolicy(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildCurrentVersionInfo(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildTaskInfo(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildInt32Status(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildVoidStatus(napi_env env, napi_value &obj, const UpdateResult &result);
    static int32_t BuildOtaStatus(napi_env env, napi_value &obj, const UpdateResult &result);

    static ClientStatus GetUpgradeInfoFromArg(napi_env env, const napi_value arg, UpgradeInfo &upgradeInfo);
    static ClientStatus GetUpdatePolicyFromArg(napi_env env, const napi_value arg, UpdatePolicy &updatePolicy);
    static ClientStatus GetUpgradeFileFromArg(napi_env env, const napi_value arg, UpgradeFile &upgradeFile);
    static ClientStatus GetUpgradeFilesFromArg(napi_env env, const napi_value arg,
        std::vector<UpgradeFile> &upgradeFiles);
    static ClientStatus GetVersionDigestInfoFromArg(napi_env env, const napi_value arg,
        VersionDigestInfo &versionDigestInfo);

    static ClientStatus GetOptionsFromArg(napi_env env, const napi_value arg, UpgradeOptions &upgradeOptions);
    static ClientStatus GetOptionsFromArg(napi_env env, const napi_value arg, ClearOptions &clearOptions);
    static ClientStatus GetOptionsFromArg(napi_env env, const napi_value arg, DownloadOptions &downloadOptions);
    static ClientStatus GetOptionsFromArg(napi_env env, const napi_value arg,
        PauseDownloadOptions &pauseDownloadOptions);
    static ClientStatus GetOptionsFromArg(napi_env env, const napi_value arg,
        ResumeDownloadOptions &resumeDownloadOptions);
    static ClientStatus GetEventClassifyInfoFromArg(napi_env env, const napi_value arg,
        EventClassifyInfo &eventClassifyInfo);
    static int32_t BuildBusinessError(napi_env env, napi_value &obj, const BusinessError &businessError);
    static ClientStatus BuildEventInfo(napi_env env, napi_value &obj, const EventInfo &eventInfo);

private:
    static ClientStatus GetNetType(napi_env env, const napi_value arg, NetType &netType);
    static ClientStatus GetOrder(napi_env env, const napi_value arg, Order &order);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_CLIENT_HELPER_H