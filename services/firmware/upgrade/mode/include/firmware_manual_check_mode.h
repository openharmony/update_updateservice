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

#ifndef FIRMWARE_MANUAL_CHECK_MODE_H
#define FIRMWARE_MANUAL_CHECK_MODE_H

#include "firmware_check_data_processor.h"

#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_iexecute_mode.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareManualCheckMode final : public FirmwareIExecuteMode {
public:
    FirmwareManualCheckMode(OnCheckComplete onCheckComplete) : onCheckComplete_(onCheckComplete) {}
    ~FirmwareManualCheckMode() = default;
    void SetCheckResult(CheckStatus status, const Duration &duration,
        const std::vector<FirmwareComponent> &componentList, const CheckAndAuthInfo &checkAndAuthInfo) final;
    FirmwareStep GetNextStep(FirmwareStep step) final;
    void HandleComplete() final;

private:
    FirmwareStep GetStepAfterInit();
    FirmwareStep GetStepAfterCheck();

private:
    OnCheckComplete onCheckComplete_;
    FirmwareCheckDataProcessor checkDataProcessor_;
    BusinessError businessError_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_MANUAL_CHECK_MODE_H