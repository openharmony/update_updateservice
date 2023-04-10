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

#ifndef FIRMWARE_MANAGER_H
#define FIRMWARE_MANAGER_H

#include "singleton.h"

#include "constant.h"
#include "firmware_common.h"
#include "firmware_flow_manager.h"
#include "firmware_result_process.h"
#include "firmware_task.h"
#include "device_adapter.h"
#include "firmware_preferences_utils.h"
#include "schedule_task.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareManager : public DelayedSingleton<FirmwareManager> {
    DECLARE_DELAYED_SINGLETON(FirmwareManager);

public:
    void Init(StartupReason startupReason);
    bool IsIdle();
    std::vector<ScheduleTask> GetScheduleTasks();
    bool Exit();

    void DoCheck(OnCheckComplete onCheckComplete);
    void DoDownload(const DownloadOptions &downloadOptions, BusinessError &businessError);
    void DoInstall(const UpgradeOptions &upgradeOptions, BusinessError &businessError, InstallType installType);
    void DoAutoDownloadSwitchChanged(bool isDownloadSwitchOn);
    void DoClearError(BusinessError &businessError);
    void HandleEvent(CommonEventType event);
    void DoCancelDownload(BusinessError &businessError);
    void DoTerminateUpgrade(BusinessError &businessError);

private:
    void DelayInit(StartupReason startupReason);
    void RestoreUpdate();
    void HandleBootComplete();
    void HandleNetChanged();
    void NotifyInitEvent();

    void HandleBootInstallOnStatusProcess(FirmwareTask &task);
    void HandleBootApplyOnStatusProcess(const FirmwareTask &task);
    void HandleBootUpdateOnStatusProcess(const FirmwareTask &task);
    void HandleAbInstallBootUpdateOnStatusProcess(const FirmwareTask &task,
        const std::map<std::string, UpdateResult> &resultMap);
    void HandleBootDownloadOnStatusProcess(FirmwareTask &task);
    void HandleBootDownloadPauseStatusProcess(FirmwareTask &task);
    void HandleBootDownloadedStatusProcess(FirmwareTask &task);
    void HandleBootUpdateSuccess(const FirmwareTask &task, const std::map<std::string, UpdateResult> &resultMap);
    void HandleBootUpdateFail(const FirmwareTask &task, const std::map<std::string, UpdateResult> &resultMap);
    void DoAutoDownload(const FirmwareTask &task);

    void HandleResumeDownload(FirmwareTask &task);
    bool CanInstall(void);

private:
    std::shared_ptr<FirmwarePreferencesUtil> preferencesUtil_ =
        DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_MANAGER_H