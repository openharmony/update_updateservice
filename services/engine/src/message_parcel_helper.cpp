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

#include "message_parcel_helper.h"

#include <cstring>
#include <string>
#include <vector>

#include "update_helper.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
void ReadErrorMessages(MessageParcel &reply, std::vector<ErrorMessage> &errorMessages)
{
    int32_t size = reply.ReadInt32();
    for (size_t i = 0; i < static_cast<size_t>(size); i++) {
        ErrorMessage errorMsg;
        errorMsg.errorCode = reply.ReadInt32();
        errorMsg.errorMessage = Str16ToStr8(reply.ReadString16());
        errorMessages.push_back(errorMsg);
    }
}

void WriteErrorMessages(MessageParcel &data, const std::vector<ErrorMessage> errorMessages)
{
    data.WriteInt32(static_cast<int32_t>(errorMessages.size()));
    for (size_t i = 0; i < errorMessages.size(); i++) {
        data.WriteInt32(errorMessages[i].errorCode);
        data.WriteString16(Str8ToStr16(errorMessages[i].errorMessage));
    }
}

void ReadComponentDescriptions(MessageParcel &reply, std::vector<ComponentDescription> &componentDescriptions)
{
    int32_t size = reply.ReadInt32();
    for (size_t i = 0; i < static_cast<size_t>(size); i++) {
        ComponentDescription componentDescription;
        componentDescription.componentId = Str16ToStr8(reply.ReadString16());
        componentDescription.descriptionInfo.descriptionType = static_cast<DescriptionType>(reply.ReadUint32());
        componentDescription.descriptionInfo.content = Str16ToStr8(reply.ReadString16());
        componentDescriptions.push_back(componentDescription);
    }
}

void WriteComponentDescriptions(MessageParcel &data, const std::vector<ComponentDescription> &componentDescriptions)
{
    data.WriteInt32(static_cast<int32_t>(componentDescriptions.size()));
    for (size_t i = 0; i < componentDescriptions.size(); i++) {
        data.WriteString16(Str8ToStr16(componentDescriptions[i].componentId));
        data.WriteUint32(static_cast<uint32_t>(componentDescriptions[i].descriptionInfo.descriptionType));
        data.WriteString16(Str8ToStr16(componentDescriptions[i].descriptionInfo.content));
    }
}

int32_t MessageParcelHelper::ReadUpgradeInfo(MessageParcel &reply, UpgradeInfo &info)
{
    info.upgradeApp = Str16ToStr8(reply.ReadString16());
    info.businessType.vendor = Str16ToStr8(reply.ReadString16());
    info.businessType.subType = static_cast<BusinessSubType>(reply.ReadInt32());
    info.upgradeDevId = Str16ToStr8(reply.ReadString16());
    info.controlDevId = Str16ToStr8(reply.ReadString16());
    return 0;
}

int32_t MessageParcelHelper::WriteUpgradeInfo(MessageParcel &data, const UpgradeInfo &info)
{
    data.WriteString16(Str8ToStr16(info.upgradeApp));
    data.WriteString16(Str8ToStr16(info.businessType.vendor));
    data.WriteInt32(static_cast<int32_t>(info.businessType.subType));
    data.WriteString16(Str8ToStr16(info.upgradeDevId));
    data.WriteString16(Str8ToStr16(info.controlDevId));
    return 0;
}

int32_t MessageParcelHelper::ReadVersionDescriptionInfo(
    MessageParcel &reply, VersionDescriptionInfo &versionDescriptionInfo)
{
    ReadComponentDescriptions(reply, versionDescriptionInfo.componentDescriptions);
    return 0;
}

int32_t MessageParcelHelper::WriteVersionDescriptionInfo(
    MessageParcel &data, const VersionDescriptionInfo &versionDescriptionInfo)
{
    WriteComponentDescriptions(data, versionDescriptionInfo.componentDescriptions);
    return 0;
}

int32_t MessageParcelHelper::ReadBusinessError(MessageParcel &reply, BusinessError &businessError)
{
    businessError.message = Str16ToStr8(reply.ReadString16());
    businessError.errorNum = static_cast<CallResult>(reply.ReadInt32());
    ReadErrorMessages(reply, businessError.data);
    return 0;
}

int32_t MessageParcelHelper::WriteBusinessError(MessageParcel &data, const BusinessError &businessError)
{
    data.WriteString16(Str8ToStr16(businessError.message));
    data.WriteInt32(static_cast<int32_t>(businessError.errorNum));
    WriteErrorMessages(data, businessError.data);
    return 0;
}

void ReadVersionComponents(MessageParcel &reply, std::vector<VersionComponent> &versionComponents)
{
    int32_t size = reply.ReadInt32();
    for (size_t i = 0; i < static_cast<size_t>(size); i++) {
        VersionComponent versionComponent;
        versionComponent.componentId = Str16ToStr8(reply.ReadString16());
        versionComponent.componentType = reply.ReadInt32();
        versionComponent.upgradeAction = Str16ToStr8(reply.ReadString16());
        versionComponent.displayVersion = Str16ToStr8(reply.ReadString16());
        versionComponent.innerVersion = Str16ToStr8(reply.ReadString16());
        versionComponent.size = static_cast<size_t>(reply.ReadUint32());
        versionComponent.effectiveMode = static_cast<size_t>(reply.ReadUint32());

        versionComponent.descriptionInfo.descriptionType = static_cast<DescriptionType>(reply.ReadUint32());
        versionComponent.descriptionInfo.content = Str16ToStr8(reply.ReadString16());
        versionComponent.componentExtra = Str16ToStr8(reply.ReadString16());
        versionComponents.push_back(versionComponent);
    }
}

void WriteVersionComponents(MessageParcel &data, const std::vector<VersionComponent> &versionComponents)
{
    data.WriteInt32(static_cast<int32_t>(versionComponents.size()));
    for (size_t i = 0; i < versionComponents.size(); i++) {
        const VersionComponent *versionComponent = &versionComponents[i];
        data.WriteString16(Str8ToStr16(versionComponent->componentId));
        data.WriteInt32(versionComponent->componentType);
        data.WriteString16(Str8ToStr16(versionComponent->upgradeAction));
        data.WriteString16(Str8ToStr16(versionComponent->displayVersion));
        data.WriteString16(Str8ToStr16(versionComponent->innerVersion));
        data.WriteUint32(static_cast<uint32_t>(versionComponent->size));
        data.WriteUint32(static_cast<uint32_t>(versionComponent->effectiveMode));

        data.WriteUint32(static_cast<uint32_t>(versionComponent->descriptionInfo.descriptionType));
        data.WriteString16(Str8ToStr16(versionComponent->descriptionInfo.content));
        data.WriteString16(Str8ToStr16(versionComponent->componentExtra));
    }
}

void ReadNewVersionInfoEx(MessageParcel &reply, NewVersionInfo &newVersionInfo)
{
    newVersionInfo.versionDigestInfo.versionDigest = Str16ToStr8(reply.ReadString16());
    ReadVersionComponents(reply, newVersionInfo.versionComponents);
}

void WriteNewVersionInfoEx(MessageParcel &data, const NewVersionInfo &newVersionInfo)
{
    data.WriteString16(Str8ToStr16(newVersionInfo.versionDigestInfo.versionDigest));
    WriteVersionComponents(data, newVersionInfo.versionComponents);
}

int32_t MessageParcelHelper::ReadCheckResult(MessageParcel &reply, CheckResult &checkResult)
{
    checkResult.isExistNewVersion = reply.ReadBool();
    ReadNewVersionInfoEx(reply, checkResult.newVersionInfo);
    return 0;
}

int32_t MessageParcelHelper::WriteCheckResult(MessageParcel &data, const CheckResult &checkResult)
{
    data.WriteBool(checkResult.isExistNewVersion);
    WriteNewVersionInfoEx(data, checkResult.newVersionInfo);
    return 0;
}

int32_t MessageParcelHelper::ReadNewVersionInfo(MessageParcel &reply, NewVersionInfo &newVersionInfo)
{
    ReadNewVersionInfoEx(reply, newVersionInfo);
    return 0;
}

int32_t MessageParcelHelper::WriteNewVersionInfo(MessageParcel &data, const NewVersionInfo &newVersionInfo)
{
    WriteNewVersionInfoEx(data, newVersionInfo);
    return 0;
}

int32_t MessageParcelHelper::ReadCurrentVersionInfo(MessageParcel &reply, CurrentVersionInfo &info)
{
    info.osVersion = Str16ToStr8(reply.ReadString16());
    info.deviceName = Str16ToStr8(reply.ReadString16());
    ReadVersionComponents(reply, info.versionComponents);
    return 0;
}

int32_t MessageParcelHelper::WriteCurrentVersionInfo(MessageParcel &data, const CurrentVersionInfo &info)
{
    data.WriteString16(Str8ToStr16(info.osVersion));
    data.WriteString16(Str8ToStr16(info.deviceName));
    WriteVersionComponents(data, info.versionComponents);
    return 0;
}

void ReadTaskBody(MessageParcel &reply, TaskBody &taskBody)
{
    taskBody.versionDigestInfo.versionDigest = Str16ToStr8(reply.ReadString16());
    taskBody.status = static_cast<UpgradeStatus>(reply.ReadInt32());
    taskBody.subStatus = reply.ReadInt32();
    taskBody.progress = reply.ReadInt32();
    taskBody.installMode = reply.ReadInt32();
    ReadErrorMessages(reply, taskBody.errorMessages);
    ReadVersionComponents(reply, taskBody.versionComponents);
}

void WriteTaskBody(MessageParcel &data, const TaskBody &taskBody)
{
    data.WriteString16(Str8ToStr16(taskBody.versionDigestInfo.versionDigest));
    data.WriteInt32(static_cast<int32_t>(taskBody.status));
    data.WriteInt32(taskBody.subStatus);
    data.WriteInt32(taskBody.progress);
    data.WriteInt32(taskBody.installMode);
    WriteErrorMessages(data, taskBody.errorMessages);
    WriteVersionComponents(data, taskBody.versionComponents);
}

int32_t MessageParcelHelper::ReadTaskInfo(MessageParcel &reply, TaskInfo &info)
{
    info.existTask = reply.ReadBool();
    ReadTaskBody(reply, info.taskBody);
    return 0;
}

int32_t MessageParcelHelper::WriteTaskInfo(MessageParcel &data, const TaskInfo &info)
{
    data.WriteBool(info.existTask);
    WriteTaskBody(data, info.taskBody);
    return 0;
}

int32_t MessageParcelHelper::ReadUpgradePolicy(MessageParcel &reply, UpgradePolicy &policy)
{
    policy.downloadStrategy = static_cast<bool>(reply.ReadBool());
    policy.autoUpgradeStrategy = static_cast<bool>(reply.ReadBool());
    size_t size = static_cast<size_t>(reply.ReadInt32());
    size_t arraySize = COUNT_OF(policy.autoUpgradePeriods);
    for (size_t i = 0; (i < size) && (i < arraySize); i++) {
        policy.autoUpgradePeriods[i].start = reply.ReadUint32();
        policy.autoUpgradePeriods[i].end = reply.ReadUint32();
    }
    return 0;
}

int32_t MessageParcelHelper::WriteUpgradePolicy(MessageParcel &data, const UpgradePolicy &policy)
{
    data.WriteBool(policy.downloadStrategy);
    data.WriteBool(policy.autoUpgradeStrategy);
    int32_t size = static_cast<int32_t>(COUNT_OF(policy.autoUpgradePeriods));
    data.WriteInt32(size);
    for (int32_t i = 0; i < size; i++) {
        data.WriteUint32(policy.autoUpgradePeriods[i].start);
        data.WriteUint32(policy.autoUpgradePeriods[i].end);
    }
    return 0;
}

int32_t MessageParcelHelper::ReadEventInfo(MessageParcel &reply, EventInfo &eventInfo)
{
    eventInfo.eventId = static_cast<EventId>(reply.ReadUint32());
    ReadTaskBody(reply, eventInfo.taskBody);
    return 0;
}

int32_t MessageParcelHelper::WriteEventInfo(MessageParcel &data, const EventInfo &eventInfo)
{
    data.WriteUint32(static_cast<uint32_t>(eventInfo.eventId));
    WriteTaskBody(data, eventInfo.taskBody);
    return 0;
}

int32_t MessageParcelHelper::ReadVersionDigestInfo(MessageParcel &reply, VersionDigestInfo &versionDigestInfo)
{
    versionDigestInfo.versionDigest = Str16ToStr8(reply.ReadString16());
    return 0;
}

int32_t MessageParcelHelper::WriteVersionDigestInfo(MessageParcel &data, const VersionDigestInfo &versionDigestInfo)
{
    data.WriteString16(Str8ToStr16(versionDigestInfo.versionDigest));
    return 0;
}

int32_t MessageParcelHelper::ReadDescriptionOptions(MessageParcel &reply, DescriptionOptions &descriptionOptions)
{
    descriptionOptions.format = static_cast<DescriptionFormat>(reply.ReadUint32());
    descriptionOptions.language = Str16ToStr8(reply.ReadString16());
    return 0;
}

int32_t MessageParcelHelper::WriteDescriptionOptions(MessageParcel &data, const DescriptionOptions &descriptionOptions)
{
    data.WriteUint32(static_cast<uint32_t>(descriptionOptions.format));
    data.WriteString16(Str8ToStr16(descriptionOptions.language));
    return 0;
}

int32_t MessageParcelHelper::ReadDownloadOptions(MessageParcel &reply, DownloadOptions &downloadOptions)
{
    downloadOptions.allowNetwork = static_cast<NetType>(reply.ReadUint32());
    downloadOptions.order = static_cast<Order>(reply.ReadUint32());
    return 0;
}

int32_t MessageParcelHelper::WriteDownloadOptions(MessageParcel &data, const DownloadOptions &downloadOptions)
{
    data.WriteUint32(static_cast<uint32_t>(downloadOptions.allowNetwork));
    data.WriteUint32(static_cast<uint32_t>(downloadOptions.order));
    return 0;
}

int32_t MessageParcelHelper::ReadPauseDownloadOptions(MessageParcel &reply, PauseDownloadOptions &pauseDownloadOptions)
{
    pauseDownloadOptions.isAllowAutoResume = reply.ReadBool();
    return 0;
}

int32_t MessageParcelHelper::WritePauseDownloadOptions(
    MessageParcel &data, const PauseDownloadOptions &pauseDownloadOptions)
{
    data.WriteBool(pauseDownloadOptions.isAllowAutoResume);
    return 0;
}

int32_t MessageParcelHelper::ReadResumeDownloadOptions(
    MessageParcel &reply, ResumeDownloadOptions &resumeDownloadOptions)
{
    resumeDownloadOptions.allowNetwork = static_cast<NetType>(reply.ReadUint32());
    return 0;
}

int32_t MessageParcelHelper::WriteResumeDownloadOptions(
    MessageParcel &data, const ResumeDownloadOptions &resumeDownloadOptions)
{
    data.WriteUint32(static_cast<uint32_t>(resumeDownloadOptions.allowNetwork));
    return 0;
}

int32_t MessageParcelHelper::ReadUpgradeOptions(MessageParcel &reply, UpgradeOptions &upgradeOptions)
{
    upgradeOptions.order = static_cast<Order>(reply.ReadUint32());
    return 0;
}

int32_t MessageParcelHelper::WriteUpgradeOptions(MessageParcel &data, const UpgradeOptions &upgradeOptions)
{
    data.WriteUint32(static_cast<uint32_t>(upgradeOptions.order));
    return 0;
}

int32_t MessageParcelHelper::ReadClearOptions(MessageParcel &reply, ClearOptions &clearOptions)
{
    clearOptions.status = static_cast<UpgradeStatus>(reply.ReadUint32());
    return 0;
}

int32_t MessageParcelHelper::WriteClearOptions(MessageParcel &data, const ClearOptions &clearOptions)
{
    data.WriteUint32(static_cast<uint32_t>(clearOptions.status));
    return 0;
}
} // namespace UpdateEngine
} // namespace OHOS
