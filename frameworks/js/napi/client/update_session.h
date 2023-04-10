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

#ifndef UPDATE_SESSION_H
#define UPDATE_SESSION_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "iupdater.h"
#include "iupdate_service.h"
#include "iupdate_session.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateSession : public IUpdateSession {
public:
    UpdateSession(IUpdater *client, SessionParams &sessionParams, size_t argc, size_t callbackNumber);

    virtual ~UpdateSession() {}

    napi_value StartWork(napi_env env, const napi_value *args, DoWorkFunction worker, void *context) override;

    IUpdater* GetUpdateClient() const
    {
        return client_;
    }

    SessionType GetType() const override
    {
        return sessionParams_.type;
    }

    uint32_t GetSessionId() const override
    {
        return sessionId;
    }

    void OnAsyncComplete(const BusinessError &businessError) final
    {
        std::lock_guard<std::mutex> lock(conditionVariableMutex_);
        businessError_ = businessError;
        asyncExecuteComplete_ = true;
        conditionVariable_.notify_all();
    }

    virtual void CompleteWork(napi_env env, napi_status status) {}
    virtual void ExecuteWork(napi_env env);
    virtual napi_value StartWork(napi_env env, size_t startIndex, const napi_value *args) = 0;

    static void CompleteWork(napi_env env, napi_status status, void *data);

    static void ExecuteWork(napi_env env, void *data);

    bool IsAsyncCompleteWork() override
    {
        return sessionParams_.type == SessionType::SESSION_CHECK_VERSION;
    }

protected:
    napi_value CreateWorkerName(napi_env env) const;

    void GetUpdateResult(UpdateResult &result)
    {
        result.businessError = businessError_;
        client_->GetUpdateResult(sessionParams_.type, result);
    }

    bool IsWorkExecuteSuccess() const
    {
        return workResult_ == INT_CALL_SUCCESS;
    }

    void GetSessionFuncParameter(std::string &funcName, std::string &permissionName)
    {
        struct UpdateFuncMap {
            SessionType type;
            std::string func;
        } sessionFuncMap[] = {
            { SessionType::SESSION_CHECK_VERSION,               "checkNewVersion" },
            { SessionType::SESSION_DOWNLOAD,                    "download" },
            { SessionType::SESSION_PAUSE_DOWNLOAD,              "pauseDownload" },
            { SessionType::SESSION_RESUME_DOWNLOAD,             "resumeDownload" },
            { SessionType::SESSION_UPGRADE,                     "upgrade" },
            { SessionType::SESSION_SET_POLICY,                  "setUpgradePolicy" },
            { SessionType::SESSION_GET_POLICY,                  "getUpgradePolicy" },
            { SessionType::SESSION_CLEAR_ERROR,                 "clearError" },
            { SessionType::SESSION_TERMINATE_UPGRADE,           "terminateUpgrade" },
            { SessionType::SESSION_GET_NEW_VERSION,             "getNewVersionInfo" },
            { SessionType::SESSION_GET_NEW_VERSION_DESCRIPTION, "getNewVersionDescription" },
            { SessionType::SESSION_SUBSCRIBE,                   "subscribe" },
            { SessionType::SESSION_UNSUBSCRIBE,                 "unsubscribe" },
            { SessionType::SESSION_GET_UPDATER,                 "getUpdater" },
            { SessionType::SESSION_APPLY_NEW_VERSION,           "applyNewVersion" },
            { SessionType::SESSION_FACTORY_RESET,               "factoryReset" },
            { SessionType::SESSION_VERIFY_PACKAGE,              "verifyPackage" },
            { SessionType::SESSION_CANCEL_UPGRADE,              "cancel" },
            { SessionType::SESSION_GET_CUR_VERSION,             "getCurrentVersionInfo" },
            { SessionType::SESSION_GET_CUR_VERSION_DESCRIPTION, "getCurrentVersionDescription" },
            { SessionType::SESSION_GET_TASK_INFO,               "getTaskInfo" },
            { SessionType::SESSION_REPLY_PARAM_ERROR,           "replyParamError" },
            { SessionType::SESSION_MAX,                         "max" }
        };
        for (auto &[type, func] : sessionFuncMap) {
            if (type == sessionParams_.type) {
                funcName = func;
                break;
            }
        }
        if (sessionParams_.type == SessionType::SESSION_FACTORY_RESET) {
            permissionName = "ohos.permission.FACTORY_RESET";
        } else {
            permissionName = "ohos.permission.UPDATE_SYSTEM";
        }
    }

    void BuildWorkBusinessErr(BusinessError &businessError)
    {
        std::string msg = "execute error";
        std::string funcName = "";
        std::string permissionName = "";
        switch (workResult_) {
            case INT_NOT_SYSTEM_APP:
                msg = "BusinessError " + std::to_string(workResult_) + ": Caller not system app.";
                break;
            case INT_APP_NOT_GRANTED:
                GetSessionFuncParameter(funcName, permissionName);
                msg = "BusinessError " + std::to_string(workResult_) + ": Permission denied. An attempt was made to " +
                    funcName + " forbidden by permission: " + permissionName + ".";
                break;
            case INT_CALL_IPC_ERR:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": IPC error.";
                break;
            case INT_UN_SUPPORT:
                GetSessionFuncParameter(funcName, permissionName);
                msg = "BusinessError " + std::to_string(workResult_) + ": Capability not supported. " +
                    "function " + funcName + " can not work correctly due to limited device capabilities.";
                break;
            case INT_PARAM_ERR:
                msg = "param error";
                break;
            case INT_CALL_FAIL:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": Execute fail.";
                break;
            case INT_FORBIDDEN:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": Forbidden execution.";
                break;
            case INT_DEV_UPG_INFO_ERR:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": Device info error.";
                break;
            case INT_TIME_OUT:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": Execute timeout.";
                break;
            case INT_DB_ERROR:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": DB error.";
                break;
            case INT_IO_ERROR:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": IO error.";
                break;
            case INT_NET_ERROR:
                msg = "BusinessError " + std::to_string(COMPONENT_ERR + workResult_) + ": Network error.";
                break;
            default:
                break;
        }
        businessError.Build(static_cast<CallResult>(workResult_), msg);
    }

    void GetBusinessError(BusinessError &businessError, const UpdateResult &result)
    {
        if (IsWorkExecuteSuccess()) {
            businessError = result.businessError;
        } else {
            BuildWorkBusinessErr(businessError);
        }
    }

    uint32_t sessionId {0};
    IUpdater *client_ = nullptr;
    BusinessError businessError_ {};
    SessionParams sessionParams_ {};
    int32_t workResult_ = INT_CALL_SUCCESS;
    size_t totalArgc_ = 0;
    size_t callbackNumber_ = 0;
    void* context_ {};
    DoWorkFunction doWorker_ {};
    std::condition_variable conditionVariable_;
    std::mutex conditionVariableMutex_;
    bool asyncExecuteComplete_ = false;
};

class UpdateAsyncession : public UpdateSession {
public:
    UpdateAsyncession(IUpdater *client, SessionParams &sessionParams, size_t argc, size_t callbackNumber = 1)
        : UpdateSession(client, sessionParams, argc, callbackNumber)
    {
        callbackRef_.resize(callbackNumber);
    }

    ~UpdateAsyncession() override
    {
        callbackRef_.clear();
    }

    void CompleteWork(napi_env env, napi_status status) override;
    napi_value StartWork(napi_env env, size_t startIndex, const napi_value *args) override;
    void NotifyJS(napi_env env, napi_value thisVar, const UpdateResult &result) override;

private:
    napi_async_work worker_ = nullptr;
    std::vector<napi_ref> callbackRef_ = {0};
};

class UpdateAsyncessionNoCallback : public UpdateAsyncession {
public:
    UpdateAsyncessionNoCallback(
        IUpdater *client, SessionParams &sessionParams, size_t argc, size_t callbackNumber = 0)
        : UpdateAsyncession(client, sessionParams, argc, callbackNumber) {}

    ~UpdateAsyncessionNoCallback() override {}

    void CompleteWork(napi_env env, napi_status status) override;
};

class UpdatePromiseSession : public UpdateSession {
public:
    UpdatePromiseSession(IUpdater *client, SessionParams &sessionParams, size_t argc, size_t callbackNumber = 0)
        : UpdateSession(client, sessionParams, argc, callbackNumber) {}

    ~UpdatePromiseSession() override {}

    void CompleteWork(napi_env env, napi_status status) override;
    napi_value StartWork(napi_env env, size_t startIndex, const napi_value *args) override;
    void NotifyJS(napi_env env, napi_value thisVar, const UpdateResult &result) override;

private:
    napi_async_work worker_ = nullptr;
    napi_deferred deferred_ = nullptr;
};

class UpdateListener : public UpdateSession {
public:
    UpdateListener(
        IUpdater *client, SessionParams &sessionParams, size_t argc, bool isOnce, size_t callbackNumber = 1)
        : UpdateSession(client, sessionParams, argc, callbackNumber), isOnce_(isOnce) {}

    ~UpdateListener() override {}

    napi_value StartWork(napi_env env, size_t startIndex, const napi_value *args) override;

    void NotifyJS(napi_env env, napi_value thisVar, const UpdateResult &result) override;

    void NotifyJS(napi_env env, napi_value thisVar, const EventInfo &eventInfo);

    bool IsOnce() const
    {
        return isOnce_;
    }

    std::string GetEventType() const
    {
        return eventType_;
    }

    bool CheckEqual(napi_env env, napi_value handler, const std::string &type);

    bool IsSubscribeEvent(const EventClassifyInfo &eventClassifyInfo) const
    {
        return eventClassifyInfo_.eventClassify == eventClassifyInfo.eventClassify;
    }

    bool IsSameListener(napi_env env, const EventClassifyInfo &eventClassifyInfo, napi_value handler);

    void RemoveHandlerRef(napi_env env);
private:
    bool isOnce_ = false;
    std::string eventType_;
    napi_ref handlerRef_ = nullptr;
    std::mutex mutex_;
    EventClassifyInfo eventClassifyInfo_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SESSION_H
