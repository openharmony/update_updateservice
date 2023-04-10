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

#include "schedule_manager.h"

#include "schedule_config.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
ScheduleManager::ScheduleManager(
    const std::shared_ptr<IAccessManager> &accessManager, const std::shared_ptr<IStartupSchedule> &startupSchedule)
{
    ENGINE_LOGD("ScheduleManager constructor");
    accessManager_ = accessManager;
    startupSchedule_ = startupSchedule;
}

ScheduleManager::~ScheduleManager()
{
    ENGINE_LOGD("ScheduleManager deConstructor");
}

bool ScheduleManager::IdleCheck()
{
    if (accessManager_ != nullptr && !accessManager_->IsIdle()) {
        return false;
    }
    return TaskManage(scheduleTask_);
}

bool ScheduleManager::Exit()
{
    if (accessManager_ != nullptr && !accessManager_->Exit()) {
        return false;
    }
    return TaskSchedule(scheduleTask_);
}

bool ScheduleManager::TaskManage(ScheduleTask &scheduleTask)
{
    if (accessManager_ == nullptr) {
        ENGINE_LOGE("TaskManage accessManager is null, return idle");
        return true;
    }

    std::vector<ScheduleTask> tasks = accessManager_->GetScheduleTasks();
    std::sort(tasks.begin(), tasks.end());
    for (ScheduleTask &task : tasks) {
        ENGINE_LOGD("TaskManage task is %{public}s", task.ToString().c_str());
    }

    if (tasks.empty() || tasks.front().minDelayTime > ScheduleConfig::GetPullupInterval()) {
        // 待执行任务列表为空 || 最长任务大于SA轮询间隔，满足空闲条件，设置延时拉起
        scheduleTask.minDelayTime = ScheduleConfig::GetPullupInterval();
        ENGINE_LOGI("TaskManage task idle");
        return true;
    }

    ENGINE_LOGI("TaskManage task not idle");
    return false;
}

bool ScheduleManager::TaskSchedule(const ScheduleTask &scheduleTask)
{
    if (startupSchedule_ == nullptr) {
        ENGINE_LOGE("TaskSchedule startupSchedule null, return false");
        return false;
    }
    ENGINE_LOGI("TaskSchedule: %{public}s", scheduleTask.ToString().c_str());
    return startupSchedule_->Schedule(scheduleTask);
}
} // namespace UpdateEngine
} // namespace OHOS