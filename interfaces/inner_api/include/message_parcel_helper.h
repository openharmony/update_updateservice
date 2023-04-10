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

#ifndef MESSAGE_PARCEL_HELPER_H
#define MESSAGE_PARCEL_HELPER_H

#include <string>

#include "parcel.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class MessageParcelHelper {
public:
    static int32_t ReadUpgradeInfo(MessageParcel &reply, UpgradeInfo &info);
    static int32_t WriteUpgradeInfo(MessageParcel &data, const UpgradeInfo &info);

    static int32_t ReadVersionDescriptionInfo(MessageParcel &reply, VersionDescriptionInfo &newVersionDescriptionInfo);
    static int32_t WriteVersionDescriptionInfo(MessageParcel &data,
        const VersionDescriptionInfo &newVersionDescriptionInfo);

    static int32_t ReadBusinessError(MessageParcel &reply, BusinessError &businessError);
    static int32_t WriteBusinessError(MessageParcel &data, const BusinessError &businessError);

    static int32_t ReadCheckResult(MessageParcel &reply, CheckResult &checkResult);
    static int32_t WriteCheckResult(MessageParcel &data, const CheckResult &checkResult);

    static int32_t ReadNewVersionInfo(MessageParcel &reply, NewVersionInfo &newVersionInfo);
    static int32_t WriteNewVersionInfo(MessageParcel &data, const NewVersionInfo &newVersionInfo);

    static int32_t ReadCurrentVersionInfo(MessageParcel &reply, CurrentVersionInfo &info);
    static int32_t WriteCurrentVersionInfo(MessageParcel &data, const CurrentVersionInfo &info);

    static int32_t ReadTaskInfo(MessageParcel &reply, TaskInfo &info);
    static int32_t WriteTaskInfo(MessageParcel &data, const TaskInfo &info);

    static int32_t ReadUpgradePolicy(MessageParcel &reply, UpgradePolicy &policy);
    static int32_t WriteUpgradePolicy(MessageParcel &data, const UpgradePolicy &policy);

    static int32_t ReadEventInfo(MessageParcel &reply, EventInfo &eventInfo);
    static int32_t WriteEventInfo(MessageParcel &data, const EventInfo &eventInfo);

    static int32_t ReadVersionDigestInfo(MessageParcel &reply, VersionDigestInfo &versionDigestInfo);
    static int32_t WriteVersionDigestInfo(MessageParcel &data, const VersionDigestInfo &versionDigestInfo);

    static int32_t ReadDescriptionOptions(MessageParcel &reply, DescriptionOptions &descriptionOptions);
    static int32_t WriteDescriptionOptions(MessageParcel &data, const DescriptionOptions &descriptionOptions);

    static int32_t ReadDownloadOptions(MessageParcel &reply, DownloadOptions &downloadOptions);
    static int32_t WriteDownloadOptions(MessageParcel &data, const DownloadOptions &downloadOptions);

    static int32_t ReadPauseDownloadOptions(MessageParcel &reply, PauseDownloadOptions &pauseDownloadOptions);
    static int32_t WritePauseDownloadOptions(MessageParcel &data, const PauseDownloadOptions &pauseDownloadOptions);

    static int32_t ReadResumeDownloadOptions(MessageParcel &reply, ResumeDownloadOptions &resumeDownloadOptions);
    static int32_t WriteResumeDownloadOptions(MessageParcel &data, const ResumeDownloadOptions &resumeDownloadOptions);

    static int32_t ReadUpgradeOptions(MessageParcel &reply, UpgradeOptions &upgradeOptions);
    static int32_t WriteUpgradeOptions(MessageParcel &data, const UpgradeOptions &upgradeOptions);

    static int32_t ReadClearOptions(MessageParcel &reply, ClearOptions &clearOptions);
    static int32_t WriteClearOptions(MessageParcel &data, const ClearOptions &clearOptions);

    static int32_t CompareVersion(const std::string &version1, const std::string &version2);
    static std::vector<std::string> SplitString(const std::string &str, const std::string &delimiter);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // MESSAGE_PARCEL_HELPER_H
