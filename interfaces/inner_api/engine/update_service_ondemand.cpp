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

#include "update_service_ondemand.h"

namespace OHOS {
namespace UpdateEngine {
sptr<UpdateServiceOnDemand> UpdateServiceOnDemand::instance_ = nullptr;
std::mutex UpdateServiceOnDemand::instanceLock_;

sptr<UpdateServiceOnDemand> UpdateServiceOnDemand::GetInstance()
{
    instanceLock_.lock();
    if (instance_ == nullptr) {
        instance_ = new UpdateServiceOnDemand;
    }
    instanceLock_.unlock();
    return instance_;
}

bool UpdateServiceOnDemand::TryLoadUpdaterSa()
{
    InitStatus();
    return LoadUpdaterSa();
}

void UpdateServiceOnDemand::OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject>& remoteObject)
{
    ENGINE_LOGI("OnLoadSystemAbilitySuccess systemAbilityId: %{public}d, IRemoteObject result: %{public}s",
        systemAbilityId, (remoteObject != nullptr) ? "succeed" : "failed");
    loadUpdaterSaStatus_ = (remoteObject != nullptr) ? LoadUpdaterSaStatus::SUCCESS : LoadUpdaterSaStatus::FAIL;
}

void UpdateServiceOnDemand::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    ENGINE_LOGE("OnLoadSystemAbilityFail systemAbilityId: %{public}d", systemAbilityId);
    loadUpdaterSaStatus_ = LoadUpdaterSaStatus::FAIL;
}

void UpdateServiceOnDemand::InitStatus()
{
    if (loadUpdaterSaStatus_ != LoadUpdaterSaStatus::WAIT_RESULT) {
        loadUpdaterSaStatus_ = LoadUpdaterSaStatus::WAIT_RESULT;
    }
}

void UpdateServiceOnDemand::WaitUpdaterSaInit()
{
    usleep(UPDATER_SA_INIT_TIME);
}

bool UpdateServiceOnDemand::CheckUpdaterSaLoaded()
{
    int32_t retry = RETRY_TIMES;
    ENGINE_LOGI("Waiting for CheckUpdaterSaLoaded");
    while (retry--) {
        usleep(SLEEP_TIME);
        LoadUpdaterSaStatus loadUpdaterSaStatus = loadUpdaterSaStatus_;
        if (loadUpdaterSaStatus != LoadUpdaterSaStatus::WAIT_RESULT) {
            bool isUpdaterSaLoaded = loadUpdaterSaStatus == LoadUpdaterSaStatus::SUCCESS;
            ENGINE_LOGI("found OnLoad result: %{public}s", isUpdaterSaLoaded ? "succeed" : "failed");
            return isUpdaterSaLoaded;
        }
    }
    ENGINE_LOGE("CheckUpdaterSaLoaded didn't get OnLoad result");
    return false;
}

bool UpdateServiceOnDemand::LoadUpdaterSa()
{
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        ENGINE_LOGE("GetSystemAbilityManager samgr object null!");
        return false;
    }
    int32_t result = sm->LoadSystemAbility(UPDATE_DISTRIBUTED_SERVICE_ID, this);
    if (result != ERR_OK) {
        ENGINE_LOGE("systemAbilityId: %{public}d, load failed, result code: %{public}d", UPDATE_DISTRIBUTED_SERVICE_ID,
            result);
        return false;
    }
    if (!CheckUpdaterSaLoaded()) {
        ENGINE_LOGE("systemAbilityId: %{public}d, CheckUpdaterSaLoaded failed", UPDATE_DISTRIBUTED_SERVICE_ID);
        return false;
    }
    WaitUpdaterSaInit();
    ENGINE_LOGI("systemAbilityId: %{public}d, load succeed", UPDATE_DISTRIBUTED_SERVICE_ID);
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS
