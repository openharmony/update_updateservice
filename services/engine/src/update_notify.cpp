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

#include "update_notify.h"

#include "iservice_registry.h"

#include "update_helper.h"
#include "update_log.h"

using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace std;

namespace OHOS {
namespace UpdateEngine {
static sptr<AAFwk::IAbilityManager> g_abilityMgr;

bool UpdateNotify::InitAbilityManagerService()
{
    if (g_abilityMgr != nullptr) {
        return true;
    }
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        ENGINE_LOGE("Failed to get system ability mgr.");
        return false;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
    if (remoteObject == nullptr) {
        ENGINE_LOGE("Failed to get ability manager service.");
        return false;
    }
    g_abilityMgr = iface_cast<IAbilityManager>(remoteObject);
    if ((g_abilityMgr == nullptr) || (g_abilityMgr->AsObject() == nullptr)) {
        ENGINE_LOGE("Failed to get system ability manager services ability");
        return false;
    }
    return true;
}

ErrCode UpdateNotify::StartAbility(const Want &want)
{
    if (!InitAbilityManagerService()) {
        ENGINE_LOGE("failed to get ability manager service");
        return OHOS::ERR_INVALID_VALUE;
    }
    ErrCode result = g_abilityMgr->StartAbility(want);
    ENGINE_LOGI("StartAbility result %{public}d", result);
    return result;
}

ErrCode UpdateNotify::StopServiceAbility(const Want &want)
{
    if (!InitAbilityManagerService()) {
        ENGINE_LOGE("failed to get ability manager service");
        return OHOS::ERR_INVALID_VALUE;
    }
    ErrCode result = g_abilityMgr->StopServiceAbility(want);
    ENGINE_LOGI("StopServiceAbility result %{public}d", result);
    return result;
}

ErrCode UpdateNotify::ConnectAbility(const Want &want, const sptr<IAbilityConnection> &connect,
    const sptr<IRemoteObject> &callerToken)
{
    if (!InitAbilityManagerService()) {
        ENGINE_LOGE("failed to get ability manager service");
        return OHOS::ERR_INVALID_VALUE;
    }
    ErrCode result = g_abilityMgr->ConnectAbility(want, connect, callerToken);
    ENGINE_LOGI("ConnectAbility result %{public}d", result);
    return result;
}

ErrCode UpdateNotify::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    if (!InitAbilityManagerService()) {
        ENGINE_LOGE("failed to get ability manager service");
        return OHOS::ERR_INVALID_VALUE;
    }
    ErrCode result = g_abilityMgr->DisconnectAbility(connect);
    ENGINE_LOGI("DisconnectAbility result %{public}d", result);
    return result;
}

Want UpdateNotify::MakeWant(const std::string &deviceId, const std::string &abilityName, const std::string &bundleName,
    const std::string &subscribeInfo, const std::string &params)
{
    ElementName element(deviceId, bundleName, abilityName);
    Want want;
    want.SetElement(element);
    want.SetParam("EventInfo", params);
    want.SetParam("SubscribeInfo", subscribeInfo);
    return want;
}

void UpdateNotify::NotifyToAppService(const std::string &eventInfo, const std::string &subscribeInfo)
{
    if (eventInfo.empty()) {
        ENGINE_LOGE("NotifyToAppService eventInfo error.");
        return;
    }
    string bundleName = OUC_PACKAGE_NAME;
    string abilityName = OUC_SERVICE_EXT_ABILITY_NAME;
    Want want = MakeWant("", abilityName, bundleName, subscribeInfo, eventInfo);
    ErrCode result = StartAbility(want);
    if (result != OHOS::ERR_OK) {
        ENGINE_LOGE("NotifyToAppService fail.");
    }
}

void NotifyConnection::OnAbilityConnectDone(const ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    ENGINE_LOGI("OnAbilityConnectDone successfully. result %{public}d", resultCode);
}

void NotifyConnection::OnAbilityDisconnectDone(const ElementName &element, int resultCode)
{
    ENGINE_LOGI("OnAbilityDisconnectDone successfully. result %{public}d", resultCode);
}
} // namespace UpdateEngine
} // namespace OHOS