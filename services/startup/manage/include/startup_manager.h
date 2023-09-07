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

#ifndef STARTUP_MANAGER_H
#define STARTUP_MANAGER_H

#include <memory>

#include "singleton.h"

#include "schedule_manager.h"
#include "startup_constant.h"

namespace OHOS {
namespace UpdateEngine {
class StartupManager final : public DelayedSingleton<StartupManager> {
    DECLARE_DELAYED_SINGLETON(StartupManager);

public:
    void Start();

private:
    void Init() const;
    void IdleLoop();
    bool IdleCheck();
    bool PreExit();
    void SAExit() const;
    StartupReason GetStartupReason() const;

private:
    std::shared_ptr<ScheduleManager> scheduleManager_ = nullptr;
    int64_t lastIdleCheckTime_ = 0;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // STARTUP_MANAGER_H