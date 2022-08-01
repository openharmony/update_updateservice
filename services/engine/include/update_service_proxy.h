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

#ifndef UPDATE_SERVICE_PROXY_H
#define UPDATE_SERVICE_PROXY_H

#include "iremote_proxy.h"
#include "iupdate_service.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceProxy : public IRemoteProxy<IUpdateService> {
public:
    explicit UpdateServiceProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IUpdateService>(impl) {}

    int32_t RegisterUpdateCallback(const UpgradeInfo &info, const sptr<IUpdateCallback>& updateCallback) override;

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

    int32_t GetNewVersion(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
        BusinessError &businessError) override;

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

private:
    static inline BrokerDelegator<UpdateServiceProxy> delegator_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_PROXY_H
