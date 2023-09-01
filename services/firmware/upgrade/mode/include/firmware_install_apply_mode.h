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

#ifndef FIRMWARE_INSTALL_APPLY_MODE_H
#define FIRMWARE_INSTALL_APPLY_MODE_H

#include "firmware_common.h"
#include "firmware_download_data_processor.h"
#include "firmware_iexecute_mode.h"
#include "firmware_install_data_processor.h"
#include "firmware_preferences_utils.h"
#include "firmware_task.h"
#include "device_adapter.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareInstallApplyMode final : public FirmwareIExecuteMode {
public:
    FirmwareInstallApplyMode(const UpgradeOptions &upgradeOptions, BusinessError &businessError,
        InstallType installType, OnExecuteFinishCallback onExecuteFinishCallback)
        : upgradeOptions_(upgradeOptions),
          businessError_(businessError),
          installType_(installType),
          onExecuteFinishCallback_(onExecuteFinishCallback) {}

    ~FirmwareInstallApplyMode() = default;
    FirmwareStep GetNextStep(FirmwareStep step) final;
    void SetInstallResult(const InstallCallbackInfo &installCallbackInfo) final;
    UpgradeOptions GetUpgradeOptions() final;
    InstallType GetInstallType();
    void HandleComplete() final;

private:
    FirmwareStep GetStepAfterInit();
    FirmwareStep GetStepAfterInstall();
    FirmwareStep GetStepAfterApply();
    FirmwareStep GetStepForInstallAndApplyOrder();
    bool IsAllowInstall();
    bool IsUpgradeFilesReady();

    void GetTask();
    void SetApplyResult(bool isSuccess);

private:
    FirmwareInstallDataProcessor installStepDataProcessor_;
    UpgradeOptions upgradeOptions_ {};
    BusinessError &businessError_;
    InstallType installType_;
    OnExecuteFinishCallback onExecuteFinishCallback_;
    FirmwareTask tasks_ {};
    std::shared_ptr<FirmwarePreferencesUtil> preferencesUtil_ =
        DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_INSTALL_APPLY_MODE_H