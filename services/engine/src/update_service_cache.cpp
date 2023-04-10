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

#include "update_service_cache.h"

#include "update_define.h"
#include "update_helper.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
UpgradeInfo UpdateServiceCache::upgradeInfo_{};
uint64_t UpdateServiceCache::checkInterval_ = 0;
uint64_t UpdateServiceCache::downloadInterval_ = 0;
UpgradeInterval UpdateServiceCache::upgradeInterval_{};

UpgradeInfo UpdateServiceCache::paramUpgradeInfo_{};
uint64_t UpdateServiceCache::paramCheckInterval_ = 0;
uint64_t UpdateServiceCache::paramDownloadInterval_ = 0;
UpgradeInterval UpdateServiceCache::paramUpgradeInterval_{};

bool UpdateServiceCache::IsTypelegal(BusinessSubType businessSubType)
{
    bool isLegal = businessSubType == BusinessSubType::FIRMWARE || businessSubType == BusinessSubType::PARAM;
    if (!isLegal) {
        ENGINE_LOGI("IsTypelegal type %{public}d", CAST_INT(businessSubType));
    }
    return isLegal;
}

bool UpdateServiceCache::IsParamType(BusinessSubType businessSubType)
{
    return businessSubType == BusinessSubType::PARAM;
}

UpgradeInfo UpdateServiceCache::GetUpgradeInfo(BusinessSubType businessSubType)
{
    if (!IsTypelegal(businessSubType)) {
        UpgradeInfo upgradeInfo;
        return upgradeInfo;
    }

    return IsParamType(businessSubType) ? paramUpgradeInfo_ : upgradeInfo_;
}

void UpdateServiceCache::SetUpgradeInfo(const UpgradeInfo &upgradeInfo)
{
    if (!IsTypelegal(upgradeInfo.businessType.subType)) {
        return;
    }

    if (IsParamType(upgradeInfo.businessType.subType)) {
        paramUpgradeInfo_ = upgradeInfo;
    } else {
        upgradeInfo_ = upgradeInfo;
    }
}

uint64_t UpdateServiceCache::GetCheckInterval(BusinessSubType businessSubType)
{
    if (!IsTypelegal(businessSubType)) {
        return 0;
    }

    return IsParamType(businessSubType) ? paramCheckInterval_ : checkInterval_;
}

void UpdateServiceCache::SetCheckInterval(BusinessSubType businessSubType, uint64_t interval)
{
    if (!IsTypelegal(businessSubType)) {
        return;
    }

    if (IsParamType(businessSubType)) {
        paramCheckInterval_ = interval;
    } else {
        checkInterval_ = interval;
    }
}

uint64_t UpdateServiceCache::GetDownloadInterval(BusinessSubType businessSubType)
{
    if (!IsTypelegal(businessSubType)) {
        return 0;
    }

    return IsParamType(businessSubType) ? paramDownloadInterval_ : downloadInterval_;
}

void UpdateServiceCache::SetDownloadInterval(BusinessSubType businessSubType, uint64_t interval)
{
    if (!IsTypelegal(businessSubType)) {
        return;
    }

    if (IsParamType(businessSubType)) {
        paramDownloadInterval_ = interval;
    } else {
        downloadInterval_ = interval;
    }
}

UpgradeInterval UpdateServiceCache::GetUpgradeInterval(BusinessSubType businessSubType)
{
    if (!IsTypelegal(businessSubType)) {
        UpgradeInterval upgradeInterval;
        return upgradeInterval;
    }

    return IsParamType(businessSubType) ? paramUpgradeInterval_ : upgradeInterval_;
}

void UpdateServiceCache::SetUpgradeStartTime(BusinessSubType businessSubType, uint64_t time)
{
    if (!IsTypelegal(businessSubType)) {
        return;
    }

    if (IsParamType(businessSubType)) {
        paramUpgradeInterval_.timeStart = time;
    } else {
        upgradeInterval_.timeStart = time;
    }
}

void UpdateServiceCache::SetUpgradeEndTime(BusinessSubType businessSubType, uint64_t time)
{
    if (!IsTypelegal(businessSubType)) {
        return;
    }

    if (IsParamType(businessSubType)) {
        paramUpgradeInterval_.timeEnd = time;
    } else {
        upgradeInterval_.timeEnd = time;
    }
}
} // namespace UpdateEngine
} // namespace OHOS
