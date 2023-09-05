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

#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <map>

#include "singleton.h"

#include "alarm_helper.h"
#include "timer_manager.h"

namespace OHOS {
namespace UpdateEngine {
using AlarmCallback = std::function<void()>;

enum class BusinessAlarmType {
    STARTUP = 1,
    AUTO_UPGRADE = 2,
    TIME_CHANGED = 3,
    NIGHT_UPGRADE = 4,
    REMIND = 5,
};

class AlarmManager {
public:
    explicit AlarmManager(AlarmType alarmType, const std::string &logTag);
    virtual ~AlarmManager();

    void RegisterRepeatingAlarm(BusinessAlarmType alarmType, const AlarmCallback &callback);
    void UnregisterRepeatingAlarm(BusinessAlarmType alarmType);

private:
    void StartTimeLooper();

private:
    AlarmType alarmType_;
    std::string logTag_;
    int32_t timeLooperInterval_ = AlarmLooper::DEFAULT_LOOPER_INTERVAL;
    std::map<BusinessAlarmType, AlarmCallback> callbackMap_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ALARM_MANAGER_H