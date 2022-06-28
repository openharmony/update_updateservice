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

#include "update_service_kits_impl.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "iupdate_service.h"
#include "iupdate_callback.h"
#include "securec.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace UpdateEngine {
UpdateServiceKits& UpdateServiceKits::GetInstance()
{
    return DelayedRefSingleton<UpdateServiceKitsImpl>::GetInstance();
}

UpdateServiceKitsImpl::UpdateServiceKitsImpl() {}

UpdateServiceKitsImpl::~UpdateServiceKitsImpl() {}

void UpdateServiceKitsImpl::ResetService(const wptr<IRemoteObject>& remote)
{
    ENGINE_LOGI("Remote is dead, reset service instance");

    std::lock_guard<std::mutex> lock(updateServiceLock_);
    if (updateService_ != nullptr) {
        sptr<IRemoteObject> object = updateService_->AsObject();
        if ((object != nullptr) && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            updateService_ = nullptr;
        }
    }
}

sptr<IUpdateService> UpdateServiceKitsImpl::GetService()
{
    std::lock_guard<std::mutex> lock(updateServiceLock_);
    if (updateService_ != nullptr) {
        return updateService_;
    }

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ENGINE_CHECK(samgr != nullptr, return nullptr, "Get samgr failed");
    sptr<IRemoteObject> object = samgr->GetSystemAbility(UPDATE_DISTRIBUTED_SERVICE_ID);
    ENGINE_CHECK(object != nullptr, return nullptr, "Get update object from samgr failed");

    if (deathRecipient_ == nullptr) {
        deathRecipient_ = new DeathRecipient();
    }

    if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient_))) {
        ENGINE_LOGE("Failed to add death recipient");
    }

    ENGINE_LOGI("get remote object ok");
    updateService_ = iface_cast<IUpdateService>(object);
    if (updateService_ == nullptr) {
        ENGINE_LOGE("update service iface_cast failed");
        return updateService_;
    }

    ENGINE_LOGI("RegisterUpdateCallback size %{public}zu", remoteUpdateCallbackMap_.size());
    for (auto &iter : remoteUpdateCallbackMap_) {
        updateService_->RegisterUpdateCallback(iter.first, iter.second);
    }
    return updateService_;
}

void UpdateServiceKitsImpl::DeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    DelayedRefSingleton<UpdateServiceKitsImpl>::GetInstance().ResetService(remote);
}

UpdateServiceKitsImpl::RemoteUpdateCallback::RemoteUpdateCallback(const UpdateCallbackInfo &cb)
    : UpdateCallback()
{
    updateCallback_ = cb;
}

UpdateServiceKitsImpl::RemoteUpdateCallback::~RemoteUpdateCallback()
{
    updateCallback_.checkNewVersionDone = nullptr;
    updateCallback_.onEvent = nullptr;
}

void UpdateServiceKitsImpl::RemoteUpdateCallback::OnCheckVersionDone(
    const BusinessError &businessError, const CheckResultEx &checkResultEx)
{
    ENGINE_LOGI("OnCheckVersionDone status %{public}d", checkResultEx.isExistNewVersion);
    if (updateCallback_.checkNewVersionDone != nullptr) {
        updateCallback_.checkNewVersionDone(businessError, checkResultEx);
    }
}

void UpdateServiceKitsImpl::RemoteUpdateCallback::OnEvent(const EventInfo &eventInfo)
{
    ENGINE_LOGI("OnEvent progress %{public}d", eventInfo.eventId);
    if (updateCallback_.onEvent != nullptr) {
        updateCallback_.onEvent(eventInfo);
    }
}

int32_t UpdateServiceKitsImpl::RegisterUpdateCallback(const UpgradeInfo &info, const UpdateCallbackInfo &cb)
{
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");

    std::lock_guard<std::mutex> lock(updateServiceLock_);
    auto remoteUpdateCallback = new RemoteUpdateCallback(cb);
    ENGINE_CHECK(remoteUpdateCallback != nullptr, return -1, "Failed to create remote callback");
    int32_t ret = updateService->RegisterUpdateCallback(info, remoteUpdateCallback);
    if (ret == 0) {
        remoteUpdateCallbackMap_[info] = remoteUpdateCallback;
    }
    return ret;
}

int32_t UpdateServiceKitsImpl::UnregisterUpdateCallback(const UpgradeInfo &info)
{
    ENGINE_LOGI("UnregisterUpdateCallback");
    std::lock_guard<std::mutex> lock(updateServiceLock_);
    remoteUpdateCallbackMap_.erase(info);
    return 0;
}

int32_t UpdateServiceKitsImpl::CheckNewVersion(const UpgradeInfo &info)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::CheckNewVersion");

    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->CheckNewVersion(info);
}

int32_t UpdateServiceKitsImpl::DownloadVersion(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DownloadOptions &downloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::DownloadVersion");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->DownloadVersion(info, versionDigestInfo, downloadOptions, businessError);
}

int32_t UpdateServiceKitsImpl::PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::PauseDownload");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->PauseDownload(info, versionDigestInfo, pauseDownloadOptions, businessError);
}

int32_t UpdateServiceKitsImpl::ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::ResumeDownload");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->ResumeDownload(info, versionDigestInfo, resumeDownloadOptions, businessError);
}

int32_t UpdateServiceKitsImpl::DoUpdate(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const UpgradeOptions &upgradeOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::DoUpdate");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->DoUpdate(info, versionDigest, upgradeOptions, businessError);
}

int32_t UpdateServiceKitsImpl::ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const ClearOptions &clearOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::ClearError");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->ClearError(info, versionDigest, clearOptions, businessError);
}

int32_t UpdateServiceKitsImpl::TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::TerminateUpgrade");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->TerminateUpgrade(info, businessError);
}

int32_t UpdateServiceKitsImpl::GetNewVersion(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetNewversion");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->GetNewVersion(info, newVersionInfo, businessError);
}

int32_t UpdateServiceKitsImpl::GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetCurrentVersionInfo");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->GetCurrentVersionInfo(info, currentVersionInfo, businessError);
}

int32_t UpdateServiceKitsImpl::GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetTaskInfo");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->GetTaskInfo(info, taskInfo, businessError);
}

int32_t UpdateServiceKitsImpl::GetOtaStatus(const UpgradeInfo &info, OtaStatus &otaStatus,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetOtaStatus");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->GetOtaStatus(info, otaStatus, businessError);
}

int32_t UpdateServiceKitsImpl::SetUpdatePolicy(const UpgradeInfo &info, const UpdatePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::SetUpdatePolicy");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->SetUpdatePolicy(info, policy, businessError);
}

int32_t UpdateServiceKitsImpl::GetUpdatePolicy(const UpgradeInfo &info, UpdatePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetUpdatePolicy");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->GetUpdatePolicy(info, policy, businessError);
}

int32_t UpdateServiceKitsImpl::Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::Cancel %d", service);
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->Cancel(info, service, businessError);
}

int32_t UpdateServiceKitsImpl::RebootAndClean(const std::string &miscFile, const std::string &cmd)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::RebootAndCleanUserData");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
#ifndef UPDATER_API_TEST
    return updateService->RebootAndClean(miscFile, cmd);
#endif
    return 1;
}

int32_t UpdateServiceKitsImpl::RebootAndInstall(const std::string &miscFile, const std::string &packageName)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::RebootAndInstall");
    auto updateService = GetService();
    ENGINE_CHECK(updateService != nullptr, return -1, "Get updateService failed");
    return updateService->RebootAndInstall(miscFile, packageName);
}
} // namespace UpdateEngine
} // namespace OHOS
