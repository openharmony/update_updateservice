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

#ifndef ISERVICE_LOCAL_UPDATER_H
#define ISERVICE_LOCAL_UPDATER_H

#include "refbase.h"

#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class IServiceLocalUpdater : public virtual RefBase {
public:
    virtual ~IServiceLocalUpdater() = default;

    virtual int32_t ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile,
        const std::string &packageName, BusinessError &businessError) = 0;

    virtual int32_t VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
        BusinessError &businessError) = 0;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ISERVICE_LOCAL_UPDATER_H
