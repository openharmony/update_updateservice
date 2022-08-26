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
#include <string>
#include <string_ex.h>
#include "parcel.h"
#include "message_parcel.h"
#include "hilog/log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace UpdateEngine {
const int CALL_RESULT_OFFSET = 2000;

#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

#define CAST_INT(enumClass) (static_cast<int32_t>(enumClass))

enum class CallResult {
    SUCCESS = 0,
    FAIL = 100,
    UN_SUPPORT = 101,
    DEV_UPG_INFO_ERR = 102,
    FORBIDDEN = 103,
    IPC_ERR = 104,
    APP_NOT_GRANTED = 200,
    PARAM_ERR = 401,
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
constexpr int32_t INT_PARAM_ERR = CAST_INT(CallResult::PARAM_ERR);

// 搜索状态
enum SearchStatus {
    NET_ERROR = -2,
    SYSTEM_ERROR,
    HAS_NEW_VERSION,
    NO_NEW_VERSION,
    SERVER_BUSY,
    CHECK_EXECUTE_ERR,
};

enum UpgradeStatus {
    UPDATE_STATE_INIT = 0,
    UPDATE_STATE_CHECK_VERSION_ON = 10,
    UPDATE_STATE_CHECK_VERSION_FAIL,
    UPDATE_STATE_CHECK_VERSION_SUCCESS,
    UPDATE_STATE_DOWNLOAD_ON = 20,
    UPDATE_STATE_DOWNLOAD_PAUSE,
    UPDATE_STATE_DOWNLOAD_CANCEL,
    UPDATE_STATE_DOWNLOAD_FAIL,
    UPDATE_STATE_DOWNLOAD_SUCCESS,
    UPDATE_STATE_VERIFY_ON = 30,
    UPDATE_STATE_VERIFY_FAIL,
    UPDATE_STATE_VERIFY_SUCCESS,
    UPDATE_STATE_PACKAGE_TRANS_ON = 70,
    UPDATE_STATE_PACKAGE_TRANS_FAIL,
    UPDATE_STATE_PACKAGE_TRANS_SUCCESS,
    UPDATE_STATE_INSTALL_ON = 80,
    UPDATE_STATE_INSTALL_FAIL,
    UPDATE_STATE_INSTALL_SUCCESS,
    UPDATE_STATE_UPDATE_ON = 90,
    UPDATE_STATE_UPDATE_FAIL,
    UPDATE_STATE_UPDATE_SUCCESS
};

enum PackageType {
    PACKAGE_TYPE_DYNAMIC = 0,
    PACKAGE_TYPE_NORMAL = 1,
    PACKAGE_TYPE_BASE = 2,
    PACKAGE_TYPE_CUST = 3,
    PACKAGE_TYPE_PRELOAD = 4,
    PACKAGE_TYPE_COTA = 5,
    PACKAGE_TYPE_VERSION = 6,
    PACKAGE_TYPE_PATCH = 7
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
    CELLULAR = 1,
    METERED_WIFI = 2,
    NOT_METERED_WIFI = 4,
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
    FIRMWARE = 1
};

enum class DescriptionType {
    CONTENT = 0,
    URI = 1
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
    EVENT_UPGRADE_FAIL
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

struct BusinessType {
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
};

const std::string LOCAL_UPGRADE_INFO = "LocalUpgradeInfo";
struct UpgradeInfo {
    std::string upgradeApp;
    BusinessType businessType;
    std::string upgradeDevId;
    std::string controlDevId;

    UpgradeInfo &operator=(const UpgradeInfo &source)
    {
        if (&source != this) {
            upgradeApp = source.upgradeApp;
            businessType = source.businessType;
            upgradeDevId = source.upgradeDevId;
            controlDevId = source.controlDevId;
        }
        return *this;
    }

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

struct VersionDigestInfo {
    std::string versionDigest;
};

struct ErrorMessage {
    int32_t errorCode = 0;
    std::string errorMessage;
};

struct DescriptionOptions {
    DescriptionFormat format;
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
    UpgradeStatus status = UpgradeStatus::UPDATE_STATE_INIT;
};

struct DescriptionInfo {
    DescriptionType descriptionType = DescriptionType::CONTENT;
    std::string content;
};

struct ComponentDescription {
    std::string componentId;
    DescriptionInfo descriptionInfo;

    ComponentDescription &operator=(const ComponentDescription &source)
    {
        if (&source != this) {
            componentId = source.componentId;
            descriptionInfo = source.descriptionInfo;
        }
        return *this;
    }
};

struct CheckResult {
    size_t size;
    PackageType packageType;
    std::string versionName;
    std::string versionCode;
    std::string verifyInfo;
    std::string descriptPackageId;
};

struct VersionInfo {
    SearchStatus status;
    std::string errMsg;
    CheckResult result[2];
    DescriptionInfo descriptInfo[2];

    VersionInfo &operator=(const VersionInfo &source)
    {
        if (&source != this) {
            status = source.status;
            errMsg = source.errMsg;
            result[0] = source.result[0];
            result[1] = source.result[1];
            descriptInfo[0] = source.descriptInfo[0];
            descriptInfo[1] = source.descriptInfo[1];
        }
        return *this;
    }
};

struct VersionComponent {
    std::string componentId;
    uint32_t componentType = static_cast<uint32_t>(ComponentType::INVALID);
    std::string upgradeAction;
    std::string displayVersion;
    std::string innerVersion;
    size_t size;
    size_t effectiveMode;
    DescriptionInfo descriptionInfo;

    VersionComponent &operator=(const VersionComponent &source)
    {
        if (&source != this) {
            componentId = source.componentId;
            componentType = source.componentType;
            upgradeAction = source.upgradeAction;
            displayVersion = source.displayVersion;
            innerVersion = source.innerVersion;
            size = source.size;
            effectiveMode = source.effectiveMode;
            descriptionInfo = source.descriptionInfo;
        }
        return *this;
    }
};

struct CurrentVersionInfo {
    std::string osVersion;
    std::string deviceName;
    VersionComponent versionComponents[2];

    CurrentVersionInfo &operator=(const CurrentVersionInfo &source)
    {
        if (&source != this) {
            osVersion = source.osVersion;
            deviceName = source.deviceName;
            versionComponents[0] = source.versionComponents[0];
            versionComponents[1] = source.versionComponents[1];
        }
        return *this;
    }
};

struct NewVersionInfo {
    VersionDigestInfo versionDigestInfo;
    VersionComponent versionComponents[2];

    NewVersionInfo &operator=(const NewVersionInfo &source)
    {
        if (&source != this) {
            versionDigestInfo = source.versionDigestInfo;
            versionComponents[0] = source.versionComponents[0];
            versionComponents[1] = source.versionComponents[1];
        }
        return *this;
    }
};

struct VersionDescriptionInfo {
    ComponentDescription componentDescriptions[2];

    VersionDescriptionInfo &operator=(const VersionDescriptionInfo &source)
    {
        if (&source != this) {
            componentDescriptions[0] = source.componentDescriptions[0];
            componentDescriptions[1] = source.componentDescriptions[1];
        }
        return *this;
    }
};

struct CheckResultEx {
    bool isExistNewVersion;
    NewVersionInfo newVersionInfo;

    CheckResultEx &operator=(const CheckResultEx &source)
    {
        if (&source != this) {
            isExistNewVersion = source.isExistNewVersion;
            newVersionInfo = source.newVersionInfo;
        }
        return *this;
    }
};

struct TaskBody {
    VersionDigestInfo versionDigestInfo;
    UpgradeStatus status = UPDATE_STATE_INIT;
    int32_t subStatus;
    uint32_t progress;
    int32_t installMode;
    ErrorMessage errorMessages[2];
    VersionComponent versionComponents[2];

    TaskBody &operator=(const TaskBody &source)
    {
        if (&source != this) {
            versionDigestInfo = source.versionDigestInfo;
            status = source.status;
            subStatus = source.subStatus;
            progress = source.progress;
            installMode = source.installMode;
            errorMessages[0] = source.errorMessages[0];
            errorMessages[1] = source.errorMessages[1];
            versionComponents[0] = source.versionComponents[0];
            versionComponents[1] = source.versionComponents[1];
        }
        return *this;
    }
};

struct TaskInfo {
    bool existTask;
    TaskBody taskBody;

    TaskInfo &operator=(const TaskInfo &source)
    {
        if (&source != this) {
            existTask = source.existTask;
            taskBody = source.taskBody;
        }
        return *this;
    }
};

struct Progress {
    uint32_t percent = 0;
    UpgradeStatus status;
    std::string endReason;
};

struct ErrMsg {
    int32_t errorCode = 0;
    std::string errorMsg;

    ErrMsg &operator=(const ErrMsg &source)
    {
        if (&source != this) {
            errorCode = source.errorCode;
            errorMsg = source.errorMsg;
        }
        return *this;
    }
};

struct OtaStatus {
    uint32_t progress;
    UpgradeStatus status;
    int32_t subStatus;
    ErrMsg errMsg[1];

    OtaStatus &operator=(const OtaStatus &source)
    {
        if (&source != this) {
            progress = source.progress;
            status = source.status;
            subStatus = source.subStatus;
            errMsg[0] = source.errMsg[0];
        }
        return *this;
    }
};

struct UpgradeInterval {
    uint64_t timeStart;
    uint64_t timeEnd;
};

enum InstallMode {
    INSTALLMODE_NORMAL = 0,
    INSTALLMODE_NIGHT,
    INSTALLMODE_AUTO
};

enum AutoUpgradeCondition {
    AUTOUPGRADECONDITION_IDLE = 0,
};

struct BusinessError {
    std::string message;
    CallResult errorNum = CallResult::SUCCESS;
    ErrorMessage data[2];

    BusinessError &operator=(const BusinessError &source)
    {
        if (&source != this) {
            message = source.message;
            errorNum = source.errorNum;
            data[0] = source.data[0];
            data[1] = source.data[1];
        }
        return *this;
    }

    BusinessError &Build(CallResult callResult, const std::string &msg)
    {
        errorNum = callResult;
        message = msg;
        return *this;
    }
};

struct UpgradePeriod {
    uint32_t start;
    uint32_t end;
};

struct UpgradePolicy {
    bool downloadStrategy;
    bool autoUpgradeStrategy;
    UpgradePeriod autoUpgradePeriods[2];

    UpgradePolicy &operator=(const UpgradePolicy &source)
    {
        if (&source != this) {
            downloadStrategy = source.downloadStrategy;
            autoUpgradeStrategy = source.autoUpgradeStrategy;
            autoUpgradePeriods[0] = source.autoUpgradePeriods[0];
            autoUpgradePeriods[1] = source.autoUpgradePeriods[1];
        }
        return *this;
    }
};

struct UpgradeFile {
    ComponentType fileType;
    std::string filePath;
};

struct EventClassifyInfo {
    EventClassify eventClassify;
    std::string extraInfo;

    EventClassifyInfo() : eventClassify(EventClassify::TASK) {}
    EventClassifyInfo(EventClassify classify) : eventClassify(classify) {}
    EventClassifyInfo(EventClassify classify, const std::string &info) : eventClassify(classify), extraInfo(info) {}
};

struct EventInfo {
    EventId eventId;
    TaskBody taskBody;

    EventInfo() {}
    EventInfo(EventId id, TaskBody body) : eventId(id), taskBody(body) {}

    EventInfo &operator=(const EventInfo &source)
    {
        if (&source != this) {
            eventId = source.eventId;
            taskBody = source.taskBody;
        }
        return *this;
    }
};

using CheckNewVersionDone = std::function<void(const BusinessError &businessError, const CheckResultEx &checkResultEx)>;
using OnEvent = std::function<void(const EventInfo &eventInfo)>;

// 回调函数
struct UpdateCallbackInfo {
    CheckNewVersionDone checkNewVersionDone;
    OnEvent onEvent;

    UpdateCallbackInfo &operator=(const UpdateCallbackInfo &source)
    {
        if (&source != this) {
            checkNewVersionDone = source.checkNewVersionDone;
            onEvent = source.onEvent;
        }
        return *this;
    }
};

#ifdef UPDATE_SERVICE
static constexpr OHOS::HiviewDFX::HiLogLabel UPDATE_LABEL = {LOG_CORE, 0XD002E00, "UPDATE_SA"};
#else
static constexpr OHOS::HiviewDFX::HiLogLabel UPDATE_LABEL = {LOG_CORE, 0XD002E00, "UPDATE_KITS"};
#endif

enum class UpdateLogLevel {
    UPDATE_DEBUG = 0,
    UPDATE_INFO,
    UPDATE_WARN,
    UPDATE_ERROR,
    UPDATE_FATAL
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
    static int32_t ReadUpgradeInfo(MessageParcel &reply, UpgradeInfo &info);
    static int32_t WriteUpgradeInfo(MessageParcel &data, const UpgradeInfo &info);

    static int32_t ReadVersionDescriptionInfo(MessageParcel &reply,
        VersionDescriptionInfo &newVersionDescriptionInfo);
    static int32_t WriteVersionDescriptionInfo(MessageParcel &data,
        const VersionDescriptionInfo &newVersionDescriptionInfo);

    static int32_t ReadBusinessError(MessageParcel &reply, BusinessError &businessError);
    static int32_t WriteBusinessError(MessageParcel &data, const BusinessError &businessError);

    static int32_t ReadCheckResult(MessageParcel &reply, CheckResultEx &checkResultEx);
    static int32_t WriteCheckResult(MessageParcel &data, const CheckResultEx &checkResultEx);

    static int32_t ReadNewVersionInfo(MessageParcel &reply, NewVersionInfo &newVersionInfo);
    static int32_t WriteNewVersionInfo(MessageParcel &data, const NewVersionInfo &newVersionInfo);

    static int32_t ReadCurrentVersionInfo(MessageParcel &reply, CurrentVersionInfo &info);
    static int32_t WriteCurrentVersionInfo(MessageParcel &data, const CurrentVersionInfo &info);

    static int32_t ReadTaskInfo(MessageParcel &reply, TaskInfo &info);
    static int32_t WriteTaskInfo(MessageParcel &data, const TaskInfo &info);

    static int32_t ReadUpgradePolicy(MessageParcel &reply, UpgradePolicy &policy);
    static int32_t WriteUpgradePolicy(MessageParcel &data, const UpgradePolicy &policy);

    static int32_t ReadEventInfo(MessageParcel &reply, EventInfo &eventInfo);
    static int32_t WriteEventInfo(MessageParcel &data, const EventInfo &eventInfo);

    static int32_t ReadVersionDigestInfo(MessageParcel &reply, VersionDigestInfo &versionDigestInfo);
    static int32_t WriteVersionDigestInfo(MessageParcel &data, const VersionDigestInfo &versionDigestInfo);

    static int32_t ReadDescriptionOptions(MessageParcel &reply, DescriptionOptions &descriptionOptions);
    static int32_t WriteDescriptionOptions(MessageParcel &data, const DescriptionOptions &descriptionOptions);

    static int32_t ReadDownloadOptions(MessageParcel &reply, DownloadOptions &downloadOptions);
    static int32_t WriteDownloadOptions(MessageParcel &data, const DownloadOptions &downloadOptions);

    static int32_t ReadPauseDownloadOptions(MessageParcel &reply, PauseDownloadOptions &pauseDownloadOptions);
    static int32_t WritePauseDownloadOptions(MessageParcel &data, const PauseDownloadOptions &pauseDownloadOptions);

    static int32_t ReadResumeDownloadOptions(MessageParcel &reply, ResumeDownloadOptions &resumeDownloadOptions);
    static int32_t WriteResumeDownloadOptions(MessageParcel &data, const ResumeDownloadOptions &resumeDownloadOptions);

    static int32_t ReadUpgradeOptions(MessageParcel &reply, UpgradeOptions &upgradeOptions);
    static int32_t WriteUpgradeOptions(MessageParcel &data, const UpgradeOptions &upgradeOptions);

    static int32_t ReadClearOptions(MessageParcel &reply, ClearOptions &clearOptions);
    static int32_t WriteClearOptions(MessageParcel &data, const ClearOptions &clearOptions);

    static std::vector<uint8_t> HexToDegist(const std::string &str);
    static int32_t CompareVersion(const std::string &version1, const std::string &version2);
    static std::vector<std::string> SplitString(const std::string &str, const std::string &delimiter);

    static bool IsErrorExist(const BusinessError &businessError);

    static std::string BuildEventDevId(const UpgradeInfo &info);
    static std::string BuildEventVersionInfo(const VersionInfo &ver);

    static std::string Anonymization(const std::string &src);

    static bool JudgeLevel(const UpdateLogLevel& level);

    static void SetLogLevel(const UpdateLogLevel& level)
    {
        level_ = level;
    }

    static const UpdateLogLevel& GetLogLevel()
    {
        return level_;
    }

    static std::string GetBriefFileName(const std::string &file);

private:
    static UpdateLogLevel level_;
};

// 暂时记录两边日志
#define PRINT_LOG(LEVEL, Level, fmt, ...) \
    if (UpdateHelper::JudgeLevel(UpdateLogLevel::LEVEL)) \
        OHOS::HiviewDFX::HiLog::Level(UPDATE_LABEL, "[%{public}s(%{public}d)] " fmt, \
        UpdateHelper::GetBriefFileName(std::string(__FILE__)).c_str(), __LINE__, ##__VA_ARGS__)

#define ENGINE_LOGI(fmt, ...) PRINT_LOG(UPDATE_INFO, Info, fmt, ##__VA_ARGS__)
#define ENGINE_LOGE(fmt, ...) PRINT_LOG(UPDATE_ERROR, Error, fmt, ##__VA_ARGS__)

#define ENGINE_CHECK(retCode, exper, ...) \
    do { \
        if (!(retCode)) {                     \
            ENGINE_LOGE(__VA_ARGS__);         \
            exper;                            \
        } \
    } while (0)
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_HELPER_H
