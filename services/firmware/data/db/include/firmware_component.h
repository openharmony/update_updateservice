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

#ifndef FIRMWARE_COMPONENT_H
#define FIRMWARE_COMPONENT_H

#include <string>

#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
struct FirmwareComponent {
    int32_t id = 0;

    std::string versionId;           // 标识包的唯一性
    int32_t packageIndex = 0;        // 包的安装顺序
    PackageType versionPackageType = PackageType::DYNAMIC;

    std::string targetBlVersionNumber;
    std::string targetBlDisplayVersionNumber;

    std::string versionNumber; // 包文件本身的versionNumber

    std::string blVersionInfo;
    std::string componentId;
    std::string descriptPackageId;
    std::string changelogUrl;
    int32_t blVersionType = 0;

    bool isNeedRestart;
    int32_t patchType = 0;
    std::string url;
    std::string reserveUrl;
    int32_t storageType = 0; // 内测或商用版本

    std::string fileName;   // 包的名字，来自filelist
    int64_t size = 0;       // 包大小，来自filelist
    std::string verifyInfo; // 包的sha256值，来自filelist
    std::string spath;

    UpgradeStatus status = UpgradeStatus::INIT;
    int32_t progress = 0;

    std::string ToString()
    {
        return std::string("FirmwareComponent: ")
        .append("id=").append(std::to_string(id)).append(",")
        .append("versionId=").append(versionId).append(",")
        .append("packageIndex=").append(std::to_string(packageIndex)).append(",")
        .append("versionPackageType=").append(std::to_string(CAST_INT(versionPackageType))).append(",")
        .append("targetBlVersionNumber=").append(targetBlVersionNumber).append(",")
        .append("targetBlDisplayVersionNumber=").append(targetBlDisplayVersionNumber).append(",")
        .append("versionNumber=").append(versionNumber).append(",")
        .append("blVersionInfo=").append(blVersionInfo).append(",")
        .append("componentId=").append(componentId).append(",")
        .append("descriptPackageId=").append(descriptPackageId).append(",")
        .append("changelogUrl=").append(changelogUrl).append(",")
        .append("blVersionType=").append(std::to_string(blVersionType)).append(",")
        .append("isNeedRestart=").append(std::to_string(isNeedRestart)).append(",")
        .append("patchType=").append(std::to_string(patchType)).append(",")
        .append("url=").append(url).append(",")
        .append("reserveUrl=").append(reserveUrl).append(",")
        .append("storageType=").append(std::to_string(storageType)).append(",")
        .append("fileName=").append(fileName).append(",")
        .append("size=").append(std::to_string(size)).append(",")
        .append("verifyInfo=").append(verifyInfo).append(",")
        .append("spath=").append(spath).append(",")
        .append("status=").append(std::to_string(CAST_INT(status))).append(",")
        .append("progress=").append(std::to_string(progress));
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_COMPONENT_H
