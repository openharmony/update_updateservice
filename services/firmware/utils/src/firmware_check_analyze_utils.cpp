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

#include "firmware_check_analyze_utils.h"

#include <iostream>
#include <memory>
#include <map>
#include <ohos_types.h>
#include <string>

#include "constant.h"
#include "file_utils.h"
#include "firmware_combine_version_utils.h"
#include "firmware_constant.h"
#include "firmware_log.h"
#include "firmware_preferences_utils.h"
#include "json_utils.h"
#include "string_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
void FirmwareCheckAnalyzeUtils::DoAnalyze(const std::string &rawJson, std::vector<FirmwareComponent> &components,
    Duration &duration, CheckAndAuthInfo &checkAndAuthInfo)
{
    BlCheckResponse response;
    int32_t ret = CAST_INT(JsonParseError::ERR_OK);
    nlohmann::json root = nlohmann::json::parse(rawJson, nullptr, false);
    if (root.is_discarded()) {
        FIRMWARE_LOGE("json parse error!");
        return;
    }
    int32_t status = CAST_INT(CheckResultStatus::STATUS_SYSTEM_ERROR);
    status = root.at("searchStatus");
    checkAndAuthInfo.responseStatus = std::to_string(status);
    if (!IsLegalStatus(status)) {
        FIRMWARE_LOGI("not found new version!");
        return;
    }
    if (status == CAST_INT(CheckResultStatus::STATUS_NEW_VERSION_AVAILABLE)) {
        ret += AnalyzeBlVersionCheckResults(root, response);
        ret += AnalyzeComponents(root);
    }

    // 解析的都是必须字段，全部解析正常，才能给component赋值
    if (ret == CAST_INT(JsonParseError::ERR_OK)) {
        components = components_;
    }
}

int32_t FirmwareCheckAnalyzeUtils::AnalyzeBlVersionCheckResults(nlohmann::json &root, BlCheckResponse &response)
{
    FIRMWARE_LOGI("blVersionCheckResults size is %{public}lu ", root["blVersionCheckResults"].size());
    int32_t ret = CAST_INT(JsonParseError::ERR_OK);
    for (auto &result : root["checkResults"]) {
        int32_t status = CAST_INT(CheckResultStatus::STATUS_SYSTEM_ERROR);
        status = root.at("searchStatus");
        if (status == CAST_INT(CheckResultStatus::STATUS_NEW_VERSION_AVAILABLE)) {
            BlVersionCheckResult checkResult;
            ret += JsonUtils::GetValueAndSetTo(result, "descriptPackageId", checkResult.descriptPackageId);
            checkResult.blVersionType = 1;
            checkResult.status = std::to_string(status);
            UpdatePackage package;
            package.versionId = "1";
            int32_t versionPackageType = CAST_INT(PackageType::DYNAMIC);
            ret += JsonUtils::GetValueAndSetTo(result, "packageType", versionPackageType);
            package.versionPackageType = static_cast<PackageType>(versionPackageType);
            package.packageIndex = 0;
            checkResult.updatePackages.push_back(package);
            TargetBlComponent component;
            component.versionPackageType = package.versionPackageType;
            ret += JsonUtils::GetValueAndSetTo(result, "versionName", component.displayVersionNumber);
            ret += JsonUtils::GetValueAndSetTo(result, "versionName", component.versionNumber);
            checkResult.targetBlComponents.push_back(component);
            checkResult.blVersionInfo = result["blVersionInfo"].dump();
            response.blVersionCheckResults.push_back(checkResult);
            Version version;
            version.versionId = "1";
            ret += JsonUtils::GetValueAndSetTo(result, "versionCode", version.versionNumber);
            ret += JsonUtils::GetValueAndSetTo(result, "url", version.url);
            response.versionList.push_back(version);
        }
    }
    return ret;
}

int32_t FirmwareCheckAnalyzeUtils::AnalyzeComponents(nlohmann::json &root)
{
    int32_t ret = CAST_INT(JsonParseError::ERR_OK);
    std::string componentId;
    for (auto &result : root["checkResults"]) {
        FirmwareComponent component;
        int32_t componetSize;
        ret += JsonUtils::GetValueAndSetTo(result, "descriptPackageId", component.descriptPackageId);
        ret += JsonUtils::GetValueAndSetTo(result, "url", component.url);
        ret += JsonUtils::GetValueAndSetTo(result, "size", componetSize);
        component.size = static_cast<int64_t>(componetSize);
        component.fileName = StringUtils::GetLastSplitString(component.url, "/");
        ret += JsonUtils::GetValueAndSetTo(result, "verifyInfo", component.verifyInfo);
        ret += JsonUtils::GetValueAndSetTo(result, "versionCode", component.versionNumber);
        ret += JsonUtils::GetValueAndSetTo(result, "versionName", component.targetBlVersionNumber);
        component.targetBlDisplayVersionNumber = component.targetBlVersionNumber;
        component.blVersionType = 1;
        component.targetBlVersionNumber = component.targetBlVersionNumber;
        component.componentId = component.descriptPackageId;
        component.versionPackageType = PackageType::NORMAL;
        componentId = component.descriptPackageId;
        components_.push_back(component);
    }

    for (auto &descriptInfo : root["descriptInfo"]) {
        int32_t descriptInfoType;
        std::string descContent;
        std::string subString = "quota";
        std::string replString = "\"";
        ret += JsonUtils::GetValueAndSetTo(descriptInfo, "descriptionType", descriptInfoType);
        ret += JsonUtils::GetValueAndSetTo(descriptInfo, "content", descContent);
        StringUtils::ReplaceStringAll(descContent, subString, replString);
        std::string changelogFilePath = Firmware::CHANGELOG_PATH + "/" + componentId + ".xml";
        FIRMWARE_LOGI("changelog file %{public}s", changelogFilePath.c_str());
        std::string data = std::to_string(descriptInfoType) + "|" + descContent;
        if (!FileUtils::SaveDataToFile(changelogFilePath, data)) {
            FIRMWARE_LOGE("write data to description file error, %{public}s", changelogFilePath.c_str());
        }
    }

    return ret;
}

bool FirmwareCheckAnalyzeUtils::IsLegalStatus(int32_t status)
{
    return status == CAST_INT(CheckResultStatus::STATUS_NEW_VERSION_AVAILABLE) ||
           status == CAST_INT(CheckResultStatus::STATUS_NEW_VERSION_NOT_AVAILABLE);
}
} // namespace UpdateEngine
} // namespace OHOS