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

#ifndef UPDATE_HELPER_H
#define UPDATE_HELPER_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ipc_types.h>
#include <list>
#include <map>
#include <string>
#include <string_ex.h>

#include "parcel.h"
#include "message_parcel.h"
#include "hilog/log.h"
#include "system_ability_definition.h"

#include "json_builder.h"
#include "update_define.h"

namespace OHOS {
namespace UpdateEngine {
static const std::string OUC_PACKAGE_NAME = "com.ohos.updateapp";
static const std::string OUC_SERVICE_EXT_ABILITY_NAME = "ServiceExtAbility";
constexpr int CALL_RESULT_OFFSET = 2000;

enum class CallResult {
// 通用错误码
    APP_NOT_GRANTED = 201,
    NOT_SYSTEM_APP = 202,
    PARAM_ERR = 401,
    UN_SUPPORT = 801,

// 模块内错误码
    SUCCESS = 0,
    FAIL = 100,
    DEV_UPG_INFO_ERR = 102,
    FORBIDDEN = 103,
    IPC_ERR = 104,
    TIME_OUT = 402,
    DB_ERROR = 501,
    IO_ERROR = 502,
    NET_ERROR = 503
};

constexpr int32_t INT_CALL_SUCCESS = CAST_INT(CallResult::SUCCESS);
constexpr int32_t INT_CALL_FAIL = CAST_INT(CallResult::FAIL);
constexpr int32_t INT_UN_SUPPORT = CAST_INT(CallResult::UN_SUPPORT);
constexpr int32_t INT_FORBIDDEN = CAST_INT(CallResult::FORBIDDEN);
constexpr int32_t INT_CALL_IPC_ERR = CAST_INT(CallResult::IPC_ERR);
constexpr int32_t INT_APP_NOT_GRANTED = CAST_INT(CallResult::APP_NOT_GRANTED);
constexpr int32_t INT_NOT_SYSTEM_APP = CAST_INT(CallResult::NOT_SYSTEM_APP);
constexpr int32_t INT_PARAM_ERR = CAST_INT(CallResult::PARAM_ERR);
constexpr int32_t INT_DEV_UPG_INFO_ERR = CAST_INT(CallResult::DEV_UPG_INFO_ERR);
constexpr int32_t INT_TIME_OUT = CAST_INT(CallResult::TIME_OUT);
constexpr int32_t INT_DB_ERROR = CAST_INT(CallResult::DB_ERROR);
constexpr int32_t INT_IO_ERROR = CAST_INT(CallResult::IO_ERROR);
constexpr int32_t INT_NET_ERROR = CAST_INT(CallResult::NET_ERROR);

// 搜索状态
enum class SearchStatus {
    NET_ERROR = -2,
    SYSTEM_ERROR,
    HAS_NEW_VERSION,
    NO_NEW_VERSION,
    SERVER_BUSY,
    CHECK_EXECUTE_ERR,
};

enum class UpgradeStatus {
    INIT = 0,
    CHECKING_VERSION = 10,
    CHECK_VERSION_FAIL,
    CHECK_VERSION_SUCCESS,
    DOWNLOADING = 20,
    DOWNLOAD_PAUSE,
    DOWNLOAD_CANCEL,
    DOWNLOAD_FAIL,
    DOWNLOAD_SUCCESS,
    VERIFYING = 30,
    VERIFY_FAIL,
    VERIFY_SUCCESS,
    PACKAGE_TRANSING = 70,
    PACKAGE_TRANS_FAIL,
    PACKAGE_TRANS_SUCCESS,
    INSTALLING = 80,
    INSTALL_FAIL,
    INSTALL_SUCCESS,
    UPDATING = 90,
    UPDATE_FAIL,
    UPDATE_SUCCESS
};

enum class PackageType {
    DYNAMIC = 0,
    NORMAL = 1,
    BASE = 2,
    CUST = 3,
    PRELOAD = 4,
    COTA = 5,
    VERSION = 6,
    PATCH = 8
};

enum class ComponentType {
    INVALID = 0,
    OTA = 1,
    PATCH = 2,
    COTA = 4,
    PARAM = 8
};

enum class EffectiveMode {
    COLD = 1,
    LIVE = 2,
    LIVE_AND_COLD = 3
};

enum class NetType {
    NO_NET = 0,
    CELLULAR = 1,
    METERED_WIFI = 2,
    NOT_METERED_WIFI = 4,
    CELLULAR_AND_METERED_WIFI = CELLULAR | METERED_WIFI,
    CELLULAR_AND_NOT_METERED_WIFI = CELLULAR | NOT_METERED_WIFI,
    WIFI = METERED_WIFI | NOT_METERED_WIFI,
    CELLULAR_AND_WIFI = CELLULAR | WIFI
};

enum class Order {
    DOWNLOAD = 1,
    INSTALL = 2,
    DOWNLOAD_AND_INSTALL = DOWNLOAD | INSTALL,
    APPLY = 4,
    INSTALL_AND_APPLY = INSTALL | APPLY
};

enum class EventClassify {
    TASK = 0x01000000,
};

enum class BusinessSubType {
    FIRMWARE = 1,
    PARAM = 2
};

enum class DescriptionType {
    CONTENT = 0,
    URI = 1,
    ID = 2
};

enum class DescriptionFormat {
    STANDARD = 0,
    SIMPLIFIED = 1
};

enum class EventId {
    EVENT_TASK_BASE = 0x01000000,
    EVENT_TASK_RECEIVE,
    EVENT_TASK_CANCEL,
    EVENT_DOWNLOAD_WAIT,
    EVENT_DOWNLOAD_START,
    EVENT_DOWNLOAD_UPDATE,
    EVENT_DOWNLOAD_PAUSE,
    EVENT_DOWNLOAD_RESUME,
    EVENT_DOWNLOAD_SUCCESS,
    EVENT_DOWNLOAD_FAIL,
    EVENT_UPGRADE_WAIT,
    EVENT_UPGRADE_START,
    EVENT_UPGRADE_UPDATE,
    EVENT_APPLY_WAIT,
    EVENT_APPLY_START,
    EVENT_UPGRADE_SUCCESS,
    EVENT_UPGRADE_FAIL,
    EVENT_AUTH_START,
    EVENT_AUTH_SUCCESS,
    EVENT_DOWNLOAD_CANCEL,
    EVENT_INITIALIZE
};

enum TaskBodyMemeberMask {
    VERSION_DIGEST_INFO = 0x00000001,
    UPGRADE_STATUS      = 0x00000010,
    SUB_STATUS          = 0x00000100,
    PROGRESS            = 0x00001000,
    INSTALL_MODE        = 0x00010000,
    ERROR_MESSAGE       = 0x00100000,
    VERSION_COMPONENT   = 0x01000000
};

static std::map<EventId, uint32_t> g_taskBodyTemplateMap = {
    { EventId::EVENT_TASK_RECEIVE,     VERSION_DIGEST_INFO },
    { EventId::EVENT_TASK_CANCEL,      VERSION_DIGEST_INFO },
    { EventId::EVENT_DOWNLOAD_WAIT,    VERSION_DIGEST_INFO | UPGRADE_STATUS | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_START,   VERSION_DIGEST_INFO | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_UPDATE,  VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_PAUSE,   VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE | ERROR_MESSAGE },
    { EventId::EVENT_DOWNLOAD_RESUME,  VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_SUCCESS, VERSION_DIGEST_INFO | INSTALL_MODE },
    { EventId::EVENT_DOWNLOAD_CANCEL,  VERSION_DIGEST_INFO | UPGRADE_STATUS },
    { EventId::EVENT_DOWNLOAD_FAIL,    VERSION_DIGEST_INFO | INSTALL_MODE | ERROR_MESSAGE },
    { EventId::EVENT_UPGRADE_WAIT,     VERSION_DIGEST_INFO | UPGRADE_STATUS | INSTALL_MODE | ERROR_MESSAGE },
    { EventId::EVENT_UPGRADE_START,    VERSION_DIGEST_INFO | UPGRADE_STATUS | INSTALL_MODE },
    { EventId::EVENT_UPGRADE_UPDATE,   VERSION_DIGEST_INFO | UPGRADE_STATUS | PROGRESS | INSTALL_MODE },
    { EventId::EVENT_APPLY_WAIT,       VERSION_DIGEST_INFO | UPGRADE_STATUS | ERROR_MESSAGE },
    { EventId::EVENT_APPLY_START,      VERSION_DIGEST_INFO },
    { EventId::EVENT_UPGRADE_SUCCESS,  VERSION_DIGEST_INFO | VERSION_COMPONENT },
    { EventId::EVENT_UPGRADE_FAIL,     VERSION_DIGEST_INFO | VERSION_COMPONENT | ERROR_MESSAGE },
    { EventId::EVENT_AUTH_START,       VERSION_DIGEST_INFO | VERSION_COMPONENT | UPGRADE_STATUS},
    { EventId::EVENT_AUTH_SUCCESS,     VERSION_DIGEST_INFO | VERSION_COMPONENT | UPGRADE_STATUS },
    { EventId::EVENT_INITIALIZE,       UPGRADE_STATUS }
};

class UpgradeAction {
public:
    static constexpr const char *UPGRADE = "upgrade";
    static constexpr const char *RECOVERY = "recovery";
};

class BusinessVendor {
public:
    static constexpr const char *PUBLIC = "public";
};

struct BaseJsonStruct {
    virtual ~BaseJsonStruct() {}
    virtual JsonBuilder GetJsonBuilder() = 0;

    virtual std::string ToJson()
    {
        return GetJsonBuilder().ToJson();
    };
};

struct BusinessType : public BaseJsonStruct {
    std::string vendor; // BusinessVendor
    BusinessSubType subType = BusinessSubType::FIRMWARE;

    bool operator<(const BusinessType &businessType) const
    {
        if (vendor < businessType.vendor) return true;
        if (vendor > businessType.vendor) return false;

        if (CAST_INT(subType) < CAST_INT(businessType.subType)) return true;
        if (CAST_INT(subType) > CAST_INT(businessType.subType)) return false;

        return false;
    }

    JsonBuilder GetJsonBuilder() final;
};

const std::string LOCAL_UPGRADE_INFO = "LocalUpgradeInfo";
struct UpgradeInfo {
    std::string upgradeApp;
    BusinessType businessType;
    std::string upgradeDevId;
    std::string controlDevId;

    bool operator<(const UpgradeInfo &r) const
    {
        if (upgradeApp < r.upgradeApp) return true;
        if (upgradeApp > r.upgradeApp) return false;

        if (businessType < r.businessType) return true;
        if (r.businessType < businessType) return false;

        if (upgradeDevId < r.upgradeDevId) return true;
        if (upgradeDevId > r.upgradeDevId) return false;

        if (controlDevId < r.controlDevId) return true;
        if (controlDevId > r.controlDevId) return false;

        return false;
    }

    std::string ToString() const;

    bool IsLocal() const
    {
        return upgradeApp == LOCAL_UPGRADE_INFO;
    }
};

struct SubscribeInfo : public BaseJsonStruct {
    std::string upgradeApp = OUC_PACKAGE_NAME;
    BusinessType businessType;
    std::string abilityName;
    std::string subscriberDevId;
    std::string devUpgradeId;

    SubscribeInfo(BusinessSubType subType)
    {
        businessType.subType = subType;
    }

    JsonBuilder GetJsonBuilder() final;
};

struct VersionDigestInfo : public BaseJsonStruct {
    std::string versionDigest;

    JsonBuilder GetJsonBuilder() final;
};

struct ErrorMessage : public BaseJsonStruct {
    int32_t errorCode = 0;
    std::string errorMessage;

    JsonBuilder GetJsonBuilder() final;
};

struct DescriptionOptions {
    DescriptionFormat format = DescriptionFormat::STANDARD;
    std::string language;
};

struct DownloadOptions {
    NetType allowNetwork = NetType::WIFI;
    Order order = Order::DOWNLOAD;
};

struct ResumeDownloadOptions {
    NetType allowNetwork = NetType::WIFI;
};

struct PauseDownloadOptions {
    bool isAllowAutoResume = false;
};

struct UpgradeOptions {
    Order order = Order::INSTALL;
};

struct ClearOptions {
    UpgradeStatus status = UpgradeStatus::INIT;
};

enum class InstallMode {
    NORMAL = 0,
    NIGHT,
    AUTO
};

struct DescriptionInfo : public BaseJsonStruct {
    DescriptionType descriptionType = DescriptionType::CONTENT;
    std::string content;

    JsonBuilder GetJsonBuilder() final;
};

struct ComponentDescription {
    std::string componentId;
    DescriptionInfo descriptionInfo;
};

struct VersionComponent : public BaseJsonStruct {
    std::string componentId;
    int32_t componentType = CAST_INT(ComponentType::INVALID);
    std::string upgradeAction;
    std::string displayVersion;
    std::string innerVersion;
    size_t size = 0;
    size_t effectiveMode = static_cast<size_t>(EffectiveMode::COLD);
    DescriptionInfo descriptionInfo;
    std::string componentExtra;

    JsonBuilder GetJsonBuilder() final;
};

struct CurrentVersionInfo {
    std::string osVersion;
    std::string deviceName;
    std::vector<VersionComponent> versionComponents;
};

struct NewVersionInfo {
    VersionDigestInfo versionDigestInfo;
    std::vector<VersionComponent> versionComponents;
};

struct VersionDescriptionInfo {
    std::vector<ComponentDescription> componentDescriptions;
};

struct CheckResult {
    bool isExistNewVersion = false;
    NewVersionInfo newVersionInfo;
};

struct TaskBody {
    VersionDigestInfo versionDigestInfo;
    UpgradeStatus status = UpgradeStatus::INIT;
    int32_t subStatus = CAST_INT(UpgradeStatus::INIT);
    int32_t progress = 0;
    int32_t installMode = CAST_INT(InstallMode::NORMAL);
    std::vector<ErrorMessage> errorMessages;
    std::vector<VersionComponent> versionComponents;

    JsonBuilder GetJsonBuilder(EventId eventId);
};

struct TaskInfo {
    bool existTask;
    TaskBody taskBody;
};

struct Progress {
    uint32_t percent = 0;
    UpgradeStatus status = UpgradeStatus::INIT;
    std::string endReason;
};

struct ErrMsg {
    int32_t errorCode = 0;
    std::string errorMsg;
};

struct UpgradeInterval {
    uint64_t timeStart = 0;
    uint64_t timeEnd = 0;
};

struct BusinessError {
    std::string message;
    CallResult errorNum = CallResult::SUCCESS;
    std::vector<ErrorMessage> data;

    BusinessError &Build(CallResult callResult, const std::string &msg)
    {
        errorNum = callResult;
        message = msg;
        return *this;
    }

    BusinessError &AddErrorMessage(int32_t errorCode, const std::string &errorMessage)
    {
        ErrorMessage errMsg;
        errMsg.errorCode = errorCode;
        errMsg.errorMessage = errorMessage;
        data.push_back(errMsg);
        return *this;
    }
};

struct UpgradePeriod {
    uint32_t start = 0;
    uint32_t end = 0;
};

struct UpgradePolicy {
    bool downloadStrategy = false;
    bool autoUpgradeStrategy = false;
    UpgradePeriod autoUpgradePeriods[2];
};

struct UpgradeFile {
    ComponentType fileType = ComponentType::INVALID;
    std::string filePath;
};

struct EventClassifyInfo {
    EventClassify eventClassify = EventClassify::TASK;
    std::string extraInfo;

    EventClassifyInfo() : eventClassify(EventClassify::TASK) {}
    EventClassifyInfo(EventClassify classify) : eventClassify(classify) {}
    EventClassifyInfo(EventClassify classify, const std::string &info) : eventClassify(classify), extraInfo(info) {}
};

struct EventInfo : public BaseJsonStruct {
    EventId eventId = EventId::EVENT_TASK_BASE;
    TaskBody taskBody;

    EventInfo() {}
    EventInfo(EventId id, TaskBody body) : eventId(id), taskBody(body) {}

    JsonBuilder GetJsonBuilder() final;
};

struct ConfigInfo {
    std::string businessDomain;
    uint32_t abInstallTimeout = 1800; // 1800s
};

enum class AuthType {
    PKI = 1,
    WD = 2
};

using CheckNewVersionDone = std::function<void(const BusinessError &businessError, const CheckResult &checkResult)>;
using OnEvent = std::function<void(const EventInfo &eventInfo)>;

// 回调函数
struct UpdateCallbackInfo {
    CheckNewVersionDone checkNewVersionDone;
    OnEvent onEvent;
};

template<typename T>
bool IsValidEnum(const std::list<T> &enumList, int32_t number)
{
    for (auto i : enumList) {
        if (number == static_cast<int32_t>(i)) {
            return true;
        }
    }
    return false;
}

class UpdateHelper {
public:
    static std::string BuildEventDevId(const UpgradeInfo &info);
    static bool IsUpgradeFailed(UpgradeStatus status);
};

struct DirInfo {
    std::string dirName;
    int32_t dirPermissions;
    bool isAllowDestroyContents = false; // 是否允许删除当前目录下的所有文件（包括子目录）
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_HELPER_H
