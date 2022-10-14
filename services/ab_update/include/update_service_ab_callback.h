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

#ifndef UPDATE_SERVICE_AB_CALLBACK_H
#define UPDATE_SERVICE_AB_CALLBACK_H

#include "isys_installer_callback_func.h"
#include "sys_installer_callback.h"
#include "sys_installer_common.h"
#include "sys_installer_kits_impl.h"

#include "update_service.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceAbCallback final : public OHOS::SysInstaller::ISysInstallerCallbackFunc {
public:
    UpdateServiceAbCallback(const UpgradeInfo &info);
    ~UpdateServiceAbCallback() = default;

    void OnUpgradeProgress(int updateStatus, int percent) final;

private:
    void InitEventInfo();

    EventInfo eventInfo_;
    UpgradeInfo info_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_AB_CALLBACK_H
