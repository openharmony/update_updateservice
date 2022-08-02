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
#define RETURN_WHEN_TOKEN_WRITE_FAIL(data)                             \
    if (!(data).WriteInterfaceToken(GetDescriptor())) {                \
        ENGINE_LOGE("UpdateServiceProxy WriteInterfaceToken fail");    \
        return INT_CALL_IPC_ERR;                                       \
    }

#define RETURN_WHEN_REMOTE_NULL(remote) \
    ENGINE_CHECK((remote) != nullptr, return INT_CALL_IPC_ERR, "Can not get remote")

#define IPC_RESULT_TO_CALL_RESULT(result)           \
    if ((result) == ERR_NONE) {                     \
        result = INT_CALL_SUCCESS;                  \
    } else if ((result) >= CALL_RESULT_OFFSET) {    \
        result = (result) - CALL_RESULT_OFFSET;     \
    } else {                                        \
        result = INT_CALL_IPC_ERR;                  \
    }

#define RETURN_FAIL_WHEN_REMOTE_ERR(methodName, res)                             \
    do {                                                                         \
        ENGINE_LOGI("%{public}s is %{public}d", methodName, res);                \
        IPC_RESULT_TO_CALL_RESULT(res);                                          \
        ENGINE_CHECK((res) == INT_CALL_SUCCESS, return (res), "Transact error"); \
    } while (0)

int32_t UpdateServiceProxy::RegisterUpdateCallback(const UpgradeInfo &info,
    const sptr<IUpdateCallback>& updateCallback)
{
    ENGINE_CHECK(updateCallback != nullptr, return INT_PARAM_ERR, "Invalid param");
    ENGINE_LOGI("UpdateServiceProxy::RegisterUpdateCallback");

    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    bool res = data.WriteRemoteObject(updateCallback->AsObject());
    ENGINE_CHECK(res, return INT_CALL_IPC_ERR, "RegisterUpdateCallback error, WriteRemoteObject fail");

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(REGISTER_CALLBACK, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::RegisterUpdateCallback", ret);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::UnregisterUpdateCallback(const UpgradeInfo &info)
{
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(UNREGISTER_CALLBACK, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::UnregisterUpdateCallback", ret);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::CheckNewVersion(const UpgradeInfo &info)
{
    ENGINE_LOGI("UpdateServiceProxy::CheckNewVersion");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CHECK_VERSION, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::CheckNewVersion", ret);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::DownloadVersion(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const DownloadOptions &downloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::DownloadVersion");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::WriteDownloadOptions(data, downloadOptions);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(DOWNLOAD, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::DownloadVersion", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::PauseDownload");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::WritePauseDownloadOptions(data, pauseDownloadOptions);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(PAUSE_DOWNLOAD, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::ResumeDownload", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
    const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::ResumeDownload");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::WriteResumeDownloadOptions(data, resumeDownloadOptions);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(RESUME_DOWNLOAD, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::ResumeDownload", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::DoUpdate(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const UpgradeOptions &upgradeOptions, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::DoUpdate, versionDigest %{public}s upgradeOptions %{public}d",
        versionDigest.versionDigest.c_str(),
        upgradeOptions.order);
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigest);
    UpdateHelper::WriteUpgradeOptions(data, upgradeOptions);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(UPGRADE, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::DoUpdate", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
    const ClearOptions &clearOptions, BusinessError &businessError)
{
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigest);
    UpdateHelper::WriteClearOptions(data, clearOptions);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CLEAR_ERROR, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::ClearError", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError)
{
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(TERMINATE_UPGRADE, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::TerminateUpgrade", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::GetNewVersion(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetNewVersion");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_NEW_VERSION, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::GetNewVersion", ret);

    UpdateHelper::ReadBusinessError(reply, businessError);
    UpdateHelper::ReadNewVersionInfo(reply, newVersionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::GetNewVersionDescription(const UpgradeInfo &info,
    const VersionDigestInfo &versionDigestInfo, const DescriptionOptions &descriptionOptions,
    VersionDescriptionInfo &newVersionDescriptionInfo, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetNewVersionDescription");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::WriteDescriptionOptions(data, descriptionOptions);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_NEW_VERSION_DESCRIPTION, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::GetNewVersionDescription", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    UpdateHelper::ReadVersionDescriptionInfo(reply, newVersionDescriptionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetCurrentVersionInfo");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_CURRENT_VERSION, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::GetCurrentVersionInfo", ret);

    UpdateHelper::ReadBusinessError(reply, businessError);
    UpdateHelper::ReadCurrentVersionInfo(reply, currentVersionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::GetCurrentVersionDescription(const UpgradeInfo &info,
    const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &currentVersionDescriptionInfo,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetCurrentVersionDescription");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteDescriptionOptions(data, descriptionOptions);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_CURRENT_VERSION_DESCRIPTION, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::GetCurrentVersionDescription", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    UpdateHelper::ReadVersionDescriptionInfo(reply, currentVersionDescriptionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetTaskInfo");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_TASK_INFO, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::GetTaskInfo", ret);

    UpdateHelper::ReadBusinessError(reply, businessError);
    UpdateHelper::ReadTaskInfo(reply, taskInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::SetUpgradePolicy(const UpgradeInfo &info, const UpgradePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::SetUpgradePolicy");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    UpdateHelper::WriteUpgradePolicy(data, policy);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(SET_POLICY, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::SetUpgradePolicy", ret);

    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::GetUpgradePolicy(const UpgradeInfo &info, UpgradePolicy &policy,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::GetUpgradePolicy");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(GET_POLICY, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::GetUpgradePolicy", ret);

    UpdateHelper::ReadBusinessError(reply, businessError);
    UpdateHelper::ReadUpgradePolicy(reply, policy);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::Cancel");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    data.WriteInt32(static_cast<int32_t>(service));

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CANCEL, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::Cancel", ret);

    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::FactoryReset(BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::FactoryReset");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = ERR_NONE; // IPC errCode: defined in ipc_types.h
#ifndef UPDATER_UT
    ret = remote->SendRequest(FACTORY_RESET, data, reply, option);
#endif
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::FactoryReset", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile,
    const std::string &packageName, BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::ApplyNewVersion");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    UpdateHelper::WriteUpgradeInfo(data, info);
    data.WriteString16(Str8ToStr16(miscFile));
    data.WriteString16(Str8ToStr16(packageName));

    MessageParcel reply;
    MessageOption option;
    int32_t ret = ERR_NONE;
#ifndef UPDATER_UT
    ret = remote->SendRequest(APPLY_NEW_VERSION, data, reply, option);
#endif
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::ApplyNewVersion", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceProxy::VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
    BusinessError &businessError)
{
    ENGINE_LOGI("UpdateServiceProxy::VerifyUpgradePackage");
    auto remote = Remote();
    RETURN_WHEN_REMOTE_NULL(remote);

    MessageParcel data;
    RETURN_WHEN_TOKEN_WRITE_FAIL(data);
    data.WriteString16(Str8ToStr16(packagePath));
    data.WriteString16(Str8ToStr16(keyPath));

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(VERIFY_UPGRADE_PACKAGE, data, reply, option);
    RETURN_FAIL_WHEN_REMOTE_ERR("UpdateServiceProxy::VerifyUpgradePackage", ret);
    UpdateHelper::ReadBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}
} // namespace UpdateEngine
} // namespace OHOS
