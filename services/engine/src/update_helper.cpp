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

#include "encrypt_utils.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
std::string UpgradeInfo::ToString() const
{
    std::string output = "upgradeApp:" + upgradeApp;
    output += ",businessType(vender:" + businessType.vendor;
    output += ",subType:" + std::to_string(CAST_INT(businessType.subType));
    output += "),upgradeDevId:" + EncryptUtils::EncryptString(upgradeDevId);
    output += ",controlDevId:" + EncryptUtils::EncryptString(controlDevId);
    return output;
}

JsonBuilder BusinessType::GetJsonBuilder()
{
    return JsonBuilder()
        .Append("{")
        .Append("vendor", vendor)
        .Append("subType", CAST_INT(subType))
        .Append("}");
}

JsonBuilder VersionDigestInfo::GetJsonBuilder()
{
    return JsonBuilder().Append("{").Append("versionDigest", versionDigest).Append("}");
}

JsonBuilder ErrorMessage::GetJsonBuilder()
{
    return JsonBuilder()
        .Append("{")
        .Append("errorCode", errorCode)
        .Append("errorMessage", errorMessage)
        .Append("}");
}

JsonBuilder SubscribeInfo::GetJsonBuilder()
{
    return JsonBuilder()
        .Append("{")
        .Append("upgradeApp", upgradeApp)
        .Append("businessType", businessType.GetJsonBuilder())
        .Append("abilityName", abilityName)
        .Append("subscriberDevId", subscriberDevId)
        .Append("devUpgradeId", devUpgradeId)
        .Append("}");
}

JsonBuilder EventInfo::GetJsonBuilder()
{
    return JsonBuilder()
        .Append("{")
        .Append("eventId", CAST_INT(eventId))
        .Append("taskBody", taskBody.GetJsonBuilder(eventId))
        .Append("}");
}

JsonBuilder DescriptionInfo::GetJsonBuilder()
{
    return JsonBuilder()
        .Append("{")
        .Append("descriptionType", CAST_INT(descriptionType))
        .Append("content", content)
        .Append("}");
}

JsonBuilder VersionComponent::GetJsonBuilder()
{
    return JsonBuilder()
        .Append("{")
        .Append("componentId", componentId)
        .Append("componentType", componentType)
        .Append("upgradeAction", upgradeAction)
        .Append("displayVersion", displayVersion)
        .Append("innerVersion", innerVersion)
        .Append("size", static_cast<int64_t>(size))
        .Append("effectiveMode", static_cast<int32_t>(effectiveMode))
        .Append("descriptionInfo", descriptionInfo.GetJsonBuilder())
        .Append("componentExtra", componentExtra)
        .Append("}");
}

JsonBuilder GetJsonBuilder(VersionComponent &versionComponent)
{
    return versionComponent.GetJsonBuilder();
}

JsonBuilder GetJsonBuilder(ErrorMessage &errorMessage)
{
    return errorMessage.GetJsonBuilder();
}

template <typename T>
std::vector<JsonBuilder> GetArrayJsonBuidlerList(const std::vector<T> &valueList)
{
    std::vector<JsonBuilder> jsonBuilderList;
    for (T value : valueList) {
        jsonBuilderList.push_back(GetJsonBuilder(value));
    }
    return jsonBuilderList;
}

JsonBuilder TaskBody::GetJsonBuilder(EventId eventId)
{
    JsonBuilder jsonBuilder = JsonBuilder();
    auto iter = g_taskBodyTemplateMap.find(eventId);
    if (iter == g_taskBodyTemplateMap.end()) {
        ENGINE_LOGE("GetJsonBuilder eventId not find");
        return jsonBuilder;
    }
    jsonBuilder.Append("{");
    uint32_t taskBodyTemplate = iter->second;
    if (taskBodyTemplate & VERSION_DIGEST_INFO) {
        jsonBuilder.Append("versionDigestInfo", versionDigestInfo.GetJsonBuilder());
    }
    if (taskBodyTemplate & UPGRADE_STATUS) {
        jsonBuilder.Append("status", CAST_INT(status));
    }
    if (taskBodyTemplate & SUB_STATUS) {
        jsonBuilder.Append("subStatus", subStatus);
    }
    if (taskBodyTemplate & PROGRESS) {
        jsonBuilder.Append("progress", progress);
    }
    if (taskBodyTemplate & INSTALL_MODE) {
        jsonBuilder.Append("installMode", installMode);
    }
    if (taskBodyTemplate & ERROR_MESSAGE) {
        jsonBuilder.Append("errorMessages", GetArrayJsonBuidlerList(errorMessages));
    }
    if (taskBodyTemplate & VERSION_COMPONENT) {
        jsonBuilder.Append("versionComponents", GetArrayJsonBuidlerList(versionComponents));
    }
    return jsonBuilder.Append("}");
}

std::string UpdateHelper::BuildEventDevId(const UpgradeInfo &info)
{
    return std::string("{") + std::string("upgradeDevId: ") + info.upgradeDevId
        + std::string(", controlDevId: ") + info.controlDevId + std::string(" }");
}

bool UpdateHelper::IsUpgradeFailed(UpgradeStatus status)
{
    return status == UpgradeStatus::DOWNLOAD_FAIL ||
           status == UpgradeStatus::PACKAGE_TRANS_FAIL ||
           status == UpgradeStatus::INSTALL_FAIL ||
           status == UpgradeStatus::UPDATE_FAIL ||
           status == UpgradeStatus::VERIFY_FAIL;
}
} // namespace UpdateEngine
} // namespace OHOS
