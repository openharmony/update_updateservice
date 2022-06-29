/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "client_helper.h"

#include <string>
#include <vector>

#include "napi_util.h"
#include "node_api.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
enum TaskBodyMemeberMask {
    VERSION_DIGEST_INFO = 0x00000001,
    UPGRADE_STATUS      = 0x00000010,
    SUB_STATUS          = 0x00000100,
    PROGRESS            = 0x00001000,
    INSTALL_MODE        = 0x00010000,
    ERROR_MESSAGE       = 0x00100000,
    VERSION_COMPONENT   = 0x01000000
};

std::map<EventId, uint32_t> g_taskBodyTemplateMap = {
    { EventId::EVENT_TASK_RECEIVE,     VERSION_DIGEST_INFO },
    { EventId::EVENT_TASK_CANCEL,      VERSION_DIGEST_INFO },
    { EventId::EVENT_DOWNLOAD_WAIT,    VERSION_DIGEST_INFO | UPGRADE_STATUS | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_START,   VERSION_DIGEST_INFO | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_UPDATE,  VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_PAUSE,   VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE | ERROR_MESSAGE },
    { EventId::EVENT_DOWNLOAD_RESUME,  VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_SUCCESS, VERSION_DIGEST_INFO | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_FAIL,    VERSION_DIGEST_INFO | INSTALL_MODE | ERROR_MESSAGE },
    { EventId::EVENT_UPGRADE_WAIT,     VERSION_DIGEST_INFO | INSTALL_MODE | ERROR_MESSAGE },
    { EventId::EVENT_UPGRADE_START,    VERSION_DIGEST_INFO | INSTALL_MODE },
    { EventId::EVENT_UPGRADE_UPDATE,   VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE },
    { EventId::EVENT_APPLY_WAIT,       VERSION_DIGEST_INFO | UPGRADE_STATUS | ERROR_MESSAGE },
    { EventId::EVENT_APPLY_START,      VERSION_DIGEST_INFO },
    { EventId::EVENT_UPGRADE_SUCCESS,  VERSION_DIGEST_INFO | VERSION_COMPONENT },
    { EventId::EVENT_UPGRADE_FAIL,     VERSION_DIGEST_INFO | VERSION_COMPONENT }
};

void BuildDescInfo(napi_env env, napi_value &obj, const DescriptionInfo &descriptInfo)
{
    napi_value napiDescriptInfo;
    napi_create_object(env, &napiDescriptInfo);
    NapiUtil::SetInt32(env, napiDescriptInfo, "descriptionType", static_cast<int32_t>(descriptInfo.descriptionType));
    NapiUtil::SetString(env, napiDescriptInfo, "content", descriptInfo.content.c_str());
    napi_set_named_property(env, obj, "descriptionInfo", napiDescriptInfo);
}

void BuildVersionComponents(napi_env env, napi_value &obj, const VersionComponent versionComponents[], size_t arraySize)
{
    napi_value napiVersionComponents;
    napi_create_array_with_length(env, arraySize, &napiVersionComponents);
    napi_status status;
    for (size_t i = 0; i < arraySize; i++) {
        napi_value napiVersionComponent;
        status = napi_create_object(env, &napiVersionComponent);
        NapiUtil::SetInt32(env, napiVersionComponent, "componentType", versionComponents[i].componentType);
        NapiUtil::SetString(env, napiVersionComponent, "upgradeAction", versionComponents[i].upgradeAction.c_str());
        NapiUtil::SetString(env, napiVersionComponent, "displayVersion", versionComponents[i].displayVersion.c_str());
        NapiUtil::SetString(env, napiVersionComponent, "innerVersion", versionComponents[i].innerVersion.c_str());
        NapiUtil::SetInt32(env, napiVersionComponent, "size", versionComponents[i].size);
        NapiUtil::SetInt32(env, napiVersionComponent, "effectiveMode", versionComponents[i].effectiveMode);
        BuildDescInfo(env, napiVersionComponent, versionComponents[i].descriptionInfo);
        napi_set_element(env, napiVersionComponents, i, napiVersionComponent);
    }
    napi_set_named_property(env, obj, "versionComponents", napiVersionComponents);
}

int32_t ClientHelper::BuildCurrentVersionInfo(napi_env env, napi_value &obj, const UpdateResult &result)
{
    CLIENT_LOGI("BuildCurrentVersionInfo");
    PARAM_CHECK(result.type == SessionType::SESSION_GET_CUR_VERSION,
        return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE), "invalid type %{public}d", result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));

    CurrentVersionInfo *info = result.result.currentVersionInfo;
    PARAM_CHECK(info != nullptr, return static_cast<int32_t>(ClientStatus::CLIENT_FAIL), "info is null");

    NapiUtil::SetString(env, obj, "osVersion", info->osVersion);
    NapiUtil::SetString(env, obj, "deviceName", info->deviceName);
    BuildVersionComponents(env, obj, info->versionComponents, COUNT_OF(info->versionComponents));
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

void BuildVersionDigestInfo(napi_env env, napi_value &obj, const VersionDigestInfo &versionDigestInfo)
{
    napi_value napiVersionDigestInfo;
    napi_create_object(env, &napiVersionDigestInfo);
    NapiUtil::SetString(env, napiVersionDigestInfo, "versionDigest", versionDigestInfo.versionDigest);
    napi_set_named_property(env, obj, "versionDigestInfo", napiVersionDigestInfo);
}

void BuildErrorMessages(napi_env env, napi_value &obj, const std::string &name, const ErrorMessage errorMessages[],
    size_t arraySize)
{
    napi_value napiErrorMessages;
    napi_create_array_with_length(env, arraySize, &napiErrorMessages);
    for (size_t i = 0; i < arraySize; i++) {
        napi_value napiErrorMessage;
        napi_create_object(env, &napiErrorMessage);
        NapiUtil::SetInt32(env, napiErrorMessage, "errorCode", errorMessages[i].errorCode);
        NapiUtil::SetString(env, napiErrorMessage, "errorMessage", errorMessages[i].errorMessage);
        napi_set_element(env, napiErrorMessages, i, napiErrorMessage);
    }
    napi_set_named_property(env, obj, name.c_str(), napiErrorMessages);
}

void BuildTaskBody(napi_env env, napi_value &obj, const TaskBody &taskBody)
{
    napi_value napiTaskBody;
    napi_create_object(env, &napiTaskBody);
    BuildVersionDigestInfo(env, napiTaskBody, taskBody.versionDigestInfo);
    NapiUtil::SetInt32(env, napiTaskBody, "status", taskBody.status);
    NapiUtil::SetInt32(env, napiTaskBody, "subStatus", taskBody.subStatus);
    NapiUtil::SetInt32(env, napiTaskBody, "progress", taskBody.progress);
    NapiUtil::SetInt32(env, napiTaskBody, "installMode", taskBody.installMode);
    BuildErrorMessages(env, napiTaskBody, "errorMessages", taskBody.errorMessages, COUNT_OF(taskBody.errorMessages));
    BuildVersionComponents(env, napiTaskBody, taskBody.versionComponents, COUNT_OF(taskBody.versionComponents));
    napi_set_named_property(env, obj, "taskBody", napiTaskBody);
}

int32_t ClientHelper::BuildTaskInfo(napi_env env, napi_value &obj, const UpdateResult &result)
{
    CLIENT_LOGI("ClientHelper::BuildTaskInfo");
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));
    if (result.result.taskInfo == nullptr) {
        return static_cast<int32_t>(ClientStatus::CLIENT_FAIL);
    }
    NapiUtil::SetBool(env, obj, "existTask", result.result.taskInfo->existTask);
    BuildTaskBody(env, obj, result.result.taskInfo->taskBody);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildNewVersionInfo(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.type == SessionType::SESSION_GET_NEW_VERSION,
        return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "invalid type %d",
        result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));
    if (result.result.newVersionInfo == nullptr) {
        return static_cast<int32_t>(ClientStatus::CLIENT_FAIL);
    }
    BuildVersionDigestInfo(env, obj, result.result.newVersionInfo->versionDigestInfo);
    BuildVersionComponents(env, obj, result.result.newVersionInfo->versionComponents,
        COUNT_OF(result.result.newVersionInfo->versionComponents));
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildCheckResultEx(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.type == SessionType::SESSION_CHECK_VERSION,
        return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE), "invalid type %d", result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));
    CheckResultEx *checkResultEx = result.result.checkResultEx;
    if (checkResultEx == nullptr) {
        return static_cast<int32_t>(ClientStatus::CLIENT_FAIL);
    }
    NapiUtil::SetBool(env, obj, "isExistNewVersion", checkResultEx->isExistNewVersion);

    napi_value newVersionInfo;
    napi_create_object(env, &newVersionInfo);
    BuildVersionDigestInfo(env, newVersionInfo, checkResultEx->newVersionInfo.versionDigestInfo);
    BuildVersionComponents(env, newVersionInfo, checkResultEx->newVersionInfo.versionComponents,
        COUNT_OF(checkResultEx->newVersionInfo.versionComponents));
    napi_set_named_property(env, obj, "newVersionInfo", newVersionInfo);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildProgress(napi_env env, napi_value &obj, const UpdateResult &result)
{
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));
    if (result.result.progress == nullptr) {
        return static_cast<int32_t>(ClientStatus::CLIENT_FAIL);
    }
    NapiUtil::SetInt32(env, obj, "status", result.result.progress->status);
    NapiUtil::SetInt32(env, obj, "percent", result.result.progress->percent);
    NapiUtil::SetString(env, obj, "endReason", result.result.progress->endReason);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildUpdatePolicy(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.type == SessionType::SESSION_GET_POLICY || result.type == SessionType::SESSION_SET_POLICY,
        return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE), "invalid type %d", result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return status, "Failed to create napi_create_object %d", status);
    if (result.result.updatePolicy == nullptr) {
        return static_cast<int32_t>(ClientStatus::CLIENT_FAIL);
    }
    UpdatePolicy &updatePolicy = *result.result.updatePolicy;

    // Add the result.
    NapiUtil::SetBool(env, obj, "downloadStrategy", updatePolicy.downloadStrategy);
    NapiUtil::SetBool(env, obj, "autoUpgradeStrategy", updatePolicy.autoUpgradeStrategy);

    napi_value autoUpgradePeriods;
    size_t count = COUNT_OF(updatePolicy.autoUpgradePeriods);
    status = napi_create_array_with_length(env, count, &autoUpgradePeriods);
    PARAM_CHECK(status == napi_ok, return status, "Failed to create array for interval %d", status);
    for (size_t i = 0; i < count; i++) {
        napi_value result;
        status = napi_create_object(env, &result);
        NapiUtil::SetInt32(env, result, "start", updatePolicy.autoUpgradePeriods[i].start);
        NapiUtil::SetInt32(env, result, "end", updatePolicy.autoUpgradePeriods[i].end);
        napi_set_element(env, autoUpgradePeriods, i, result);
    }
    status = napi_set_named_property(env, obj, "autoUpgradePeriods", autoUpgradePeriods);
    return napi_ok;
}

int32_t ClientHelper::BuildInt32Status(napi_env env, napi_value &obj, const UpdateResult &result)
{
    return napi_create_int32(env, result.result.status, &obj);
}

int32_t ClientHelper::BuildVoidStatus(napi_env env, napi_value &obj, const UpdateResult &result)
{
    return napi_ok;
}

int32_t ClientHelper::BuildOtaStatus(napi_env env, napi_value &obj, const UpdateResult &result)
{
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));
    OtaStatus *otaStatus = result.result.otaStatus;
    if (otaStatus == nullptr) {
        return static_cast<int32_t>(ClientStatus::CLIENT_FAIL);
    }
    NapiUtil::SetInt32(env, obj, "progress", otaStatus->progress);
    NapiUtil::SetInt32(env, obj, "status", otaStatus->status);
    NapiUtil::SetInt32(env, obj, "subStatus", otaStatus->subStatus);
    napi_value errMsgs;
    size_t arraySize = COUNT_OF(otaStatus->errMsg);
    napi_create_array_with_length(env, arraySize, &errMsgs);
    for (size_t i = 0; i < arraySize; i++) {
        napi_value result;
        status = napi_create_object(env, &result);
        NapiUtil::SetInt32(env, result, "errorCode", otaStatus->errMsg[i].errorCode);
        NapiUtil::SetString(env, result, "errorMsg", otaStatus->errMsg[i].errorMsg);
        napi_set_element(env, errMsgs, i, result);
    }
    napi_set_named_property(env, obj, "errMsg", errMsgs);
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

ClientStatus CheckNapiObjectType(napi_env env, const napi_value arg)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, arg, &type);
    PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_INVALID_TYPE,
        "Invalid argc %d", static_cast<int32_t>(status));
    PARAM_CHECK(type == napi_object, return ClientStatus::CLIENT_INVALID_TYPE,
        "Invalid argc %d", static_cast<int32_t>(type))
    return ClientStatus::CLIENT_SUCCESS;
}

void ParseBusinessType(napi_env env, const napi_value arg, UpgradeInfo &upgradeInfo)
{
    bool result = false;
    napi_status status = napi_has_named_property(env, arg, "businessType", &result);
    if (result && (status == napi_ok)) {
        napi_value businessTypeValue;
        status = napi_get_named_property(env, arg, "businessType", &businessTypeValue);
        NapiUtil::GetString(env, businessTypeValue, "vendor", upgradeInfo.businessType.vendor);

        int32_t subType;
        NapiUtil::GetInt32(env, businessTypeValue, "subType", subType);
        upgradeInfo.businessType.subType = static_cast<BusinessSubType>(subType);
    }
    upgradeInfo.businessType.subType = BusinessSubType::FIRMWARE;
}

ClientStatus ClientHelper::GetUpgradeInfoFromArg(napi_env env, const napi_value arg, UpgradeInfo &upgradeInfo)
{
    NapiUtil::GetString(env, arg, "upgradeApp", upgradeInfo.upgradeApp);
    ParseBusinessType(env, arg, upgradeInfo);
    NapiUtil::GetString(env, arg, "upgradeDevId", upgradeInfo.upgradeDevId);
    NapiUtil::GetString(env, arg, "controlDevId", upgradeInfo.controlDevId);
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetUpdatePolicyFromArg(napi_env env, const napi_value arg, UpdatePolicy &updatePolicy)
{
    PARAM_CHECK(CheckNapiObjectType(env, arg) == ClientStatus::CLIENT_SUCCESS,
        return ClientStatus::CLIENT_INVALID_TYPE, "GetUpdatePolicyFromArg type invalid");

    // updatePolicy
    NapiUtil::GetBool(env, arg, "downloadStrategy", updatePolicy.downloadStrategy);
    NapiUtil::GetBool(env, arg, "autoUpgradeStrategy", updatePolicy.autoUpgradeStrategy);

    // Get the array.
    bool result = false;
    napi_status status = napi_has_named_property(env, arg, "autoUpgradePeriods", &result);
    if (result && (status == napi_ok)) {
        napi_value value;
        status = napi_get_named_property(env, arg, "autoUpgradePeriods", &value);
        PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_FAIL, "Failed to get attr autoUpgradePeriods");
        status = napi_is_array(env, value, &result);
        PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_FAIL, "napi_is_array failed");
        uint32_t count = 0;
        status = napi_get_array_length(env, value, &count);
        PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_FAIL,
            "napi_get_array_length failed");
        uint32_t i = 0;
        do {
            if (i >= COUNT_OF(updatePolicy.autoUpgradePeriods)) {
                break;
            }
            napi_value element;
            napi_get_element(env, value, i, &element);
            napi_get_value_uint32(env, element, &updatePolicy.autoUpgradePeriods[i].start);
            napi_get_value_uint32(env, element, &updatePolicy.autoUpgradePeriods[i].end);
            CLIENT_LOGI("updatePolicy autoUpgradeInterval");
            i++;
        } while (i < count);
    }
    CLIENT_LOGI("updatePolicy autoDownload:%d autoDownloadNet:%d",
        static_cast<int32_t>(updatePolicy.downloadStrategy),
        static_cast<int32_t>(updatePolicy.autoUpgradeStrategy));
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetNetType(napi_env env, const napi_value arg, NetType &netType)
{
    int allowNetwork = 0;
    NapiUtil::GetInt32(env, arg, "allowNetwork", allowNetwork);
    static const std::list netTypeList = {NetType::CELLULAR, NetType::METERED_WIFI, NetType::NOT_METERED_WIFI,
        NetType::WIFI, NetType::CELLULAR_AND_WIFI};
    PARAM_CHECK(IsValidEnum(netTypeList, allowNetwork), return ClientStatus::CLIENT_INVALID_TYPE,
        "GetNetType error, invalid NetType:%{public}d", allowNetwork);
    netType = static_cast<NetType>(allowNetwork);
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetOrder(napi_env env, const napi_value arg, Order &order)
{
    int tmpOrder = 0;
    NapiUtil::GetInt32(env, arg, "order", tmpOrder);
    static const std::list orderList = {Order::DOWNLOAD, Order::INSTALL, Order::APPLY, Order::DOWNLOAD_AND_INSTALL,
        Order::INSTALL_AND_APPLY};
    PARAM_CHECK(IsValidEnum(orderList, tmpOrder), return ClientStatus::CLIENT_INVALID_TYPE,
        "GetOrder error, invalid order:%{public}d", tmpOrder);
    order = static_cast<Order>(tmpOrder);
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetOptionsFromArg(napi_env env, const napi_value arg, DownloadOptions &downloadOptions)
{
    ClientStatus ret = GetNetType(env, arg, downloadOptions.allowNetwork);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return ClientStatus::CLIENT_INVALID_TYPE,
        "GetDownloadOptionsFromArg GetNetType error");

    ret = GetOrder(env, arg, downloadOptions.order);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return ClientStatus::CLIENT_INVALID_TYPE,
        "GetDownloadOptionsFromArg GetOrder error");
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetOptionsFromArg(napi_env env, const napi_value arg,
    PauseDownloadOptions &pauseDownloadOptions)
{
    NapiUtil::GetBool(env, arg, "isAllowAutoResume", pauseDownloadOptions.isAllowAutoResume);
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetOptionsFromArg(napi_env env, const napi_value arg,
    ResumeDownloadOptions &resumeDownloadOptions)
{
    return GetNetType(env, arg, resumeDownloadOptions.allowNetwork);
}

ClientStatus ClientHelper::GetVersionDigestInfoFromArg(napi_env env, const napi_value arg,
    VersionDigestInfo &versionDigestInfo)
{
    NapiUtil::GetString(env, arg, "versionDigest", versionDigestInfo.versionDigest);
    CLIENT_LOGI("GetVersionDigestInfoFromArg versionDigest:%{public}s", versionDigestInfo.versionDigest.c_str());
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetOptionsFromArg(napi_env env, const napi_value arg, UpgradeOptions &upgradeOptions)
{
    return GetOrder(env, arg, upgradeOptions.order);
}

ClientStatus ClientHelper::GetOptionsFromArg(napi_env env, const napi_value arg, ClearOptions &clearOptions)
{
    int32_t status = 0;
    NapiUtil::GetInt32(env, arg, "status", status);
    static const std::list statusList = {
        UpgradeStatus::UPDATE_STATE_INIT,                  UpgradeStatus::UPDATE_STATE_CHECK_VERSION_ON,
        UpgradeStatus::UPDATE_STATE_CHECK_VERSION_FAIL,    UpgradeStatus::UPDATE_STATE_CHECK_VERSION_SUCCESS,
        UpgradeStatus::UPDATE_STATE_DOWNLOAD_ON,           UpgradeStatus::UPDATE_STATE_DOWNLOAD_PAUSE,
        UpgradeStatus::UPDATE_STATE_DOWNLOAD_CANCEL,       UpgradeStatus::UPDATE_STATE_DOWNLOAD_FAIL,
        UpgradeStatus::UPDATE_STATE_DOWNLOAD_SUCCESS,      UpgradeStatus::UPDATE_STATE_VERIFY_ON,
        UpgradeStatus::UPDATE_STATE_VERIFY_FAIL,           UpgradeStatus::UPDATE_STATE_VERIFY_SUCCESS,
        UpgradeStatus::UPDATE_STATE_PACKAGE_TRANS_ON,      UpgradeStatus::UPDATE_STATE_PACKAGE_TRANS_FAIL,
        UpgradeStatus::UPDATE_STATE_PACKAGE_TRANS_SUCCESS, UpgradeStatus::UPDATE_STATE_INSTALL_ON,
        UpgradeStatus::UPDATE_STATE_INSTALL_FAIL,          UpgradeStatus::UPDATE_STATE_INSTALL_SUCCESS,
        UpgradeStatus::UPDATE_STATE_UPDATE_ON,             UpgradeStatus::UPDATE_STATE_UPDATE_FAIL,
        UpgradeStatus::UPDATE_STATE_UPDATE_SUCCESS };
    PARAM_CHECK(IsValidEnum(statusList, status), return ClientStatus::CLIENT_INVALID_TYPE,
        "GetClearOptionsFromArg error, invalid status:%{public}d", status);
    clearOptions.status = static_cast<UpgradeStatus>(status);
    CLIENT_LOGI("GetClearOptionsFromArg status:%{public}d", clearOptions.status);
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ParseUpgradeFile(napi_env env, const napi_value arg, UpgradeFile &upgradeFile)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, arg, &type);
    PARAM_CHECK(status == napi_ok && type == napi_object, return ClientStatus::CLIENT_INVALID_TYPE,
        "ParseUpgradeFile error, error type");

    int32_t fileType = 0;
    NapiUtil::GetInt32(env, arg, "fileType", fileType);
    static const std::list enumList = { ComponentType::OTA, ComponentType::PATCH, ComponentType::COTA,
        ComponentType::PARAM };
    PARAM_CHECK(IsValidEnum(enumList, fileType), return ClientStatus::CLIENT_INVALID_PARAM,
        "ParseUpgradeFile error, invalid fileType:%{public}d", fileType);
    upgradeFile.fileType = static_cast<ComponentType>(fileType);

    NapiUtil::GetString(env, arg, "filePath", upgradeFile.filePath);
    CLIENT_LOGI("ParseUpgradeFile fileType:%{public}d, filePath:%s", fileType, upgradeFile.filePath.c_str());
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetUpgradeFileFromArg(napi_env env, const napi_value arg, UpgradeFile &upgradeFile)
{
    return ParseUpgradeFile(env, arg, upgradeFile);
}

ClientStatus ClientHelper::GetUpgradeFilesFromArg(napi_env env, const napi_value arg,
    std::vector<UpgradeFile> &upgradeFiles)
{
    bool result = false;
    napi_status status = napi_is_array(env, arg, &result);
    PARAM_CHECK(status == napi_ok && result, return ClientStatus::CLIENT_FAIL,
        "GetUpgradeFilesFromArg error, napi_is_array failed");

    uint32_t count = 0;
    status = napi_get_array_length(env, arg, &count);
    PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_FAIL,
        "GetUpgradeFilesFromArg error, napi_get_array_length failed");
    for (uint32_t idx = 0; idx < count; idx++) {
        napi_value element;
        napi_get_element(env, arg, idx, &element);
        UpgradeFile upgradeFile;
        if (ParseUpgradeFile(env, element, upgradeFile) != ClientStatus::CLIENT_SUCCESS) {
            return ClientStatus::CLIENT_FAIL;
        }
        upgradeFiles.emplace_back(upgradeFile);
    }
    CLIENT_LOGI("GetUpgradeFilesFromArg success, size:%{public}zu", upgradeFiles.size());
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetEventClassifyInfoFromArg(napi_env env, const napi_value arg,
    EventClassifyInfo &eventClassifyInfo)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, arg, &type);
    PARAM_CHECK(status == napi_ok && type == napi_object, return ClientStatus::CLIENT_INVALID_TYPE,
        "GetEventClassifyInfoFromArg error, error type");

    int32_t eventClassify = 0;
    NapiUtil::GetInt32(env, arg, "eventClassify", eventClassify);
    static const std::list enumList = { EventClassify::TASK };
    PARAM_CHECK(IsValidEnum(enumList, eventClassify), return ClientStatus::CLIENT_INVALID_TYPE,
        "GetEventClassifyInfoFromArg error, invalid eventClassify:0x%{public}x", eventClassify);
    eventClassifyInfo.eventClassify = static_cast<EventClassify>(eventClassify);

    NapiUtil::GetString(env, arg, "extraInfo", eventClassifyInfo.extraInfo);
    CLIENT_LOGI("GetEventClassifyInfoFromArg eventClassify:0x%{public}x, extraInfo:%s",
        eventClassify, eventClassifyInfo.extraInfo.c_str());
    return ClientStatus::CLIENT_SUCCESS;
}

int32_t ClientHelper::BuildBusinessError(napi_env env, napi_value &obj, const BusinessError &businessError)
{
    if (!UpdateHelper::IsErrorExist(businessError)) {
        // success, no need to set businessError
        return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
    }
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok,
        return static_cast<int32_t>(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d",
        static_cast<int32_t>(status));

    NapiUtil::SetString(env, obj, "message", businessError.message);
    NapiUtil::SetInt32(env, obj, "errorNum", static_cast<int32_t>(businessError.errorNum));
    BuildErrorMessages(env, obj, "data", businessError.data, COUNT_OF(businessError.data));
    return static_cast<int32_t>(ClientStatus::CLIENT_SUCCESS);
}

ClientStatus BuildTaskBody(napi_env env, napi_value &obj, EventId eventId, const TaskBody &taskBody)
{
    auto iter = g_taskBodyTemplateMap.find(eventId);
    PARAM_CHECK(iter != g_taskBodyTemplateMap.end(), return ClientStatus::CLIENT_INVALID_PARAM,
        "BuildTaskBody error, eventId %{public}d", CAST_INT(eventId));
    uint32_t taskBodyTemplate = iter->second;
    napi_value napiTaskBody = nullptr;
    napi_create_object(env, &napiTaskBody);
    if (taskBodyTemplate & VERSION_DIGEST_INFO) {
        BuildVersionDigestInfo(env, napiTaskBody, taskBody.versionDigestInfo);
    }
    if (taskBodyTemplate & UPGRADE_STATUS) {
        NapiUtil::SetInt32(env, napiTaskBody, "status", taskBody.status);
    }
    if (taskBodyTemplate & SUB_STATUS) {
        NapiUtil::SetInt32(env, napiTaskBody, "subStatus", taskBody.subStatus);
    }
    if (taskBodyTemplate & PROGRESS) {
        NapiUtil::SetInt32(env, napiTaskBody, "progress", taskBody.progress);
    }
    if (taskBodyTemplate & INSTALL_MODE) {
        NapiUtil::SetInt32(env, napiTaskBody, "installMode", taskBody.installMode);
    }
    if (taskBodyTemplate & ERROR_MESSAGE) {
        BuildErrorMessages(env, napiTaskBody, "errorMessages", taskBody.errorMessages,
            COUNT_OF(taskBody.errorMessages));
    }
    if (taskBodyTemplate & VERSION_COMPONENT) {
        BuildVersionComponents(env, napiTaskBody, taskBody.versionComponents, COUNT_OF(taskBody.versionComponents));
    }
    napi_set_named_property(env, obj, "taskBody", napiTaskBody);
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::BuildEventInfo(napi_env env, napi_value &obj, const EventInfo &eventInfo)
{
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return ClientStatus::CLIENT_FAIL,
        "BuildEventInfo error, failed to create napi_create_object %{public}d", CAST_INT(status));

    NapiUtil::SetInt32(env, obj, "eventId", CAST_INT(eventInfo.eventId));
    ClientStatus ret = BuildTaskBody(env, obj, eventInfo.eventId, eventInfo.taskBody);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return ClientStatus::CLIENT_FAIL,
        "BuildEventInfo error, build task info fail");
    return ClientStatus::CLIENT_SUCCESS;
}
} // namespace UpdateEngine
} // namespace OHOS