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

#ifndef FIRMWARE_SYS_INSTALLER_CALLBACK_H
#define FIRMWARE_SYS_INSTALLER_CALLBACK_H

#include "isys_installer_callback.h"
#include "isys_installer_callback_func.h"

#include "firmware_sys_installer_install.h"

namespace OHOS {
namespace UpdateEngine {
class SysInstallerCallback : public SysInstaller::ISysInstallerCallbackFunc {
public:
    explicit SysInstallerCallback(SysInstallerExecutorCallback &installCallback);

    void OnUpgradeProgress(SysInstaller::UpdateStatus updateStatus, int percent, const std::string &resultMsg) final;

private:
    SysInstallerExecutorCallback sysInstallCallback_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_SYS_INSTALLER_CALLBACK_H