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

#ifndef UPDATER_INTERFACE_H
#define UPDATER_INTERFACE_H

#include <memory>
#include "client_helper.h"
#include "iupdate_session.h"
#include "napi_util.h"
#include "session_manager.h"

namespace OHOS {
namespace UpdateEngine {
class IUpdater {
public:
    virtual ~IUpdater() {}

    napi_value On(napi_env env, napi_callback_info info);

    napi_value Off(napi_env env, napi_callback_info info);

    virtual void GetUpdateResult(SessionType type, UpdateResult &result);

    void RemoveSession(uint32_t sessionId)
    {
        if (sessionsMgr_ == nullptr) {
            return;
        }
        sessionsMgr_->RemoveSession(sessionId);
    }

protected:
    napi_value StartSession(napi_env env, napi_callback_info info, SessionParams &sessionParams,
        IUpdateSession::DoWorkFunction function);

    napi_value StartParamErrorSession(napi_env env, napi_callback_info info, CALLBACK_POSITION callbackPosition);

    void NotifyEventInfo(const EventInfo &eventInfo);

    std::shared_ptr<SessionManager> sessionsMgr_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATER_INTERFACE_H