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

#ifndef SCHEDULE_TASK_H
#define SCHEDULE_TASK_H

#include <memory>
#include <string>

#include "startup_constant.h"
#include "update_define.h"

namespace OHOS {
namespace UpdateEngine {
class ScheduleTask final {
public:
    TaskPriority priority = TaskPriority::LOW;
    uint64_t minDelayTime = 0L;  // 单位S
    uint64_t maxDelayTime = 0L;  // 单位S
    int netType = 0;
    int batteryLevel = 0;
    bool deviceIdle = false;
    std::string taskExtra;
    StartupReason startupReason = StartupReason::UNKNOWN;

    // 优先保证高优先级任务按时拉起
    bool operator<(const ScheduleTask &other) const
    {
        if (minDelayTime != other.minDelayTime) {
            // 最先执行，优先级更高
            return minDelayTime < other.minDelayTime;
        }

        // 最大延时时间越小，越早触发，优先级更高
        return maxDelayTime < other.maxDelayTime;
    }

    bool operator==(const ScheduleTask &other) const
    {
        return priority == other.priority && minDelayTime == other.minDelayTime &&
               maxDelayTime == other.maxDelayTime && netType == other.netType &&
               batteryLevel == other.batteryLevel && deviceIdle == other.deviceIdle &&
               taskExtra == other.taskExtra;
    }

    std::string ToString() const
    {
        return std::string("Task:[")
                .append("priority:").append(std::to_string(CAST_INT(priority))).append(", ")
                .append("minDelayTime:").append(std::to_string(minDelayTime)).append(", ")
                .append("maxDelayTime:").append(std::to_string(maxDelayTime)).append(", ")
                .append("netType:").append(std::to_string(netType)).append(", ")
                .append("batteryLevel:").append(std::to_string(batteryLevel)).append(", ")
                .append("deviceIdle:").append(std::to_string(CAST_INT(deviceIdle))).append(", ")
                .append("taskExtra:").append(taskExtra)
                .append("]");
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // SCHEDULE_TASK_H