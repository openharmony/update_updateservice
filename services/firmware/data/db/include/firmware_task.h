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

#ifndef FIRMWARE_TASK_H
#define FIRMWARE_TASK_H

#include <string>

#include "firmware_common.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
struct FirmwareTask {
    std::string taskId;
    UpgradeStatus status = UpgradeStatus::INIT;
    int32_t progress = 0;
    int errorCode = 0;
    std::string errorMsg;
    bool isExistTask = false;
    std::string downloadTaskId;
    CombinationType combinationType = CombinationType::INVALLID_TYPE;

    Order downloadOrder = Order::DOWNLOAD;
    NetType downloadAllowNetwork = NetType::NOT_METERED_WIFI;
    DownloadMode downloadMode = DownloadMode::MANUAL;

    Order upgradeOrder = Order::INSTALL;
    UpgradeMode upgradeMode = UpgradeMode::MANUAL;

    std::string authResult;
    std::string updateToken;
    std::string deviceToken;
    std::string tokenHMac;
    std::string currentVersionList;
    int64_t authTime = 0;

    std::string ToString()
    {
        return std::string("FirmwareTask: ")
        .append("taskId=").append(taskId).append(",")
        .append("status=").append(std::to_string(CAST_INT(status))).append(",")
        .append("progress=").append(std::to_string(progress)).append(",")
        .append("errorCode=").append(std::to_string(errorCode)).append(",")
        .append("errorMsg=").append(errorMsg).append(",")
        .append("combinationType=").append(std::to_string(CAST_INT(combinationType))).append(",")
        .append("downloadMode=").append(std::to_string(CAST_INT(downloadMode))).append(",")
        .append("downloadAllowNetwork=").append(std::to_string(CAST_INT(downloadAllowNetwork))).append(",")
        .append("downloadOrder=").append(std::to_string(CAST_INT(downloadOrder))).append(",")
        .append("upgradeMode=").append(std::to_string(CAST_INT(upgradeMode))).append(",")
        .append("upgradeOrder=").append(std::to_string(CAST_INT(upgradeOrder))).append(",")
        .append("authResult=").append(authResult).append(",")
        .append("updateToken=").append(updateToken).append(",")
        .append("deviceToken=").append(deviceToken).append(",")
        .append("tokenHMac=").append(tokenHMac).append(",")
        .append("currentVersionList=").append(currentVersionList).append(",")
        .append("authTime=").append(std::to_string(authTime)).append(",");
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_TASK_H
