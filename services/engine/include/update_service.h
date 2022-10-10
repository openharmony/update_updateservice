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

#include "cJSON.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iremote_stub.h"
#include "iupdate_service.h"
#include "progress_thread.h"
#include "system_ability.h"
#include "update_helper.h"
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

    int32_t DownloadVersion(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const DownloadOptions &downloadOptions, BusinessError &businessError) override;

    int32_t PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError) override;

    int32_t ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError) override;

    int32_t DoUpdate(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
        const UpgradeOptions &upgradeOptions, BusinessError &businessError) override;

    int32_t ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
        const ClearOptions &clearOptions, BusinessError &businessError) override;

    int32_t TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError) override;

    int32_t GetNewVersion(
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

    void DownloadCallback(const Progress &progress);

    int Dump(int fd, const std::vector<std::u16string> &args) override;

    void SearchCallback(const std::string &msg, SearchStatus status);

    void SearchCallbackEx(BusinessError &businessError, CheckResultEx &checkResult);

    static sptr<UpdateService> GetInstance();
    sptr<IUpdateCallback> GetUpgradeCallback(const UpgradeInfo &info);

    void SetCheckInterval(uint64_t interval);
    void SetDownloadInterval(uint64_t interval);
    uint64_t GetCheckInterval();
    uint64_t GetDownloadInterval();
    void GetUpgradeContext(std::string &devIdInfo);

    static int32_t ParseJsonFile(const std::vector<char> &buffer, VersionInfo &info, std::string &url);
    static int32_t ReadCheckVersionResult(const cJSON* results, VersionInfo &info, std::string &url);
    static int32_t ReadCheckVersiondescriptInfo(const cJSON *descriptInfo, VersionInfo &info);

#ifndef UPDATER_UT
protected:
#endif
    void OnStart() override;
    void OnStop() override;

private:
    void DumpUpgradeCallback(const int fd);
    void GetCheckResult(CheckResultEx &checkResult);
    void UpgradeCallback(const Progress &progress);
    std::string GetDownloadServerUrl() const;
    void InitVersionInfo(VersionInfo &versionInfo) const;
    void SendEvent(const UpgradeInfo &upgradeInfo, EventId eventId);

#ifndef UPDATER_UT
private:
#else
public:
#endif
    bool VerifyDownloadPkg(const std::string &pkgName, Progress &progress);
    void ReadDataFromSSL(int32_t engineSocket);

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
        ClientProxy &operator=(const ClientProxy &source);
        void AddDeathRecipient();
        void RemoveDeathRecipient();
        sptr<IUpdateCallback> Get();
    private:
        sptr<IUpdateCallback> proxy_;
        sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    };

private:
    UpgradePolicy policy_ = {
        1, 1, {{10, 20}, {10, 20}}
    };
    UpgradeStatus upgradeStatus_ = UPDATE_STATE_INIT;
    VersionInfo versionInfo_ {};
    CheckResultEx checkResultEx_ {};
    OtaStatus otaStatus_ {};
    UpgradeInterval upgradeInterval_ {};
    uint64_t checkInterval_ = 0;
    uint64_t downloadInterval_ = 0;
    std::mutex clientProxyMapLock_;
    std::map<UpgradeInfo, ClientProxy> clientProxyMap_;
    DownloadThread *downloadThread_  { nullptr };
    UpgradeInfo upgradeInfo_ {};
    std::string downloadUrl_;
    static sptr<UpdateService> updateService_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_H
