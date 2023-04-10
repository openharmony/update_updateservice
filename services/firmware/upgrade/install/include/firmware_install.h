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

#ifndef FIRMWARE_INSTALL_H
#define FIRMWARE_INSTALL_H

#include <ohos_types.h>
#include <string>
#include <vector>

#include "firmware_common.h"
#include "firmware_component.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
constexpr int32_t SLEEP_INSTALL = 1;

struct InstallProgress {
    Progress progress;
    ErrMsg errMsg;
};

using OnFirmwareProgress = std::function<void(const FirmwareComponent &component)>;
using OnFirmwareEvent = std::function<void(const bool result, const ErrMsg &errMsg)>;
using OnFirmwareStatus = std::function<void(const UpgradeStatus &status)>;
struct FirmwareInstallCallback {
    OnFirmwareProgress onFirmwareProgress;
    OnFirmwareEvent onFirmwareEvent;
    OnFirmwareStatus onFirmwareStatus;
};

class FirmwareInstall {
public:
    virtual ~FirmwareInstall() = default;
    void StartInstall(const std::vector<FirmwareComponent> &componentList, FirmwareInstallCallback &cb);

private:
    virtual bool IsComponentLegal(const std::vector<FirmwareComponent> &componentList) = 0;
    virtual bool PerformInstall(const std::vector<FirmwareComponent> &componentList) = 0;

    void SetIsInstalling(bool isInstalling);
    bool IsInstalling();
    void CallbackResult(FirmwareInstallCallback &cb, const bool result);
    void CallbackFailedResult(FirmwareInstallCallback &cb, const std::string &errorMsg, int32_t errCode);

protected:
    FirmwareInstallCallback onInstallCallback_;
    ErrMsg errMsg_;

private:
    std::mutex mutex_;
    bool isInstalling_ = false;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_INSTALL_H