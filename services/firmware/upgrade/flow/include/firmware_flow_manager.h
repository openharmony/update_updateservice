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

#ifndef FIRMWARE_FLOW_MANAGER_H
#define FIRMWARE_FLOW_MANAGER_H

#include "update_helper.h"
#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_iexecute_mode.h"
#include "firmware_iexecutor.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareFlowManager {
public:
    FirmwareFlowManager();
    ~FirmwareFlowManager();

    void SetExecuteMode(std::shared_ptr<FirmwareIExecuteMode> executeMode);
    void Start();

private:
    void Execute();
    std::shared_ptr<FirmwareIExecutor> CreateInstance(FirmwareStep step);
    void Complete();

    void OnCheckCallback(CheckStatus status, const Duration &duration,
        const std::vector<FirmwareComponent> &componentList, const CheckAndAuthInfo &checkAndAuthInfo);

    void OnDownloadCallback(const Progress &progress);
    void OnInstallCallback(const InstallCallbackInfo &installCallbackInfo);
    void OnApplyCallback(bool isSuccess);

private:
    std::shared_ptr<FirmwareIExecuteMode> executeMode_;
    std::shared_ptr<FirmwareIExecutor> executor_;
    FirmwareStep nextStep_ = FirmwareStep::INIT;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_FLOW_MANAGER_H