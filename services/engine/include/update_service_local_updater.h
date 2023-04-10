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

#ifndef UPDATE_SERVICE_LOCAL_UPDATER_H
#define UPDATE_SERVICE_LOCAL_UPDATER_H

#include "iservice_local_updater.h"

#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceLocalUpdater final : public IServiceLocalUpdater {
public:
    UpdateServiceLocalUpdater() = default;

    ~UpdateServiceLocalUpdater() = default;

    DISALLOW_COPY_AND_MOVE(UpdateServiceLocalUpdater);

    int32_t ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile, const std::string &packageName,
        BusinessError &businessError) final;

    int32_t VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
        BusinessError &businessError) final;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_LOCAL_UPDATER_H