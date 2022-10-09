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

#include "update_service_ab_update.h"

#include "parameters.h"

namespace OHOS {
namespace UpdateEngine {
sptr<OHOS::SysInstaller::ISysInstallerCallback> UpdateServiceAbUpdate::cb_ = nullptr;
const std::string PARAM_NAME_FOR_BOOTSLOTS = "ohos.boot.bootslots";
const std::string BOOTSLOTS_DEFAULT_VALUE = "0";

bool UpdateServiceAbUpdate::IsAbUpdate()
{
    int32_t bootslots = atoi(OHOS::system::GetParameter(PARAM_NAME_FOR_BOOTSLOTS, BOOTSLOTS_DEFAULT_VALUE).c_str());
    ENGINE_LOGE("bootslots is [%d]", bootslots);
    return (bootslots > 1);
}

int32_t UpdateServiceAbUpdate::DoAbUpdate(const UpgradeInfo &info, const std::string &packageName)
{
    if (!IsAbUpdate()) {
        ENGINE_LOGE("IsAbUpdate false, UpdateServiceAbUpdate fail");
        return INT_CALL_FAIL;
    }
    int32_t ret = OHOS::SysInstaller::SysInstallerKitsImpl::GetInstance().SysInstallerInit();
    if (ret != 0) {
        ENGINE_LOGE("SysInstallerInit fail, ret = %d, UpdateServiceAbUpdate fail", ret);
        return INT_CALL_FAIL;
    }
    if (cb_ == nullptr) {
        cb_ = new UpdateServiceAbCallback(info);
    }
    ret = OHOS::SysInstaller::SysInstallerKitsImpl::GetInstance().SetUpdateCallback(cb_);
    if (ret != 0) {
        ENGINE_LOGE("SetUpdateCallback fail, ret = %d, UpdateServiceAbUpdate fail", ret);
        return INT_CALL_FAIL;
    }
    auto updateStatus = OHOS::SysInstaller::SysInstallerKitsImpl::GetInstance().GetUpdateStatus();
    if (updateStatus != OHOS::SysInstaller::UPDATE_STATE_INIT) {
        ENGINE_LOGE("AbUpgrade process status is not init, status is %d, UpdateServiceAbUpdate fail", updateStatus);
        return INT_CALL_FAIL;
    }
    ret = OHOS::SysInstaller::SysInstallerKitsImpl::GetInstance().StartUpdatePackageZip(packageName);
    if (ret != 0) {
        ENGINE_LOGE("StartUpdatePackageZip fail, ret = %d, UpdateServiceAbUpdate fail", ret);
        return INT_CALL_FAIL;
    }
    return INT_CALL_SUCCESS;
}
} // namespace UpdateEngine
} // namespace OHOS
