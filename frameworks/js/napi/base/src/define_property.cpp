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

#include "define_property.h"

#include "napi/native_common.h"

#include "client_helper.h"
#include "napi_util.h"
#include "string_utils.h"
#include "update_define.h"
#include "update_helper.h"

#define DECLARE_ENUM_PROPERTY(item)      \
    {StringUtils::GetEnumValueString(#item), NapiUtil::CreateUint32(env, CAST_UINT((item)))}

#define DECLARE_STRING_PROPERTY(item)    \
    {StringUtils::GetEnumValueString(#item), NapiUtil::CreateStringUtf8(env, (item))}

#define DECLARE_CALL_RESULT_PROPERTY(item)      \
    {StringUtils::GetEnumValueString(#item),    \
        NapiUtil::CreateUint32(env, CAST_UINT(ClientHelper::ConvertToErrorCode(item)))}

namespace OHOS {
namespace UpdateEngine {
void DefineProperty::DefineProperties(napi_env env, napi_value exports)
{
    DefineCallResult(env, exports);
    DefineUpgradeStatus(env, exports);
    DefineComponentType(env, exports);
    DefineEffectiveMode(env, exports);
    DefineNetType(env, exports);
    DefineOrder(env, exports);
    DefineEventClassify(env, exports);
    DefineBusinessSubType(env, exports);
    DefineDescriptionType(env, exports);
    DefineDescriptionFormat(env, exports);
    DefineEventId(env, exports);
    DefineUpgradeAction(env, exports);
    DefineBusinessVendor(env, exports);
}

void DefineProperty::DefineCallResult(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> callResultList {
        DECLARE_CALL_RESULT_PROPERTY(CallResult::APP_NOT_GRANTED),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::PARAM_ERR),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::UN_SUPPORT),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::SUCCESS),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::FAIL),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::DEV_UPG_INFO_ERR),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::FORBIDDEN),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::IPC_ERR),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::TIME_OUT),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::DB_ERROR),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::IO_ERROR),
        DECLARE_CALL_RESULT_PROPERTY(CallResult::NET_ERROR)
    };
    NapiUtil::CreateProperty(env, exports, "CallResult", callResultList);
}

void DefineProperty::DefineUpgradeStatus(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> upgradeStatusList = {
        DECLARE_ENUM_PROPERTY(UpgradeStatus::INIT),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::CHECKING_VERSION),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::CHECK_VERSION_FAIL),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::CHECK_VERSION_SUCCESS),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::DOWNLOADING),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::DOWNLOAD_PAUSE),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::DOWNLOAD_CANCEL),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::DOWNLOAD_FAIL),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::DOWNLOAD_SUCCESS),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::VERIFYING),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::VERIFY_FAIL),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::VERIFY_SUCCESS),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::PACKAGE_TRANSING),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::PACKAGE_TRANS_FAIL),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::PACKAGE_TRANS_SUCCESS),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::INSTALLING),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::INSTALL_FAIL),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::INSTALL_SUCCESS),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::UPDATING),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::UPDATE_FAIL),
        DECLARE_ENUM_PROPERTY(UpgradeStatus::UPDATE_SUCCESS)
    };
    NapiUtil::CreateProperty(env, exports, "UpgradeStatus", upgradeStatusList);
}

void DefineProperty::DefineComponentType(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> componentTypeList = {
        DECLARE_ENUM_PROPERTY(ComponentType::OTA),
    };
    NapiUtil::CreateProperty(env, exports, "ComponentType", componentTypeList);
}

void DefineProperty::DefineEffectiveMode(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> effectiveModeList = {
        DECLARE_ENUM_PROPERTY(EffectiveMode::COLD),
        DECLARE_ENUM_PROPERTY(EffectiveMode::LIVE),
        DECLARE_ENUM_PROPERTY(EffectiveMode::LIVE_AND_COLD)
    };
    NapiUtil::CreateProperty(env, exports, "EffectiveMode", effectiveModeList);
}

void DefineProperty::DefineNetType(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> netTypeList = {
        DECLARE_ENUM_PROPERTY(NetType::CELLULAR),
        DECLARE_ENUM_PROPERTY(NetType::METERED_WIFI),
        DECLARE_ENUM_PROPERTY(NetType::NOT_METERED_WIFI),
        DECLARE_ENUM_PROPERTY(NetType::WIFI),
        DECLARE_ENUM_PROPERTY(NetType::CELLULAR_AND_WIFI)
    };
    NapiUtil::CreateProperty(env, exports, "NetType", netTypeList);
}

void DefineProperty::DefineOrder(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> orderList = {
        DECLARE_ENUM_PROPERTY(Order::DOWNLOAD),
        DECLARE_ENUM_PROPERTY(Order::INSTALL),
        DECLARE_ENUM_PROPERTY(Order::DOWNLOAD_AND_INSTALL),
        DECLARE_ENUM_PROPERTY(Order::APPLY),
        DECLARE_ENUM_PROPERTY(Order::INSTALL_AND_APPLY)
    };
    NapiUtil::CreateProperty(env, exports, "Order", orderList);
}

void DefineProperty::DefineEventClassify(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> eventClassifyList = {
        DECLARE_ENUM_PROPERTY(EventClassify::TASK)
    };
    NapiUtil::CreateProperty(env, exports, "EventClassify", eventClassifyList);
}

void DefineProperty::DefineBusinessSubType(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> businessSubTypeList = {
        DECLARE_ENUM_PROPERTY(BusinessSubType::FIRMWARE),
        DECLARE_ENUM_PROPERTY(BusinessSubType::PARAM)
    };
    NapiUtil::CreateProperty(env, exports, "BusinessSubType", businessSubTypeList);
}

void DefineProperty::DefineDescriptionType(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> descriptionTypeList = {
        DECLARE_ENUM_PROPERTY(DescriptionType::CONTENT),
        DECLARE_ENUM_PROPERTY(DescriptionType::URI),
        DECLARE_ENUM_PROPERTY(DescriptionType::ID)
    };
    NapiUtil::CreateProperty(env, exports, "DescriptionType", descriptionTypeList);
}

void DefineProperty::DefineDescriptionFormat(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> descriptionFormatList = {
        DECLARE_ENUM_PROPERTY(DescriptionFormat::STANDARD),
        DECLARE_ENUM_PROPERTY(DescriptionFormat::SIMPLIFIED)
    };
    NapiUtil::CreateProperty(env, exports, "DescriptionFormat", descriptionFormatList);
}

void DefineProperty::DefineEventId(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> eventIdList = {
        DECLARE_ENUM_PROPERTY(EventId::EVENT_TASK_BASE),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_TASK_RECEIVE),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_TASK_CANCEL),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_WAIT),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_START),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_UPDATE),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_PAUSE),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_RESUME),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_SUCCESS),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_FAIL),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_UPGRADE_WAIT),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_UPGRADE_START),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_UPGRADE_UPDATE),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_APPLY_WAIT),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_APPLY_START),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_UPGRADE_SUCCESS),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_UPGRADE_FAIL),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_AUTH_START),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_AUTH_SUCCESS),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_DOWNLOAD_CANCEL),
        DECLARE_ENUM_PROPERTY(EventId::EVENT_INITIALIZE)
    };
    NapiUtil::CreateProperty(env, exports, "EventId", eventIdList);
}

void DefineProperty::DefineUpgradeAction(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> upgradeActionList = {
        DECLARE_STRING_PROPERTY(UpgradeAction::UPGRADE),
        DECLARE_STRING_PROPERTY(UpgradeAction::RECOVERY)
    };
    NapiUtil::CreateProperty(env, exports, "UpgradeAction", upgradeActionList);
}

void DefineProperty::DefineBusinessVendor(napi_env env, napi_value exports)
{
    std::vector<std::pair<std::string, napi_value>> businessVendorList = {
        DECLARE_STRING_PROPERTY(BusinessVendor::PUBLIC),
    };
    NapiUtil::CreateProperty(env, exports, "BusinessVendor", businessVendorList);
}
} // namespace UpdateEngine
} // namespace OHOS