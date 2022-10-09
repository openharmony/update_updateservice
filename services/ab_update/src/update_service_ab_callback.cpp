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
    sptr<UpdateService> service = UpdateService::GetInstance();
    if (service == nullptr) {
        ENGINE_LOGI("UpdateServiceAbCallback UpdateService no instance");
        return;
    }
    upgradeCallback_ = service->GetUpgradeCallback(info);
    if (upgradeCallback_ == nullptr) {
        ENGINE_LOGE("UpdateServiceAbCallback upgradeCallback_ is null");
        return;
    }
}

void UpdateServiceAbCallback::OnUpgradeProgress(int updateStatus, int percent)
{
    ENGINE_LOGI("UpdateServiceAbCallback OnUpgradeProgress progress %d percent %d", updateStatus, percent);
    switch (updateStatus) {
        case OHOS::SysInstaller::UPDATE_STATE_FAILED:
            eventInfo_.eventId = EventId::EVENT_UPGRADE_FAIL;
            break;
        default:
            eventInfo_.eventId = EventId::EVENT_UPGRADE_UPDATE;
            break;
    }
    eventInfo_.taskBody.progress = percent;
    if (upgradeCallback_ == nullptr) {
        ENGINE_LOGE("OnUpgradeProgress upgradeCallback_ is null");
        return;
    }
    upgradeCallback_->OnEvent(eventInfo_);
}

void UpdateServiceAbCallback::InitEventInfo()
{
    eventInfo_.taskBody.installMode = INSTALLMODE_NORMAL;
    eventInfo_.taskBody.status = UPDATE_STATE_UPDATE_ON;
    eventInfo_.taskBody.subStatus = 0;
    eventInfo_.taskBody.versionDigestInfo.versionDigest = "versionDigest";
}
} // namespace UpdateEngine
} // namespace OHOS
