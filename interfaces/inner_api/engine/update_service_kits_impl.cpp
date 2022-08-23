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
#include "system_ability_definition.h"
#include "update_service_ondemand.h"

namespace OHOS {
namespace UpdateEngine {
#define RETURN_FAIL_WHEN_SERVICE_NULL(updateService) \
    ENGINE_CHECK((updateService) != nullptr, return INT_CALL_IPC_ERR, "Get updateService failed")

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
    if (object == nullptr) {
        ENGINE_CHECK(UpdateServiceOnDemand::GetInstance()->TryLoadUpdaterSa(), return nullptr, "TryLoadUpdaterSa fail");
        object = samgr->GetSystemAbility(UPDATE_DISTRIBUTED_SERVICE_ID);
        ENGINE_CHECK(object != nullptr, return nullptr, "Get update object from samgr failed");
    }

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
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);

    std::lock_guard<std::mutex> lock(updateServiceLock_);
    auto remoteUpdateCallback = new RemoteUpdateCallback(cb);
    ENGINE_CHECK(remoteUpdateCallback != nullptr, return INT_PARAM_ERR, "Failed to create remote callback");
    int32_t ret = updateService->RegisterUpdateCallback(info, remoteUpdateCallback);
    if (ret == INT_CALL_SUCCESS) {
        remoteUpdateCallbackMap_[info] = remoteUpdateCallback;
    }
    return ret;
}

int32_t UpdateServiceKitsImpl::UnregisterUpdateCallback(const UpgradeInfo &info)
{
    ENGINE_LOGI("UnregisterUpdateCallback");
    std::lock_guard<std::mutex> lock(updateServiceLock_);
    remoteUpdateCallbackMap_.erase(info);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceKitsImpl::CheckNewVersion(const UpgradeInfo &info)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::CheckNewVersion");

    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->CheckNewVersion(info);
}

int32_t UpdateServiceKitsImpl::Download(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DownloadOptions &downloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::Download");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->DownloadVersion(info, versionDigestInfo, downloadOptions, businessError);
}

int32_t UpdateServiceKitsImpl::PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::PauseDownload");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->PauseDownload(info, versionDigestInfo, pauseDownloadOptions, businessError);
}

int32_t UpdateServiceKitsImpl::ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::ResumeDownload");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->ResumeDownload(info, versionDigestInfo, resumeDownloadOptions, businessError);
}

int32_t UpdateServiceKitsImpl::Upgrade(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const UpgradeOptions &upgradeOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::Upgrade");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->DoUpdate(info, versionDigest, upgradeOptions, businessError);
}

int32_t UpdateServiceKitsImpl::ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const ClearOptions &clearOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::ClearError");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->ClearError(info, versionDigest, clearOptions, businessError);
}

int32_t UpdateServiceKitsImpl::TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::TerminateUpgrade");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->TerminateUpgrade(info, businessError);
}

int32_t UpdateServiceKitsImpl::GetNewVersionInfo(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetNewVersionInfo");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->GetNewVersion(info, newVersionInfo, businessError);
}

int32_t UpdateServiceKitsImpl::GetNewVersionDescription(const UpgradeInfo &info,
    const VersionDigestInfo &versionDigestInfo, const DescriptionOptions &descriptionOptions,
    VersionDescriptionInfo &newVersionDescriptionInfo, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetNewVersionDescription");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->GetNewVersionDescription(info, versionDigestInfo, descriptionOptions,
        newVersionDescriptionInfo, businessError);
}

int32_t UpdateServiceKitsImpl::GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetCurrentVersionInfo");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->GetCurrentVersionInfo(info, currentVersionInfo, businessError);
}

int32_t UpdateServiceKitsImpl::GetCurrentVersionDescription(const UpgradeInfo &info,
    const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &currentVersionDescriptionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetCurrentVersionDescription");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->GetCurrentVersionDescription(info, descriptionOptions, currentVersionDescriptionInfo,
        businessError);
}

int32_t UpdateServiceKitsImpl::GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetTaskInfo");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->GetTaskInfo(info, taskInfo, businessError);
}

int32_t UpdateServiceKitsImpl::SetUpgradePolicy(const UpgradeInfo &info, const UpgradePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::SetUpgradePolicy");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->SetUpgradePolicy(info, policy, businessError);
}

int32_t UpdateServiceKitsImpl::GetUpgradePolicy(const UpgradeInfo &info, UpgradePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::GetUpgradePolicy");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->GetUpgradePolicy(info, policy, businessError);
}

int32_t UpdateServiceKitsImpl::Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::Cancel %d", service);
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->Cancel(info, service, businessError);
}

int32_t UpdateServiceKitsImpl::FactoryReset(BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::FactoryReset");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
#ifndef UPDATER_API_TEST
    return updateService->FactoryReset(businessError);
#endif
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceKitsImpl::ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile,
    const std::string &packageName, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::ApplyNewVersion");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->ApplyNewVersion(info, miscFile, packageName, businessError);
}

int32_t UpdateServiceKitsImpl::VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceKitsImpl::VerifyUpgradePackage");
    auto updateService = GetService();
    RETURN_FAIL_WHEN_SERVICE_NULL(updateService);
    return updateService->VerifyUpgradePackage(packagePath, keyPath, businessError);
}
} // namespace UpdateEngine
} // namespace OHOS
