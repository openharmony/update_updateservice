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

#ifndef FIRMWARE_IEXECUTOR_H
#define FIRMWARE_IEXECUTOR_H

#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_log.h"

namespace OHOS {
namespace UpdateEngine {
using FirmwareComponentCallback = std::function<void(
    CheckStatus status, const Duration &duration, const std::vector<FirmwareComponent> &componentList,
    const CheckAndAuthInfo &checkAndAuthInfo)>;
struct FirmwareCheckComponentCallback {
    FirmwareComponentCallback firmwareComponentCallback;
};

using ProgressCallback = std::function<void(const Progress &progress)>;
struct FirmwareProgressCallback {
    ProgressCallback progressCallback;
};

struct InstallCallbackInfo {
    Progress progress;
    ErrorMessage errorMessage;
};

using InstallCallback = std::function<void(const InstallCallbackInfo &installCallbackInfo)>;
struct FirmwareInstallExecutorCallback {
    InstallCallback installCallback;
};

using ApplyCallback = std::function<void(bool isSuccess)>;
struct FirmwareApplyCallback {
    ApplyCallback applyCallback;
};

class FirmwareIExecutor {
public:
    virtual ~FirmwareIExecutor() = default;
    virtual void Execute() = 0;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_IEXECUTOR_H