/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef UPDATE_NOTIFY_H
#define UPDATE_NOTIFY_H

#include <string>
#include "ability_connect_callback_stub.h"
#include "ability_connect_callback_interface.h"
#include "ability_manager_interface.h"
#include "ability_manager_client.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "update_no_constructor.h"
#include "want.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateNotify : public NoConstructor {
public:
    static void NotifyToAppService(const std::string &eventInfo, const std::string &subscribeInfo);

private:
    static ErrCode ConnectAbility(const AAFwk::Want &want, const sptr<AAFwk::IAbilityConnection> &connect,
        const sptr<IRemoteObject> &callerToken);
    static ErrCode DisconnectAbility(const sptr<AAFwk::IAbilityConnection> &connect);
    static ErrCode StopServiceAbility(const AAFwk::Want &want);
    static ErrCode StartAbility(const AAFwk::Want &want);
    static AAFwk::Want MakeWant(const std::string &deviceId, const std::string &abilityName,
        const std::string &bundleName, const std::string &subscribeInfo, const std::string &params = {});
    static bool InitAbilityManagerService();
};

class NotifyConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit NotifyConnection() = default;
    ~NotifyConnection() = default;

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
        int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_NOTIFY_H