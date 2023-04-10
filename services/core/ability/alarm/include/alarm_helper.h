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

#ifndef ALARM_HELPER_H
#define ALARM_HELPER_H

#include <string>

#include "string_utils.h"
#include "timer_manager.h"

namespace OHOS {
namespace UpdateEngine {
namespace AlarmLooper {
    constexpr int32_t DEFAULT_LOOPER_INTERVAL = 10;   // 默认定时器轮询周期
    constexpr int32_t STARTUP_LOOPER_INTERVAL = 60;   // 动态启停定时器轮询周期
} // namespace AlarmLooper

class AlarmHelper {
public:
    static int32_t GetLooperInterval(AlarmType alarmType)
    {
        return alarmType == AlarmType::STARTUP_TIME_LOOPER ? AlarmLooper::STARTUP_LOOPER_INTERVAL
                                                           : AlarmLooper::DEFAULT_LOOPER_INTERVAL;
    }

    static std::string GetStartupAlarmTypeStr()
    {
        return GET_ENUM_VALUE_STRING(AlarmType::STARTUP_TIME_LOOPER);
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ALARM_HELPER_H