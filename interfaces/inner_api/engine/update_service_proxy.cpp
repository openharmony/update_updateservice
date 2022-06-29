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

#include "update_service_proxy.h"
#include "update_helper.h"
#include "securec.h"

namespace OHOS {
namespace UpdateEngine {
int32_t UpdateServiceProxy::RegisterUpdateCallback(const UpgradeInfo &info,
    const sptr<IUpdateCallback>& updateCallback)
{
    ENGINE_CHECK(updateCallback != nullptr, return ERR_INVALID_VALUE, "Invalid param");
    ENGINE_LOGI("UpdateServiceProxy::RegisterUpdateCallback");

    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    bool ret = data.WriteRemoteObject(updateCallback->AsObject());
    ENGINE_CHECK(ret, return ERR_FLATTEN_OBJECT, "Can not get remote");
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    int32_t res = remote->SendRequest(REGISTER_CALLBACK, data, reply, option);
    ENGINE_CHECK(res == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return reply.ReadInt32();
}

int32_t UpdateServiceProxy::UnregisterUpdateCallback(const UpgradeInfo &info)
{
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    int32_t res = remote->SendRequest(UNREGISTER_CALLBACK, data, reply, option);
    ENGINE_CHECK(res == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return reply.ReadInt32();
}

int32_t UpdateServiceProxy::CheckNewVersion(const UpgradeInfo &info)
{
    ENGINE_LOGI("UpdateServiceProxy::CheckNewVersion");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    int32_t res = remote->SendRequest(CHECK_VERSION, data, reply, option);
    ENGINE_CHECK(res == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return reply.ReadInt32();
}

int32_t UpdateServiceProxy::DownloadVersion(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DownloadOptions &downloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::DownloadVersion");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    // Construct a data sending message to the stub.
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::WriteDownloadOptions(data, downloadOptions);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(DOWNLOAD, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::PauseDownload");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    // Construct a data sending message to the stub.
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::WritePauseDownloadOptions(data, pauseDownloadOptions);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(PAUSE_DOWNLOAD, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::ResumeDownload");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    // Construct a data sending message to the stub.
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::WriteResumeDownloadOptions(data, resumeDownloadOptions);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(RESUME_DOWNLOAD, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::DoUpdate(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const UpgradeOptions &upgradeOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::DoUpdate, versionDigest %{public}s upgradeOptions %{public}d",
        versionDigest.versionDigest.c_str(),
        upgradeOptions.order);
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    // Construct a data sending message to the stub.
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigest);
    UpdateHelper::WriteUpgradeOptions(data, upgradeOptions);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(UPGRADE, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const ClearOptions &clearOptions, BusinessError &businessError)
{
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    // Construct a data sending message to the stub.
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigest);
    UpdateHelper::WriteClearOptions(data, clearOptions);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CLEAR_ERROR, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError)
{
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    // Construct a data sending message to the stub.
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(TERMINATE_UPGRADE, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");

    ENGINE_LOGI("UpdateServiceProxy TerminateUpgrade finish %{public}d", ret);
    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::GetNewVersion(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetNewVersion");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_NEW_VERSION, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");

    UpdateHelper::ReadBusinessError(reply, businessError);
    return UpdateHelper::ReadNewVersionInfo(reply, newVersionInfo);
}

int32_t UpdateServiceProxy::GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetCurrentVersionInfo");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_CURRENT_VERSION, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");

    UpdateHelper::ReadBusinessError(reply, businessError);
    return UpdateHelper::ReadCurrentVersionInfo(reply, currentVersionInfo);
}

int32_t UpdateServiceProxy::GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetTaskInfo");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_TASK_INFO, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");

    UpdateHelper::ReadBusinessError(reply, businessError);
    return UpdateHelper::ReadTaskInfo(reply, taskInfo);
}

int32_t UpdateServiceProxy::GetOtaStatus(const UpgradeInfo &info, OtaStatus &otaStatus, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetOtaStatus");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_OTA_STATUS, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");

    UpdateHelper::ReadBusinessError(reply, businessError);
    return UpdateHelper::ReadOtaStatus(reply, otaStatus);
}

int32_t UpdateServiceProxy::SetUpdatePolicy(const UpgradeInfo &info, const UpdatePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::SetUpdatePolicy");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteUpdatePolicy(data, policy);
    MessageParcel reply;
    MessageOption option;
    int32_t res = remote->SendRequest(SET_POLICY, data, reply, option);
    ENGINE_CHECK(res == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");

    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::GetUpdatePolicy(const UpgradeInfo &info, UpdatePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetUpdatePolicy");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_POLICY, data, reply, option);
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");

    UpdateHelper::ReadBusinessError(reply, businessError);
    return UpdateHelper::ReadUpdatePolicy(reply, policy);
}

int32_t UpdateServiceProxy::Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::Cancel");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }

    UpdateHelper::WriteUpgradeInfo(data, info);
    data.WriteInt32(static_cast<int32_t>(service));
    MessageParcel reply;
    MessageOption option;
    int32_t res = remote->SendRequest(CANCEL, data, reply, option);
    ENGINE_CHECK(res == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error res %d", res);

    return UpdateHelper::ReadBusinessError(reply, businessError);
}

int32_t UpdateServiceProxy::RebootAndClean(const std::string &miscFile, const std::string &cmd)
{
    ENGINE_LOGI("UpdateServiceProxy::RebootAndCleanUserData");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }
    data.WriteString16(Str8ToStr16(miscFile));
    data.WriteString16(Str8ToStr16(cmd));
    MessageParcel reply;
    MessageOption option;
    int32_t ret = ERR_OK;
#ifndef UPDATER_UT
    ret = remote->SendRequest(REBOOT_CLEAN, data, reply, option);
#endif
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return reply.ReadInt32();
}

int32_t UpdateServiceProxy::RebootAndInstall(const std::string &miscFile, const std::string &packageName)
{
    ENGINE_LOGI("UpdateServiceProxy::RebootAndCleanUserData");
    auto remote = Remote();
    ENGINE_CHECK(remote != nullptr, return ERR_FLATTEN_OBJECT, "Can not get remote");

    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ENGINE_LOGI("UpdateServiceProxy WriteInterfaceToken fail");
        return -1;
    }
    data.WriteString16(Str8ToStr16(miscFile));
    data.WriteString16(Str8ToStr16(packageName));
    MessageParcel reply;
    MessageOption option;
    int32_t ret = ERR_OK;
#ifndef UPDATER_UT
    ret = remote->SendRequest(REBOOT_INSTALL, data, reply, option);
#endif
    ENGINE_CHECK(ret == ERR_OK, return ERR_FLATTEN_OBJECT, "Transact error");
    return reply.ReadInt32();
}
} // namespace UpdateEngine
} // namespace OHOS
