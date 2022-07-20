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

#include "fuzztest_helper.h"

namespace OHOS {
namespace UpdateEngine {
constexpr uint32_t CHAR_TO_INT_INDEX0 = 0;
constexpr uint32_t CHAR_TO_INT_INDEX1 = 1;
constexpr uint32_t CHAR_TO_INT_INDEX2 = 2;
constexpr uint32_t CHAR_TO_INT_INDEX3 = 3;

constexpr uint32_t CHAR_TO_INT_MOVE_LEFT0 = 0;
constexpr uint32_t CHAR_TO_INT_MOVE_LEFT1 = 8;
constexpr uint32_t CHAR_TO_INT_MOVE_LEFT2 = 16;
constexpr uint32_t CHAR_TO_INT_MOVE_LEFT3 = 24;

constexpr uint32_t COUNT_BOOL_TYPE = 2;

constexpr uint32_t FUZZ_HEAD_DATA = 0;
constexpr uint32_t FUZZ_INT_LEN_DATA = 4;
constexpr uint32_t FUZZ_CHAR_ARRAY_LEN_DATA = 64;

static void FtCheckNewVersionDone(const BusinessError &businessError, const CheckResultEx &checkResultEx)
{
}

static void FtOnEvent(const EventInfo &eventInfo)
{
}

FuzztestHelper::FuzztestHelper(const uint8_t* data, size_t size)
{
    if (size < FUZZ_DATA_LEN) {
        return;
    }
    if (memcpy_s(data_, sizeof(data_), data, sizeof(data_)) != EOK) {
        return;
    }
}

UpdateCallbackInfo FuzztestHelper::BuildUpdateCallbackInfo()
{
    UpdateCallbackInfo cb {};
    cb.checkNewVersionDone = FtCheckNewVersionDone;
    cb.onEvent = FtOnEvent;
    return cb;
}

UpgradePolicy FuzztestHelper::BuildUpgradePolicy()
{
    UpgradePolicy upgradePolicy;
    upgradePolicy.downloadStrategy = static_cast<bool>(GetUInt() % COUNT_BOOL_TYPE);
    upgradePolicy.autoUpgradeStrategy = static_cast<bool>(GetUInt() % COUNT_BOOL_TYPE);
    upgradePolicy.autoUpgradePeriods[0].start = GetUInt();
    upgradePolicy.autoUpgradePeriods[0].end = GetUInt();
    upgradePolicy.autoUpgradePeriods[1].start = GetUInt();
    upgradePolicy.autoUpgradePeriods[1].end = GetUInt();
    return upgradePolicy;
}

BusinessType FuzztestHelper::BuildBusinessType()
{
    BusinessType businessType;
    businessType.vendor = BusinessVendor::PUBLIC;
    businessType.subType = BusinessSubType::FIRMWARE;
    return businessType;
}

UpgradeInfo FuzztestHelper::BuildUpgradeInfo()
{
    UpgradeInfo upgradeInfo;
    char upgradeApp[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeApp, FUZZ_CHAR_ARRAY_LEN_DATA);
    upgradeInfo.upgradeApp = upgradeApp;

    upgradeInfo.businessType = BuildBusinessType();

    char upgradeDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeDevId, FUZZ_CHAR_ARRAY_LEN_DATA);
    upgradeInfo.upgradeDevId = upgradeDevId;

    char controlDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(controlDevId, FUZZ_CHAR_ARRAY_LEN_DATA);
    upgradeInfo.controlDevId = controlDevId;
    return upgradeInfo;
}

VersionDigestInfo FuzztestHelper::BuildVersionDigestInfo()
{
    VersionDigestInfo versionDigestInfo;
    versionDigestInfo.versionDigest = "versionDigest";
    return versionDigestInfo;
}

void FuzztestHelper::GetCharArray(char *charArray, uint32_t arraySize)
{
    if (index_ + arraySize > FUZZ_DATA_LEN) {
        index_ = FUZZ_HEAD_DATA;
    }
    for (uint32_t i = 0; i < arraySize; i++) {
        charArray[i] = static_cast<char>(data_[i + index_]);
    }
    index_ += arraySize;
}

int32_t FuzztestHelper::GetInt()
{
    int32_t number;
    if (index_ + FUZZ_INT_LEN_DATA > FUZZ_DATA_LEN) {
        index_ = FUZZ_HEAD_DATA;
    }
    number = static_cast<int32_t>(
        (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX0]) <<
        CHAR_TO_INT_MOVE_LEFT3) +
        (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX1]) <<
        CHAR_TO_INT_MOVE_LEFT2) +
        (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX2]) <<
        CHAR_TO_INT_MOVE_LEFT1) +
        (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX3]) <<
        CHAR_TO_INT_MOVE_LEFT0));
    index_ += FUZZ_INT_LEN_DATA;
    return number;
}

uint32_t FuzztestHelper::GetUInt()
{
    uint32_t number;
    if (index_ + FUZZ_INT_LEN_DATA > FUZZ_DATA_LEN) {
        index_ = FUZZ_HEAD_DATA;
    }
    number = (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX0]) <<
        CHAR_TO_INT_MOVE_LEFT3) +
        (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX1]) <<
        CHAR_TO_INT_MOVE_LEFT2) +
        (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX2]) <<
        CHAR_TO_INT_MOVE_LEFT1) +
        (static_cast<uint32_t>(data_[index_ + CHAR_TO_INT_INDEX3]) <<
        CHAR_TO_INT_MOVE_LEFT0);
    index_ += FUZZ_INT_LEN_DATA;
    return number;
}
} // namespace UpdateEngine
} // namespace OHOS
