/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DOWNLOAD_INFO_H
#define DOWNLOAD_INFO_H

#include <string>

#include "encrypt_utils.h"
#include "update_define.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
struct DownloadInfo {
public:
    DownloadInfo() = default;
    ~DownloadInfo() = default;

    int32_t id;
    std::string downloadId;
    std::string versionId;
    std::string taskId;
    std::string url;
    std::string reserveUrl;
    std::string path;
    std::string veriftInfo;
    std::map<std::string, std::string> header {};
    std::string requestBody;
    int32_t retryTimes = 0;
    int64_t packageSize = 0;
    int64_t downloadedSize = 0;
    bool isNeedAutoResume = false;

    bool operator<(const DownloadInfo &downloadInfo) const
    {
        if (downloadId < downloadInfo.downloadId)
            return true;
        if (downloadId > downloadInfo.downloadId)
            return false;
        return false;
    }

    std::string ToString()
    {
        return std::string("DownloadInfo: ")
        .append("id=").append(std::to_string(id)).append(",")
        .append("taskId=").append(taskId).append(",")
        .append("url=").append(EncryptUtils::EncryptUrl(url)).append(",")
        .append("path=").append(path).append(",")
        .append("veriftInfo=").append(veriftInfo).append(",")
        .append("retryTimes=").append(std::to_string(retryTimes)).append(",")
        .append("packageSize=").append(std::to_string(packageSize)).append(",")
        .append("downloadedSize=").append(std::to_string(downloadedSize))
        .append("isNeedAutoResume=").append(std::to_string(isNeedAutoResume));
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // DOWNLOAD_INFO_H
