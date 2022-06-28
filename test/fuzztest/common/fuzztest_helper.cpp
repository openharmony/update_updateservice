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
namespace update_engine {
constexpr uint32_t CHAR_TO_INT_INDEX0 = 0;
constexpr uint32_t CHAR_TO_INT_INDEX1 = 1;
constexpr uint32_t CHAR_TO_INT_INDEX2 = 2;
constexpr uint32_t CHAR_TO_INT_INDEX3 = 3;

constexpr uint32_t CHAR_TO_INT_MOVE_LEFT0 = 0;
constexpr uint32_t CHAR_TO_INT_MOVE_LEFT1 = 8;
constexpr uint32_t CHAR_TO_INT_MOVE_LEFT2 = 16;
constexpr uint32_t CHAR_TO_INT_MOVE_LEFT3 = 24;

constexpr uint32_t COUNT_BOOL_TYPE = 2;
constexpr uint32_t COUNT_INSTALL_MODE_TYPE = 3;

constexpr uint32_t FUZZ_HEAD_DATA = 0;
constexpr uint32_t FUZZ_INT_LEN_DATA = 4;
constexpr uint32_t FUZZ_CHAR_ARRAY_LEN_DATA = 64;

static void FtCheckProcess(const VersionInfo &info)
{
}

static void FtDownloadProgress(const Progress &progress)
{
}

static void FtUpgradeProgress(const Progress &progress)
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
    cb.checkNewVersionDone = FtCheckProcess;
    cb.downloadProgress = FtDownloadProgress;
    cb.upgradeProgress = FtUpgradeProgress;
    return cb;
}

UpdateContext FuzztestHelper::BuildUpdateContext()
{
    UpdateContext ctx {};
    char controlDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(controlDevId, FUZZ_CHAR_ARRAY_LEN_DATA);
    ctx.controlDevId = controlDevId;

    char type[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(type, FUZZ_CHAR_ARRAY_LEN_DATA);
    ctx.type = type;

    char upgradeApp[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeApp, FUZZ_CHAR_ARRAY_LEN_DATA);
    ctx.upgradeApp = upgradeApp;

    char upgradeDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeDevId, FUZZ_CHAR_ARRAY_LEN_DATA);
    ctx.upgradeDevId = upgradeDevId;

    char upgradeFile[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeFile, FUZZ_CHAR_ARRAY_LEN_DATA);
    ctx.upgradeFile = upgradeFile;
    return ctx;
}

UpdatePolicy FuzztestHelper::BuildUpdatePolicy()
{
    UpdatePolicy updatePolicy {};
    updatePolicy.autoDownload = static_cast<bool>(GetUInt() % COUNT_BOOL_TYPE);
    updatePolicy.autoDownloadNet = static_cast<bool>(GetUInt() % COUNT_BOOL_TYPE);
    updatePolicy.autoUpgradeCondition = static_cast<AutoUpgradeCondition>(0);
    updatePolicy.autoUpgradeInterval[0] = GetUInt();
    updatePolicy.autoUpgradeInterval[1] = GetUInt();
    updatePolicy.mode = static_cast<InstallMode>(GetUInt() % COUNT_INSTALL_MODE_TYPE);
    return updatePolicy;
}

UpgradeInfo FuzztestHelper::BuildUpgradeInfo()
{
    UpgradeInfo upgradeInfo {};
    return upgradeInfo;
}

VersionInfo FuzztestHelper::BuildVersionInfo()
{
    VersionInfo versionInfo {};
    return versionInfo;
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
    int32_t number {};
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
    uint32_t number {};
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
} // namespace update_engine
} // namespace OHOS
