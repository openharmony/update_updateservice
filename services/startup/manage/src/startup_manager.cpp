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

#include "startup_manager.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "access_factory.h"
#include "file_utils.h"
#include "firmware_preferences_utils.h"
#include "schedule_config.h"
#include "string_utils.h"
#include "time_utils.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
StartupManager::StartupManager()
{
    ENGINE_LOGD("StartupManager constructor");
    scheduleManager_ = std::make_shared<ScheduleManager>(
        DelayedSingleton<AccessManager>::GetInstance(), DelayedSingleton<StartupSchedule>::GetInstance());
    lastIdleCheckTime_ = TimeUtils::GetTimestamp();
    ENGINE_LOGI("init lastIdleCheckTime is %{public}s", TimeUtils::GetPrintTimeStr(lastIdleCheckTime_).c_str());
}

StartupManager::~StartupManager()
{
    ENGINE_LOGD("StartupManager deConstructor");
}

void StartupManager::Start()
{
    ENGINE_LOGI("Start");
    Init();
    IdleLoop();
}

void StartupManager::Init() const
{
    ENGINE_LOGI("Init");
    ScheduleConfig::InitConfig();
    StartupReason startupReason = GetStartupReason();
    ENGINE_LOGI("startupReason is %{public}d", CAST_INT(startupReason));

    if (startupReason == StartupReason::PROCESS_ENV_RESET) {
        FileUtils::DestroyBaseDirectory(BASE_DIR_INFOS);
    }
    FileUtils::InitAndCreateBaseDirs(BASE_DIR_INFOS);

    for (auto &type : Startup::ACCESS_TYPES) {
        AccessFactory::GetInstance(type)->Init(startupReason);
    }
}

void StartupManager::IdleLoop()
{
    ENGINE_LOGI("IdleLoop");
    DelayedSingleton<StartupSchedule>::GetInstance()->RegisterLooper([=]() {
        if (!IdleCheck()) {
            ENGINE_LOGD("IdleLoop not idle");
            return;
        }

        if (!PreExit()) {
            ENGINE_LOGI("IdleLoop pre exit fail");
            return;
        }

        SAExit();
    });
}

bool StartupManager::IdleCheck()
{
    if (scheduleManager_ == nullptr) {
        ENGINE_LOGE("IdleCheck scheduleManager is null, return idle");
        return true;
    }

    if (abs(TimeUtils::GetTimestamp() - lastIdleCheckTime_) <
        static_cast<int64_t>(ScheduleConfig::GetIdleCheckInterval())) {
        ENGINE_LOGD("IdleCheck check time not arrive: lastIdleCheckTime is %{public}s",
            TimeUtils::GetPrintTimeStr(lastIdleCheckTime_).c_str());
        return false;
    }

    lastIdleCheckTime_ = TimeUtils::GetTimestamp();
    ENGINE_LOGI(
        "IdleCheck update lastIdleCheckTime: %{public}s", TimeUtils::GetPrintTimeStr(lastIdleCheckTime_).c_str());

    bool isIdle = scheduleManager_->IdleCheck();
    ENGINE_LOGI("IdleCheck idleState is %{public}s", StringUtils::GetBoolStr(isIdle).c_str());
    return isIdle;
}

bool StartupManager::PreExit()
{
    if (scheduleManager_ == nullptr) {
        ENGINE_LOGE("IdleCheck PreExit is null, return true");
        return true;
    }
    return scheduleManager_->Exit();
}

void StartupManager::SAExit() const
{
    ENGINE_LOGI("SAExit");
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        ENGINE_LOGE("GetSystemAbilityManager samgr object null!");
        return;
    }
    int32_t res = sm->UnloadSystemAbility(UPDATE_DISTRIBUTED_SERVICE_ID);
    ENGINE_LOGI("UnloadSystemAbility res is %{public}d", res);
}

StartupReason StartupManager::GetStartupReason() const
{
    if (TimeUtils::IsInRebootDuration()) {
        return StartupReason::DEVICE_REBOOT;
    }
    // 由于SaMgr暂不支持记录启动原因，因此临时从SP文件中读取启动原因
    auto preferenceUtil = DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
    auto reasonFromSp = static_cast<StartupReason>(
        preferenceUtil->ObtainInt(Constant::PROCESS_RESTART_REASON, CAST_INT(StartupReason::UNKNOWN)));
    if (reasonFromSp == StartupReason::UNKNOWN) {
        return StartupReason::SCHEDULE_TASK;
    }
    // 读取后清除SP文件中记录的标记
    preferenceUtil->Remove(Constant::PROCESS_RESTART_REASON);
    return reasonFromSp;
}
} // namespace UpdateEngine
} // namespace OHOS