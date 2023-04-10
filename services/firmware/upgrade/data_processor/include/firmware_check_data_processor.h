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

#ifndef FIRMWARE_CHECK_DATA_PROCESSOR_H
#define FIRMWARE_CHECK_DATA_PROCESSOR_H

#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_preferences_utils.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareCheckDataProcessor {
public:
    FirmwareCheckDataProcessor();
    virtual ~FirmwareCheckDataProcessor();

    void SetCheckResult(
        CheckStatus status, const Duration &duration, const std::vector<FirmwareComponent> &componentList);
    bool IsCheckFailed();
    bool IsSameWithDb();
    bool HasNewVersion();
    void BuildCheckResult(CheckResult &checkResult);
    bool IsUpdateOnStatus();

protected:
    virtual void HandleUndoneEvent() {}

protected:
    std::shared_ptr<FirmwarePreferencesUtil> preferencesUtil_ =
        DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();

private:
    void HandleCheckResult();
    void HandleNewVersion();
    void HandleNoNewVersion();
    bool IsVersionSameWithDb();
    void UpdateFirmwareComponent();
    bool HasUndoneTask();
    void RefreshPollingCycle();

    CombinationType GetCombinationType();
    void BuildComponentSPath();
    void BuildVersionDigest(NewVersionInfo &newVersionInfo, const std::vector<FirmwareComponent> &componentList);

private:
    std::vector<FirmwareComponent> componentList_;
    Duration duration_;
    bool isSameWithDb_ = true;
    CheckStatus status_ = CheckStatus::CHECK_FAIL;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_CHECK_DATA_PROCESSOR_H