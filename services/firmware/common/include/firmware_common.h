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

#ifndef FIRMWARE_COMMON_H
#define FIRMWARE_COMMON_H

#include <string>

#include "nlohmann/json.hpp"

#include "constant.h"
#include "encrypt_utils.h"
#include "firmware_component.h"
#include "firmware_log.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
enum class FirmwareStep {
    INIT = 0,
    CHECK_STEP,
    DOWNLOAD_STEP,
    AUTH_STEP,
    INSTALL_STEP,
    APPLY_STEP,
    COMPLETE = 10
};

enum class CheckStatus {
    CHECK_SUCCESS = 0,
    CHECK_FAIL,
    FILE_LIST_REQUEST_SUCCESS,
    FILE_LIST_REQUEST_FAIL,
    CHANGELOG_REQUEST_SUCCESS,
    CHANGELOG_REQUEST_FAIL
};

enum class BlType {
    HOTA_TYPE = 1,
    COTA_TYPE = 2,
    DYNAMIC_TYPE = 100
};

enum class CheckResultStatus {
    STATUS_SYSTEM_ERROR = -1,
    STATUS_NEW_VERSION_AVAILABLE = 0,
    STATUS_NEW_VERSION_NOT_AVAILABLE = 1,
    STATUS_SERVER_IS_BUSY = 2
};

enum class InstallType {
    QUICK_FIX = 1,
    SYS_INSTALLER,
    UPDATER
};

struct Duration {
    int32_t duration = -1; // 搜包周期 单位：小时
};

struct VersionPackageRule {
public:
    std::string versionNumber;
    std::string displayVersionNumber;
    std::string versionPackageType;

    nlohmann::ordered_json ToCheckJson()
    {
        nlohmann::ordered_json json = nlohmann::ordered_json::object();
        json["versionPackageType"] = versionPackageType;
        json["versionNumber"] = versionNumber;
        return json;
    }
};

enum VersionIndex {
    BASE_INDEX = 0,
    CUST_INDEX,
    PRELOAD_INDEX
};

enum StorageType {
    COMMERCIAL = 1,
    BETA = 2
};

struct UpdatePackage {
    std::string versionId;
    int32_t packageIndex = 0;
    PackageType versionPackageType = PackageType::DYNAMIC;

    nlohmann::ordered_json ToJson()
    {
        nlohmann::ordered_json json = nlohmann::ordered_json::object();
        json["versionId"] = versionId;
        json["packageIndex"] = packageIndex;
        json["versionPackageType"] = CAST_INT(versionPackageType);
        return json;
    }
};

struct TargetBlComponent {
    std::string versionNumber;
    std::string displayVersionNumber;
    PackageType versionPackageType = PackageType::DYNAMIC;
};

struct BlVersionCheckResult {
    std::string status;
    std::string pollingPeriod;
    std::string blVersionInfo;
    int32_t blVersionType = 0;
    std::string descriptPackageId;
    std::vector<UpdatePackage> updatePackages;
    std::vector<TargetBlComponent> targetBlComponents;
};

struct Version {
    std::string url;
    std::string reserveUrl;
    int32_t storageType = 0;
    std::string versionId;
    std::string versionNumber;
};

struct BlCheckResponse {
    std::string status;
    std::vector<BlVersionCheckResult> blVersionCheckResults;
    std::vector<Version> versionList;
};

struct CheckAndAuthInfo {
    std::string responseStatus;
};

enum class CombinationType {
    INVALLID_TYPE = 0,
    HOTA = 1
};

enum class DownloadMode {
    MANUAL = 0,
    AUTO
};

enum class UpgradeMode {
    MANUAL = 0,
    NIGHT
};

enum class RequestType {
    CHECK = 0,
    AUTH
};

enum class DownloadEndReason {
    INIT = 0,
    SUCCESS = 1,
    DOWNLOADING = 2,
    FAIL = 3,
    VERIFY_FAIL = 4,
    IO_EXCEPTION = 5,
    REDIRECT = 6,
    SERVER_TIMEOUT = 7,
    PAUSE = 8,
    CANCEL = 9,
    NO_ENOUGH_MEMORY = 10,
    NET_NOT_AVAILIABLE,
    DOWNLAOD_INFO_EMPTY,
    CURL_ERROR,
    SYSTEM_BUSY,
    INIT_PACKAGE_FAIL,
    NET_CHANGE,
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_COMMON_H
