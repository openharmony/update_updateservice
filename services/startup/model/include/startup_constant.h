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

#ifndef STARTUP_CONSTANT_H
#define STARTUP_CONSTANT_H

#include <cstdint>
#include <string>
#include <vector>

namespace OHOS {
namespace UpdateEngine {
enum class StartupReason {
    UNKNOWN = -1,
    DEVICE_REBOOT = 0,
    SCHEDULE_TASK = 1,
    PROCESS_ENV_RESET = 2,
};

enum class TaskPriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
};

enum class AccessType {
    DEFAULT = 0,
    FIRMWARE = 1,
    PARAM = 2,
    DOWNLOAD = 3,
    REPORT = 4,
};

namespace Startup {
const std::vector<AccessType> ACCESS_TYPES{AccessType::FIRMWARE};
const std::string UPDATER_SA_NAME = "updater_sa";

constexpr uint64_t ONE_SECOND_MILLISECONDS = 1000L;
constexpr uint64_t ONE_MINUTE_SECONDS = 60L;
constexpr uint64_t ONE_HOUR_MINUTES = 60L;

const std::string PULLUP_INTERVAL_CONFIG = "pullupInterval";                      // SA拉起间隔配置项
constexpr uint64_t PULLUP_INTERVAL = 4 * ONE_HOUR_MINUTES * ONE_MINUTE_SECONDS;   // SA拉起间隔
const std::string IDLE_CHECK_INTERVAL_CONFIG = "idleCheckInterval";               // 空闲检测间隔配置项
constexpr uint64_t IDLE_CHECK_INTERVAL = 5 * ONE_MINUTE_SECONDS;                  // 空闲检测间隔
} // namespace Startup
} // namespace UpdateEngine
} // namespace OHOS
#endif // STARTUP_CONSTANT_H