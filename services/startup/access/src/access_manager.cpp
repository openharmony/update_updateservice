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

#include "access_manager.h"

#include "string_utils.h"
#include "update_define.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
AccessManager::AccessManager()
{
    ENGINE_LOGD("AccessManager constructor");
}

AccessManager::~AccessManager()
{
    ENGINE_LOGD("AccessManager deConstructor");
}

bool AccessManager::IsIdle()
{
    if (!isRemoteIdle_) {
        ENGINE_LOGI("AccessManager remote not idle");
        return false;
    }

    std::lock_guard<std::recursive_mutex> guard(mutex_);
    for (auto &[type, access] : accessMap_) {
        if (access != nullptr && !access->IsIdle()) {
            return false;
        }
    }
    return true;
}

std::vector<ScheduleTask> AccessManager::GetScheduleTasks()
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    ENGINE_LOGI("AccessManager GetScheduleTasks");
    std::vector<ScheduleTask> scheduleTasks;
    for (auto &[type, access] : accessMap_) {
        if (access != nullptr) {
            std::vector<ScheduleTask> tasks = access->GetScheduleTasks();
            scheduleTasks.insert(scheduleTasks.end(), tasks.begin(), tasks.end());
        }
    }
    return scheduleTasks;
}

bool AccessManager::Exit()
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    ENGINE_LOGI("AccessManager Exit");
    for (auto &[type, access] : accessMap_) {
        if (access != nullptr && !access->Exit()) {
            return false;
        }
    }
    return true;
}

bool AccessManager::Register(AccessType type, const std::shared_ptr<IAccess> &access)
{
    if (access == nullptr) {
        ENGINE_LOGE("AccessManager Register access is null");
        return false;
    }
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    ENGINE_LOGI("AccessManager Register: type is %{public}d", CAST_INT(type));
    accessMap_[type] = access;
    return true;
}

bool AccessManager::Unregister(AccessType type)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if (accessMap_.find(type) == accessMap_.end()) {
        ENGINE_LOGI("AccessManager Unregister type: %{public}d not exist", CAST_INT(type));
        return false;
    }
    ENGINE_LOGI("AccessManager Unregister: type is %{public}d", CAST_INT(type));
    accessMap_.erase(type);
    return true;
}

void AccessManager::SetRemoteIdle(bool isRemoteIdle)
{
    ENGINE_LOGI("AccessManager SetRemoteIdle %{public}s", StringUtils::GetBoolStr(isRemoteIdle).c_str());
    isRemoteIdle_ = isRemoteIdle;
}
} // namespace UpdateEngine
} // namespace OHOS