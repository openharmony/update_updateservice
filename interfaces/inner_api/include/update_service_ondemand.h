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

#ifndef UPDATE_SERVICE_ONDEMAND_H
#define UPDATE_SERVICE_ONDEMAND_H

#include "update_helper.h"

#include <mutex>
#include <unistd.h>

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "system_ability_load_callback_stub.h"

namespace OHOS {
namespace UpdateEngine {
static constexpr int64_t UPDATER_SA_INIT_TIME = 1000 * 1000;
static constexpr int64_t SLEEP_TIME = 500 * 1000;
static constexpr int32_t RETRY_TIMES = 10;

enum class LoadUpdaterSaStatus {
    WAIT_RESULT = 0,
    SUCCESS,
    FAIL,
};

class UpdateServiceOnDemand : public SystemAbilityLoadCallbackStub {
public:
    static sptr<UpdateServiceOnDemand> GetInstance();
    bool TryLoadUpdaterSa();
    void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject>& remoteObject) override;
    void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;

private:
    void InitStatus();
    void WaitUpdaterSaInit();
    bool CheckUpdaterSaLoaded();
    bool LoadUpdaterSa();

    LoadUpdaterSaStatus loadUpdaterSaStatus_ = LoadUpdaterSaStatus::WAIT_RESULT;
    static sptr<UpdateServiceOnDemand> instance_;
    static std::mutex instanceLock_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_ONDEMAND_H
