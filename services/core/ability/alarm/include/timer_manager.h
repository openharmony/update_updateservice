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

#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <map>

#include "singleton.h"
#include "timer.h"

namespace OHOS {
namespace UpdateEngine {
enum class AlarmType {
    STARTUP_TIME_LOOPER = 1,
    FIRMWARE_TIME_LOOPER = 2,
    PARAM_TIME_LOOPER = 3,
};

class TimerManager : public DelayedSingleton<TimerManager> {
    DECLARE_DELAYED_SINGLETON(TimerManager);

public:
    void RegisterRepeatingAlarm(
        AlarmType alarmType, int64_t repeatingTime, const OHOS::Utils::Timer::TimerCallback &callback);
    void UnregisterRepeatingAlarm(AlarmType alarmType);

private:
    void BuildTimer();

private:
    std::map<AlarmType, uint32_t> lastRegisterTimeIdMap_;
    std::shared_ptr<OHOS::Utils::Timer> timer_ = nullptr;
    std::mutex mutex_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // TIMER_MANAGER_H