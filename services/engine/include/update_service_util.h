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

#ifndef UPDATE_SERVICE_UTIL_H
#define UPDATE_SERVICE_UTIL_H

#include "iupdate_callback.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceUtil {
public:
    static void BuildTaskBody(const Progress &progress, TaskBody &taskBody);
    static sptr<IUpdateCallback> GetUpgradeCallback(const UpgradeInfo &upgradeInfo);
    static void SearchCallback(const UpgradeInfo &upgradeInfo, BusinessError &businessError, CheckResult &checkResult);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_UTIL_H