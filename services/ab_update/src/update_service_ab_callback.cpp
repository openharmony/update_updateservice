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

#include "update_service_ab_callback.h"

namespace OHOS {
namespace UpdateEngine {
UpdateServiceAbCallback::UpdateServiceAbCallback(const UpgradeInfo &info)
{
    InitEventInfo();
    info_ = info;
}

void UpdateServiceAbCallback::OnUpgradeProgress(int updateStatus, int percent)
{
    ENGINE_LOGI("UpdateServiceAbCallback OnUpgradeProgress progress %d percent %d", updateStatus, percent);
    switch (updateStatus) {
        case OHOS::SysInstaller::UPDATE_STATE_INIT:
        case OHOS::SysInstaller::UPDATE_STATE_ONGOING:
            eventInfo_.taskBody.status = UPDATE_STATE_UPDATE_ON;
            eventInfo_.eventId = EventId::EVENT_UPGRADE_UPDATE;
            break;
        case OHOS::SysInstaller::UPDATE_STATE_SUCCESSFUL:
            eventInfo_.taskBody.status = UPDATE_STATE_UPDATE_SUCCESS;
            eventInfo_.eventId = EventId::EVENT_UPGRADE_UPDATE;
            break;
        case OHOS::SysInstaller::UPDATE_STATE_FAILED:
            eventInfo_.taskBody.status = UPDATE_STATE_UPDATE_FAIL;
            eventInfo_.eventId = EventId::EVENT_UPGRADE_FAIL;
            break;
        default:
            ENGINE_LOGE("updateStatus invalid, is %d", updateStatus);
            return;
    }
    eventInfo_.taskBody.progress = percent;
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service == nullptr) {
        ENGINE_LOGE("UpdateServiceAbCallback OnUpgradeProgress UpdateService no instance");
        return;
    }
    sptr<IUpdateCallback> upgradeCallback = service->GetUpgradeCallback(info_);
    if (upgradeCallback == nullptr) {
        ENGINE_LOGE("UpdateServiceAbCallback OnUpgradeProgress upgradeCallback_ is null");
        return;
    }
    upgradeCallback->OnEvent(eventInfo_);
}

void UpdateServiceAbCallback::InitEventInfo()
{
    eventInfo_.taskBody.installMode = INSTALLMODE_NORMAL;
    eventInfo_.taskBody.subStatus = 0;
    eventInfo_.taskBody.versionDigestInfo.versionDigest = "versionDigest";
}
} // namespace UpdateEngine
} // namespace OHOS
