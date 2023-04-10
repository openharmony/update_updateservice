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

#include "timer_manager.h"

#include "string_utils.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
constexpr int32_t SECOND_MILLIS = 1000;

TimerManager::TimerManager()
{
    ENGINE_LOGD("TimerManager::TimerManager");
}

TimerManager::~TimerManager()
{
    ENGINE_LOGD("TimerManager::~TimerManager");
    if (timer_ != nullptr) {
        for (auto &[alarmType, timerId] : lastRegisterTimeIdMap_) {
            timer_->Unregister(timerId);
        }
        timer_->Shutdown();
    }
}

void TimerManager::RegisterRepeatingAlarm(
    AlarmType alarmType, int64_t repeatingTime, const OHOS::Utils::Timer::TimerCallback &callback)
{
    ENGINE_LOGI("TimerManager RegisterRepeatingAlarm type %{public}d repeatingTime %{public}ld success",
        static_cast<AlarmType>(alarmType), repeatingTime);
    BuildTimer();
    UnregisterRepeatingAlarm(alarmType);
    uint32_t registerTimerId = timer_->Register(callback, repeatingTime * SECOND_MILLIS, false);
    std::lock_guard<std::mutex> lockGuard(mutex_);
    lastRegisterTimeIdMap_.emplace(alarmType, registerTimerId);
}

void TimerManager::UnregisterRepeatingAlarm(AlarmType alarmType)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    bool isSuccess = false;
    auto result = lastRegisterTimeIdMap_.find(alarmType);
    if (result != lastRegisterTimeIdMap_.end()) {
        uint32_t lastRegisterTimerId = result->second;
        if (timer_ != nullptr) {
            timer_->Unregister(lastRegisterTimerId);
        }
        lastRegisterTimeIdMap_.erase(result);
        isSuccess = true;
    }
    ENGINE_LOGI("TimerManager UnregisterRepeatingAlarm type %{public}d status %{public}s",
        static_cast<AlarmType>(alarmType), StringUtils::GetBoolStr(isSuccess).c_str());
}

void TimerManager::BuildTimer()
{
    if (timer_ == nullptr) {
        timer_ = std::make_shared<OHOS::Utils::Timer>("OTA Timer");
        uint32_t ret = timer_->Setup();
        ENGINE_LOGI("TimerManager BuildTimer result %{public}d", ret);
    }
}
} // namespace UpdateEngine
} // namespace OHOS
