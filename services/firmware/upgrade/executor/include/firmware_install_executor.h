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

#ifndef FIRMWARE_INSTALL_EXECUTOR_H
#define FIRMWARE_INSTALL_EXECUTOR_H

#include "firmware_component.h"
#include "firmware_common.h"
#include "firmware_iexecutor.h"
#include "firmware_task.h"
#include "firmware_preferences_utils.h"
#include "firmware_install.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareInstallExecutor : public FirmwareIExecutor {
public:
    FirmwareInstallExecutor(InstallType installType, FirmwareInstallExecutorCallback firmwareInstallCallback)
        : installType_(installType), installCallback_(firmwareInstallCallback) {}
    ~FirmwareInstallExecutor() = default;
    void Execute() final;

private:
    void DoInstall();
    void GetTask();
    void HandleInstallProgress(const FirmwareComponent &component, const Progress &progress);
    void HandleInstallResult(const bool result, const ErrMsg &errMsg);

private:
    InstallType installType_;
    FirmwareInstallExecutorCallback installCallback_;
    InstallCallbackInfo installCallbackInfo_;
    std::vector<FirmwareComponent> components_ {};
    FirmwareTask tasks_ {};
    Progress taskProgress_ {};
    std::shared_ptr<FirmwarePreferencesUtil> preferencesUtil_ =
        DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_INSTALL_EXECUTOR_H