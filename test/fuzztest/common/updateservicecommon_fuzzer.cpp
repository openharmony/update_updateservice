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

#include "updateservicecommon_fuzzer.h"

const uint32_t CHAR_TO_INT_INDEX0 = 0;
const uint32_t CHAR_TO_INT_INDEX1 = 1;
const uint32_t CHAR_TO_INT_INDEX2 = 2;
const uint32_t CHAR_TO_INT_INDEX3 = 3;

const uint32_t CHAR_TO_INT_MOVE_LEFT0 = 0;
const uint32_t CHAR_TO_INT_MOVE_LEFT1 = 8;
const uint32_t CHAR_TO_INT_MOVE_LEFT2 = 16;
const uint32_t CHAR_TO_INT_MOVE_LEFT3 = 24;

const uint32_t COUNT_BOOL_TYPE = 2;
const uint32_t COUNT_INSTALL_MODE_TYPE = 3;

const uint32_t FUZZ_INT_LEN_DATA = 4;
const uint32_t FUZZ_CHAR_ARRAY_LEN_DATA = 64;

namespace OHOS {
namespace update_engine {
static void GetCharArray(char *getCharArray, uint32_t arraySize, const uint8_t* data, size_t size)
{
    if (FuzztestHelper::GetInstance()->index_ + arraySize > FUZZ_DATA_LEN) {
        FuzztestHelper::GetInstance()->index_ = FUZZ_HEAD_DATA;
    }
    for (uint32_t i = 0; i < arraySize; i++) {
        getCharArray[i] = static_cast<char>(data[i + FuzztestHelper::GetInstance()->index_]);
    }
    FuzztestHelper::GetInstance()->index_ += arraySize;
}

static void GetInt(int32_t &getInt, const uint8_t* data, size_t size)
{
    if (FuzztestHelper::GetInstance()->index_ + FUZZ_INT_LEN_DATA > FUZZ_DATA_LEN) {
        FuzztestHelper::GetInstance()->index_ = FUZZ_HEAD_DATA;
    }
    getInt = static_cast<int32_t>(
        (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX0]) <<
        CHAR_TO_INT_MOVE_LEFT3) +
        (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX1]) <<
        CHAR_TO_INT_MOVE_LEFT2) +
        (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX2]) <<
        CHAR_TO_INT_MOVE_LEFT1) +
        (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX3]) <<
        CHAR_TO_INT_MOVE_LEFT0));
    FuzztestHelper::GetInstance()->index_ += FUZZ_INT_LEN_DATA;
}

static void GetUInt(uint32_t &getUInt, const uint8_t* data, size_t size)
{
    if (FuzztestHelper::GetInstance()->index_ + FUZZ_INT_LEN_DATA > FUZZ_DATA_LEN) {
        FuzztestHelper::GetInstance()->index_ = FUZZ_HEAD_DATA;
    }
    getUInt = (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX0]) <<
        CHAR_TO_INT_MOVE_LEFT3) +
        (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX1]) <<
        CHAR_TO_INT_MOVE_LEFT2) +
        (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX2]) <<
        CHAR_TO_INT_MOVE_LEFT1) +
        (static_cast<uint32_t>(data[FuzztestHelper::GetInstance()->index_ + CHAR_TO_INT_INDEX3]) <<
        CHAR_TO_INT_MOVE_LEFT0);
    FuzztestHelper::GetInstance()->index_ += FUZZ_INT_LEN_DATA;
}

static void FtCheckProcess(const VersionInfo &info)
{
}

static void FtDownloadProgress(const Progress &progress)
{
}

static void FtUpgradeProgress(const Progress &progress)
{
}

FuzztestHelper::~FuzztestHelper()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

FuzztestHelper* FuzztestHelper::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new FuzztestHelper();
    }
    return instance_;
}

int32_t FuzztestHelper::BuildService(const uint8_t* data, size_t size)
{
    int32_t service;
    GetInt(service, data, size);
    return service;
}

UpdateCallbackInfo FuzztestHelper::BuildUpdateCallbackInfo(const uint8_t* data, size_t size)
{
    UpdateCallbackInfo cb {};
    cb.checkNewVersionDone = FtCheckProcess;
    cb.downloadProgress = FtDownloadProgress;
    cb.upgradeProgress = FtUpgradeProgress;
    return cb;
}

UpdateContext FuzztestHelper::BuildUpdateContext(const uint8_t* data, size_t size)
{
    UpdateContext ctx {};
    char controlDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(controlDevId, FUZZ_CHAR_ARRAY_LEN_DATA, data, size);
    ctx.controlDevId = controlDevId;

    char type[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(type, FUZZ_CHAR_ARRAY_LEN_DATA, data, size);
    ctx.type = type;

    char upgradeApp[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeApp, FUZZ_CHAR_ARRAY_LEN_DATA, data, size);
    ctx.upgradeApp = upgradeApp;

    char upgradeDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeDevId, FUZZ_CHAR_ARRAY_LEN_DATA, data, size);
    ctx.upgradeDevId = upgradeDevId;

    char upgradeFile[FUZZ_CHAR_ARRAY_LEN_DATA];
    GetCharArray(upgradeFile, FUZZ_CHAR_ARRAY_LEN_DATA, data, size);
    ctx.upgradeFile = upgradeFile;
    return ctx;
}

UpdatePolicy FuzztestHelper::BuildUpdatePolicy(const uint8_t* data, size_t size)
{
    UpdatePolicy updatePolicy {};
    uint32_t autoDownload;
    GetUInt(autoDownload, data, size);
    updatePolicy.autoDownload = static_cast<bool>(autoDownload % COUNT_BOOL_TYPE);

    uint32_t autoDownloadNet;
    GetUInt(autoDownloadNet, data, size);
    updatePolicy.autoDownloadNet = static_cast<bool>(autoDownloadNet % COUNT_BOOL_TYPE);

    updatePolicy.autoUpgradeCondition = static_cast<AutoUpgradeCondition>(0);

    uint32_t autoUpgradeInterval[2];
    GetUInt(autoUpgradeInterval[0], data, size);
    updatePolicy.autoUpgradeInterval[0] = autoUpgradeInterval[0];
    GetUInt(autoUpgradeInterval[1], data, size);
    updatePolicy.autoUpgradeInterval[1] = autoUpgradeInterval[1];

    uint32_t mode;
    GetUInt(mode, data, size);
    updatePolicy.mode = static_cast<InstallMode>(mode % COUNT_INSTALL_MODE_TYPE);
    return updatePolicy;
}

UpgradeInfo FuzztestHelper::BuildUpgradeInfo(const uint8_t* data, size_t size)
{
    UpgradeInfo upgradeInfo {};
    return upgradeInfo;
}

VersionInfo FuzztestHelper::BuildVersionInfo(const uint8_t* data, size_t size)
{
    VersionInfo versionInfo {};
    return versionInfo;
}
} // namespace update_engine
} // namespace OHOS
