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

#ifndef UPDATE_SERVICE_CACHE_H
#define UPDATE_SERVICE_CACHE_H

#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceCache {
public:
    static UpgradeInfo GetUpgradeInfo(BusinessSubType businessSubType);
    static void SetUpgradeInfo(const UpgradeInfo &upgradeInfo);
    static uint64_t GetCheckInterval(BusinessSubType businessSubType);
    static void SetCheckInterval(BusinessSubType businessSubType, uint64_t interval);
    static uint64_t GetDownloadInterval(BusinessSubType businessSubType);
    static void SetDownloadInterval(BusinessSubType businessSubType, uint64_t interval);
    static UpgradeInterval GetUpgradeInterval(BusinessSubType businessSubType);
    static void SetUpgradeStartTime(BusinessSubType businessSubType, uint64_t time);
    static void SetUpgradeEndTime(BusinessSubType businessSubType, uint64_t time);

private:
    static bool IsTypelegal(BusinessSubType businessSubType);
    static bool IsParamType(BusinessSubType businessSubType);

    static UpgradeInfo upgradeInfo_;
    static uint64_t checkInterval_;
    static uint64_t downloadInterval_;
    static UpgradeInterval upgradeInterval_;

    static UpgradeInfo paramUpgradeInfo_;
    static uint64_t paramCheckInterval_;
    static uint64_t paramDownloadInterval_;
    static UpgradeInterval paramUpgradeInterval_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_CACHE_H