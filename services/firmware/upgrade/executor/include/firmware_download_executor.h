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

#ifndef FIRMWARE_DOWNLOAD_EXECUTOR_H
#define FIRMWARE_DOWNLOAD_EXECUTOR_H

#include "firmware_component.h"
#include "firmware_iexecutor.h"
#include "firmware_task.h"
#include "progress_thread.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareDownloadExecutor : public FirmwareIExecutor {
public:
    FirmwareDownloadExecutor(const DownloadOptions &downloadOptions, FirmwareProgressCallback progressCallback)
        : downloadOptions_(downloadOptions), firmwareProgressCallback_(progressCallback) {}
    ~FirmwareDownloadExecutor() = default;
    void Execute() final;

private:
    void DoDownload();
    void GetTask();
    void PerformDownload();
    void DownloadCallback(std::string serverUrl, std::string packageName, Progress progress);
    bool VerifyDownloadPkg(const std::string &pkgName, Progress &progress);

private:
    DownloadOptions downloadOptions_;
    std::vector<FirmwareComponent> components_;
    FirmwareProgressCallback firmwareProgressCallback_;
    FirmwareTask tasks_;
    UpgradeStatus upgradeStatus_;
    std::shared_ptr<DownloadThread> downloadThread_ = nullptr;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_DOWNLOAD_EXECUTOR_H