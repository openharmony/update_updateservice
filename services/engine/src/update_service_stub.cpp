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

#include "accesstoken_kit.h"
#include "hap_token_info.h"
#include "ipc_skeleton.h"
#include "securec.h"
#include "update_helper.h"
#include "update_service_stub.h"
#include "update_system_event.h"

using namespace std;

namespace OHOS {
namespace UpdateEngine {
#define CALL_RESULT_TO_IPC_RESULT(callResult) ((callResult) + CALL_RESULT_OFFSET)

#define RETURN_FAIL_WHEN_SERVICE_NULL(service) \
    ENGINE_CHECK((service) != nullptr, return INT_CALL_IPC_ERR, "service null")

static int32_t GetNewVersionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    NewVersionInfo newVersionInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetNewVersion(upgradeInfo, newVersionInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetNewVersion");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteNewVersionInfo(reply, newVersionInfo);
    return INT_CALL_SUCCESS;
}

static int32_t GetNewVersionDescriptionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    DescriptionOptions descriptionOptions;
    VersionDescriptionInfo newVersionDescriptionInfo;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadDescriptionOptions(data, descriptionOptions);

    int32_t ret = service->GetNewVersionDescription(upgradeInfo, versionDigestInfo, descriptionOptions,
        newVersionDescriptionInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetNewVersionDescription");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteVersionDescriptionInfo(reply, newVersionDescriptionInfo);
    return INT_CALL_SUCCESS;
}

static int32_t GetCurrentVersionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    CurrentVersionInfo currentVersionInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetCurrentVersionInfo(upgradeInfo, currentVersionInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetCurrentVersion");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteCurrentVersionInfo(reply, currentVersionInfo);
    return INT_CALL_SUCCESS;
}

static int32_t GetCurrentVersionDescriptionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    DescriptionOptions descriptionOptions;
    VersionDescriptionInfo currentVersionDescriptionInfo;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadDescriptionOptions(data, descriptionOptions);

    int32_t ret = service->GetCurrentVersionDescription(upgradeInfo, descriptionOptions, currentVersionDescriptionInfo,
        businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetCurrentVersionDescription");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteVersionDescriptionInfo(reply, currentVersionDescriptionInfo);
    return INT_CALL_SUCCESS;
}

static int32_t GetTaskInfoStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    TaskInfo taskInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetTaskInfo(upgradeInfo, taskInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetTaskInfo");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteTaskInfo(reply, taskInfo);
    return INT_CALL_SUCCESS;
}

static int32_t CheckNewVersionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    return service->CheckNewVersion(upgradeInfo);
}

static int32_t DownloadVersionStub(UpdateServiceStub::UpdateServiceStubPtr service, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    DownloadOptions downloadOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadDownloadOptions(data, downloadOptions);
    int32_t ret = service->DownloadVersion(upgradeInfo, versionDigestInfo, downloadOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to DownloadVersion");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t PauseDownloadStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    PauseDownloadOptions pauseDownloadOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadPauseDownloadOptions(data, pauseDownloadOptions);

    int32_t ret = service->PauseDownload(upgradeInfo, versionDigestInfo, pauseDownloadOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to PauseDownload");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t ResumeDownloadStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    ResumeDownloadOptions resumeDownloadOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadResumeDownloadOptions(data, resumeDownloadOptions);

    int32_t ret = service->ResumeDownload(upgradeInfo, versionDigestInfo, resumeDownloadOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to ResumeDownload");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t DoUpdateStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    UpgradeOptions upgradeOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadUpgradeOptions(data, upgradeOptions);

    int32_t ret = service->DoUpdate(upgradeInfo, versionDigestInfo, upgradeOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to DoUpdate");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t ClearErrorStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    ClearOptions clearOptions;
    BusinessError businessError;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpdateHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    UpdateHelper::ReadClearOptions(data, clearOptions);

    int32_t ret = service->ClearError(upgradeInfo, versionDigestInfo, clearOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to ClearError");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t TerminateUpgradeStub(UpdateServiceStub::UpdateServiceStubPtr service, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);

    BusinessError businessError;
    int32_t ret = service->TerminateUpgrade(upgradeInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to TerminateUpgrade");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t SetUpgradePolicyStub(
    UpdateServiceStub::UpdateServiceStubPtr service, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpgradePolicy policy = {};
    UpdateHelper::ReadUpgradePolicy(data, policy);
    BusinessError businessError = {};
    int32_t ret = service->SetUpgradePolicy(upgradeInfo, policy, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to SetUpgradePolicy");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t GetUpgradePolicyStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpgradePolicy policy = {};
    BusinessError businessError = {};
    int32_t ret = service->GetUpgradePolicy(upgradeInfo, policy, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetUpgradePolicy");
    UpdateHelper::WriteBusinessError(reply, businessError);
    UpdateHelper::WriteUpgradePolicy(reply, policy);
    return INT_CALL_SUCCESS;
}

static int32_t RegisterUpdateCallbackStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_LOGI("RegisterUpdateCallbackStub");
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    auto remote = data.ReadRemoteObject();
    ENGINE_CHECK(remote != nullptr, return INT_CALL_IPC_ERR, "Failed to read remote obj");
    return service->RegisterUpdateCallback(upgradeInfo, iface_cast<IUpdateCallback>(remote));
}

static int32_t UnregisterUpdateCallbackStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    return service->UnregisterUpdateCallback(upgradeInfo);
}

static int32_t CancelStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    BusinessError businessError = {};
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    int32_t ret = service->Cancel(upgradeInfo, data.ReadInt32(), businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to cancel");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t FactoryResetStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    SYS_EVENT_SYSTEM_RESET(0, UpdateSystemEvent::RESET_START);
    BusinessError businessError;
    int32_t ret = service->FactoryReset(businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to FactoryReset");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t ApplyNewVersionStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    UpdateHelper::ReadUpgradeInfo(data, upgradeInfo);
    string miscFile = Str16ToStr8(data.ReadString16());
    string packageName = Str16ToStr8(data.ReadString16());
    BusinessError businessError;
    int32_t ret = service->ApplyNewVersion(upgradeInfo, miscFile, packageName, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to ApplyNewVersion");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static int32_t VerifyUpgradePackageStub(UpdateServiceStub::UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    string packagePath = Str16ToStr8(data.ReadString16());
    string keyPath = Str16ToStr8(data.ReadString16());
    BusinessError businessError;
    int32_t ret = service->VerifyUpgradePackage(packagePath, keyPath, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to VerifyUpgradePackage");
    UpdateHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

static bool IsPermissionGranted(uint32_t code)
{
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    string permission = "ohos.permission.UPDATE_SYSTEM";
    if (code == IUpdateService::FACTORY_RESET) {
        permission = "ohos.permission.FACTORY_RESET";
    }
    int verifyResult = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permission);
    bool isPermissionGranted = verifyResult == Security::AccessToken::PERMISSION_GRANTED;
    if (!isPermissionGranted) {
        ENGINE_LOGE("%{public}s not granted, code:%{public}d", permission.c_str(), code);
    }
    return isPermissionGranted;
}

int32_t UpdateServiceStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ENGINE_LOGI("UpdateServiceStub ReadInterfaceToken fail");
        return CALL_RESULT_TO_IPC_RESULT(INT_CALL_IPC_ERR);
    }
    static std::map<uint32_t, UpdateServiceStub::RequestFuncType> requestFuncMap = {
        {IUpdateService::CHECK_VERSION, CheckNewVersionStub},
        {IUpdateService::DOWNLOAD, DownloadVersionStub},
        {IUpdateService::PAUSE_DOWNLOAD, PauseDownloadStub},
        {IUpdateService::RESUME_DOWNLOAD, ResumeDownloadStub},
        {IUpdateService::UPGRADE, DoUpdateStub},
        {IUpdateService::CLEAR_ERROR, ClearErrorStub},
        {IUpdateService::TERMINATE_UPGRADE, TerminateUpgradeStub},
        {IUpdateService::SET_POLICY, SetUpgradePolicyStub},
        {IUpdateService::GET_POLICY, GetUpgradePolicyStub},
        {IUpdateService::GET_NEW_VERSION, GetNewVersionStub},
        {IUpdateService::GET_NEW_VERSION_DESCRIPTION, GetNewVersionDescriptionStub},
        {IUpdateService::GET_CURRENT_VERSION, GetCurrentVersionStub},
        {IUpdateService::GET_CURRENT_VERSION_DESCRIPTION, GetCurrentVersionDescriptionStub},
        {IUpdateService::GET_TASK_INFO, GetTaskInfoStub},
        {IUpdateService::REGISTER_CALLBACK, RegisterUpdateCallbackStub},
        {IUpdateService::UNREGISTER_CALLBACK, UnregisterUpdateCallbackStub},
        {IUpdateService::CANCEL, CancelStub},
        {IUpdateService::FACTORY_RESET, FactoryResetStub},
        {IUpdateService::APPLY_NEW_VERSION, ApplyNewVersionStub},
        {IUpdateService::VERIFY_UPGRADE_PACKAGE, VerifyUpgradePackageStub},
    };

    ENGINE_LOGI("UpdateServiceStub func code %{public}u", code);
    auto iter = requestFuncMap.find(code);
    if (iter == requestFuncMap.end()) {
        ENGINE_LOGE("UpdateServiceStub OnRemoteRequest code %{public}u not found", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    if (!IsPermissionGranted(code)) {
        UpgradeInfo tmpInfo;
        UpdateHelper::ReadUpgradeInfo(data, tmpInfo);
        SYS_EVENT_VERIFY_FAILED(0, UpdateHelper::BuildEventDevId(tmpInfo),
            UpdateSystemEvent::EVENT_PERMISSION_VERIFY_FAILED);
        return CALL_RESULT_TO_IPC_RESULT(INT_APP_NOT_GRANTED);
    }
    return CALL_RESULT_TO_IPC_RESULT(iter->second(this, data, reply, option));
}
} // namespace UpdateEngine
} // namespace OHOS