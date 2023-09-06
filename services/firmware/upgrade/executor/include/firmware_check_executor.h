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

#ifndef FIRMWARE_CHECK_EXECUTOR_H
#define FIRMWARE_CHECK_EXECUTOR_H

#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_iexecutor.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareCheckExecutor : public FirmwareIExecutor {
public:
    FirmwareCheckExecutor(FirmwareCheckComponentCallback checkComponentCallback)
        : checkComponentCallback_(checkComponentCallback) {}
    ~FirmwareCheckExecutor() = default;
    void Execute() final;

private:
    void DoCheck();

    void SetComponentCheckStatus();
    void Complete();

private:
    FirmwareCheckComponentCallback checkComponentCallback_;
    Duration duration_;
    CheckAndAuthInfo checkAndAuthInfo_;
    std::vector<FirmwareComponent> componentList_;
    CheckStatus status_ = CheckStatus::CHECK_SUCCESS;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_CHECK_EXECUTOR_H