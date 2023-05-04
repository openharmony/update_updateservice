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

#include "firmware_sys_installer_callback.h"

#include "firmware_log.h"

namespace OHOS {
namespace UpdateEngine {
SysInstallerCallback::SysInstallerCallback(SysInstallerExecutorCallback &installCallback)
{
    sysInstallCallback_ = installCallback;
}

void SysInstallerCallback::OnUpgradeProgress(SysInstaller::UpdateStatus updateStatus, int percent, const std::string &resultMsg)
{
    FIRMWARE_LOGI("sysInstallerCallback OnUpgradeProgress status %{public}d percent %{public}d", updateStatus, percent);
    InstallProgress installProgress = {};
    switch (updateStatus) {
        case SysInstaller::UpdateStatus::UPDATE_STATE_INIT:
        case SysInstaller::UpdateStatus::UPDATE_STATE_ONGOING:
            installProgress.progress.status = UpgradeStatus::INSTALLING;
            break;
        case SysInstaller::UpdateStatus::UPDATE_STATE_SUCCESSFUL:
            installProgress.progress.status = UpgradeStatus::INSTALL_SUCCESS;
            break;
        default:
            installProgress.progress.status = UpgradeStatus::INSTALL_FAIL;
            installProgress.errMsg.errorMsg = resultMsg;
            break;
    }

    installProgress.progress.percent = static_cast<uint32_t>(percent);
    installProgress.errMsg.errorCode = CAST_INT(updateStatus);
    sysInstallCallback_.onSysInstallerCallback(installProgress);
}
} // namespace UpdateEngine
} // namespace OHOS
