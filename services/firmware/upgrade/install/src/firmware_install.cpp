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

#include "firmware_install.h"

#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statfs.h>

#include "dupdate_errno.h"
#include "firmware_log.h"
#include "file_utils.h"

namespace OHOS {
namespace UpdateEngine {
void FirmwareInstall::StartInstall(const std::vector<FirmwareComponent> &componentList, FirmwareInstallCallback &cb)
{
    if (IsInstalling()) {
        FIRMWARE_LOGE("install is busying");
        CallbackFailedResult(cb, "install is busying", DUPDATE_ERR_SYSTEM_BUSY_ON_INSTALL);
        return;
    }

    if (!IsComponentLegal(componentList)) {
        FIRMWARE_LOGE("check component failed!");
        CallbackFailedResult(cb, "check component failed!", DUPDATE_ERR_UPDATE_PRECHECK_FAIL);
        return;
    }

    SetIsInstalling(true);
    onInstallCallback_ = cb;
    bool result = PerformInstall(componentList);
    SetIsInstalling(false);
    CallbackResult(cb, result);
}

bool FirmwareInstall::IsInstalling()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return isInstalling_;
}

void FirmwareInstall::SetIsInstalling(bool isInstalling)
{
    std::lock_guard<std::mutex> lock(mutex_);
    isInstalling_ = isInstalling;
}

void FirmwareInstall::CallbackFailedResult(FirmwareInstallCallback &cb, const std::string &errorMsg, int32_t errCode)
{
    ErrMsg errMsg = { errCode, errorMsg };
    cb.onFirmwareEvent(false, errMsg);
}

void FirmwareInstall::CallbackResult(FirmwareInstallCallback &cb, const bool result)
{
    cb.onFirmwareEvent(result, errMsg_);
}
} // namespace UpdateEngine
} // namespace OHOS
