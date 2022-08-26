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

#include "update_helper.h"

#include <cstring>
#include <string>

namespace OHOS {
namespace UpdateEngine {
#ifdef UPDATE_SERVICE
const std::string LOG_LABEL = "update_engine";
const std::string LOG_NAME = "/data/ota_package/update_service_log.txt";
#else
const std::string LOG_LABEL = "update_engine";
const std::string LOG_NAME = "/data/ota_package/update_client.log.txt";
#endif
constexpr int HEX_DIGEST_NUM = 2;
constexpr int HEX_DIGEST_BASE = 16;
constexpr int DATA_ANONYMIZATION_LEN = 4;

UpdateLogLevel UpdateHelper::level_ = UpdateLogLevel::UPDATE_INFO;

std::string UpgradeInfo::ToString() const
{
    std::string output = "upgradeApp:" + upgradeApp;
    output += ",businessType(vender:" + businessType.vendor;
    output += ",subType:" + std::to_string(CAST_INT(businessType.subType));
    output += "),upgradeDevId:" + UpdateHelper::Anonymization(upgradeDevId);
    output += ",controlDevId:" + UpdateHelper::Anonymization(controlDevId);
    return output;
}

void ReadErrorMessages(MessageParcel &reply, ErrorMessage errorMessages[], size_t arraySize)
{
    int32_t size = reply.ReadInt32();
    for (size_t i = 0; (i < static_cast<size_t>(size)) && (i < arraySize); i++) {
        errorMessages[i].errorCode = reply.ReadInt32();
        errorMessages[i].errorMessage = Str16ToStr8(reply.ReadString16());
    }
}

void WriteErrorMessages(MessageParcel &data, const ErrorMessage errorMessages[], size_t arraySize)
{
    data.WriteInt32(static_cast<int32_t>(arraySize));
    for (size_t i = 0; i < arraySize; i++) {
        data.WriteInt32(errorMessages[i].errorCode);
        data.WriteString16(Str8ToStr16(errorMessages[i].errorMessage));
    }
}

void ReadComponentDescriptions(MessageParcel &reply, ComponentDescription componentDescriptions[], size_t arraySize)
{
    int32_t size = reply.ReadInt32();
    for (size_t i = 0; (i < static_cast<size_t>(size)) && (i < arraySize); i++) {
        componentDescriptions[i].componentId = Str16ToStr8(reply.ReadString16());
        componentDescriptions[i].descriptionInfo.descriptionType = static_cast<DescriptionType>(reply.ReadUint32());
        componentDescriptions[i].descriptionInfo.content = Str16ToStr8(reply.ReadString16());
    }
}

void WriteComponentDescriptions(MessageParcel &data, const ComponentDescription componentDescriptions[],
    size_t arraySize)
{
    data.WriteInt32(static_cast<int32_t>(arraySize));
    for (size_t i = 0; i < arraySize; i++) {
        data.WriteString16(Str8ToStr16(componentDescriptions[i].componentId));
        data.WriteUint32(static_cast<uint32_t>(componentDescriptions[i].descriptionInfo.descriptionType));
        data.WriteString16(Str8ToStr16(componentDescriptions[i].descriptionInfo.content));
    }
}

int32_t UpdateHelper::ReadUpgradeInfo(MessageParcel &reply, UpgradeInfo &info)
{
    info.upgradeApp = Str16ToStr8(reply.ReadString16());
    info.businessType.vendor = Str16ToStr8(reply.ReadString16());
    info.businessType.subType = static_cast<BusinessSubType>(reply.ReadInt32());
    info.upgradeDevId = Str16ToStr8(reply.ReadString16());
    info.controlDevId = Str16ToStr8(reply.ReadString16());
    return 0;
}

int32_t UpdateHelper::WriteUpgradeInfo(MessageParcel &data, const UpgradeInfo &info)
{
    data.WriteString16(Str8ToStr16(info.upgradeApp));
    data.WriteString16(Str8ToStr16(info.businessType.vendor));
    data.WriteInt32(static_cast<int32_t>(info.businessType.subType));
    data.WriteString16(Str8ToStr16(info.upgradeDevId));
    data.WriteString16(Str8ToStr16(info.controlDevId));
    return 0;
}

int32_t UpdateHelper::ReadVersionDescriptionInfo(MessageParcel &reply,
    VersionDescriptionInfo &versionDescriptionInfo)
{
    ReadComponentDescriptions(reply, versionDescriptionInfo.componentDescriptions,
        COUNT_OF(versionDescriptionInfo.componentDescriptions));
    return 0;
}

int32_t UpdateHelper::WriteVersionDescriptionInfo(MessageParcel &data,
    const VersionDescriptionInfo &versionDescriptionInfo)
{
    WriteComponentDescriptions(data, versionDescriptionInfo.componentDescriptions,
        COUNT_OF(versionDescriptionInfo.componentDescriptions));
    return 0;
}

int32_t UpdateHelper::ReadBusinessError(MessageParcel &reply, BusinessError &businessError)
{
    businessError.message = Str16ToStr8(reply.ReadString16());
    businessError.errorNum = static_cast<CallResult>(reply.ReadInt32());
    ReadErrorMessages(reply, businessError.data, COUNT_OF(businessError.data));
    return 0;
}

int32_t UpdateHelper::WriteBusinessError(MessageParcel &data, const BusinessError &businessError)
{
    data.WriteString16(Str8ToStr16(businessError.message));
    data.WriteInt32(static_cast<int32_t>(businessError.errorNum));
    WriteErrorMessages(data, businessError.data, COUNT_OF(businessError.data));
    return 0;
}

void ReadVersionComponents(MessageParcel &reply, VersionComponent versionComponents[], size_t arraySize)
{
    int32_t size = reply.ReadInt32();
    for (size_t i = 0; (i < static_cast<size_t>(size)) && (i < arraySize); i++) {
        VersionComponent *versionComponent = &versionComponents[i];
        versionComponent->componentId = Str16ToStr8(reply.ReadString16());
        versionComponent->componentType = reply.ReadUint32();
        versionComponent->upgradeAction = Str16ToStr8(reply.ReadString16());
        versionComponent->displayVersion = Str16ToStr8(reply.ReadString16());
        versionComponent->innerVersion = Str16ToStr8(reply.ReadString16());
        versionComponent->size = static_cast<size_t>(reply.ReadUint32());
        versionComponent->effectiveMode = static_cast<size_t>(reply.ReadUint32());

        versionComponent->descriptionInfo.descriptionType = static_cast<DescriptionType>(reply.ReadUint32());
        versionComponent->descriptionInfo.content = Str16ToStr8(reply.ReadString16());
    }
}

void WriteVersionComponents(MessageParcel &data, const VersionComponent versionComponents[], size_t arraySize)
{
    data.WriteInt32(static_cast<int32_t>(arraySize));
    for (size_t i = 0; i < arraySize; i++) {
        const VersionComponent *versionComponent = &versionComponents[i];
        data.WriteString16(Str8ToStr16(versionComponent->componentId));
        data.WriteUint32(versionComponent->componentType);
        data.WriteString16(Str8ToStr16(versionComponent->upgradeAction));
        data.WriteString16(Str8ToStr16(versionComponent->displayVersion));
        data.WriteString16(Str8ToStr16(versionComponent->innerVersion));
        data.WriteUint32(static_cast<uint32_t>(versionComponent->size));
        data.WriteUint32(static_cast<uint32_t>(versionComponent->effectiveMode));

        data.WriteUint32(static_cast<uint32_t>(versionComponent->descriptionInfo.descriptionType));
        data.WriteString16(Str8ToStr16(versionComponent->descriptionInfo.content));
    }
}

void ReadNewVersionInfoEx(MessageParcel &reply, NewVersionInfo &newVersionInfo)
{
    newVersionInfo.versionDigestInfo.versionDigest = Str16ToStr8(reply.ReadString16());
    ReadVersionComponents(reply, newVersionInfo.versionComponents, COUNT_OF(newVersionInfo.versionComponents));
}

void WriteNewVersionInfoEx(MessageParcel &data, const NewVersionInfo &newVersionInfo)
{
    data.WriteString16(Str8ToStr16(newVersionInfo.versionDigestInfo.versionDigest));
    WriteVersionComponents(data, newVersionInfo.versionComponents, COUNT_OF(newVersionInfo.versionComponents));
}

int32_t UpdateHelper::ReadCheckResult(MessageParcel &reply, CheckResultEx &checkResultEx)
{
    checkResultEx.isExistNewVersion = reply.ReadBool();
    ReadNewVersionInfoEx(reply, checkResultEx.newVersionInfo);
    return 0;
}

int32_t UpdateHelper::WriteCheckResult(MessageParcel &data, const CheckResultEx &checkResultEx)
{
    data.WriteBool(checkResultEx.isExistNewVersion);
    WriteNewVersionInfoEx(data, checkResultEx.newVersionInfo);
    return 0;
}

int32_t UpdateHelper::ReadNewVersionInfo(MessageParcel &reply, NewVersionInfo &newVersionInfo)
{
    ReadNewVersionInfoEx(reply, newVersionInfo);
    return 0;
}

int32_t UpdateHelper::WriteNewVersionInfo(MessageParcel &data, const NewVersionInfo &newVersionInfo)
{
    WriteNewVersionInfoEx(data, newVersionInfo);
    return 0;
}

int32_t UpdateHelper::ReadCurrentVersionInfo(MessageParcel &reply, CurrentVersionInfo &info)
{
    ENGINE_LOGI("ReadCurrentVersionInfo");
    info.osVersion = Str16ToStr8(reply.ReadString16());
    info.deviceName = Str16ToStr8(reply.ReadString16());
    ReadVersionComponents(reply, info.versionComponents, COUNT_OF(info.versionComponents));
    return 0;
}

int32_t UpdateHelper::WriteCurrentVersionInfo(MessageParcel &data, const CurrentVersionInfo &info)
{
    ENGINE_LOGI("WriteCurrentVersionInfo");
    data.WriteString16(Str8ToStr16(info.osVersion));
    data.WriteString16(Str8ToStr16(info.deviceName));
    WriteVersionComponents(data, info.versionComponents, COUNT_OF(info.versionComponents));
    return 0;
}

void ReadTaskBody(MessageParcel &reply, TaskBody &taskBody)
{
    taskBody.versionDigestInfo.versionDigest = Str16ToStr8(reply.ReadString16());
    taskBody.status = static_cast<UpgradeStatus>(reply.ReadInt32());
    taskBody.subStatus = reply.ReadInt32();
    taskBody.progress = reply.ReadUint32();
    taskBody.installMode = reply.ReadInt32();
    ReadErrorMessages(reply, taskBody.errorMessages, COUNT_OF(taskBody.errorMessages));
    ReadVersionComponents(reply, taskBody.versionComponents, COUNT_OF(taskBody.versionComponents));
}

void WriteTaskBody(MessageParcel &data, const TaskBody &taskBody)
{
    data.WriteString16(Str8ToStr16(taskBody.versionDigestInfo.versionDigest));
    data.WriteInt32(static_cast<int32_t>(taskBody.status));
    data.WriteInt32(taskBody.subStatus);
    data.WriteUint32(taskBody.progress);
    data.WriteInt32(taskBody.installMode);
    WriteErrorMessages(data, taskBody.errorMessages, COUNT_OF(taskBody.errorMessages));
    WriteVersionComponents(data, taskBody.versionComponents, COUNT_OF(taskBody.versionComponents));
}

int32_t UpdateHelper::ReadTaskInfo(MessageParcel &reply, TaskInfo &info)
{
    ENGINE_LOGI("ReadTaskInfo");
    info.existTask = reply.ReadBool();
    ReadTaskBody(reply, info.taskBody);
    return 0;
}

int32_t UpdateHelper::WriteTaskInfo(MessageParcel &data, const TaskInfo &info)
{
    ENGINE_LOGI("WriteTaskInfo");
    data.WriteBool(info.existTask);
    WriteTaskBody(data, info.taskBody);
    return 0;
}

int32_t UpdateHelper::ReadUpgradePolicy(MessageParcel &reply, UpgradePolicy &policy)
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

int32_t UpdateHelper::WriteUpgradePolicy(MessageParcel &data, const UpgradePolicy &policy)
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

int32_t UpdateHelper::ReadEventInfo(MessageParcel &reply, EventInfo &eventInfo)
{
    eventInfo.eventId = static_cast<EventId>(reply.ReadUint32());
    ReadTaskBody(reply, eventInfo.taskBody);
    return 0;
}

int32_t UpdateHelper::WriteEventInfo(MessageParcel &data, const EventInfo &eventInfo)
{
    data.WriteUint32(static_cast<uint32_t>(eventInfo.eventId));
    WriteTaskBody(data, eventInfo.taskBody);
    return 0;
}

int32_t UpdateHelper::ReadVersionDigestInfo(MessageParcel &reply, VersionDigestInfo &versionDigestInfo)
{
    versionDigestInfo.versionDigest = Str16ToStr8(reply.ReadString16());
    return 0;
}

int32_t UpdateHelper::WriteVersionDigestInfo(MessageParcel &data, const VersionDigestInfo &versionDigestInfo)
{
    data.WriteString16(Str8ToStr16(versionDigestInfo.versionDigest));
    return 0;
}

int32_t UpdateHelper::ReadDescriptionOptions(MessageParcel &reply, DescriptionOptions &descriptionOptions)
{
    descriptionOptions.format = static_cast<DescriptionFormat>(reply.ReadUint32());
    descriptionOptions.language = Str16ToStr8(reply.ReadString16());
    return 0;
}

int32_t UpdateHelper::WriteDescriptionOptions(MessageParcel &data, const DescriptionOptions &descriptionOptions)
{
    data.WriteUint32(static_cast<uint32_t>(descriptionOptions.format));
    data.WriteString16(Str8ToStr16(descriptionOptions.language));
    return 0;
}

int32_t UpdateHelper::ReadDownloadOptions(MessageParcel &reply, DownloadOptions &downloadOptions)
{
    downloadOptions.allowNetwork = static_cast<NetType>(reply.ReadUint32());
    downloadOptions.order = static_cast<Order>(reply.ReadUint32());
    return 0;
}

int32_t UpdateHelper::WriteDownloadOptions(MessageParcel &data, const DownloadOptions &downloadOptions)
{
    data.WriteUint32(static_cast<uint32_t>(downloadOptions.allowNetwork));
    data.WriteUint32(static_cast<uint32_t>(downloadOptions.order));
    return 0;
}

int32_t UpdateHelper::ReadPauseDownloadOptions(MessageParcel &reply, PauseDownloadOptions &pauseDownloadOptions)
{
    pauseDownloadOptions.isAllowAutoResume = reply.ReadBool();
    return 0;
}

int32_t UpdateHelper::WritePauseDownloadOptions(MessageParcel &data, const PauseDownloadOptions &pauseDownloadOptions)
{
    data.WriteBool(pauseDownloadOptions.isAllowAutoResume);
    return 0;
}

int32_t UpdateHelper::ReadResumeDownloadOptions(MessageParcel &reply, ResumeDownloadOptions &resumeDownloadOptions)
{
    resumeDownloadOptions.allowNetwork = static_cast<NetType>(reply.ReadUint32());
    return 0;
}

int32_t UpdateHelper::WriteResumeDownloadOptions(MessageParcel &data,
    const ResumeDownloadOptions &resumeDownloadOptions)
{
    data.WriteUint32(static_cast<uint32_t>(resumeDownloadOptions.allowNetwork));
    return 0;
}

int32_t UpdateHelper::ReadUpgradeOptions(MessageParcel &reply, UpgradeOptions &upgradeOptions)
{
    upgradeOptions.order = static_cast<Order>(reply.ReadUint32());
    return 0;
}

int32_t UpdateHelper::WriteUpgradeOptions(MessageParcel &data, const UpgradeOptions &upgradeOptions)
{
    data.WriteUint32(static_cast<uint32_t>(upgradeOptions.order));
    return 0;
}

int32_t UpdateHelper::ReadClearOptions(MessageParcel &reply, ClearOptions &clearOptions)
{
    clearOptions.status = static_cast<UpgradeStatus>(reply.ReadUint32());
    return 0;
}

int32_t UpdateHelper::WriteClearOptions(MessageParcel &data, const ClearOptions &clearOptions)
{
    data.WriteUint32(static_cast<uint32_t>(clearOptions.status));
    return 0;
}

bool UpdateHelper::JudgeLevel(const UpdateLogLevel& level)
{
    const UpdateLogLevel& curLevel = UpdateHelper::GetLogLevel();
    if (level <= curLevel) {
        return true;
    }
    return true;
}

std::string UpdateHelper::GetBriefFileName(const std::string &file)
{
    auto pos = file.find_last_of("/");
    if (pos != std::string::npos) {
        return file.substr(pos + 1);
    }

    pos = file.find_last_of("\\");
    if (pos != std::string::npos) {
        return file.substr(pos + 1);
    }

    return file;
}

std::vector<std::string> UpdateHelper::SplitString(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> result;
    ENGINE_CHECK(!str.empty(), return result, "string is empty");

    size_t found = std::string::npos;
    size_t start = 0;
    while (true) {
        found = str.find_first_of(delimiter, start);
        result.push_back(str.substr(start, found - start));
        if (found == std::string::npos) {
            break;
        }
        start = found + 1;
    }
    return result;
}

bool UpdateHelper::IsErrorExist(const BusinessError &businessError)
{
    return businessError.errorNum != CallResult::SUCCESS;
}

int32_t UpdateHelper::CompareVersion(const std::string &version1, const std::string &version2)
{
    std::vector<std::string> result1 = SplitString(version1, ".");
    std::vector<std::string> result2 = SplitString(version2, ".");
    if (result1.size() != result2.size()) {
        return ((result1.size() > result2.size()) ? -1 : 1);
    }

    for (size_t i = 1; i < result1.size(); i++) {
        long long ver1 = std::stoll(result1[i]);
        long long ver2 = std::stoll(result2[i]);
        if (ver1 == ver2) {
            continue;
        }
        return ((ver1 > ver2) ? 1 : -1);
    }
    return 0;
}

std::vector<uint8_t> UpdateHelper::HexToDegist(const std::string &str)
{
    std::vector<uint8_t> result;
    for (size_t i = 0; i < str.length(); i += HEX_DIGEST_NUM) {
        std::string byte = str.substr(i, HEX_DIGEST_NUM);
        long byteToLong = strtol(byte.c_str(), nullptr, HEX_DIGEST_BASE);
        if (byteToLong == 0) {
            return result;
        }
        auto chr = static_cast<uint8_t>(static_cast<int>(byteToLong));
        result.push_back(chr);
    }
    return result;
}

std::string UpdateHelper::Anonymization(const std::string &src)
{
    int len = static_cast<int>(src.length());
    if (len <= DATA_ANONYMIZATION_LEN) {
        return std::string("");
    }
    return std::string("*****") + src.substr(0, DATA_ANONYMIZATION_LEN);
}

std::string UpdateHelper::BuildEventVersionInfo(const VersionInfo &ver)
{
    return std::string("{") + std::string("pkgSize: ") + std::to_string(ver.result[0].size)
        + std::string(", packageType: ") + std::to_string(ver.result[0].packageType)
        + std::string(", versionCode: ") + ver.result[0].versionCode + std::string(" }");
}

std::string UpdateHelper::BuildEventDevId(const UpgradeInfo &info)
{
    return std::string("{") + std::string("upgradeDevId: ") + info.upgradeDevId
        + std::string(", controlDevId: ") + info.controlDevId + std::string(" }");
}
} // namespace UpdateEngine
} // namespace OHOS
