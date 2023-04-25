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

#include "firmware_download_executor.h"

#include <unistd.h>
#include <sys/stat.h>
#include <thread>

#include "constant.h"
#include "dupdate_errno.h"
#include "download_info.h"
#include "firmware_callback_utils.h"
#include "firmware_component_operator.h"
#include "firmware_log.h"
#include "firmware_task_operator.h"
#include "firmware_update_helper.h"
#include "progress_thread.h"
#include "string_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
const mode_t MKDIR_MODE = 0777;
void FirmwareDownloadExecutor::Execute()
{
    FIRMWARE_LOGI("FirmwareDownloadExecutor::Execute");
    std::thread downloadThread(&FirmwareDownloadExecutor::DoDownload, this);
    downloadThread.detach();
}

void FirmwareDownloadExecutor::DoDownload()
{
    FirmwareComponentOperator().QueryAll(components_);
    FIRMWARE_LOGI("Execute size %{public}d", CAST_INT(components_.size()));
    if (components_.size() == 0) {
        Progress progress;
        progress.status = UpgradeStatus::DOWNLOAD_FAIL;
        progress.endReason = "no task";
        firmwareProgressCallback_.progressCallback(progress);
        return;
    }

    GetTask();
    if (tasks_.downloadTaskId.empty()) {
        // 首次触发下载
        PerformDownload();
    } else {
        // 恢复下载
        Progress progress;
        progress.status = UpgradeStatus::DOWNLOAD_FAIL;
        progress.endReason = "not support";
        firmwareProgressCallback_.progressCallback(progress);
        return;
    }
}

void FirmwareDownloadExecutor::PerformDownload()
{
    std::vector<DownloadInfo> downLoadInfos;
    for (const auto &component : components_) {
        DownloadInfo downloadInfo;
        downloadInfo.versionId = component.url;
        downloadInfo.url = component.url;
        downloadInfo.path = component.spath;
        downloadInfo.packageSize = component.size;
        downloadInfo.veriftInfo = component.verifyInfo;
        downloadInfo.isNeedAutoResume = false;
        downLoadInfos.push_back(downloadInfo);
        FIRMWARE_LOGD("downloadInfo %s", downloadInfo.ToString().c_str());

        if (access(Constant::DUPDATE_ENGINE_PACKAGE_ROOT_PATH.c_str(), 0) == -1) {
            mkdir(Constant::DUPDATE_ENGINE_PACKAGE_ROOT_PATH.c_str(), MKDIR_MODE);
        }

        Progress progress0 = {0, UpgradeStatus::DOWNLOADING, ""};

        std::string downloadFileName = downloadInfo.path;
        size_t localFileLength = DownloadThread::GetLocalFileLength(downloadFileName);
        ENGINE_LOGI("Download %zu %s", localFileLength, downloadFileName.c_str());
        if (localFileLength == downloadInfo.packageSize && downloadInfo.packageSize != 0) {
            progress0.percent = DOWNLOAD_FINISH_PERCENT;
            progress0.status = UpgradeStatus::DOWNLOAD_SUCCESS;
            DownloadCallback(downloadInfo.url, downloadFileName, progress0);
        }
        upgradeStatus_ = UpgradeStatus::DOWNLOADING;
        downloadThread_ = std::make_shared<DownloadThread>(
            [&](const std::string serverUrl, const std::string &fileName,
                const Progress &progress) -> void {
                DownloadCallback(serverUrl, fileName, progress);
            });
        ProgressThread::isCancel_ = false;
        int32_t ret = downloadThread_->StartDownload(downloadFileName, downloadInfo.url);
        if (ret != 0) {
            Progress progress0;
            progress0.status = UpgradeStatus::DOWNLOAD_FAIL;
            progress0.endReason = std::to_string(CAST_INT(DownloadEndReason::FAIL));
            firmwareProgressCallback_.progressCallback(progress0);
        }
    }

    DelayedSingleton<FirmwareCallbackUtils>::GetInstance()->NotifyEvent(
        tasks_.taskId, EventId::EVENT_DOWNLOAD_START, UpgradeStatus::DOWNLOADING);
}

void FirmwareDownloadExecutor::GetTask()
{
    FirmwareTaskOperator().QueryTask(tasks_);
}

void FirmwareDownloadExecutor::DownloadCallback(std::string serverUrl, std::string packageName, Progress progress)
{
    ENGINE_LOGD("FirmwareDownloadExecutor::DownloadCallback progress.status = %{public}d,"
        " progress.percent = %{public}d", progress.status, progress.percent);
    Progress downloadProgress {};
    upgradeStatus_ = UpgradeStatus::DOWNLOADING;
    if (progress.status == UpgradeStatus::DOWNLOAD_FAIL ||
        progress.status == UpgradeStatus::DOWNLOAD_SUCCESS) {
        upgradeStatus_ = progress.status;
    }
    downloadProgress.percent = progress.percent;
    downloadProgress.status = progress.status;
    downloadProgress.endReason = progress.endReason;

#ifdef UPDATER_UT
    upgradeStatus_ = UpgradeStatus::DOWNLOAD_SUCCESS;
#endif
    std::string fileName = packageName;
    ENGINE_LOGI("DownloadCallback status: %{public}d progress: %{public}d", progress.status, progress.percent);

    if (upgradeStatus_ == UpgradeStatus::DOWNLOAD_SUCCESS) {
        ENGINE_LOGI("DownloadCallback fileName %{public}s", fileName.c_str());
        if (!VerifyDownloadPkg(fileName, downloadProgress)) {
            // If the verification fails, delete the corresponding package.
            remove(fileName.c_str());
            downloadProgress.status = UpgradeStatus::DOWNLOAD_FAIL;
            downloadProgress.endReason = std::to_string(DUpdateErrno::DUPDATE_ERR_VERIFY_PACKAGE_FAIL);
        }
    }

    FirmwareComponentOperator firmwareComponentOperator;
    // 单包进度插入到 component 表
    ENGINE_LOGI("DownloadCallback serverUrl %s, status %{public}d %{public}d", serverUrl.c_str(),
        progress.status, progress.percent);
    firmwareComponentOperator.UpdateProgressByUrl(serverUrl, progress.status, progress.percent);
    // 整体进度插入到 task 表
    FirmwareTaskOperator().UpdateProgressByTaskId(tasks_.taskId, downloadProgress.status, downloadProgress.percent);
    firmwareProgressCallback_.progressCallback(downloadProgress);
}

bool FirmwareDownloadExecutor::VerifyDownloadPkg(const std::string &pkgName, Progress &progress)
{
    std::string verifyInfo = "";
    for (auto &component : components_) {
        if (component.spath == pkgName) {
            verifyInfo = component.verifyInfo;
            break;
        }
    }
    ENGINE_LOGI("Start Checking file Sha256 %{public}s, verifyInfo %{public}s", pkgName.c_str(), verifyInfo.c_str());
    if (!verifyInfo.empty() && !Sha256Utils::CheckFileSha256String(pkgName, verifyInfo)) {
        ENGINE_LOGE("file sha256 check error, fileName:%{public}s", pkgName.c_str());
        return false;
    }
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS
