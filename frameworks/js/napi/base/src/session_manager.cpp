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

#include <uv.h>

#include "client_helper.h"
#include "node_api.h"
#include "update_helper.h"
#include "update_session.h"
#include "session_manager.h"

using namespace std;

namespace OHOS {
namespace UpdateEngine {
struct NotifyInput {
    NotifyInput() = default;
    NotifyInput(SessionManager *sesssionManager, const string &itype, UpdateResult *iresult)
        : sesssionMgr(sesssionManager), type(itype), result(iresult) {}
    SessionManager *sesssionMgr = nullptr;
    string type;
    UpdateResult *result = nullptr;
};

SessionManager::SessionManager(napi_env env, napi_ref thisReference) : env_(env), thisReference_(thisReference) {}

SessionManager::~SessionManager()
{
    sessions_.clear();
    if (thisReference_ != nullptr) {
        napi_delete_reference(env_, thisReference_);
    }
}

void SessionManager::AddSession(std::shared_ptr<IUpdateSession> session)
{
    PARAM_CHECK(session != nullptr, return, "Invalid param");
#ifndef UPDATER_API_TEST
    std::lock_guard<std::mutex> guard(sessionMutex_);
#endif
    sessions_.insert(make_pair(session->GetSessionId(), session));
}

void SessionManager::RemoveSession(uint32_t sessionId)
{
    CLIENT_LOGI("RemoveSession sess");
#ifndef UPDATER_API_TEST
    std::lock_guard<std::mutex> guard(sessionMutex_);
#endif
    IUpdateSession *sess = nullptr;
    auto iter = sessions_.find(sessionId);
    if (iter != sessions_.end()) {
        sess = iter->second.get();
        sessions_.erase(iter);
    }
}

void SessionManager::Clear()
{
    sessions_.clear();
    if (thisReference_ != nullptr) {
        napi_delete_reference(env_, thisReference_);
    }
}

bool SessionManager::GetFirstSessionId(uint32_t &sessionId)
{
#ifndef UPDATER_API_TEST
    std::lock_guard<std::mutex> guard(sessionMutex_);
#endif
    {
        if (sessions_.empty()) {
            return false;
        }
        sessionId = sessions_.begin()->second->GetSessionId();
        return true;
    }
}

bool SessionManager::GetNextSessionId(uint32_t &sessionId)
{
#ifndef UPDATER_API_TEST
    std::lock_guard<std::mutex> guard(sessionMutex_);
#endif
    {
        auto iter = sessions_.find(sessionId);
        if (iter == sessions_.end()) {
            return false;
        }
        iter++;
        if (iter == sessions_.end()) {
            return false;
        }
        sessionId = iter->second->GetSessionId();
    }
    return true;
}

int32_t SessionManager::ProcessUnsubscribe(const std::string &eventType, size_t argc, napi_value arg)
{
    napi_handle_scope scope;
    napi_status status = napi_open_handle_scope(env_, &scope);
    PARAM_CHECK(status == napi_ok, return -1, "Error open handle");

    uint32_t nextSessId = 0;
    bool hasNext = GetFirstSessionId(nextSessId);
    while (hasNext) {
        uint32_t currSessId = nextSessId;
        auto iter = sessions_.find(currSessId);
        if (iter == sessions_.end()) {
            break;
        }
        hasNext = GetNextSessionId(nextSessId);

        UpdateListener *listener = static_cast<UpdateListener *>(iter->second.get());
        if (listener->GetType() != SessionType::SESSION_SUBSCRIBE ||
            eventType.compare(listener->GetEventType()) != 0) {
            continue;
        }
        CLIENT_LOGI("ProcessUnsubscribe remove session");
        if (argc == 1) {
            listener->RemoveHandlerRef(env_);
            RemoveSession(currSessId);
        } else if (listener->CheckEqual(env_, arg, eventType)) {
            listener->RemoveHandlerRef(env_);
            RemoveSession(currSessId);
            break;
        }
    }
    napi_close_handle_scope(env_, scope);
    return 0;
}

void SessionManager::Unsubscribe(const EventClassifyInfo &eventClassifyInfo, napi_value handle)
{
    std::lock_guard<std::mutex> guard(sessionMutex_);
    for (auto iter = sessions_.begin(); iter != sessions_.end();) {
        if (iter->second == nullptr) {
            iter = sessions_.erase(iter); // erase nullptr
            continue;
        }

        if (iter->second->GetType() != SessionType::SESSION_SUBSCRIBE) {
            ++iter;
            continue;
        }

        auto listener = static_cast<UpdateListener *>(iter->second.get());
        if (handle == nullptr && listener->IsSubscribeEvent(eventClassifyInfo)) {
            CLIENT_LOGI("Unsubscribe, remove session %{public}d without handle", listener->GetSessionId());
            iter = sessions_.erase(iter);
            continue;
        }

        if (listener->IsSameListener(env_, eventClassifyInfo, handle)) {
            CLIENT_LOGI("Unsubscribe, remove session %{public}d", listener->GetSessionId());
            iter = sessions_.erase(iter);
            continue;
        }

        ++iter;
    }
}

IUpdateSession *SessionManager::FindSessionByHandle(napi_env env, const std::string &eventType, napi_value arg)
{
    uint32_t nextSessId = 0;
    bool hasNext = GetFirstSessionId(nextSessId);
    while (hasNext) {
        uint32_t currSessId = nextSessId;
        auto iter = sessions_.find(currSessId);
        if (iter == sessions_.end()) {
            break;
        }
        hasNext = GetNextSessionId(nextSessId);

        UpdateListener *listener = static_cast<UpdateListener *>(iter->second.get());
        if (listener->GetType() != SessionType::SESSION_SUBSCRIBE) {
            continue;
        }
        if ((eventType.compare(listener->GetEventType()) == 0) && listener->CheckEqual(env_, arg, eventType)) {
            return listener;
        }
    }
    return nullptr;
}

IUpdateSession *SessionManager::FindSessionByHandle(napi_env env, const EventClassifyInfo &eventClassifyInfo,
    napi_value arg)
{
    std::lock_guard<std::mutex> guard(sessionMutex_);
    for (auto &iter : sessions_) {
        if (iter.second == nullptr) {
            continue;
        }

        if (iter.second->GetType() != SessionType::SESSION_SUBSCRIBE) {
            continue;
        }

        auto listener = static_cast<UpdateListener *>(iter.second.get());
        if (listener->IsSameListener(env, eventClassifyInfo, arg)) {
            return listener;
        }
    }
    return nullptr;
}

void SessionManager::PublishToJS(const std::string &type, const UpdateResult &result)
{
    CLIENT_LOGI("PublishToJS %{public}s", type.c_str());
    napi_handle_scope scope;
    napi_status status = napi_open_handle_scope(env_, &scope);
    PARAM_CHECK_NAPI_CALL(env_, status == napi_ok, return, "Error open_handle_scope");
    napi_value thisVar = nullptr;
    status = napi_get_reference_value(env_, thisReference_, &thisVar);
    PARAM_CHECK_NAPI_CALL(env_, status == napi_ok, napi_close_handle_scope(env_, scope); return,
        "Error get_reference");

    uint32_t nextSessId = 0;
    bool hasNext = GetFirstSessionId(nextSessId);
    while (hasNext) {
        uint32_t currSessId = nextSessId;
        auto iter = sessions_.find(currSessId);
        if (iter == sessions_.end()) {
            break;
        }
        hasNext = GetNextSessionId(nextSessId);
        if (iter->second == nullptr) {
            CLIENT_LOGE("PublishToJS error, updateSession is null, %{public}d", iter->first);
            continue;
        }
        IUpdateSession *updateSession = (iter->second).get();
        CLIENT_LOGI("PublishToJS GetType %{public}d", updateSession->GetType());
        if (updateSession->GetType() == SessionType::SESSION_SUBSCRIBE) {
            UpdateListener *listener = static_cast<UpdateListener *>(updateSession);
            if (type.compare(listener->GetEventType()) != 0) {
                continue;
            }

            listener->NotifyJS(env_, thisVar, result);
            if (listener->IsOnce()) {
                listener->RemoveHandlerRef(env_);
                RemoveSession(currSessId);
            }
        } else if (updateSession->IsAsyncCompleteWork()) {
            updateSession->NotifyJS(env_, thisVar, result);
            RemoveSession(currSessId);
        } else {
            CLIENT_LOGI("PublishToJS GetType unknown type");
        }
    }
    napi_close_handle_scope(env_, scope);
}

void SessionManager::PublishToJS(const EventClassifyInfo &eventClassifyInfo, const EventInfo &eventInfo)
{
    napi_handle_scope scope;
    napi_status status = napi_open_handle_scope(env_, &scope);
    PARAM_CHECK_NAPI_CALL(env_, status == napi_ok, return, "Error open_handle_scope");
    napi_value thisVar = nullptr;
    status = napi_get_reference_value(env_, thisReference_, &thisVar);
    PARAM_CHECK_NAPI_CALL(env_, status == napi_ok, napi_close_handle_scope(env_, scope); return,
        "Error get_reference");

    std::lock_guard<std::mutex> guard(sessionMutex_);
    for (auto &iter : sessions_) {
        if (iter.second == nullptr) {
            continue;
        }

        if (iter.second->GetType() != SessionType::SESSION_SUBSCRIBE) {
            continue;
        }

        UpdateListener *listener = static_cast<UpdateListener *>(iter.second.get());
        if (!listener->IsSubscribeEvent(eventClassifyInfo)) {
            continue;
        }

        listener->NotifyJS(env_, thisVar, eventInfo);
    }
    napi_close_handle_scope(env_, scope);
}

void SessionManager::Emit(const std::string &type, const UpdateResult &result)
{
    auto freeUpdateResult = [](UpdateResult *lres) {
        if (lres != nullptr) {
            lres->Release();
        }
        delete lres;
    };
    CLIENT_LOGI("SessionManager::Emit %{public}s", type.c_str());

    UpdateResult *res = new (std::nothrow) UpdateResult();
    if (res == nullptr) {
        return;
    }
    *res = result;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    PARAM_CHECK(loop != nullptr, freeUpdateResult(res); return, "get event loop failed.");

    uv_work_t *work = new (std::nothrow) uv_work_t;
    PARAM_CHECK(work != nullptr, freeUpdateResult(res); return, "alloc work failed.");

    work->data = (void *)new (std::nothrow) NotifyInput(this, type, res);
    PARAM_CHECK(work->data != nullptr, freeUpdateResult(res); delete work; return, "alloc work data failed.");

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {}, // run in C++ thread
        [](uv_work_t *work, int status) {
            NotifyInput *input = (NotifyInput *)work->data;
            input->sesssionMgr->PublishToJS(input->type, *input->result);
            input->result->Release();
            delete input->result;
            delete input;
            delete work;
        });
}

void SessionManager::Emit(const EventClassifyInfo &eventClassifyInfo, const EventInfo &eventInfo)
{
    CLIENT_LOGI("SessionManager::Emit 0x%{public}x", CAST_INT(eventClassifyInfo.eventClassify));
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    PARAM_CHECK(loop != nullptr, return, "get event loop failed.");

    using UvWorkData = std::tuple<SessionManager*, EventClassifyInfo, EventInfo>;
    UvWorkData* data = new (std::nothrow) std::tuple(this, eventClassifyInfo, eventInfo);
    PARAM_CHECK(data != nullptr, return, "alloc data failed.");

    uv_work_t *work = new (std::nothrow) uv_work_t;
    PARAM_CHECK(work != nullptr, delete data; return, "alloc work failed.");

    work->data = static_cast<void *>(data);
    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            UvWorkData *data = static_cast<UvWorkData*>(work->data);
            auto &[mgr, eventClassifyInfo, eventInfo] = *data;
            mgr->PublishToJS(eventClassifyInfo, eventInfo);
            delete data;
            delete work;
        });
}
} // namespace UpdateEngine
} // namespace OHOS