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

    virtual void CompleteWork(napi_env env, napi_status status) {}
    virtual void ExecuteWork(napi_env env);
    virtual napi_value StartWork(napi_env env, size_t startIndex, const napi_value *args) = 0;

    static void CompleteWork(napi_env env, napi_status status, void *data);

    static void ExecuteWork(napi_env env, void *data);

    bool IsAsyncCompleteWork() override
    {
        return sessionParams_.type == SessionType::SESSION_VERIFY_PACKAGE;
    }

    void GetUpdateResult(UpdateResult &result)
    {
        result.businessError = businessError_;
        client_->GetUpdateResult(sessionParams_.type, result);
    }

protected:
    napi_value CreateWorkerName(napi_env env) const;

protected:
    uint32_t sessionId {0};
    IUpdater *client_ = nullptr;
    BusinessError businessError_ {};
    SessionParams sessionParams_ {};
    size_t totalArgc_ = 0;
    size_t callbackNumber_ = 0;
    void* context_ {};
    DoWorkFunction doWorker_ {};
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

    bool IsSubscribeEvent(const EventClassifyInfo &eventClassifyInfo)
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
