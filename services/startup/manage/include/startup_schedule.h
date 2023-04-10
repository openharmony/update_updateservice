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

#ifndef STARTUP_SCHEDULE_H
#define STARTUP_SCHEDULE_H

#include "singleton.h"

#include "alarm_manager.h"
#include "schedule_task.h"

namespace OHOS {
namespace UpdateEngine {
using ScheduleLooper = std::function<void()>;

class IStartupSchedule {
public:
    virtual ~IStartupSchedule() = default;
    virtual void RegisterLooper(const ScheduleLooper &looper) = 0;
    virtual void UnregisterLooper() = 0;
    virtual bool Schedule(const ScheduleTask &task) = 0;
};

class StartupSchedule final : public IStartupSchedule, public AlarmManager, public DelayedSingleton<StartupSchedule> {
    DECLARE_DELAYED_SINGLETON(StartupSchedule);

public:
    void RegisterLooper(const ScheduleLooper &looper) final;
    void UnregisterLooper() final;
    bool Schedule(const ScheduleTask &task) final;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // STARTUP_SCHEDULE_H