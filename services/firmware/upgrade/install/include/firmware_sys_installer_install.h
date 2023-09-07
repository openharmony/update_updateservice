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

#ifndef FIRMWARE_SYS_INSTALLER_INSTALL_H
#define FIRMWARE_SYS_INSTALLER_INSTALL_H

#include <string>

#include "firmware_component.h"
#include "firmware_install.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
using OnSysInstallerCallback = std::function<void(const InstallProgress &installProgress)>;

struct SysInstallerExecutorCallback {
    OnSysInstallerCallback onSysInstallerCallback;
};

class SysInstallerInstall final : public FirmwareInstall {
private:
    bool IsComponentLegal(const std::vector<FirmwareComponent> &componentList) final;
    bool PerformInstall(const std::vector<FirmwareComponent> &componentList) final;

    int32_t DoSysInstall(const FirmwareComponent &firmwareComponent);
    void InitInstallProgress();
    int32_t WaitInstallResult();

private:
    Progress sysInstallProgress_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_SYS_INSTALLER_INSTALL_H