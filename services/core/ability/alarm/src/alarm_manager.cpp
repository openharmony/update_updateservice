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

#include "alarm_manager.h"

#include "update_define.h"
#include "update_log.h"
#include "time_utils.h"

namespace OHOS {
namespace UpdateEngine {
AlarmManager::AlarmManager(AlarmType alarmType, const std::string &logTag)
{
    ENGINE_LOGI("AlarmManager::AlarmManager alarmType %{public}d", CAST_INT(alarmType));
    alarmType_ = alarmType;
    logTag_ = logTag;
    timeLooperInterval_ = AlarmHelper::GetLooperInterval(alarmType);
    StartTimeLooper();
}

AlarmManager::~AlarmManager()
{
    ENGINE_LOGI("AlarmManager::~AlarmManager");
    DelayedSingleton<TimerManager>::GetInstance()->UnregisterRepeatingAlarm(alarmType_);
}

void AlarmManager::StartTimeLooper()
{
    DelayedSingleton<TimerManager>::GetInstance()->RegisterRepeatingAlarm(alarmType_, timeLooperInterval_, [=]() {
        int64_t currentTime = TimeUtils::GetTimestamp();
        ENGINE_LOGD("%{public}s timeLooper currentTime %{public}s",
            logTag_.c_str(), TimeUtils::GetPrintTimeStr(currentTime).c_str());
        for (auto &[alarmType, callback] : callbackMap_) {
            callback();
        }
    });
}

void AlarmManager::RegisterRepeatingAlarm(BusinessAlarmType alarmType, const AlarmCallback &callback)
{
    ENGINE_LOGI("%{public}s RegisterRepeatingAlarm alarmType %{public}d", logTag_.c_str(), CAST_INT(alarmType));
    callbackMap_[alarmType] = callback;
}

void AlarmManager::UnregisterRepeatingAlarm(BusinessAlarmType alarmType)
{
    ENGINE_LOGI("%{public}s UnregisterRepeatingAlarm alarmType %{public}d", logTag_.c_str(), CAST_INT(alarmType));
    callbackMap_.erase(alarmType);
}
} // namespace UpdateEngine
} // namespace OHOS
