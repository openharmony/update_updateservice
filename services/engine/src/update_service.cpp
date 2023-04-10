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

#include "update_service.h"

#include <unistd.h>

#include "iservice_registry.h"
#include "ohos_types.h"
#include "system_ability_definition.h"

#include "access_manager.h"
#include "dupdate_net_manager.h"
#include "firmware_common.h"
#include "firmware_manager.h"
#include "startup_manager.h"
#include "update_log.h"
#include "update_service_cache.h"
#include "update_service_local_updater.h"
#include "update_service_restorer.h"
#include "update_service_util.h"

namespace OHOS {
namespace UpdateEngine {
REGISTER_SYSTEM_ABILITY_BY_ID(UpdateService, UPDATE_DISTRIBUTED_SERVICE_ID, true)

OHOS::sptr<UpdateService> UpdateService::updateService_ { nullptr };

void UpdateService::ClientDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    ENGINE_LOGI("client DeathRecipient OnRemoteDied: %{public}s", upgradeInfo_.ToString().c_str());
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service != nullptr) {
        service->UnregisterUpdateCallback(upgradeInfo_);
    }
}

UpdateService::ClientProxy::ClientProxy(const UpgradeInfo &info, const sptr<IUpdateCallback> &callback)
    : proxy_(callback)
{
    ENGINE_LOGI("UpdateService::ClientProxy constructor");
    auto clientDeathRecipient = new (std::nothrow) ClientDeathRecipient(info);
    if (clientDeathRecipient != nullptr) {
        deathRecipient_ = sptr<ClientDeathRecipient>(clientDeathRecipient);
    } else {
        ENGINE_LOGE("UpdateService::ClientProxy, new fail");
    }
}

void UpdateService::ClientProxy::AddDeathRecipient()
{
    ENGINE_LOGI("UpdateService::ClientProxy AddDeathRecipient in");
    if (proxy_ != nullptr) {
        auto remoteObject = proxy_->AsObject();
        if ((remoteObject != nullptr) && (deathRecipient_ != nullptr)) {
            remoteObject->AddDeathRecipient(deathRecipient_);
            ENGINE_LOGI("UpdateService::ClientProxy AddDeathRecipient success");
        }
    }
}

void UpdateService::ClientProxy::RemoveDeathRecipient()
{
    ENGINE_LOGI("UpdateService::ClientProxy RemoveDeathRecipient in");
    if (proxy_ != nullptr) {
        auto remoteObject = proxy_->AsObject();
        if ((remoteObject != nullptr) && (deathRecipient_ != nullptr)) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
            ENGINE_LOGI("UpdateService::ClientProxy RemoveDeathRecipient success");
        }
    }
}

sptr<IUpdateCallback> UpdateService::ClientProxy::Get()
{
    return proxy_;
}

UpdateService::UpdateService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate)
{
    updateImplMgr_ = std::make_shared<UpdateServiceImplManager>();
}

UpdateService::~UpdateService()
{
    ENGINE_LOGI("UpdateServerTest free now");
    for (auto &iter : clientProxyMap_) {
        iter.second.RemoveDeathRecipient();
    }
}

sptr<UpdateService> UpdateService::GetInstance()
{
    return updateService_;
}

int32_t UpdateService::RegisterUpdateCallback(const UpgradeInfo &info, const sptr<IUpdateCallback> &updateCallback)
{
    ENGINE_LOGI("RegisterUpdateCallback");
    UnregisterUpdateCallback(info);
    {
        std::lock_guard<std::mutex> lock(clientProxyMapLock_);
        ClientProxy clientProxy(info, updateCallback);
        clientProxy.AddDeathRecipient();
        clientProxyMap_.insert({info, clientProxy});
    }
    if (!info.IsLocal()) {
        UpdateServiceCache::SetUpgradeInfo(info);
    }
    DelayedSingleton<AccessManager>::GetInstance()->SetRemoteIdle(clientProxyMap_.empty());
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::UnregisterUpdateCallback(const UpgradeInfo &info)
{
    ENGINE_LOGI("UnregisterUpdateCallback");
    std::lock_guard<std::mutex> lock(clientProxyMapLock_);
    auto iter = clientProxyMap_.find(info);
    if (iter == clientProxyMap_.end()) {
        return INT_CALL_SUCCESS;
    }
    iter->second.RemoveDeathRecipient();
    clientProxyMap_.erase(info);
    DelayedSingleton<AccessManager>::GetInstance()->SetRemoteIdle(clientProxyMap_.empty());
    return INT_CALL_SUCCESS;
}

sptr<IUpdateCallback> UpdateService::GetUpgradeCallback(const UpgradeInfo &info)
{
    std::lock_guard<std::mutex> lock(clientProxyMapLock_);
    auto iter = clientProxyMap_.find(info);
    if (iter == clientProxyMap_.end()) {
        return nullptr;
    }
    return iter->second.Get();
}

int32_t UpdateService::GetNewVersionInfo(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
    BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("GetNewVersionInfo onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->GetNewVersionInfo(info, newVersionInfo, businessError);
}

int32_t UpdateService::GetNewVersionDescription(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &newVersionDescriptionInfo,
    BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("GetNewVersionDescription onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->GetNewVersionDescription(info, versionDigestInfo, descriptionOptions,
        newVersionDescriptionInfo, businessError);
}

int32_t UpdateService::GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
    BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("GetCurrentVersionInfo onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->GetCurrentVersionInfo(info, currentVersionInfo, businessError);
}

int32_t UpdateService::GetCurrentVersionDescription(const UpgradeInfo &info,
    const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &currentVersionDescriptionInfo,
    BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("GetCurrentVersionDescription onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->GetCurrentVersionDescription(info, descriptionOptions, currentVersionDescriptionInfo,
        businessError);
}

int32_t UpdateService::GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("GetTaskInfo onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->GetTaskInfo(info, taskInfo, businessError);
}

int32_t UpdateService::SetUpgradePolicy(const UpgradeInfo &info, const UpgradePolicy &policy,
    BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("SetUpgradePolicy onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->SetUpgradePolicy(info, policy, businessError);
}

int32_t UpdateService::GetUpgradePolicy(const UpgradeInfo &info, UpgradePolicy &policy, BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("GetUpgradePolicy onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->GetUpgradePolicy(info, policy, businessError);
}

int32_t UpdateService::CheckNewVersion(const UpgradeInfo &info)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("CheckNewVersion onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->CheckNewVersion(info);
}

int32_t UpdateService::Download(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DownloadOptions &downloadOptions, BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("Download onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->Download(info, versionDigestInfo, downloadOptions, businessError);
}

int32_t UpdateService::PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("PauseDownload");
    businessError.errorNum = CallResult::SUCCESS;
    businessError.Build(CallResult::UN_SUPPORT, "PauseDownload unsupport");
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("ResumeDownload allowNetwork:%{public}d", CAST_INT(resumeDownloadOptions.allowNetwork));
    businessError.Build(CallResult::UN_SUPPORT, "ResumeDownload unsupport");
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::Upgrade(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const UpgradeOptions &upgradeOptions, BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("Upgrade onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->Upgrade(info, versionDigestInfo, upgradeOptions, businessError);
}

int32_t UpdateService::ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ClearOptions &clearOptions, BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("ClearError onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->ClearError(info, versionDigestInfo, clearOptions, businessError);
}

int32_t UpdateService::TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("TerminateUpgrade onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->TerminateUpgrade(info, businessError);
}

int32_t UpdateService::Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError)
{
    sptr<IServiceOnlineUpdater> onlineUpdater = updateImplMgr_->GetOnlineUpdater(info);
    if (onlineUpdater == nullptr) {
        ENGINE_LOGI("Cancel onlineUpdater null");
        return INT_CALL_FAIL;
    }
    return onlineUpdater->Cancel(info, service, businessError);
}

int32_t UpdateService::FactoryReset(BusinessError &businessError)
{
    sptr<UpdateServiceRestorer> restorer = new UpdateServiceRestorer();
    if (restorer == nullptr) {
        ENGINE_LOGI("FactoryReset restorer null");
        return INT_CALL_FAIL;
    }
    return restorer->FactoryReset(businessError);
}

int32_t UpdateService::ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile,
    const std::string &packageName, BusinessError &businessError)
{
    sptr<UpdateServiceLocalUpdater> localUpdater = new UpdateServiceLocalUpdater();
    if (localUpdater == nullptr) {
        ENGINE_LOGI("FactoryReset localUpdater null");
        return INT_CALL_FAIL;
    }
    return localUpdater->ApplyNewVersion(info, miscFile, packageName, businessError);
}

int32_t UpdateService::VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
    BusinessError &businessError)
{
    sptr<UpdateServiceLocalUpdater> localUpdater = new UpdateServiceLocalUpdater();
    if (localUpdater == nullptr) {
        ENGINE_LOGI("FactoryReset localUpdater null");
        return INT_CALL_FAIL;
    }
    return localUpdater->VerifyUpgradePackage(packagePath, keyPath, businessError);
}

void BuildUpgradeInfoDump(const int fd, UpgradeInfo &info)
{
    dprintf(fd, "---------------------upgrade info info--------------------\n");
    dprintf(fd, "UpgradeApp: %s\n", info.upgradeApp.c_str());
    dprintf(fd, "vendor: %s\n", info.businessType.vendor.c_str());
    dprintf(fd, "subType: %d\n", static_cast<int>(info.businessType.subType));
}

void BuildVersionInfoDump(const int fd, const CheckResult &checkResult)
{
    dprintf(fd, "---------------------version info--------------------\n");
    dprintf(fd, "isExistNewVersion: %d\n", checkResult.isExistNewVersion);
    if (checkResult.newVersionInfo.versionComponents.empty()) {
        return;
    }
    dprintf(fd, "PackageSize: %zu\n", static_cast<size_t>(checkResult.newVersionInfo.versionComponents[0].size));
    dprintf(fd, "ComponentType: %d\n", checkResult.newVersionInfo.versionComponents[0].componentType);
    dprintf(fd, "UpgradeAction: %s\n", checkResult.newVersionInfo.versionComponents[0].upgradeAction.c_str());
    dprintf(fd, "DisplayVersion: %s\n", checkResult.newVersionInfo.versionComponents[0].displayVersion.c_str());
    dprintf(fd, "InnerVersion: %s\n", checkResult.newVersionInfo.versionComponents[0].innerVersion.c_str());
    dprintf(fd, "Content: %s\n", checkResult.newVersionInfo.versionComponents[0].descriptionInfo.content.c_str());
}

void BuildTaskInfoDump(const int fd)
{
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service == nullptr) {
        ENGINE_LOGI("BuildTaskInfoDump no instance");
        return;
    }

    TaskInfo taskInfo;
    BusinessError businessError;
    UpgradeInfo upgradeInfo;
    service->GetTaskInfo(upgradeInfo, taskInfo, businessError);
    if (!taskInfo.existTask) {
        dprintf(fd, "TaskInfo is empty\n");
        return;
    }

    dprintf(fd, "---------------------OTA status info--------------------\n");
    dprintf(fd, "Progress: %d\n", taskInfo.taskBody.progress);
    dprintf(fd, "UpgradeStatus: %d\n", taskInfo.taskBody.status);
    dprintf(fd, "SubStatus: %d\n", taskInfo.taskBody.subStatus);
    for (auto &iter : taskInfo.taskBody.errorMessages) {
        dprintf(fd, "ErrorCode: %d\n", iter.errorCode);
        dprintf(fd, "ErrorMsg: %s\n", iter.errorMessage.c_str());
    }
}

void UpdateService::DumpUpgradeCallback(const int fd)
{
    dprintf(fd, "---------------------callback info--------------------\n");
    for (const auto &iter : clientProxyMap_) {
        const UpgradeInfo& info = iter.first;
        dprintf(fd, "%s\n", info.ToString().c_str());
    }
}

int UpdateService::Dump(int fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        ENGINE_LOGI("HiDumper handle invalid");
        return -1;
    }

    if (args.size() == 0) {
        UpgradeInfo upgradeInfo = UpdateServiceCache::GetUpgradeInfo(BusinessSubType::FIRMWARE);
        BuildUpgradeInfoDump(fd, upgradeInfo);
        BuildTaskInfoDump(fd);
        DumpUpgradeCallback(fd);
    } else {
        dprintf(fd, "input error, no parameters required\n");
    }
    return 0;
}

void UpdateService::OnStart()
{
    ENGINE_LOGI("UpdaterService OnStart");
    updateService_ = this;
    if (updateService_ == nullptr) {
        ENGINE_LOGE("updateService_ null");
    }

    DelayedSingleton<NetManager>::GetInstance()->Init();

    // 动态启停流程启动
    DelayedSingleton<StartupManager>::GetInstance()->Start();

    if (Publish(this)) {
        ENGINE_LOGI("UpdaterService OnStart publish success");
    } else {
        ENGINE_LOGI("UpdaterService OnStart publish fail");
    }
}

void UpdateService::OnStop()
{
    ENGINE_LOGI("UpdaterService OnStop");
}
} // namespace UpdateEngine
} // namespace OHOS
