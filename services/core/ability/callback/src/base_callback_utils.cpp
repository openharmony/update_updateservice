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

#include "base_callback_utils.h"

#include "iupdate_callback.h"
#include "update_define.h"
#include "update_helper.h"
#include "update_log.h"
#include "update_notify.h"
#include "update_service.h"
#include "update_service_cache.h"
#include "update_service_util.h"

namespace OHOS {
namespace UpdateEngine {
void BaseCallbackUtils::NotifyEvent(const std::string &versionDigestInfo, EventId eventId, UpgradeStatus status,
    const ErrorMessage &errorMessage, const std::vector<VersionComponent> &versionComponents)
{
    TaskBody taskBody;
    taskBody.status = status;
    taskBody.versionDigestInfo.versionDigest = versionDigestInfo;
    taskBody.errorMessages.push_back(errorMessage);
    taskBody.versionComponents = versionComponents;
    EventInfo info(eventId, taskBody);
    NotifyToHap(info);
}

void BaseCallbackUtils::ProgressCallback(const std::string &versionDigestInfo, const Progress &progress)
{
    ENGINE_LOGI("ProgressCallback progress versionDigestInfo %{public}s status:%{public}d",
        versionDigestInfo.c_str(), progress.status);
    static std::map<UpgradeStatus, EventId> statusMap = {
        {UpgradeStatus::DOWNLOADING, EventId::EVENT_DOWNLOAD_UPDATE},
        {UpgradeStatus::DOWNLOAD_PAUSE, EventId::EVENT_DOWNLOAD_PAUSE},
        {UpgradeStatus::DOWNLOAD_CANCEL, EventId::EVENT_DOWNLOAD_CANCEL},
        {UpgradeStatus::DOWNLOAD_FAIL, EventId::EVENT_DOWNLOAD_FAIL},
        {UpgradeStatus::VERIFY_FAIL, EventId::EVENT_DOWNLOAD_FAIL},
        {UpgradeStatus::DOWNLOAD_SUCCESS, EventId::EVENT_DOWNLOAD_SUCCESS},
        {UpgradeStatus::INSTALLING, EventId::EVENT_UPGRADE_UPDATE},
        {UpgradeStatus::INSTALL_FAIL, EventId::EVENT_UPGRADE_FAIL},
        {UpgradeStatus::INSTALL_SUCCESS, EventId::EVENT_APPLY_WAIT},
        {UpgradeStatus::UPDATING, EventId::EVENT_UPGRADE_UPDATE},
        {UpgradeStatus::UPDATE_FAIL, EventId::EVENT_UPGRADE_FAIL},
        {UpgradeStatus::UPDATE_SUCCESS, EventId::EVENT_UPGRADE_SUCCESS},
    };
    EventId eventId = EventId::EVENT_TASK_BASE;
    for (auto iter = statusMap.begin(); iter != statusMap.end(); iter++) {
        if (iter->first == progress.status) {
            eventId = iter->second;
            break;
        }
    }

    TaskBody taskBody;
    taskBody.versionDigestInfo.versionDigest = versionDigestInfo;
    UpdateServiceUtil::BuildTaskBody(progress, taskBody);

    EventInfo eventInfo(eventId, taskBody);
    CallbackToHap(eventInfo);
}

void BaseCallbackUtils::CallbackToHap(EventInfo &eventInfo)
{
    UpgradeInfo upgradeInfo = UpdateServiceCache::GetUpgradeInfo(GetBusinessSubType());
    auto upgradeCallback = GetUpgradeCallback(upgradeInfo);
    if (upgradeCallback != nullptr) {
        ENGINE_LOGD("CallbackToHap upgradeCallback eventInfoStr %{public}s", eventInfo.ToJson().c_str());
        upgradeCallback->OnEvent(eventInfo);
    } else {
        NotifyToHap(eventInfo);
    }
}

sptr<IUpdateCallback> BaseCallbackUtils::GetUpgradeCallback(const UpgradeInfo &upgradeInfo)
{
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service == nullptr) {
        ENGINE_LOGI("GetUpgradeCallback no instance");
        return nullptr;
    }
    return service->GetUpgradeCallback(upgradeInfo);
}

void BaseCallbackUtils::NotifyToHap(EventInfo &info)
{
    std::string eventInfoStr = info.ToJson();
    ENGINE_LOGI("Notify eventInfoStr %{public}s", eventInfoStr.c_str());
    if (!eventInfoStr.empty()) {
        SubscribeInfo subscribeInfo{GetBusinessSubType()};
        OHOS::UpdateEngine::UpdateNotify::NotifyToAppService(eventInfoStr, subscribeInfo.ToJson());
    }
}
} // namespace UpdateEngine
} // namespace OHOS
