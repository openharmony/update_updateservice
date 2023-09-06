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

#ifndef FIRMWARE_INSTALL_DATA_RPOCESSOR_H
#define FIRMWARE_INSTALL_DATA_RPOCESSOR_H

#include "firmware_iexecutor.h"
#include "firmware_task.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareInstallDataProcessor {
public:
    FirmwareInstallDataProcessor() = default;
    ~FirmwareInstallDataProcessor() = default;

    void SetInstallResult(const InstallCallbackInfo &installCallbackInfo);
    bool HasInstallSuccess();
    bool HasUpdateSuccess();
    bool IsSpaceEnough(int64_t &requireTotalSize);
    bool IsUpgradeFileCheckSuccess();

private:
    void GetTask();

private:
    FirmwareTask tasks_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_INSTALL_DATA_RPOCESSOR_H