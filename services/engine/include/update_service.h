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

#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include <iostream>
#include <thread>

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iremote_stub.h"
#include "system_ability.h"

#include "update_helper.h"
#include "update_service_impl_manager.h"
#include "update_service_stub.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateService : public SystemAbility, public UpdateServiceStub {
public:
    DECLARE_SYSTEM_ABILITY(UpdateService);
    DISALLOW_COPY_AND_MOVE(UpdateService);
    explicit UpdateService(int32_t systemAbilityId, bool runOnCreate = true);
    ~UpdateService() override;

    int32_t RegisterUpdateCallback(const UpgradeInfo &info, const sptr<IUpdateCallback> &updateCallback) override;

    int32_t UnregisterUpdateCallback(const UpgradeInfo &info) override;

    int32_t CheckNewVersion(const UpgradeInfo &info) override;

    int32_t Download(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const DownloadOptions &downloadOptions, BusinessError &businessError) override;

    int32_t PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError) override;

    int32_t ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError) override;

    int32_t Upgrade(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
        const UpgradeOptions &upgradeOptions, BusinessError &businessError) override;

    int32_t ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
        const ClearOptions &clearOptions, BusinessError &businessError) override;

    int32_t TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError) override;

    int32_t GetNewVersionInfo(
        const UpgradeInfo &info, NewVersionInfo &newVersionInfo, BusinessError &businessError) override;

    int32_t GetNewVersionDescription(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &newVersionDescriptionInfo,
        BusinessError &businessError) override;

    int32_t GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
        BusinessError &businessError) override;

    int32_t GetCurrentVersionDescription(const UpgradeInfo &info, const DescriptionOptions &descriptionOptions,
        VersionDescriptionInfo &currentVersionDescriptionInfo, BusinessError &businessError) override;

    int32_t GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError) override;

    int32_t SetUpgradePolicy(const UpgradeInfo &info, const UpgradePolicy &policy,
        BusinessError &businessError) override;

    int32_t GetUpgradePolicy(const UpgradeInfo &info, UpgradePolicy &policy, BusinessError &businessError) override;

    int32_t Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError) override;

    int32_t FactoryReset(BusinessError &businessError) override;

    int32_t ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile, const std::string &packageName,
        BusinessError &businessError) override;

    int32_t VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
        BusinessError &businessError) override;

    int Dump(int fd, const std::vector<std::u16string> &args) override;

    static sptr<UpdateService> GetInstance();

    sptr<IUpdateCallback> GetUpgradeCallback(const UpgradeInfo &info);

#ifndef UPDATER_UT
protected:
#endif
    void OnStart() override;
    void OnStop() override;

private:
    void DumpUpgradeCallback(const int fd);

#ifndef UPDATER_UT
private:
#else
public:
#endif
    class ClientDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        ClientDeathRecipient(const UpgradeInfo &upgradeInfo) : upgradeInfo_(upgradeInfo) {}
        ~ClientDeathRecipient() final {}
        DISALLOW_COPY_AND_MOVE(ClientDeathRecipient);
        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    private:
        UpgradeInfo upgradeInfo_;
    };

    class ClientProxy {
    public:
        ClientProxy(const UpgradeInfo &info, const sptr<IUpdateCallback> &callback);
        void AddDeathRecipient();
        void RemoveDeathRecipient();
        sptr<IUpdateCallback> Get();
    private:
        sptr<IUpdateCallback> proxy_;
        sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    };

private:
    std::mutex clientProxyMapLock_;
    std::map<UpgradeInfo, ClientProxy> clientProxyMap_ {};
    static sptr<UpdateService> updateService_;
    std::shared_ptr<UpdateServiceImplManager> updateImplMgr_ = nullptr;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_H