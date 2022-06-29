/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
namespace UpdateEngine {
#define EVENT_WRITE(eventName, type, ...)  \
    OHOS::HiviewDFX::HiSysEvent::Write("UPDATE", eventName, type, ##__VA_ARGS__)

#define SYS_EVENT_UPDATE_INTERVAL(validCheck, versionInfo, type, interval)  \
    if (!(validCheck)) {    \
        EVENT_WRITE("UPDATE_INTERVAL", OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,    \
            "VERSION_INFO", versionInfo, "TYPE", type, "INTERVAL", interval);    \
    }

#define SYS_EVENT_SYSTEM_UPGRADE_FAULT(validCheck, deviceId, versionInfo, reason, location)  \
    if (!(validCheck)) {    \
        EVENT_WRITE("SYSTEM_UPGRADE_FAULT", OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,    \
            "DEVICE_ID", deviceId, "VERSION_INFO", versionInfo, "FAILED_REASON", reason,    \
            "LOCATION", location);    \
    }

#define SYS_EVENT_SYSTEM_UPGRADE(validCheck, result)  \
    if (!(validCheck)) {    \
        EVENT_WRITE("SYSTEM_UPGRADE", OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,    \
            "UPDATE_RESULT", result);    \
    }

#define SYS_EVENT_SYSTEM_RESET(validCheck, result)  \
    if (!(validCheck)) {    \
        EVENT_WRITE("SYSTEM_RESET", OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,    \
        "RESET_RESULT", result);    \
    }

#define SYS_EVENT_VERIFY_FAILED(validCheck, deviceId, description)  \
    if (!(validCheck)) {    \
        EVENT_WRITE("VERIFY_FAILED", OHOS::HiviewDFX::HiSysEvent::EventType::SECURITY,    \
        "DEVICE_ID", deviceId, "DESCRIPTION", description);    \
    }

class UpdateSystemEvent {
public:
    static constexpr const char *EMPTY = "";
    static constexpr const char *UPGRADE_START = "upgrade start";
    static constexpr const char *RESET_START = "reset start";
    static constexpr const char *EVENT_FAILED_RESULT = "failed";
    static constexpr const char *EVENT_SUCCESS_RESULT = "success";

    static constexpr const char *EVENT_UPGRADE_INTERVAL = "upgrade";
    static constexpr const char *EVENT_CHECK_INTERVAL = "check";
    static constexpr const char *EVENT_DOWNLOAD_INTERVAL = "download";

    static constexpr const char *EVENT_PERMISSION_VERIFY_FAILED = "permission verify failed";
    static constexpr const char *EVENT_PKG_VERIFY_FAILED = "upgrade package verify failed";
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SYSTEM_EVENT_H