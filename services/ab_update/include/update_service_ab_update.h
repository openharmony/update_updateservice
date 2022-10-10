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

#ifndef UPDATE_SERVICE_AB_UPDATE_H
#define UPDATE_SERVICE_AB_UPDATE_H

#include <unistd.h>

#include "update_service_ab_callback.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceAbUpdate {
public:
    UpdateServiceAbUpdate() = default;
    ~UpdateServiceAbUpdate() = default;

    DISALLOW_COPY_AND_MOVE(UpdateServiceAbUpdate);

    static bool IsAbUpdate();
    static int32_t DoAbUpdate(const UpgradeInfo &info, const std::string &packageName);

private:
    static sptr<OHOS::SysInstaller::ISysInstallerCallback> cb_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_AB_UPDATE_H