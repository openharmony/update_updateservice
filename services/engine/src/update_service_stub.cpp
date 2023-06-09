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
#include "access_token.h"
#include "ipc_skeleton.h"
#include "message_parcel_helper.h"
#include "securec.h"
#include "tokenid_kit.h"
#include "update_define.h"
#include "update_helper.h"
#include "update_log.h"
#include "update_service_stub.h"
#include "update_system_event.h"
#include "updater_sa_ipc_interface_code.h"

using namespace std;

namespace OHOS {
namespace UpdateEngine {
constexpr const pid_t ROOT_UID = 0;
constexpr const pid_t EDM_UID = 3057;

#define CALL_RESULT_TO_IPC_RESULT(callResult) ((callResult) + CALL_RESULT_OFFSET)

#define RETURN_FAIL_WHEN_SERVICE_NULL(service) \
    ENGINE_CHECK((service) != nullptr, return INT_CALL_IPC_ERR, "service null")

UpdateServiceStub::UpdateServiceStub()
{
    requestFuncMap_ = {
        {CAST_UINT(UpdaterSaInterfaceCode::CHECK_VERSION), &UpdateServiceStub::CheckNewVersionStub},
        {CAST_UINT(UpdaterSaInterfaceCode::DOWNLOAD), &UpdateServiceStub::DownloadVersionStub},
        {CAST_UINT(UpdaterSaInterfaceCode::PAUSE_DOWNLOAD), &UpdateServiceStub::PauseDownloadStub},
        {CAST_UINT(UpdaterSaInterfaceCode::RESUME_DOWNLOAD), &UpdateServiceStub::ResumeDownloadStub},
        {CAST_UINT(UpdaterSaInterfaceCode::UPGRADE), &UpdateServiceStub::DoUpdateStub},
        {CAST_UINT(UpdaterSaInterfaceCode::CLEAR_ERROR), &UpdateServiceStub::ClearErrorStub},
        {CAST_UINT(UpdaterSaInterfaceCode::TERMINATE_UPGRADE), &UpdateServiceStub::TerminateUpgradeStub},
        {CAST_UINT(UpdaterSaInterfaceCode::SET_POLICY), &UpdateServiceStub::SetUpgradePolicyStub},
        {CAST_UINT(UpdaterSaInterfaceCode::GET_POLICY), &UpdateServiceStub::GetUpgradePolicyStub},
        {CAST_UINT(UpdaterSaInterfaceCode::GET_NEW_VERSION), &UpdateServiceStub::GetNewVersionStub},
        {CAST_UINT(UpdaterSaInterfaceCode::GET_NEW_VERSION_DESCRIPTION),
            &UpdateServiceStub::GetNewVersionDescriptionStub},
        {CAST_UINT(UpdaterSaInterfaceCode::GET_CURRENT_VERSION), &UpdateServiceStub::GetCurrentVersionStub},
        {CAST_UINT(UpdaterSaInterfaceCode::GET_CURRENT_VERSION_DESCRIPTION),
            &UpdateServiceStub::GetCurrentVersionDescriptionStub},
        {CAST_UINT(UpdaterSaInterfaceCode::GET_TASK_INFO), &UpdateServiceStub::GetTaskInfoStub},
        {CAST_UINT(UpdaterSaInterfaceCode::REGISTER_CALLBACK), &UpdateServiceStub::RegisterUpdateCallbackStub},
        {CAST_UINT(UpdaterSaInterfaceCode::UNREGISTER_CALLBACK), &UpdateServiceStub::UnregisterUpdateCallbackStub},
        {CAST_UINT(UpdaterSaInterfaceCode::CANCEL), &UpdateServiceStub::CancelStub},
        {CAST_UINT(UpdaterSaInterfaceCode::FACTORY_RESET), &UpdateServiceStub::FactoryResetStub},
        {CAST_UINT(UpdaterSaInterfaceCode::APPLY_NEW_VERSION), &UpdateServiceStub::ApplyNewVersionStub},
        {CAST_UINT(UpdaterSaInterfaceCode::VERIFY_UPGRADE_PACKAGE), &UpdateServiceStub::VerifyUpgradePackageStub}
    };
}

int32_t UpdateServiceStub::GetNewVersionStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    NewVersionInfo newVersionInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetNewVersionInfo(upgradeInfo, newVersionInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetNewVersionInfo");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    MessageParcelHelper::WriteNewVersionInfo(reply, newVersionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::GetNewVersionDescriptionStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    DescriptionOptions descriptionOptions;
    VersionDescriptionInfo newVersionDescriptionInfo;
    BusinessError businessError;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    MessageParcelHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    MessageParcelHelper::ReadDescriptionOptions(data, descriptionOptions);

    int32_t ret = service->GetNewVersionDescription(upgradeInfo, versionDigestInfo, descriptionOptions,
        newVersionDescriptionInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetNewVersionDescription");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    MessageParcelHelper::WriteVersionDescriptionInfo(reply, newVersionDescriptionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::GetCurrentVersionStub(UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    CurrentVersionInfo currentVersionInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetCurrentVersionInfo(upgradeInfo, currentVersionInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetCurrentVersion");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    MessageParcelHelper::WriteCurrentVersionInfo(reply, currentVersionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::GetCurrentVersionDescriptionStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    DescriptionOptions descriptionOptions;
    VersionDescriptionInfo currentVersionDescriptionInfo;
    BusinessError businessError;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    MessageParcelHelper::ReadDescriptionOptions(data, descriptionOptions);

    int32_t ret = service->GetCurrentVersionDescription(upgradeInfo, descriptionOptions, currentVersionDescriptionInfo,
        businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetCurrentVersionDescription");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    MessageParcelHelper::WriteVersionDescriptionInfo(reply, currentVersionDescriptionInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::GetTaskInfoStub(UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    TaskInfo taskInfo = {};
    BusinessError businessError = {};
    int32_t ret = service->GetTaskInfo(upgradeInfo, taskInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetTaskInfo");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    MessageParcelHelper::WriteTaskInfo(reply, taskInfo);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::CheckNewVersionStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    return service->CheckNewVersion(upgradeInfo);
}

int32_t UpdateServiceStub::DownloadVersionStub(UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    DownloadOptions downloadOptions;
    BusinessError businessError;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    MessageParcelHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    MessageParcelHelper::ReadDownloadOptions(data, downloadOptions);
    int32_t ret = service->Download(upgradeInfo, versionDigestInfo, downloadOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to DownloadVersion");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::PauseDownloadStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    PauseDownloadOptions pauseDownloadOptions;
    BusinessError businessError;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    MessageParcelHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    MessageParcelHelper::ReadPauseDownloadOptions(data, pauseDownloadOptions);

    int32_t ret = service->PauseDownload(upgradeInfo, versionDigestInfo, pauseDownloadOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to PauseDownload");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::ResumeDownloadStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    ResumeDownloadOptions resumeDownloadOptions;
    BusinessError businessError;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    MessageParcelHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    MessageParcelHelper::ReadResumeDownloadOptions(data, resumeDownloadOptions);

    int32_t ret = service->ResumeDownload(upgradeInfo, versionDigestInfo, resumeDownloadOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to ResumeDownload");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::DoUpdateStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    UpgradeOptions upgradeOptions;
    BusinessError businessError;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    MessageParcelHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    MessageParcelHelper::ReadUpgradeOptions(data, upgradeOptions);

    int32_t ret = service->Upgrade(upgradeInfo, versionDigestInfo, upgradeOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to Upgrade");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::ClearErrorStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    VersionDigestInfo versionDigestInfo;
    ClearOptions clearOptions;
    BusinessError businessError;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    MessageParcelHelper::ReadVersionDigestInfo(data, versionDigestInfo);
    MessageParcelHelper::ReadClearOptions(data, clearOptions);

    int32_t ret = service->ClearError(upgradeInfo, versionDigestInfo, clearOptions, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to ClearError");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::TerminateUpgradeStub(UpdateServiceStubPtr service, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);

    BusinessError businessError;
    int32_t ret = service->TerminateUpgrade(upgradeInfo, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to TerminateUpgrade");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::SetUpgradePolicyStub(UpdateServiceStubPtr service,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpgradePolicy policy = {};
    MessageParcelHelper::ReadUpgradePolicy(data, policy);
    BusinessError businessError = {};
    int32_t ret = service->SetUpgradePolicy(upgradeInfo, policy, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to SetUpgradePolicy");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::GetUpgradePolicyStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    UpgradePolicy policy = {};
    BusinessError businessError = {};
    int32_t ret = service->GetUpgradePolicy(upgradeInfo, policy, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to GetUpgradePolicy");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    MessageParcelHelper::WriteUpgradePolicy(reply, policy);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::RegisterUpdateCallbackStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ENGINE_LOGI("RegisterUpdateCallbackStub");
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    auto remote = data.ReadRemoteObject();
    ENGINE_CHECK(remote != nullptr, return INT_CALL_IPC_ERR, "Failed to read remote obj");
    return service->RegisterUpdateCallback(upgradeInfo, iface_cast<IUpdateCallback>(remote));
}

int32_t UpdateServiceStub::UnregisterUpdateCallbackStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    return service->UnregisterUpdateCallback(upgradeInfo);
}

int32_t UpdateServiceStub::CancelStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo {};
    BusinessError businessError = {};
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    int32_t ret = service->Cancel(upgradeInfo, data.ReadInt32(), businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to cancel");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::FactoryResetStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    SYS_EVENT_SYSTEM_RESET(0, UpdateSystemEvent::RESET_START);
    BusinessError businessError;
    int32_t ret = service->FactoryReset(businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to FactoryReset");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::ApplyNewVersionStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    UpgradeInfo upgradeInfo;
    MessageParcelHelper::ReadUpgradeInfo(data, upgradeInfo);
    string miscFile = Str16ToStr8(data.ReadString16());
    string packageName = Str16ToStr8(data.ReadString16());
    BusinessError businessError;
    int32_t ret = service->ApplyNewVersion(upgradeInfo, miscFile, packageName, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to ApplyNewVersion");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

int32_t UpdateServiceStub::VerifyUpgradePackageStub(UpdateServiceStubPtr service,
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    RETURN_FAIL_WHEN_SERVICE_NULL(service);
    string packagePath = Str16ToStr8(data.ReadString16());
    string keyPath = Str16ToStr8(data.ReadString16());
    BusinessError businessError;
    int32_t ret = service->VerifyUpgradePackage(packagePath, keyPath, businessError);
    ENGINE_CHECK(ret == INT_CALL_SUCCESS, return ret, "Failed to VerifyUpgradePackage");
    MessageParcelHelper::WriteBusinessError(reply, businessError);
    return INT_CALL_SUCCESS;
}

bool UpdateServiceStub::IsCallerValid()
{
    OHOS::Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    auto callerTokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenType(callerToken);
    switch (callerTokenType) {
        case OHOS::Security::AccessToken::TypeATokenTypeEnum::TOKEN_HAP: {
            uint64_t callerFullTokenID = IPCSkeleton::GetCallingFullTokenID();
            // hap进程只允许系统应用调用
            return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(callerFullTokenID);
        }
        case OHOS::Security::AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE: {
            pid_t callerUid = IPCSkeleton::GetCallingUid();
            // native进程只允许root权限和edm调用
            return callerUid == ROOT_UID || callerUid == EDM_UID;
        }
        default:
            // 其他情况调用予以禁止
            return false;
    }
}

bool UpdateServiceStub::IsPermissionGranted(uint32_t code)
{
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    string permission = "ohos.permission.UPDATE_SYSTEM";
    if (code == CAST_UINT(UpdaterSaInterfaceCode::FACTORY_RESET)) {
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

    ENGINE_LOGI("UpdateServiceStub func code %{public}u", code);
    auto iter = requestFuncMap_.find(code);
    if (iter == requestFuncMap_.end()) {
        ENGINE_LOGE("UpdateServiceStub OnRemoteRequest code %{public}u not found", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    if (!IsCallerValid()) {
        return CALL_RESULT_TO_IPC_RESULT(INT_NOT_SYSTEM_APP);
    }

    if (!IsPermissionGranted(code)) {
        UpgradeInfo tmpInfo;
        MessageParcelHelper::ReadUpgradeInfo(data, tmpInfo);
        SYS_EVENT_VERIFY_FAILED(0, UpdateHelper::BuildEventDevId(tmpInfo),
            UpdateSystemEvent::EVENT_PERMISSION_VERIFY_FAILED);
        return CALL_RESULT_TO_IPC_RESULT(INT_APP_NOT_GRANTED);
    }
    return CALL_RESULT_TO_IPC_RESULT((this->*(iter->second))(this, data, reply, option));
}
} // namespace UpdateEngine
} // namespace OHOS
