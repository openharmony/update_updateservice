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

#ifndef FIRMWARE_IEXECUTE_MODE_H
#define FIRMWARE_IEXECUTE_MODE_H

#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_iexecutor.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
using OnCheckComplete = std::function<void(BusinessError &businessError, CheckResult &checkResult)>;
using OnExecuteFinishCallback = std::function<void()>;

class FirmwareIExecuteMode {
public:
    virtual ~FirmwareIExecuteMode() = default;
    virtual FirmwareStep GetNextStep(FirmwareStep step)
    {
        return FirmwareStep::COMPLETE;
    };

    virtual void SetCheckResult(CheckStatus status, const Duration &duration,
        const std::vector<FirmwareComponent> &componentList, const CheckAndAuthInfo &checkAndAuthInfo) {};
    virtual void SetDownloadProgress(const Progress &progress) {};
    virtual void SetInstallResult(const InstallCallbackInfo &installCallbackInfo) {};
    virtual void SetApplyResult(bool isSuccess) {};

    // HandleComplete 往FirmwareManager回调之后，当前mode会被析构掉，因此后续不应该再调用任何逻辑
    virtual void HandleComplete() {};

    virtual DownloadOptions GetDownloadOptions()
    {
        DownloadOptions downloadOptions;
        return downloadOptions;
    };

    virtual UpgradeOptions GetUpgradeOptions()
    {
        UpgradeOptions upgradeOptions;
        return upgradeOptions;
    };

    virtual InstallType GetInstallType()
    {
        return InstallType::UPDATER;
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_IEXECUTE_MODE_H