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

#include "firmware_combine_version_utils.h"

#include "firmware_log.h"
#include "device_adapter.h"

namespace OHOS {
namespace UpdateEngine {
std::string CombinePackageVersionUtils::GetPackageVersion(std::string &baseVersion, std::string &custVersion,
    std::string &preloadVersion)
{
    std::string version;
    std::string base = HandleBaseVersion(baseVersion);
    std::string baseLog;
    std::string log;
    HandleBaseVersionLog(baseVersion, baseLog, log);
    std::string cust = HandleCustVersion(custVersion);
    std::string preload = HandlePreloadVersion(preloadVersion);
    version.append(DeviceAdapter::GetDeviceName()).append(base).append("(")
        .append(baseLog).append(cust).append(preload).append(log).append(")");
    return version;
}

std::string CombinePackageVersionUtils::HandleBaseVersion(std::string &baseVersion)
{
    std::string::size_type start = baseVersion.find(" ");
    if (start == std::string::npos) {
        return "";
    }
    std::string::size_type end = baseVersion.find("(");
    std::string base;
    if (end == std::string::npos) {
        base = baseVersion.substr(start);
    } else {
        base = baseVersion.substr(start, end - start);
    }
    return base;
}

void CombinePackageVersionUtils::HandleBaseVersionLog(std::string &baseVersion, std::string &base, std::string &log)
{
    std::string::size_type start = baseVersion.find_last_of("(");
    std::string::size_type end = baseVersion.find_last_of(")");
    if ((start == std::string::npos) || (end == std::string::npos)) {
        return;
    }
    start++;
    std::string::size_type mid = start;
    bool isNumbers = false;
    while (mid < end) {
        mid++;
        if (isdigit(baseVersion[mid])) {
            isNumbers = true;
        } else {
            if (isNumbers) {
                break;
            }
        }
    }
    if ((mid == end) && (!isNumbers)) {
        log = baseVersion.substr(start, mid - start);
        base = "";
        return;
    }
    base = baseVersion.substr(start, mid - start);
    log = baseVersion.substr(mid, end - mid);
}

std::string CombinePackageVersionUtils::HandleCustVersion(std::string &custVersion)
{
    std::string::size_type mid = custVersion.find_last_of("(");
    std::string::size_type end = custVersion.find_last_of(")");
    std::string::size_type start = custVersion.find_last_of(".");
    if ((start == std::string::npos) || (mid == std::string::npos) || (end == std::string::npos)) {
        return "";
    }
    std::string cust = custVersion.substr(mid + 1, end - mid - 1);
    std::string getEnum = custVersion.substr(start + 1, mid - start - 1);
    cust.append("E").append(getEnum);
    return cust;
}

std::string CombinePackageVersionUtils::HandlePreloadVersion(std::string &preloadVersion)
{
    std::string::size_type start = preloadVersion.find_last_of("R");
    std::string::size_type end = preloadVersion.find_last_of(")");
    if ((start == std::string::npos) || (end == std::string::npos)) {
        return "";
    }
    std::string preload = preloadVersion.substr(start, end - start);
    start = preloadVersion.find_last_of(".");
    end = preloadVersion.find_last_of("(");
    if ((start == std::string::npos) || (end == std::string::npos)) {
        return "";
    }
    std::string getPnum = preloadVersion.substr(start + 1, end - start - 1);
    preload.append("P").append(getPnum);
    return preload;
}
} // namespace UpdateEngine
} // namespace OHOS
