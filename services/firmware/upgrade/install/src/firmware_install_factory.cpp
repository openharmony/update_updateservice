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

#include "firmware_install_factory.h"
#include "firmware_sys_installer_install.h"
#include "firmware_updater_install.h"

namespace OHOS {
namespace UpdateEngine {
std::shared_ptr<FirmwareInstall> InstallFactory::GetInstance(InstallType type)
{
    switch (type) {
        case InstallType::SYS_INSTALLER:
            return std::make_shared<SysInstallerInstall>();
        default:
            return std::make_shared<UpdaterInstall>();
    }
}
} // namespace UpdateEngine
} // namespace OHOS