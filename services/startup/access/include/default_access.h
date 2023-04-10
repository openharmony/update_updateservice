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

#ifndef DEFAULT_ACCESS_H
#define DEFAULT_ACCESS_H

#include "singleton.h"

#include "i_access.h"

namespace OHOS {
namespace UpdateEngine {
class DefaultAccess final : public IAccess, public DelayedSingleton<DefaultAccess> {
    DECLARE_DELAYED_SINGLETON(DefaultAccess);

public:
    void Init(StartupReason startupReason) final;
    bool IsIdle() final;
    std::vector<ScheduleTask> GetScheduleTasks() final;
    bool Exit() final;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // DEFAULT_ACCESS_H