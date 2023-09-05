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

#ifndef ACCESS_MANAGER_H
#define ACCESS_MANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "singleton.h"

#include "i_access.h"
#include "startup_constant.h"

namespace OHOS {
namespace UpdateEngine {
class IAccessManager {
public:
    virtual ~IAccessManager() = default;
    virtual bool IsIdle() = 0;
    virtual std::vector<ScheduleTask> GetScheduleTasks() = 0;
    virtual bool Exit() = 0;
    virtual bool Register(AccessType type, const std::shared_ptr<IAccess> &access) = 0;
    virtual bool Unregister(AccessType type) = 0;
    virtual void SetRemoteIdle(bool isRemoteIdle) = 0;
};

class AccessManager final : public IAccessManager, public DelayedSingleton<AccessManager> {
    DECLARE_DELAYED_SINGLETON(AccessManager);

public:
    bool IsIdle() final;
    std::vector<ScheduleTask> GetScheduleTasks() final;
    bool Exit() final;

    bool Register(AccessType type, const std::shared_ptr<IAccess> &access) final;
    bool Unregister(AccessType type) final;
    void SetRemoteIdle(bool isRemoteIdle) final;

private:
    std::recursive_mutex mutex_;
    std::map<AccessType, std::shared_ptr<IAccess>> accessMap_;
    bool isRemoteIdle_ = true;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ACCESS_MANAGER_H