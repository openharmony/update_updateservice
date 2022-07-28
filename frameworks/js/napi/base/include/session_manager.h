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

#ifndef UPDATE_SESSION_MGR_H
#define UPDATE_SESSION_MGR_H

#include "iupdate_session.h"
#include "js_native_api_types.h"

namespace OHOS {
namespace UpdateEngine {
class SessionManager {
public:
    SessionManager(napi_env env, napi_ref thisReference);
    ~SessionManager();

    void AddSession(std::shared_ptr<IUpdateSession> session);
    void RemoveSession(uint32_t sessionId);
    void Clear();

    int32_t ProcessUnsubscribe(const std::string &eventType, size_t argc, napi_value arg);
    void Unsubscribe(const EventClassifyInfo &eventClassifyInfo, napi_value handle);

    IUpdateSession *FindSessionByHandle(napi_env env, const std::string &eventType, napi_value arg);
    IUpdateSession *FindSessionByHandle(napi_env env, const EventClassifyInfo &eventClassifyInfo, napi_value arg);

    void PublishToJS(const std::string &type, const UpdateResult &result);
    void PublishToJS(const EventClassifyInfo &eventClassifyInfo, const EventInfo &eventInfo);

    void Emit(const std::string &type, const UpdateResult &result);
    void Emit(const EventClassifyInfo &eventClassifyInfo, const EventInfo &eventInfo);

private:
    bool GetFirstSessionId(uint32_t &sessionId);
    bool GetNextSessionId(uint32_t &sessionId);

    napi_env env_ {};
    napi_ref thisReference_ {};
    std::map<uint32_t, std::shared_ptr<IUpdateSession>> sessions_;
#ifndef UPDATER_API_TEST
    std::mutex sessionMutex_;
#endif
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SESSION_MGR_H