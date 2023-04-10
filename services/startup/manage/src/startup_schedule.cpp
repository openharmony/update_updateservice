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

#include "startup_schedule.h"

#include "service_control.h"

#include "alarm_helper.h"
#include "constant.h"
#include "firmware_preferences_utils.h"
#include "startup_constant.h"
#include "time_utils.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
StartupSchedule::StartupSchedule() : AlarmManager(AlarmType::STARTUP_TIME_LOOPER, AlarmHelper::GetStartupAlarmTypeStr())
{
    ENGINE_LOGD("StartupSchedule constructor");
}

StartupSchedule::~StartupSchedule()
{
    ENGINE_LOGD("StartupSchedule deConstructor");
}

void StartupSchedule::RegisterLooper(const ScheduleLooper &looper)
{
    ENGINE_LOGI("RegisterLooper");
    RegisterRepeatingAlarm(BusinessAlarmType::STARTUP, [=]() { looper(); });
}

void StartupSchedule::UnregisterLooper()
{
    ENGINE_LOGI("UnregisterLooper");
    UnregisterRepeatingAlarm(BusinessAlarmType::STARTUP);
}

bool StartupSchedule::Schedule(const ScheduleTask &task)
{
    ENGINE_LOGI("Schedule next SA start time is %{public}s",
        TimeUtils::GetPrintTimeStr(TimeUtils::GetTimestamp() + task.minDelayTime).c_str());
    uint64_t scheduleTime = task.minDelayTime * Startup::ONE_SECOND_MILLISECONDS;

    // 由于SaMgr暂不支持记录启动原因，因此临时将启动原因写入SP文件中
    DelayedSingleton<FirmwarePreferencesUtil>::GetInstance()->SaveInt(Constant::PROCESS_RESTART_REASON,
        CAST_INT(task.startupReason));

    int32_t ret = StartServiceByTimer(Startup::UPDATER_SA_NAME.c_str(), scheduleTime);
    ENGINE_LOGI("StartServiceByTimer finish, ret is %{public}d", ret);
    return ret == 0;
}
} // namespace UpdateEngine
} // namespace OHOS