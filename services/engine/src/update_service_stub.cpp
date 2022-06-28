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

#include "accesstoken_kit.h"
#include "hap_token_info.h"
#include "ipc_skeleton.h"
#include "securec.h"
#include "update_helper.h"
#include "update_service_stub.h"
#include "update_system_event.h"

using namespace std;

namespace OHOS {
namespace UpdateEngine {
class DeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    DeathRecipient(UpgradeInfo upgradeInfo, UpdateServiceStub::UpdateServiceStubPtr service)
        : upgradeInfo_(upgradeInfo), service_(service) {}
    ~DeathRecipient() final {}
    DISALLOW_COPY_AND_MOVE(DeathRecipient);
    void OnRemoteDied(const wptr<IRemoteObject>& remote) final;
private:
    UpgradeInfo upgradeInfo_;
    UpdateServiceStub::UpdateServiceStubPtr service_;
};

std::mutex g_clientDeathRecipientMapLock_;
std::map<UpgradeInfo, std::unique_ptr<DeathRecipient>> g_clientDeathRecipientMap;

void DeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    ENGINE_LOGI("client DeathRecipient OnRemoteDied: %{public}s", upgradeInfo_.ToString().c_str());
    if (service_ != nullptr) {
        service_->UnregisterUpdateCallback(upgradeInfo_);
    }
    std::lock_guard<std::mutex> lock(g_clientDeathRecipientMapLock_);
    g_clientDeathRecipientMap.erase(upgradeInfo_);
}

static int32_t GetNewVersionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    NewVersionInfo newVersionInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetNewVersion(upgradeInfo, newVersionInfo, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to GetNewVersion");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteNewVersionInfo(reply, newVersionInfo);
    return 0;
}

static int32_t GetCurrentVersionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    CurrentVersionInfo currentVersionInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetCurrentVersionInfo(upgradeInfo, currentVersionInfo, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to GetCurrentVersionInfo");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteCurrentVersionInfo(reply, currentVersionInfo);
    return 0;
}

static int32_t GetTaskInfoStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    TaskInfo taskInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetTaskInfo(upgradeInfo, taskInfo, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to GetTaskInfo");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteTaskInfo(reply, taskInfo);
    return 0;
}

static int32_t GetOtaStatusStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    OtaStatus otaStatus = {};
    BusinessError businessError = {};
    int32_t ret = service->GetOtaStatus(upgradeInfo, otaStatus, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to GetOtaStatus");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteOtaStatus(reply, otaStatus);
    return 0;
}

static int32_t CheckNewVersionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    int32_t ret = service->CheckNewVersion(upgradeInfo);
    reply.WriteInt32(ret);
    return 0;
}

static int32_t DownloadVersionStub(UpdateServiceStub::UpdateServiceStubPtr service, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    DownloadOptions downloadOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadDownloadOptions(data, downloadOptions);

    int32_t ret = service->DownloadVersion(upgradeInfo, versionDigestInfo, downloadOptions, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to DownloadVersion");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t PauseDownloadStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    PauseDownloadOptions pauseDownloadOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadPauseDownloadOptions(data, pauseDownloadOptions);

    int32_t ret = service->PauseDownload(upgradeInfo, versionDigestInfo, pauseDownloadOptions, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to PauseDownload");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t ResumeDownloadStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    ResumeDownloadOptions resumeDownloadOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadResumeDownloadOptions(data, resumeDownloadOptions);

    int32_t ret = service->ResumeDownload(upgradeInfo, versionDigestInfo, resumeDownloadOptions, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to ResumeDownload");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t DoUpdateStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    UpgradeOptions upgradeOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadUpgradeOptions(data, upgradeOptions);

    int32_t ret = service->DoUpdate(upgradeInfo, versionDigestInfo, upgradeOptions, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to DoUpdate");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t ClearErrorStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    ClearOptions clearOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadClearOptions(data, clearOptions);

    int32_t ret = service->ClearError(upgradeInfo, versionDigestInfo, clearOptions, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to ClearError");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t TerminateUpgradeStub(UpdateServiceStub::UpdateServiceStubPtr service, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");

    UpgradeInfo upgradeInfo;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);

    BusinessError businessError;
    int32_t ret = service->TerminateUpgrade(upgradeInfo, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to TerminateUpgrade");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t SetUpdatePolicyStub(
    UpdateServiceStub::UpdateServiceStubPtr service, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdatePolicy policy = {};
    UpdateHelper::ReadUpdatePolicy(data, policy);
    BusinessError businessError = {};
    int32_t ret = service->SetUpdatePolicy(upgradeInfo, policy, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to SetUpdatePolicy");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t GetUpdatePolicyStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdatePolicy policy = {};
    BusinessError businessError = {};
    int32_t ret = service->GetUpdatePolicy(upgradeInfo, policy, businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to GetUpdatePolicy");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteUpdatePolicy(reply, policy);
    return 0;
}

static int32_t RegisterUpdateCallbackStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_LOGI("RegisterUpdateCallbackStub");
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    int32_t ret = -1;
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    auto remote = data.ReadRemoteObject();
    ENGINE_CHECK(remote != nullptr, reply.WriteInt32(ret); return 0, "Failed to read remote obj");
    ret = service->RegisterUpdateCallback(upgradeInfo, iface_cast<IUpdateCallback>(remote));
    if (ret == 0 && remote->IsProxyObject()) {
        auto deathRecipient = std::make_unique<DeathRecipient>(upgradeInfo, service);
        if (remote->AddDeathRecipient(deathRecipient.get())) {
            ENGINE_LOGI("add client DeathRecipient success");
            std::lock_guard<std::mutex> lock(g_clientDeathRecipientMapLock_);
            g_clientDeathRecipientMap.insert({upgradeInfo, std::move(deathRecipient)});
        } else {
            ENGINE_LOGE("Failed to add death recipient");
        }
    }
    reply.WriteInt32(ret);
    return 0;
}

static int32_t UnregisterUpdateCallbackStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    int32_t ret = service->UnregisterUpdateCallback(upgradeInfo);
    reply.WriteInt32(ret);
    return 0;
}

static int32_t CancelStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    UpgradeInfo upgradeInfo {};
    BusinessError businessError = {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    int32_t ret = service->Cancel(upgradeInfo, data.ReadInt32(), businessError);
    ENGINE_CHECK(ret == 0, return -1, "Failed to cancel");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return 0;
}

static int32_t RebootAndCleanStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    SYS_EVENT_SYSTEM_RESET(0, UpdateSystemEvent::RESET_START);
    string miscFile = Str16ToStr8(data.ReadString16());
    string cmd = Str16ToStr8(data.ReadString16());
    int32_t ret = service->RebootAndClean(miscFile, cmd);
    reply.WriteInt32(ret);
    return 0;
}

static int32_t RebootAndInstallStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_CHECK(service != nullptr, return -1, "Invalid param");
    string miscFile = Str16ToStr8(data.ReadString16());
    string packageName = Str16ToStr8(data.ReadString16());
    int32_t ret = service->RebootAndInstall(miscFile, packageName);
    reply.WriteInt32(ret);
    return 0;
}

int32_t UpdateServiceStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ENGINE_LOGI("UpdateServiceStub ReadInterfaceToken fail");
        return -1;
    }
    static std::map<uint32_t, UpdateServiceStub::RequestFuncType> requestFuncMap = {
        {IUpdateService::CHECK_VERSION, CheckNewVersionStub},
        {IUpdateService::DOWNLOAD, DownloadVersionStub},
        {IUpdateService::PAUSE_DOWNLOAD, PauseDownloadStub},
        {IUpdateService::RESUME_DOWNLOAD, ResumeDownloadStub},
        {IUpdateService::UPGRADE, DoUpdateStub},
        {IUpdateService::CLEAR_ERROR, ClearErrorStub},
        {IUpdateService::TERMINATE_UPGRADE, TerminateUpgradeStub},
        {IUpdateService::SET_POLICY, SetUpdatePolicyStub},
        {IUpdateService::GET_POLICY, GetUpdatePolicyStub},
        {IUpdateService::GET_NEW_VERSION, GetNewVersionStub},
        {IUpdateService::GET_CURRENT_VERSION, GetCurrentVersionStub},
        {IUpdateService::GET_TASK_INFO, GetTaskInfoStub},
        {IUpdateService::GET_OTA_STATUS, GetOtaStatusStub},
        {IUpdateService::REGISTER_CALLBACK, RegisterUpdateCallbackStub},
        {IUpdateService::UNREGISTER_CALLBACK, UnregisterUpdateCallbackStub},
        {IUpdateService::CANCEL, CancelStub},
        {IUpdateService::REBOOT_CLEAN, RebootAndCleanStub},
        {IUpdateService::REBOOT_INSTALL, RebootAndInstallStub},
    };

    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    ENGINE_LOGI("UpdateServiceStub OnRemoteRequest code %{public}u", code);
    string permission = "ohos.permission.UPDATE_SYSTEM";
    if (code == IUpdateService::REBOOT_CLEAN) {
        permission = "ohos.permission.FACTORY_RESET";
    }

    int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permission);
    if (result != Security::AccessToken::PERMISSION_GRANTED) {
        UpgradeInfo tmpInfo {};
        UpdateHelper::ReadUpgradeInfo(data, tmpInfo);
        SYS_EVENT_VERIFY_FAILED(0, UpdateHelper::BuildEventDevId(tmpInfo),
            UpdateSystemEvent::EVENT_PERMISSION_VERIFY_FAILED);
        ENGINE_LOGE("permissionCheck %{public}s false", permission.c_str());
        return -1;
    }
    ENGINE_LOGI("UpdateServiceStub func code %{public}u", code);
    for (auto inter = requestFuncMap.begin(); inter != requestFuncMap.end(); inter++) {
        if (inter->first == code) {
            return inter->second(this, data, reply, option);
        }
    }
    ENGINE_LOGE("UpdateServiceStub OnRemoteRequest code %{public}u not found", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace UpdateEngine
} // namespace OHOS