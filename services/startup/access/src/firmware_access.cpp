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

#include "firmware_access.h"

#include "access_manager.h"
#include "firmware_manager.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareAccess::FirmwareAccess()
{
    ENGINE_LOGD("FirmwareAccess constructor");
}

FirmwareAccess::~FirmwareAccess()
{
    ENGINE_LOGD("FirmwareAccess deConstructor");
}

void FirmwareAccess::Init(StartupReason startupReason)
{
    ENGINE_LOGI("FirmwareAccess Init");
    DelayedSingleton<FirmwareManager>::GetInstance()->Init(startupReason);
    DelayedSingleton<AccessManager>::GetInstance()->Register(
        AccessType::FIRMWARE, DelayedSingleton<FirmwareAccess>::GetInstance());
}

bool FirmwareAccess::IsIdle()
{
    return DelayedSingleton<FirmwareManager>::GetInstance()->IsIdle();
}

std::vector<ScheduleTask> FirmwareAccess::GetScheduleTasks()
{
    return DelayedSingleton<FirmwareManager>::GetInstance()->GetScheduleTasks();
}

bool FirmwareAccess::Exit()
{
    return DelayedSingleton<FirmwareManager>::GetInstance()->Exit();
}
} // namespace UpdateEngine
} // namespace OHOS