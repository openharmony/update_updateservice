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

void ClientHelper::TrimString(std::string &str)
{
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
}

bool ClientHelper::IsValidUpgradeFile(const std::string &upgradeFile)
{
    if (upgradeFile.empty()) {
        return false;
    }

    std::string::size_type pos = upgradeFile.find_first_of('/');
    if (pos != 0) {
        return false;
    }

    pos = upgradeFile.find_last_of('.');
    if (pos == std::string::npos) {
        return false;
    }

    std::string postfix = upgradeFile.substr(pos + 1);
    std::transform(postfix.begin(), postfix.end(), postfix.begin(), ::tolower);
    if (postfix.compare("bin") == 0) {
        return true;
    } else if (postfix.compare("zip") == 0) {
        return true;
    } else if (postfix.compare("lz4") == 0) {
        return true;
    } else if (postfix.compare("gz") == 0) {
        return true;
    }
    return false;
}

void BuildDescInfo(napi_env env, napi_value &obj, const DescriptionInfo &descriptInfo)
{
    napi_value napiDescriptInfo;
    napi_create_object(env, &napiDescriptInfo);
    NapiUtil::SetInt32(env, napiDescriptInfo, "descriptionType", static_cast<int32_t>(descriptInfo.descriptionType));
    NapiUtil::SetString(env, napiDescriptInfo, "content", descriptInfo.content.c_str());
    napi_set_named_property(env, obj, "descriptionInfo", napiDescriptInfo);
}

bool IsValidData(const ErrorMessage &errorMessage)
{
    return errorMessage.errorCode != 0;
}

bool IsValidData(const ComponentDescription &componentDescription)
{
    return !componentDescription.componentId.empty();
}

bool IsValidData(const VersionComponent &versionComponent)
{
    return versionComponent.componentType != static_cast<uint32_t>(ComponentType::INVALID);
}

template<typename T>
size_t GetValidDataCount(const T dataArray[], size_t arraySize)
{
    size_t validDataCount = 0;
    for (size_t i = 0; i < arraySize; i++) {
        if (IsValidData(dataArray[i])) {
            validDataCount++;
        }
    }
    return validDataCount;
}

void BuildComponentDescriptions(napi_env env, napi_value &obj, const ComponentDescription componentDescriptions[],
    size_t arraySize)
{
    size_t validComponentCount = GetValidDataCount(componentDescriptions, arraySize);
    if (validComponentCount == 0) {
        return;
    }
    napi_create_array_with_length(env, validComponentCount, &obj);
    napi_status status;
    size_t index = 0;
    for (size_t i = 0; (i < arraySize) && (index < validComponentCount); i++) {
        if (IsValidData(componentDescriptions[i])) {
            napi_value napiComponentDescription;
            status = napi_create_object(env, &napiComponentDescription);
            NapiUtil::SetString(env, napiComponentDescription, "componentId",
                componentDescriptions[i].componentId.c_str());
            BuildDescInfo(env, napiComponentDescription, componentDescriptions[i].descriptionInfo);
            napi_set_element(env, obj, index, napiComponentDescription);
            index++;
        }
    }
}

void BuildVersionComponents(napi_env env, napi_value &obj, const VersionComponent versionComponents[], size_t arraySize)
{
    size_t validComponentCount = GetValidDataCount(versionComponents, arraySize);
    if (validComponentCount == 0) {
        return;
    }
    napi_value napiVersionComponents;
    napi_create_array_with_length(env, validComponentCount, &napiVersionComponents);
    napi_status status;
    size_t index = 0;
    for (size_t i = 0; (i < arraySize) && (index < validComponentCount); i++) {
        if (IsValidData(versionComponents[i])) {
            napi_value napiVersionComponent;
            status = napi_create_object(env, &napiVersionComponent);
            NapiUtil::SetString(env, napiVersionComponent, "componentId", versionComponents[i].componentId.c_str());
            NapiUtil::SetInt32(env, napiVersionComponent, "componentType", versionComponents[i].componentType);
            NapiUtil::SetString(env, napiVersionComponent, "upgradeAction",
                versionComponents[i].upgradeAction.c_str());
            NapiUtil::SetString(env, napiVersionComponent, "displayVersion",
                versionComponents[i].displayVersion.c_str());
            NapiUtil::SetString(env, napiVersionComponent, "innerVersion", versionComponents[i].innerVersion.c_str());
            NapiUtil::SetInt32(env, napiVersionComponent, "size", versionComponents[i].size);
            NapiUtil::SetInt32(env, napiVersionComponent, "effectiveMode", versionComponents[i].effectiveMode);
            BuildDescInfo(env, napiVersionComponent, versionComponents[i].descriptionInfo);
            napi_set_element(env, napiVersionComponents, index, napiVersionComponent);
            index++;
        }
    }
    napi_set_named_property(env, obj, "versionComponents", napiVersionComponents);
}

int32_t ClientHelper::BuildCurrentVersionInfo(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.result.currentVersionInfo != nullptr, return CAST_INT(ClientStatus::CLIENT_SUCCESS),
        "ClientHelper::BuildCurrentVersionInfo null");
    CLIENT_LOGI("BuildCurrentVersionInfo");
    PARAM_CHECK(result.type == SessionType::SESSION_GET_CUR_VERSION,
        return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE), "invalid type %{public}d", result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));

    CurrentVersionInfo *info = result.result.currentVersionInfo;
    PARAM_CHECK(info != nullptr, return CAST_INT(ClientStatus::CLIENT_FAIL), "info is null");

    NapiUtil::SetString(env, obj, "osVersion", info->osVersion);
    NapiUtil::SetString(env, obj, "deviceName", info->deviceName);
    BuildVersionComponents(env, obj, info->versionComponents, COUNT_OF(info->versionComponents));
    return CAST_INT(ClientStatus::CLIENT_SUCCESS);
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
    size_t validErrorMsgCount = GetValidDataCount(errorMessages, arraySize);
    if (validErrorMsgCount == 0) {
        return;
    }

    napi_value napiErrorMessages;
    napi_create_array_with_length(env, validErrorMsgCount, &napiErrorMessages);
    size_t index = 0;
    for (size_t i = 0; (i < arraySize) && (index < validErrorMsgCount); i++) {
        if (IsValidData(errorMessages[i])) {
            napi_value napiErrorMessage;
            napi_create_object(env, &napiErrorMessage);
            NapiUtil::SetInt32(env, napiErrorMessage, "errorCode", errorMessages[i].errorCode);
            NapiUtil::SetString(env, napiErrorMessage, "errorMessage", errorMessages[i].errorMessage);
            napi_set_element(env, napiErrorMessages, index, napiErrorMessage);
            index++;
        }
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
    PARAM_CHECK(result.result.taskInfo != nullptr, return CAST_INT(ClientStatus::CLIENT_SUCCESS),
        "ClientHelper::BuildTaskInfo null");
    CLIENT_LOGI("ClientHelper::BuildTaskInfo");
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));
    NapiUtil::SetBool(env, obj, "existTask", result.result.taskInfo->existTask);
    if (result.result.taskInfo->existTask) {
        BuildTaskBody(env, obj, result.result.taskInfo->taskBody);
    }
    return CAST_INT(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildNewVersionInfo(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.result.newVersionInfo != nullptr, return CAST_INT(ClientStatus::CLIENT_SUCCESS),
        "ClientHelper::BuildNewVersionInfo null");
    PARAM_CHECK(result.type == SessionType::SESSION_GET_NEW_VERSION,
        return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE),
        "invalid type %d",
        result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));

    BuildVersionDigestInfo(env, obj, result.result.newVersionInfo->versionDigestInfo);
    BuildVersionComponents(env, obj, result.result.newVersionInfo->versionComponents,
        COUNT_OF(result.result.newVersionInfo->versionComponents));
    return CAST_INT(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildVersionDescriptionInfo(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.result.versionDescriptionInfo != nullptr, return CAST_INT(ClientStatus::CLIENT_SUCCESS),
        "ClientHelper::BuildVersionDescriptionInfo null");
    CLIENT_LOGI("BuildVersionDescriptionInfo");
    PARAM_CHECK(result.type == SessionType::SESSION_GET_NEW_VERSION_DESCRIPTION ||
        result.type == SessionType::SESSION_GET_CUR_VERSION_DESCRIPTION,
        return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE), "invalid type %{public}d", result.type);

    VersionDescriptionInfo *info = result.result.versionDescriptionInfo;
    PARAM_CHECK(info != nullptr, return CAST_INT(ClientStatus::CLIENT_FAIL), "info is null");

    BuildComponentDescriptions(env, obj, info->componentDescriptions, COUNT_OF(info->componentDescriptions));
    PARAM_CHECK(obj != nullptr, return CAST_INT(ClientStatus::CLIENT_SUCCESS), "BuildComponentDescriptions null");
    return CAST_INT(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildCheckResultEx(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.result.checkResultEx != nullptr, return CAST_INT(ClientStatus::CLIENT_SUCCESS),
        "ClientHelper::BuildCheckResultEx null");
    PARAM_CHECK(result.type == SessionType::SESSION_CHECK_VERSION,
        return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE), "invalid type %d", result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d", static_cast<int32_t>(status));
    CheckResultEx *checkResultEx = result.result.checkResultEx;
    NapiUtil::SetBool(env, obj, "isExistNewVersion", checkResultEx->isExistNewVersion);

    if (checkResultEx->isExistNewVersion) {
        napi_value newVersionInfo;
        napi_create_object(env, &newVersionInfo);
        BuildVersionDigestInfo(env, newVersionInfo, checkResultEx->newVersionInfo.versionDigestInfo);
        BuildVersionComponents(env, newVersionInfo, checkResultEx->newVersionInfo.versionComponents,
            COUNT_OF(checkResultEx->newVersionInfo.versionComponents));
        napi_set_named_property(env, obj, "newVersionInfo", newVersionInfo);
    }
    return CAST_INT(ClientStatus::CLIENT_SUCCESS);
}

int32_t ClientHelper::BuildUpgradePolicy(napi_env env, napi_value &obj, const UpdateResult &result)
{
    PARAM_CHECK(result.result.upgradePolicy != nullptr, return CAST_INT(ClientStatus::CLIENT_SUCCESS),
        "ClientHelper::BuildUpgradePolicy null");
    PARAM_CHECK(result.type == SessionType::SESSION_GET_POLICY,
        return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE), "invalid type %d", result.type);
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok, return status, "Failed to create napi_create_object %d", status);
    UpgradePolicy &upgradePolicy = *result.result.upgradePolicy;

    // Add the result.
    NapiUtil::SetBool(env, obj, "downloadStrategy", upgradePolicy.downloadStrategy);
    NapiUtil::SetBool(env, obj, "autoUpgradeStrategy", upgradePolicy.autoUpgradeStrategy);

    napi_value autoUpgradePeriods;
    size_t count = COUNT_OF(upgradePolicy.autoUpgradePeriods);
    status = napi_create_array_with_length(env, count, &autoUpgradePeriods);
    PARAM_CHECK(status == napi_ok, return status, "Failed to create array for interval %d", status);
    for (size_t i = 0; i < count; i++) {
        napi_value result;
        status = napi_create_object(env, &result);
        PARAM_CHECK(status == napi_ok, return status, "Failed to create napi_create_object %d", status);
        NapiUtil::SetInt32(env, result, "start", upgradePolicy.autoUpgradePeriods[i].start);
        NapiUtil::SetInt32(env, result, "end", upgradePolicy.autoUpgradePeriods[i].end);
        napi_set_element(env, autoUpgradePeriods, i, result);
    }
    (void)napi_set_named_property(env, obj, "autoUpgradePeriods", autoUpgradePeriods);
    return napi_ok;
}

int32_t ClientHelper::BuildUndefinedStatus(napi_env env, napi_value &obj, const UpdateResult &result)
{
    return napi_get_undefined(env, &obj);
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
    }
    upgradeInfo.businessType.subType = BusinessSubType::FIRMWARE;
}

ClientStatus ClientHelper::GetUpgradeInfoFromArg(napi_env env, const napi_value arg, UpgradeInfo &upgradeInfo)
{
    PARAM_CHECK(CheckNapiObjectType(env, arg) == ClientStatus::CLIENT_SUCCESS,
        return ClientStatus::CLIENT_INVALID_TYPE, "GetUpgradeInfoFromArg type invalid");
    NapiUtil::GetString(env, arg, "upgradeApp", upgradeInfo.upgradeApp);
    ParseBusinessType(env, arg, upgradeInfo);
    NapiUtil::GetString(env, arg, "upgradeDevId", upgradeInfo.upgradeDevId);
    NapiUtil::GetString(env, arg, "controlDevId", upgradeInfo.controlDevId);
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetUpgradePolicyFromArg(napi_env env, const napi_value arg, UpgradePolicy &upgradePolicy)
{
    PARAM_CHECK(CheckNapiObjectType(env, arg) == ClientStatus::CLIENT_SUCCESS,
        return ClientStatus::CLIENT_INVALID_TYPE, "GetUpgradePolicyFromArg type invalid");

    // upgradePolicy
    NapiUtil::GetBool(env, arg, "downloadStrategy", upgradePolicy.downloadStrategy);
    NapiUtil::GetBool(env, arg, "autoUpgradeStrategy", upgradePolicy.autoUpgradeStrategy);

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
            if (i >= COUNT_OF(upgradePolicy.autoUpgradePeriods)) {
                break;
            }
            napi_value element;
            napi_get_element(env, value, i, &element);
            napi_get_value_uint32(env, element, &upgradePolicy.autoUpgradePeriods[i].start);
            napi_get_value_uint32(env, element, &upgradePolicy.autoUpgradePeriods[i].end);
            CLIENT_LOGI("upgradePolicy autoUpgradeInterval");
            i++;
        } while (i < count);
    }
    CLIENT_LOGI("upgradePolicy autoDownload:%d autoDownloadNet:%d",
        static_cast<int32_t>(upgradePolicy.downloadStrategy),
        static_cast<int32_t>(upgradePolicy.autoUpgradeStrategy));
    return ClientStatus::CLIENT_SUCCESS;
}

ClientStatus ClientHelper::GetDescriptionFormat(napi_env env, const napi_value arg, DescriptionFormat &format)
{
    int tmpFormat = 0;
    NapiUtil::GetInt32(env, arg, "format", tmpFormat);
    static const std::list formatList = {DescriptionFormat::STANDARD, DescriptionFormat::SIMPLIFIED};
    PARAM_CHECK(IsValidEnum(formatList, tmpFormat), return ClientStatus::CLIENT_INVALID_TYPE,
        "GetDescriptionFormat error, invalid format:%{public}d", tmpFormat);
    format = static_cast<DescriptionFormat>(tmpFormat);
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

ClientStatus ClientHelper::GetOptionsFromArg(napi_env env, const napi_value arg, DescriptionOptions &descriptionOptions)
{
    ClientStatus ret = GetDescriptionFormat(env, arg, descriptionOptions.format);
    PARAM_CHECK(ret == ClientStatus::CLIENT_SUCCESS, return ClientStatus::CLIENT_INVALID_TYPE,
        "GetDescriptionOptionsFromArg GetDescriptionFormat error");
    NapiUtil::GetString(env, arg, "language", descriptionOptions.language);
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
    static const std::list enumList = { ComponentType::OTA };
    PARAM_CHECK(IsValidEnum(enumList, fileType), return ClientStatus::CLIENT_INVALID_PARAM,
        "ParseUpgradeFile error, invalid fileType:%{public}d", fileType);
    upgradeFile.fileType = static_cast<ComponentType>(fileType);

    NapiUtil::GetString(env, arg, "filePath", upgradeFile.filePath);
    ClientHelper::TrimString(upgradeFile.filePath);
    if (!ClientHelper::IsValidUpgradeFile(upgradeFile.filePath)) {
        CLIENT_LOGE("ParseUpgradeFile, invalid filePath:%s", upgradeFile.filePath.c_str());
        return ClientStatus::CLIENT_INVALID_PARAM;
    }
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
    PARAM_CHECK((status == napi_ok) && (count > 0), return ClientStatus::CLIENT_FAIL,
        "GetUpgradeFilesFromArg error, napi_get_array_length failed");
    for (uint32_t idx = 0; idx < count; idx++) {
        napi_value element;
        napi_get_element(env, arg, idx, &element);
        UpgradeFile upgradeFile;
        ClientStatus ret = ParseUpgradeFile(env, element, upgradeFile);
        if (ret != ClientStatus::CLIENT_SUCCESS) {
            return ret;
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
        return CAST_INT(ClientStatus::CLIENT_SUCCESS);
    }
    napi_status status = napi_create_object(env, &obj);
    PARAM_CHECK(status == napi_ok,
        return CAST_INT(ClientStatus::CLIENT_INVALID_TYPE),
        "Failed to create napi_create_object %d",
        static_cast<int32_t>(status));

    NapiUtil::SetString(env, obj, "message", businessError.message);
    NapiUtil::SetInt32(env, obj, "errorNum", static_cast<int32_t>(businessError.errorNum));
    BuildErrorMessages(env, obj, "data", businessError.data, COUNT_OF(businessError.data));
    return CAST_INT(ClientStatus::CLIENT_SUCCESS);
}

napi_value ClientHelper::BuildThrowError(napi_env env, const BusinessError &businessError)
{
    napi_value message = nullptr;
    napi_create_string_utf8(env, businessError.message.c_str(), NAPI_AUTO_LENGTH, &message);
    napi_value error = nullptr;
    napi_status status = napi_create_error(env, nullptr, message, &error);
    PARAM_CHECK(status == napi_ok, return nullptr, "Failed to create napi_create_object %d",
        static_cast<int32_t>(status));
    NapiUtil::SetInt32(env, error, "code", static_cast<int32_t>(businessError.errorNum));
    NapiUtil::SetString(env, error, "message", businessError.message);
    BuildErrorMessages(env, error, "data", businessError.data, COUNT_OF(businessError.data));
    return error;
}

static std::string ConvertVectorToStr(std::vector<std::string> &strVector)
{
    std::string strValue;
    for (auto &str : strVector) {
        if (!strValue.empty()) {
            strValue.append(", ");
        }
        strValue.append(str);
    }
    return strValue;
}

void ClientHelper::NapiThrowParamError(
    napi_env env, std::vector<std::string> &paramNames, std::vector<std::string> &paramTypes)
{
    BusinessError businessError;
    CallResult errCode = CallResult::PARAM_ERR;
    std::string errMsg = "BusinessError " + std::to_string(static_cast<int32_t>(errCode))
        .append(": Parameter error. The type of { ").append(ConvertVectorToStr(paramNames)).append(" }")
        .append("must be { ").append(ConvertVectorToStr(paramTypes)).append(" }.");
    businessError.Build(errCode, errMsg);
    napi_value msg = BuildThrowError(env, businessError);
    napi_status status = napi_throw(env, msg);
    PARAM_CHECK(status == napi_ok, return, "Failed to napi_throw %d", static_cast<int32_t>(status));
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
