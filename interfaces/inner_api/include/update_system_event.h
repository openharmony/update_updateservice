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

#ifndef UPDATE_SYSTEM_EVENT_H
#define UPDATE_SYSTEM_EVENT_H

#include <string>

#include "hisysevent.h"

namespace OHOS {
namespace update_engine {
const std::string EMPTY = "";
const std::string EVENT_FAILED_RESULT = "failed";
const std::string EVENT_SUCCESS_RESULT = "success";

const std::string UPGRADE_INTERVAL = "upgrade";
const std::string CHECK_INTERVAL = "check";
const std::string DOWNLOAD_INTERVAL = "download";

const std::string PERMISSION_VERIFY_FAILED = "permission verify failed";
const std::string JSON_TRANSFOR_EXCEPTION = "json transformation exception";
const std::string PKG_VERIFY_FAILED = "upgrade package verify failed";
const std::string VERSION_TOO_EARLIER = "update package version earlier than the local version";
const std::string EMPTY_VERIFY_INFO = "invalid verify info";

#define EVENT_WRITE(eventName, type, ...)    \
    OHOS::HiviewDFX::HiSysEvent::Write("UPDATE", eventName, type, ##__VA_ARGS__);

#define SYS_EVENT_UPDATE_INTERVAL(validCheck, versionInfo, type, interval)   \
    if (!(validCheck)) {    \
        EVENT_WRITE("UPDATE_INTERVAL", OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,    \
        "VERSION_INFO", versionInfo, "TYPE", type, "INTERVAL", interval);    \
    }

#define SYS_EVENT_SYSTEM_FAULT(validCheck, deviceId, versionInfo, reason, location)   \
    if (!(validCheck)) {    \
        EVENT_WRITE("SYSTEM_UPGRADE_FAULT", OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,    \
        "DEVICE_ID", deviceId, "VERSION_INFO", versionInfo, "FAILED_REASON", reason,    \
        "LOCATION", location);    \
    }

#define SYS_EVENT_SYSTEM_UPGRADE(validCheck, result)   \
    if (!(validCheck)) {    \
        EVENT_WRITE("SYSTEM_UPDATE", OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,    \
        "UPDATE_RESULT", result);    \
    }

#define SYS_EVENT_SYSTEM_RESET(validCheck, result)   \
    if (!(validCheck)) {    \
        EVENT_WRITE("SYSTEM_RESET", OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,    \
        "RESET_RESULT", result);    \
    }

#define SYS_EVENT_VERIFY_FAILED(validCheck, deviceId, description)   \
    if (!(validCheck)) {    \
        EVENT_WRITE("SYSTEM_VERIFY_FAILED", OHOS::HiviewDFX::HiSysEvent::EventType::SECURITY,    \
        "DEVICE_ID", deviceId, "DESCRIPTION", description);    \
    }
} // namespace update_engine
} // namespace OHOS
#endif // UPDATE_SYSTEM_EVENT_H