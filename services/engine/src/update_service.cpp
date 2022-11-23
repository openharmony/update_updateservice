/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "update_service.h"

#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <sys/reboot.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <vector>

#include "cJSON.h"
#include "init_reboot.h"
#include "iservice_registry.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "openssl/err.h"
#include "openssl/ssl.h"
#include "package/package.h"
#include "parameter.h"
#include "parameters.h"
#include "progress_thread.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "update_service_ab_update.h"
#include "update_system_event.h"
#include "updaterkits/updaterkits.h"

namespace OHOS {
namespace UpdateEngine {
REGISTER_SYSTEM_ABILITY_BY_ID(UpdateService, UPDATE_DISTRIBUTED_SERVICE_ID, true)

constexpr int32_t PORT_NUMBER = 5022;
const mode_t MKDIR_MODE = 0777;
constexpr int32_t JSON_MAX_SIZE = 4096;
constexpr int32_t OS_NAME_MAX_LEN = 16;
constexpr int32_t DEV_NAME_MAX_LEN = 32;
constexpr int32_t VER_NAME_MAX_LEN = 64;
constexpr uint32_t MAX_PERCENT = 100;

const std::string UPDATER_PKG_NAME = "/data/ota_package/updater.zip";
const std::string MISC_FILE = "/dev/block/by-name/misc";
const std::string BASE_PATH = "/data/ota_package";
const std::string CMD_WIPE_DATA = "--user_wipe_data";
#ifndef UPDATER_UT
const std::string SIGNING_CERT_NAME = "/data/ota_package/signing_cert.crt";
#else
const std::string SIGNING_CERT_NAME = "/data/ota_package/signing_cert.crt";
#endif
const std::string PARAM_NAME_FOR_VERSION = "hw_sc.build.os.version";
const std::string DEFAULT_VERSION = "2.2.0";
const std::string PARAM_NAME_FOR_SEARCH = "update.serverip.search";
const std::string PARAM_NAME_FOR_DOWNLOAD = "update.serverip.download";
const std::string DEFAULT_SERVER_IP = "127.0.0.1";

OHOS::sptr<UpdateService> UpdateService::updateService_ { nullptr };

void UpdateService::ClientDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    ENGINE_LOGI("client DeathRecipient OnRemoteDied: %{public}s", upgradeInfo_.ToString().c_str());
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service != nullptr) {
        service->UnregisterUpdateCallback(upgradeInfo_);
    }
}

UpdateService::ClientProxy::ClientProxy(const UpgradeInfo &info, const sptr<IUpdateCallback> &callback)
    : proxy_(callback)
{
    ENGINE_LOGI("UpdateService::ClientProxy constructor");
    auto clientDeathRecipient = new (std::nothrow) ClientDeathRecipient(info);
    if (clientDeathRecipient != nullptr) {
        deathRecipient_ = sptr<ClientDeathRecipient>(clientDeathRecipient);
    } else {
        ENGINE_LOGE("UpdateService::ClientProxy, new fail");
    }
}

UpdateService::ClientProxy &UpdateService::ClientProxy::operator=(const UpdateService::ClientProxy &source)
{
    if (&source != this) {
        proxy_ = source.proxy_;
        deathRecipient_ = source.deathRecipient_;
    }
    return *this;
}

void UpdateService::ClientProxy::AddDeathRecipient()
{
    ENGINE_LOGI("UpdateService::ClientProxy AddDeathRecipient in");
    if (proxy_ != nullptr) {
        auto remoteObject = proxy_->AsObject();
        if ((remoteObject != nullptr) && (deathRecipient_ != nullptr)) {
            remoteObject->AddDeathRecipient(deathRecipient_);
            ENGINE_LOGI("UpdateService::ClientProxy AddDeathRecipient success");
        }
    }
}

void UpdateService::ClientProxy::RemoveDeathRecipient()
{
    ENGINE_LOGI("UpdateService::ClientProxy RemoveDeathRecipient in");
    if (proxy_ != nullptr) {
        auto remoteObject = proxy_->AsObject();
        if ((remoteObject != nullptr) && (deathRecipient_ != nullptr)) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
            ENGINE_LOGI("UpdateService::ClientProxy RemoveDeathRecipient success");
        }
    }
}

sptr<IUpdateCallback> UpdateService::ClientProxy::Get()
{
    return proxy_;
}

UpdateService::UpdateService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate)
{
    InitVersionInfo(versionInfo_);
}

UpdateService::~UpdateService()
{
    ENGINE_LOGE("UpdateServerTest free now");
    if (downloadThread_ != nullptr) {
        downloadThread_->StopDownload();
        delete downloadThread_;
        downloadThread_ = nullptr;
    }

    for (auto &iter : clientProxyMap_) {
        iter.second.RemoveDeathRecipient();
    }
}

sptr<UpdateService> UpdateService::GetInstance()
{
    return updateService_;
}

int32_t UpdateService::RegisterUpdateCallback(const UpgradeInfo &info, const sptr<IUpdateCallback> &updateCallback)
{
    ENGINE_LOGI("RegisterUpdateCallback");
    UnregisterUpdateCallback(info);
    {
        std::lock_guard<std::mutex> lock(clientProxyMapLock_);
        ClientProxy clientProxy(info, updateCallback);
        clientProxy.AddDeathRecipient();
        clientProxyMap_.insert({info, clientProxy});
    }
    if (!info.IsLocal()) {
        upgradeInfo_ = info;
    }
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::UnregisterUpdateCallback(const UpgradeInfo &info)
{
    ENGINE_LOGI("UnregisterUpdateCallback");
    std::lock_guard<std::mutex> lock(clientProxyMapLock_);
    auto iter = clientProxyMap_.find(info);
    if (iter == clientProxyMap_.end()) {
        return INT_CALL_SUCCESS;
    }
    iter->second.RemoveDeathRecipient();
    clientProxyMap_.erase(info);
    return INT_CALL_SUCCESS;
}

sptr<IUpdateCallback> UpdateService::GetUpgradeCallback(const UpgradeInfo &info)
{
    std::lock_guard<std::mutex> lock(clientProxyMapLock_);
    auto iter = clientProxyMap_.find(info);
    if (iter == clientProxyMap_.end()) {
        return nullptr;
    }
    return iter->second.Get();
}

void UpdateService::GetCheckResult(CheckResultEx &checkResult)
{
    ENGINE_LOGI("GetCheckResult start");
    checkResult.isExistNewVersion = versionInfo_.result[0].verifyInfo != "";
    checkResult.newVersionInfo.versionDigestInfo.versionDigest = "versionDigest";

    // packages
    checkResult.newVersionInfo.versionComponents[0].componentType = static_cast<uint32_t>(ComponentType::OTA);
    checkResult.newVersionInfo.versionComponents[0].upgradeAction = UpgradeAction::UPGRADE;
    checkResult.newVersionInfo.versionComponents[0].displayVersion = versionInfo_.result[0].versionName;
    checkResult.newVersionInfo.versionComponents[0].innerVersion = versionInfo_.result[0].versionCode;
    checkResult.newVersionInfo.versionComponents[0].size = versionInfo_.result[0].size;
    checkResult.newVersionInfo.versionComponents[0].effectiveMode = static_cast<size_t>(EffectiveMode::COLD);

    // descriptInfo
    checkResult.newVersionInfo.versionComponents[0].descriptionInfo.descriptionType = DescriptionType::CONTENT;
    checkResult.newVersionInfo.versionComponents[0].descriptionInfo.content = versionInfo_.descriptInfo[0].content;
}

int32_t UpdateService::GetNewVersion(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("GetNewVersion start");
    businessError.errorNum = CallResult::SUCCESS;
    CheckResultEx checkResult;
    GetCheckResult(checkResult);
    newVersionInfo = checkResult.newVersionInfo;

    ENGINE_LOGI("GetNewVersion finish %{public}s", newVersionInfo.versionComponents[0].displayVersion.c_str());
    ENGINE_LOGI("GetNewVersion componentType %{public}d", newVersionInfo.versionComponents[0].componentType);
    ENGINE_LOGI("GetNewVersion innerVersion %{public}s", newVersionInfo.versionComponents[0].innerVersion.c_str());
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::GetNewVersionDescription(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &newVersionDescriptionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGE("GetNewVersionDescription versionDigestInfo %{public}s format %{public}d language %{public}s",
        versionDigestInfo.versionDigest.c_str(),
        CAST_INT(descriptionOptions.format),
        descriptionOptions.language.c_str());
    businessError.Build(CallResult::UN_SUPPORT, "GetNewVersionDescription unsupport");
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
    BusinessError &businessError)
{
    businessError.errorNum = CallResult::SUCCESS;
    char osName[OS_NAME_MAX_LEN] = {0};
    if (strcpy_s(osName, sizeof(osName), GetOSFullName()) != EOK) {
        ENGINE_LOGE("GetOSFullName fail");
    }
    currentVersionInfo.osVersion = osName;

    char deviceName[DEV_NAME_MAX_LEN] = {0};
    if (strcpy_s(deviceName, sizeof(deviceName), GetProductModel()) != EOK) {
        ENGINE_LOGE("GetProductModel fail");
    }
    currentVersionInfo.deviceName = deviceName;

    char version[VER_NAME_MAX_LEN] = {0};
    if (strcpy_s(version, sizeof(version), GetDisplayVersion()) != EOK) {
        ENGINE_LOGE("GetDisplayVersion fail");
    }

    VersionComponent *versionComponent = &currentVersionInfo.versionComponents[0];
    versionComponent->displayVersion = version;
    versionComponent->innerVersion = version;

    versionComponent->descriptionInfo.descriptionType = DescriptionType::CONTENT;
    versionComponent->descriptionInfo.content = versionInfo_.descriptInfo[0].content;
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::GetCurrentVersionDescription(const UpgradeInfo &info,
    const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &currentVersionDescriptionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGE("GetCurrentVersionDescription format %{public}d language %{public}s",
        CAST_INT(descriptionOptions.format),
        descriptionOptions.language.c_str());
    businessError.Build(CallResult::UN_SUPPORT, "GetCurrentVersionDescription unsupport");
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateService::GetTaskInfo");
    businessError.errorNum = CallResult::SUCCESS;
    CheckResultEx checkResult;
    GetCheckResult(checkResult);
    taskInfo.existTask = checkResult.isExistNewVersion;
    if (!taskInfo.existTask) {
        ENGINE_LOGI("GetTaskInfo no new version");
        return INT_CALL_SUCCESS;
    }

    taskInfo.taskBody.versionDigestInfo.versionDigest = checkResult.newVersionInfo.versionDigestInfo.versionDigest;
    taskInfo.taskBody.status = otaStatus_.status;
    taskInfo.taskBody.subStatus = otaStatus_.subStatus;
    taskInfo.taskBody.progress = otaStatus_.progress;
    taskInfo.taskBody.errorMessages[0].errorCode = otaStatus_.errMsg[0].errorCode;
    taskInfo.taskBody.errorMessages[0].errorMessage = otaStatus_.errMsg[0].errorMsg;
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::SetUpgradePolicy(const UpgradeInfo &info, const UpgradePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("autoDownload %{public}d installmode %{public}d startTime %{public}d endTime %{public}d",
        policy.downloadStrategy, policy.autoUpgradeStrategy, policy.autoUpgradePeriods[0].start,
        policy.autoUpgradePeriods[1].end);
    businessError.errorNum = CallResult::SUCCESS;
    policy_ = policy;
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::GetUpgradePolicy(const UpgradeInfo &info, UpgradePolicy &policy, BusinessError &businessError)
{
    ENGINE_LOGI("GetUpgradePolicy");
    businessError.errorNum = CallResult::SUCCESS;
    policy = policy_;
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::CheckNewVersion(const UpgradeInfo &info)
{
    upgradeStatus_ = UPDATE_STATE_CHECK_VERSION_ON;
    otaStatus_.status = upgradeStatus_;
    int32_t engineSocket = socket(AF_INET, SOCK_STREAM, 0);
    ENGINE_CHECK(engineSocket >= 0, SearchCallback("socket error !", SERVER_BUSY);
        return INT_CALL_SUCCESS, "socket error !");

    std::string serverIp = OHOS::system::GetParameter(PARAM_NAME_FOR_SEARCH, DEFAULT_SERVER_IP);
    ENGINE_LOGI("CheckNewVersion serverIp: %s ", serverIp.c_str());

    sockaddr_in engineSin {};
    engineSin.sin_family = AF_INET;
    engineSin.sin_port = htons(PORT_NUMBER);
    int32_t ret = inet_pton(AF_INET, serverIp.c_str(), &engineSin.sin_addr);
    ENGINE_CHECK(ret > 0, close(engineSocket); SearchCallback("Invalid ip!", SERVER_BUSY);
        return INT_CALL_SUCCESS, "socket error");

    struct timeval tv = {TIMEOUT_FOR_CONNECT, 0};
    setsockopt(engineSocket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));
    ret = connect(engineSocket, reinterpret_cast<sockaddr*>(&engineSin), sizeof(engineSin));
    ENGINE_CHECK(ret == 0,
        SearchCallback("Connect error !", SERVER_BUSY);
        close(engineSocket);
        return INT_CALL_SUCCESS, "connect error");
    ReadDataFromSSL(engineSocket);
    return INT_CALL_SUCCESS;
}

static uint64_t GetTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

int32_t UpdateService::DownloadVersion(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DownloadOptions &downloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("DownloadVersion versionDigestInfo %{public}s allowNetwork %{public}d order %{public}d",
        versionDigestInfo.versionDigest.c_str(),
        CAST_INT(downloadOptions.allowNetwork),
        CAST_INT(downloadOptions.order));
    downloadInterval_ = GetTimestamp();
    if (access(BASE_PATH.c_str(), 0) == -1) {
        mkdir(BASE_PATH.c_str(), MKDIR_MODE);
    }

    Progress progress0 = {0, UPDATE_STATE_DOWNLOAD_ON, ""};
    ENGINE_CHECK(upgradeStatus_ >= UPDATE_STATE_CHECK_VERSION_SUCCESS,
        progress0.status = UPDATE_STATE_DOWNLOAD_FAIL;
        progress0.endReason = "Invalid status";
        DownloadCallback(progress0);
        return INT_CALL_SUCCESS, "Invalid status %d", upgradeStatus_);

    ENGINE_CHECK(!versionInfo_.result[0].verifyInfo.empty(),
        progress0.status = UPDATE_STATE_DOWNLOAD_FAIL;
        progress0.endReason = "Invalid verify info";
        DownloadCallback(progress0);
        return INT_CALL_SUCCESS, "Invalid verify info");
    std::string downloadFileName = BASE_PATH + "/" + versionInfo_.result[0].verifyInfo;
    size_t localFileLength = DownloadThread::GetLocalFileLength(downloadFileName);
    ENGINE_LOGI("Download %zu %s", localFileLength, downloadFileName.c_str());
    if (localFileLength == versionInfo_.result[0].size && versionInfo_.result[0].size != 0) {
        progress0.percent = DOWNLOAD_FINISH_PERCENT;
        progress0.status = UPDATE_STATE_DOWNLOAD_SUCCESS;
        DownloadCallback(progress0);
        return INT_CALL_SUCCESS;
    }

    upgradeStatus_ = UPDATE_STATE_DOWNLOAD_ON;
    if (downloadThread_ == nullptr) {
        downloadThread_ = new DownloadThread([&](const std::string &fileName, const Progress &progress) -> int {
            DownloadCallback(progress);
            return INT_CALL_SUCCESS;
        });
        ENGINE_CHECK(downloadThread_ != nullptr,
            progress0.status = UPDATE_STATE_DOWNLOAD_FAIL;
            progress0.endReason = "Failed to start thread";
            DownloadCallback(progress0);
            return INT_CALL_SUCCESS, "Failed to start thread");
    }
    int32_t ret = downloadThread_->StartDownload(downloadFileName, GetDownloadServerUrl());
    if (ret != 0) {
        businessError.Build(CallResult::FAIL, "start download fail");
    }
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGE("PauseDownload versionDigestInfo %{public}s isAllowAutoResume %{public}d",
        versionDigestInfo.versionDigest.c_str(),
        pauseDownloadOptions.isAllowAutoResume);
    businessError.Build(CallResult::UN_SUPPORT, "PauseDownload unsupport");
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGE("ResumeDownload versionDigestInfo %{public}s allowNetwork %{public}d",
        versionDigestInfo.versionDigest.c_str(),
        CAST_INT(resumeDownloadOptions.allowNetwork));
    businessError.Build(CallResult::UN_SUPPORT, "ResumeDownload unsupport");
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::DoUpdate(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const UpgradeOptions &upgradeOptions, BusinessError &businessError)
{
    ENGINE_LOGE("DoUpdate versionDigest=%{public}s UpgradeOptions %{public}d",
        versionDigestInfo.versionDigest.c_str(),
        CAST_INT(upgradeOptions.order));
    if (UpdateServiceAbUpdate::IsAbUpdate()) {
        if (upgradeOptions.order == Order::APPLY) {
            ENGINE_LOGI("Try DoUpdate Now");
            if (DoReboot(NULL) != 0) {
                ENGINE_LOGE("AbUpdate DoReboot fail");
                return INT_CALL_FAIL;
            }
            return INT_CALL_SUCCESS;
        }
        int32_t ret = UpdateServiceAbUpdate::DoAbUpdate(info, UPDATER_PKG_NAME);
        SYS_EVENT_SYSTEM_UPGRADE(ret == INT_CALL_SUCCESS, UpdateSystemEvent::EVENT_FAILED_RESULT);
        if (ret != INT_CALL_SUCCESS) {
            ENGINE_LOGE("AbUpdate err=%{public}d", ret);
            businessError.Build(CallResult::FAIL, "result is " + std::to_string(ret));
            return INT_CALL_FAIL;
        }
        return INT_CALL_SUCCESS;
    }
    SYS_EVENT_SYSTEM_UPGRADE(0, UpdateSystemEvent::UPGRADE_START);
    upgradeInterval_.timeStart = GetTimestamp();
    Progress progress;
    progress.percent = 1;
    progress.status = UPDATE_STATE_INSTALL_ON;
    SYS_EVENT_SYSTEM_UPGRADE(upgradeStatus_ < UPDATE_STATE_DOWNLOAD_SUCCESS, UpdateSystemEvent::EVENT_FAILED_RESULT);
    ENGINE_CHECK(upgradeStatus_ >= UPDATE_STATE_DOWNLOAD_SUCCESS,
        progress.endReason = "Invalid status";
        progress.status = UPDATE_STATE_INSTALL_FAIL;
        UpgradeCallback(progress);
        return INT_CALL_SUCCESS, "Invalid status %d", upgradeStatus_);

    progress.status = UPDATE_STATE_INSTALL_SUCCESS;
    std::vector<std::string> updaterPkgName;
    updaterPkgName.push_back(UPDATER_PKG_NAME);
    bool ret = RebootAndInstallUpgradePackage(MISC_FILE, updaterPkgName);
    SYS_EVENT_SYSTEM_UPGRADE(0, ret ? UpdateSystemEvent::EVENT_SUCCESS_RESULT : UpdateSystemEvent::EVENT_FAILED_RESULT);
    if (!ret) {
        ENGINE_LOGI("UpdateService::DoUpdate execute failed");
        businessError.Build(CallResult::FAIL, "result is " + std::to_string(ret));
        return INT_CALL_SUCCESS;
    }
    progress.percent = DOWNLOAD_FINISH_PERCENT;
    UpgradeCallback(progress);
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ClearOptions &clearOptions, BusinessError &businessError)
{
    ENGINE_LOGE("ClearError, versionDigestInfo %{public}s ClearOptions %{public}d",
        versionDigestInfo.versionDigest.c_str(),
        CAST_INT(clearOptions.status));
    businessError.Build(CallResult::UN_SUPPORT, "ClearError unsupport");
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError)
{
    ENGINE_LOGE("TerminateUpgrade");
    businessError.Build(CallResult::UN_SUPPORT, "TerminateUpgrade unsupport");
    return INT_CALL_SUCCESS;
}

void UpdateService::SearchCallbackEx(BusinessError &businessError, CheckResultEx &checkResult)
{
    ENGINE_LOGI("SearchCallbackEx isExistNewVersion %{public}d", checkResult.isExistNewVersion);
    checkResultEx_ = checkResult;
    auto updateCallback = GetUpgradeCallback(upgradeInfo_);
    if (updateCallback == nullptr) {
        ENGINE_LOGE("SearchCallbackEx updateCallback is null");
        return;
    }
    updateCallback->OnCheckVersionDone(businessError, checkResultEx_);
}


void UpdateService::SearchCallback(const std::string &msg, SearchStatus status)
{
    ENGINE_LOGI("SearchCallback status:%{public}d msg:%{public}s ", status, msg.c_str());
    versionInfo_.status = status;
    versionInfo_.errMsg = msg;
    if (status == HAS_NEW_VERSION || status == NO_NEW_VERSION) {
        upgradeStatus_ = UPDATE_STATE_CHECK_VERSION_SUCCESS;

        // Compare the downloaded version with the local version.
        std::string loadVersion = OHOS::system::GetParameter(PARAM_NAME_FOR_VERSION, DEFAULT_VERSION);
        int32_t ret = UpdateHelper::CompareVersion(versionInfo_.result[0].versionCode, loadVersion);
        if (ret <= 0) {
            versionInfo_.status = NO_NEW_VERSION;
        }
    } else {
        upgradeStatus_ = UPDATE_STATE_CHECK_VERSION_FAIL;
    }
    otaStatus_.status = upgradeStatus_;
    auto upgradeCallback = GetUpgradeCallback(upgradeInfo_);
    if (upgradeCallback == nullptr) {
        ENGINE_LOGE("SearchCallback upgradeCallback is null");
        return;
    }
    SYS_EVENT_UPDATE_INTERVAL(0, UpdateHelper::BuildEventVersionInfo(versionInfo_),
        UpdateSystemEvent::EVENT_CHECK_INTERVAL, checkInterval_);
    BusinessError businessError {};
    CheckResultEx checkResultEx {};
    GetCheckResult(checkResultEx);

    businessError.errorNum = CallResult::SUCCESS;
    businessError.data[0].errorCode = CAST_INT(status);
    businessError.data[0].errorMessage = msg;
    ENGINE_LOGI("SearchCallback errorCode:%{public}d msg:%{public}s ",
        CAST_INT(businessError.data[0].errorCode),
        businessError.data[0].errorMessage.c_str());
    upgradeCallback->OnCheckVersionDone(businessError, checkResultEx);
}

void UpdateService::DownloadCallback(const Progress &progress)
{
    Progress downloadProgress {};
    upgradeStatus_ = UPDATE_STATE_DOWNLOAD_ON;
    if (progress.status == UPDATE_STATE_DOWNLOAD_FAIL ||
        progress.status == UPDATE_STATE_DOWNLOAD_SUCCESS) {
        upgradeStatus_ = progress.status;
    }
    downloadProgress = progress;

    otaStatus_.progress = progress.percent;
    otaStatus_.status = progress.status;
    otaStatus_.errMsg[0].errorMsg = progress.endReason;

#ifdef UPDATER_UT
    upgradeStatus_ = UPDATE_STATE_DOWNLOAD_SUCCESS;
#endif
    std::string fileName = BASE_PATH + "/" + versionInfo_.result[0].verifyInfo;
    ENGINE_LOGI("DownloadCallback status %{public}d %{public}d", progress.status, progress.percent);
    if (upgradeStatus_ == UPDATE_STATE_DOWNLOAD_SUCCESS) {
        ENGINE_LOGI("DownloadCallback fileName %{public}s %{public}s", fileName.c_str(), UPDATER_PKG_NAME.c_str());
        if (rename(fileName.c_str(), UPDATER_PKG_NAME.c_str())) {
            ENGINE_LOGE("Rename file fail %s", fileName.c_str());
            remove(UPDATER_PKG_NAME.c_str());
            downloadProgress.status = UPDATE_STATE_DOWNLOAD_FAIL;
        } else if (!VerifyDownloadPkg(UPDATER_PKG_NAME, downloadProgress)) {
            // If the verification fails, delete the corresponding package.
            remove(UPDATER_PKG_NAME.c_str());
            downloadProgress.status = UPDATE_STATE_VERIFY_FAIL;
        }
    }

    if (downloadProgress.percent == MAX_PERCENT) {
        downloadInterval_ = GetTimestamp();
        SYS_EVENT_UPDATE_INTERVAL(0, UpdateHelper::BuildEventVersionInfo(versionInfo_),
            UpdateSystemEvent::EVENT_DOWNLOAD_INTERVAL, downloadInterval_);
    }
    switch (downloadProgress.status) {
        case UPDATE_STATE_DOWNLOAD_ON:
            SendEvent(upgradeInfo_, EventId::EVENT_DOWNLOAD_UPDATE);
            break;
        case UPDATE_STATE_DOWNLOAD_PAUSE:
            SendEvent(upgradeInfo_, EventId::EVENT_DOWNLOAD_PAUSE);
            break;
        case UPDATE_STATE_DOWNLOAD_FAIL:
        case UPDATE_STATE_VERIFY_FAIL:
            SendEvent(upgradeInfo_, EventId::EVENT_DOWNLOAD_FAIL);
            break;
        case UPDATE_STATE_DOWNLOAD_SUCCESS:
            SendEvent(upgradeInfo_, EventId::EVENT_DOWNLOAD_SUCCESS);
            break;
        default:
            ENGINE_LOGE("DownloadCallback downloadProgress error status:%d", downloadProgress.status);
            break;
    }
}

void UpdateService::UpgradeCallback(const Progress &progress)
{
    upgradeStatus_ = progress.status;
    otaStatus_.status = progress.status;
    ENGINE_LOGE("UpgradeCallback status %{public}d  %{public}d", progress.status, progress.percent);

    auto upgradeCallback = GetUpgradeCallback(upgradeInfo_);
    if (upgradeCallback == nullptr) {
        ENGINE_LOGE("UpgradeCallback upgradeCallback is null");
        return;
    }

    if (progress.percent == MAX_PERCENT) {
        upgradeInterval_.timeEnd = GetTimestamp();
        SYS_EVENT_UPDATE_INTERVAL(0, UpdateHelper::BuildEventVersionInfo(versionInfo_),
            UpdateSystemEvent::EVENT_UPGRADE_INTERVAL,
            upgradeInterval_.timeEnd > upgradeInterval_.timeStart
            ? (upgradeInterval_.timeEnd - upgradeInterval_.timeStart) : 0);
    }
    SendEvent(upgradeInfo_, EventId::EVENT_UPGRADE_UPDATE);
}

void UpdateService::ReadDataFromSSL(int32_t engineSocket)
{
    SearchStatus result = SERVER_BUSY;
    std::string errMsg = "Couldn't connect to server";
    std::vector<char> buffer(JSON_MAX_SIZE);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX* sslCtx = SSL_CTX_new(SSLv23_client_method());
    ENGINE_CHECK(sslCtx != nullptr, return, "sslCtx is nullptr");
    SSL *ssl = SSL_new(sslCtx);
    ENGINE_CHECK(ssl != nullptr, SSL_CTX_free(sslCtx); return, "ssl is nullptr");
    SSL_set_fd(ssl, engineSocket);
    int32_t ret = SSL_connect(ssl);
    if (ret != -1) {
        int32_t len = SSL_read(ssl, buffer.data(), JSON_MAX_SIZE);
        if (len > 0 && ParseJsonFile(buffer, versionInfo_, downloadUrl_) == 0) {
            result = HAS_NEW_VERSION;
            errMsg = "";
        } else {
            result = SYSTEM_ERROR;
            errMsg = "Couldn't read data";
        }
    } else {
        result = SYSTEM_ERROR;
        errMsg = "Couldn't connect to server";
    }
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(engineSocket);
    SSL_CTX_free(sslCtx);

    SearchCallback(errMsg, result);
    return;
}

int32_t UpdateService::ParseJsonFile(const std::vector<char> &buffer, VersionInfo &info, std::string &url)
{
    ENGINE_CHECK(buffer.size() > 0, return -1, "JsonFile length must > 0");
    cJSON *root = cJSON_Parse(buffer.data());
    ENGINE_CHECK(root != nullptr, return -1, "Error get root");

    cJSON *item = cJSON_GetObjectItem(root, "searchStatus");
    ENGINE_CHECK(item != nullptr, cJSON_Delete(root); return -1, "Error get searchStatus");
    info.status = static_cast<SearchStatus>(item->valueint);

    item = cJSON_GetObjectItem(root, "errMsg");
    ENGINE_CHECK(item != nullptr, cJSON_Delete(root); return -1, "Error get errMsg");
    info.errMsg = item->valuestring;

    cJSON *results = cJSON_GetObjectItem(root, "checkResults");
    ENGINE_CHECK(results != nullptr, cJSON_Delete(root); return -1, "Error get checkResults");
    int32_t ret = ReadCheckVersionResult(results, info, url);
    ENGINE_CHECK(ret == 0, cJSON_Delete(root); return -1, "Error get checkResults");

    cJSON *descriptInfo = cJSON_GetObjectItem(root, "descriptInfo");
    ENGINE_CHECK(descriptInfo != nullptr, cJSON_Delete(root); return -1, "Error get descriptInfo");
    ret = ReadCheckVersiondescriptInfo(descriptInfo, info);
    ENGINE_CHECK(ret == 0, cJSON_Delete(root); return -1, "Error get descriptInfo");

    cJSON_Delete(root);
    if (info.status == HAS_NEW_VERSION) {
        ENGINE_CHECK(!info.result[0].verifyInfo.empty() &&
            !info.result[0].versionName.empty() &&
            info.result[0].size > 0, return -1, "Error get descriptInfo");
    }
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::ReadCheckVersionResult(const cJSON* results, VersionInfo &info, std::string &url)
{
    size_t number = (size_t)cJSON_GetArraySize(results);
    for (size_t i = 0; i < number && i < sizeof(info.result) / sizeof(info.result[0]); i++) {
        cJSON *result = cJSON_GetArrayItem(results, i);
        ENGINE_CHECK(result != nullptr, return -1, "Error get result");

        cJSON *item = cJSON_GetObjectItem(result, "versionName");
        ENGINE_CHECK(item != nullptr, return -1, "Error get versionName");
        info.result[i].versionName = item->valuestring;

        item = cJSON_GetObjectItem(result, "versionCode");
        ENGINE_CHECK(item != nullptr, return -1, "Error get versionCode");
        info.result[i].versionCode = item->valuestring;

        item = cJSON_GetObjectItem(result, "verifyInfo");
        ENGINE_CHECK(item != nullptr, return -1, "Error get verifyInfo");
        info.result[i].verifyInfo = item->valuestring;

        item = cJSON_GetObjectItem(result, "size");
        ENGINE_CHECK(item != nullptr,  return -1, "Error get size");
        info.result[i].size = (size_t)item->valueint;

        item = cJSON_GetObjectItem(result, "packageType");
        ENGINE_CHECK(item != nullptr, return -1, "Error get packageType");
        info.result[i].packageType = (PackageType)(item->valueint);

        item = cJSON_GetObjectItem(result, "url");
        ENGINE_CHECK(item != nullptr, return -1, "Error get url");
        url = item->valuestring;

        item = cJSON_GetObjectItem(result, "descriptPackageId");
        ENGINE_CHECK(item != nullptr, return -1, "Error get descriptPackageId");
        info.result[i].descriptPackageId = item->valuestring;
    }
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::ReadCheckVersiondescriptInfo(const cJSON *descriptInfo, VersionInfo &info)
{
    size_t number = (size_t)cJSON_GetArraySize(descriptInfo);
    for (size_t i = 0; i < number && i < sizeof(info.result) / sizeof(info.result[0]); i++) {
        cJSON *descript = cJSON_GetArrayItem(descriptInfo, i);
        ENGINE_CHECK(descript != nullptr, return -1, "Error get descriptInfo");

        cJSON *item = cJSON_GetObjectItem(descript, "descriptionType");
        if (item != nullptr) {
            info.descriptInfo[i].descriptionType = static_cast<DescriptionType>(item->valueint);
        }
        item = cJSON_GetObjectItem(descript, "content");
        if (item != nullptr) {
            info.descriptInfo[i].content = item->valuestring;
            ENGINE_LOGI(" descriptInfo content %s", info.descriptInfo[i].content.c_str());
        }
    }
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
    BusinessError &businessError)
{
    businessError.errorNum = CallResult::SUCCESS;
    int32_t ret = ::VerifyPackageWithCallback(packagePath.c_str(), keyPath.c_str(),
        [](int32_t result, uint32_t percent) {});
    ENGINE_LOGI("VerifyUpgradePackage %s, %s, %{public}d", packagePath.c_str(), keyPath.c_str(), ret);
    ret = (ret == 0) ? INT_CALL_SUCCESS : INT_CALL_FAIL;
    return ret;
}

bool UpdateService::VerifyDownloadPkg(const std::string &pkgName, Progress &progress)
{
    // Compare the downloaded version with the local version. Only update is supported.
    std::string loadVersion = OHOS::system::GetParameter(PARAM_NAME_FOR_VERSION, DEFAULT_VERSION);
    int32_t ret = UpdateHelper::CompareVersion(versionInfo_.result[0].versionCode, loadVersion);
    if (ret <= 0) {
        progress.endReason = "Update package version earlier than the local version";
        ENGINE_LOGE("Version compare Failed local '%{public}s' server '%{public}s'",
            loadVersion.c_str(), versionInfo_.result[0].versionCode.c_str());
        return false;
    }
    ENGINE_LOGI("versionInfo_.result[0].verifyInfo %s ", versionInfo_.result[0].verifyInfo.c_str());
    std::vector<uint8_t> digest = UpdateHelper::HexToDegist(versionInfo_.result[0].verifyInfo);
    ret = ::VerifyPackage(pkgName.c_str(),
        SIGNING_CERT_NAME.c_str(), versionInfo_.result[0].versionCode.c_str(), digest.data(), digest.size());
    if (ret != 0) {
        progress.endReason = "Upgrade package verify Failed";
        SYS_EVENT_VERIFY_FAILED(0, UpdateHelper::BuildEventDevId(upgradeInfo_),
            UpdateSystemEvent::EVENT_PKG_VERIFY_FAILED);
        ENGINE_LOGE("Package %{public}s verification Failed, ret = %d", pkgName.c_str(), ret);
        return false;
    }
    ENGINE_LOGE("Package verify success");
    return true;
}

std::string UpdateService::GetDownloadServerUrl() const
{
    return downloadUrl_;
}

int32_t UpdateService::Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError)
{
    ENGINE_LOGI("Cancel %{public}d", service);
    businessError.errorNum = CallResult::SUCCESS;
    if (downloadThread_ != nullptr && service == DOWNLOAD) {
        downloadThread_->StopDownload();
    }
    return INT_CALL_SUCCESS;
}

int32_t UpdateService::FactoryReset(BusinessError &businessError)
{
#ifndef UPDATER_UT
    businessError.errorNum = CallResult::SUCCESS;
    int32_t ret = RebootAndCleanUserData(MISC_FILE, CMD_WIPE_DATA) ? INT_CALL_SUCCESS : INT_CALL_FAIL;
    ENGINE_LOGI("FactoryReset result : %{public}d", ret);
    SYS_EVENT_SYSTEM_RESET(
        0, ret == INT_CALL_SUCCESS ? UpdateSystemEvent::EVENT_SUCCESS_RESULT : UpdateSystemEvent::EVENT_FAILED_RESULT);
    return ret;
#else
    return INT_CALL_SUCCESS;
#endif
}

int32_t UpdateService::ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile,
    const std::string &packageName, BusinessError &businessError)
{
#ifndef UPDATER_UT
    SYS_EVENT_SYSTEM_UPGRADE(0, UpdateSystemEvent::UPGRADE_START);
    businessError.errorNum = CallResult::SUCCESS;
    SendEvent(info, EventId::EVENT_UPGRADE_UPDATE);
    std::vector<std::string> packagePath;
    packagePath.push_back(packageName);
    int32_t ret = RebootAndInstallUpgradePackage(miscFile, packagePath) ? INT_CALL_SUCCESS : INT_CALL_FAIL;
    ENGINE_LOGI("ApplyNewVersion result : %{public}d", ret);
    SendEvent(info, ret == INT_CALL_SUCCESS ? EventId::EVENT_UPGRADE_SUCCESS : EventId::EVENT_UPGRADE_FAIL);
    SYS_EVENT_SYSTEM_UPGRADE(
        0, ret == INT_CALL_SUCCESS ? UpdateSystemEvent::EVENT_SUCCESS_RESULT : UpdateSystemEvent::EVENT_FAILED_RESULT);
    return ret;
#else
    return INT_CALL_SUCCESS;
#endif
}

void UpdateService::InitVersionInfo(VersionInfo &versionInfo) const
{
    versionInfo.status = HAS_NEW_VERSION;
    std::string versionName = OHOS::system::GetParameter(PARAM_NAME_FOR_VERSION, DEFAULT_VERSION);
    if (versionName.empty()) {
        versionInfo.status = SYSTEM_ERROR;
        versionInfo.errMsg = "Can not get local version";
    }

    size_t i;
    for (i = 0; i < sizeof(versionInfo.result) / sizeof(versionInfo.result[0]); i++) {
        versionInfo.result[i].size = 0;
        versionInfo.result[i].packageType = PACKAGE_TYPE_NORMAL;
        versionInfo.result[i].versionName = versionName;
        versionInfo.result[i].versionCode = "";
        versionInfo.result[i].verifyInfo = "";
        versionInfo.result[i].descriptPackageId = "";
    }
    for (i = 0; i < sizeof(versionInfo.descriptInfo) / sizeof(versionInfo.descriptInfo[0]); i++) {
        versionInfo.descriptInfo[i].content = "";
        versionInfo.descriptInfo[i].descriptionType = DescriptionType::CONTENT;
    }
}

void UpdateService::SetCheckInterval(uint64_t interval)
{
    checkInterval_ = interval;
}

void UpdateService::SetDownloadInterval(uint64_t interval)
{
    downloadInterval_ = interval;
}

uint64_t UpdateService::GetCheckInterval()
{
    return checkInterval_;
}

uint64_t UpdateService::GetDownloadInterval()
{
    return downloadInterval_;
}

void UpdateService::GetUpgradeContext(std::string &devIdInfo)
{
    devIdInfo = UpdateHelper::BuildEventDevId(upgradeInfo_);
}

void BuildUpgradeInfoDump(const int fd, UpgradeInfo &info)
{
    dprintf(fd, "---------------------upgrade info--------------------\n");
    dprintf(fd, "UpgradeApp: %s\n", info.upgradeApp.c_str());
    dprintf(fd, "vendor: %s\n", info.businessType.vendor.c_str());
    dprintf(fd, "subType: %d\n", static_cast<int>(info.businessType.subType));
}

void BuildVersionInfoDump(const int fd, VersionInfo &ver)
{
    dprintf(fd, "---------------------version info--------------------\n");
    dprintf(fd, "SearchStatus: %d\n", ver.status);
    dprintf(fd, "ErrorMsg: %s\n", ver.errMsg.c_str());
    dprintf(fd, "PackageSize: %zu\n", ver.result[0].size);
    dprintf(fd, "PackageType: %d\n", ver.result[0].packageType);
    dprintf(fd, "VersionName: %s\n", ver.result[0].versionName.c_str());
    dprintf(fd, "VersionCode: %s\n", ver.result[0].versionCode.c_str());
    dprintf(fd, "DescriptPackageId: %s\n", ver.result[0].descriptPackageId.c_str());
    dprintf(fd, "Content: %s\n", ver.descriptInfo[0].content.c_str());
}

void BuildTaskInfoDump(const int fd)
{
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service == nullptr) {
        ENGINE_LOGI("BuildTaskInfoDump no instance");
        return;
    }

    TaskInfo taskInfo;
    BusinessError businessError;
    UpgradeInfo upgradeInfo;
    service->GetTaskInfo(upgradeInfo, taskInfo, businessError);
    if (!taskInfo.existTask) {
        dprintf(fd, "TaskInfo is empty\n");
        return;
    }

    dprintf(fd, "---------------------OTA status info--------------------\n");
    dprintf(fd, "Progress: %d\n", taskInfo.taskBody.progress);
    dprintf(fd, "UpgradeStatus: %d\n", taskInfo.taskBody.status);
    dprintf(fd, "SubStatus: %d\n", taskInfo.taskBody.subStatus);
    dprintf(fd, "ErrorCode: %d\n", taskInfo.taskBody.errorMessages[0].errorCode);
    dprintf(fd, "ErrorMsg: %s\n", taskInfo.taskBody.errorMessages[0].errorMessage.c_str());
}

void UpdateService::DumpUpgradeCallback(const int fd)
{
    dprintf(fd, "---------------------callback info--------------------\n");
    for (const auto &iter : clientProxyMap_) {
        const UpgradeInfo& info = iter.first;
        dprintf(fd, "%s\n", info.ToString().c_str());
    }
}

int UpdateService::Dump(int fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        ENGINE_LOGI("HiDumper handle invalid");
        return -1;
    }

    if (args.size() == 0) {
        BuildUpgradeInfoDump(fd, upgradeInfo_);
        BuildVersionInfoDump(fd, versionInfo_);
        BuildTaskInfoDump(fd);
        DumpUpgradeCallback(fd);
    } else {
        dprintf(fd, "input error, no parameters required\n");
    }
    return INT_CALL_SUCCESS;
}

void UpdateService::OnStart()
{
    ENGINE_LOGI("UpdaterService OnStart");
    bool res = Publish(this);
    if (!res) {
        ENGINE_LOGI("UpdaterService OnStart failed");
        return;
    }
    updateService_ = this;
    if (updateService_ == nullptr) {
        ENGINE_LOGE("updateService_ null");
    }
    return;
}

void UpdateService::OnStop()
{
    ENGINE_LOGI("UpdaterService OnStop");
}

void UpdateService::SendEvent(const UpgradeInfo &upgradeInfo, EventId eventId)
{
    auto upgradeCallback = GetUpgradeCallback(upgradeInfo);
    ENGINE_CHECK(upgradeCallback != nullptr, return, "SendEvent Error, upgradeCallback is nullptr");

    ENGINE_LOGI("SendEvent %{public}d", CAST_INT(eventId));
    TaskInfo taskInfo;
    BusinessError businessError;
    GetTaskInfo(upgradeInfo, taskInfo, businessError);

    EventInfo eventInfo(eventId, taskInfo.taskBody);
    upgradeCallback->OnEvent(eventInfo);
}
} // namespace UpdateEngine
} // namespace OHOS
