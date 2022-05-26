/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "updateservicegetupgradestatus_fuzzer.h"

using namespace OHOS::update_engine;

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

const uint32_t FUZZ_HEAD_DATA = 0;
const uint32_t FUZZ_INT_LEN_DATA = 4;
const uint32_t FUZZ_CHAR_ARRAY_UPD_APP_NAME_LEN_DATA = 64;
const uint32_t FUZZ_CHAR_ARRAY_DEV_ID_LEN_DATA = 68;

const int FUZZ_DATA_LEN = 500;

static uint8_t g_data[FUZZ_DATA_LEN];

uint32_t g_index = FUZZ_HEAD_DATA;

namespace OHOS {
    static void FtGetCharArray(char *getCharArray, uint32_t size)
    {
        for (uint32_t i = 0; i < size; i++) {
            getCharArray[i] = static_cast<char>(g_data[i + g_index]);
        }
        g_index += size;
    }

    static void FtGetInt(int32_t &getInt)
    {
        getInt = static_cast<int32_t>(
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX0]) << CHAR_TO_INT_MOVE_LEFT3) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX1]) << CHAR_TO_INT_MOVE_LEFT2) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX2]) << CHAR_TO_INT_MOVE_LEFT1) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX3]) << CHAR_TO_INT_MOVE_LEFT0));
        g_index += FUZZ_INT_LEN_DATA;
    }

    static void FtGetUInt(uint32_t &getUInt)
    {
        getUInt = (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX0]) << CHAR_TO_INT_MOVE_LEFT3) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX1]) << CHAR_TO_INT_MOVE_LEFT2) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX2]) << CHAR_TO_INT_MOVE_LEFT1) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX3]) << CHAR_TO_INT_MOVE_LEFT0);
        g_index += FUZZ_INT_LEN_DATA;
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

    int32_t FuzzUpdateServiceGetUpgradeStatusImpl(void)
    {
        UpdateContext ctx;
        UpdateCallbackInfo cb = {
            .checkNewVersionDone = FtCheckProcess,
            .downloadProgress = FtDownloadProgress,
            .upgradeProgress = FtUpgradeProgress,
        };

        uint32_t autoDownload;
        FtGetUInt(autoDownload);
        uint32_t autoDownloadNet;
        FtGetUInt(autoDownloadNet);
        uint32_t mode;
        FtGetUInt(mode);
        UpdatePolicy policy = {static_cast<bool>(autoDownload % COUNT_BOOL_TYPE),
            static_cast<bool>(autoDownloadNet % COUNT_BOOL_TYPE),
            static_cast<InstallMode>(mode % COUNT_INSTALL_MODE_TYPE),
            static_cast<AutoUpgradeCondition>(0)};

        uint32_t autoUpgradeInterval[2];
        FtGetUInt(autoUpgradeInterval[0]);
        policy.autoUpgradeInterval[0] = autoUpgradeInterval[0];
        FtGetUInt(autoUpgradeInterval[1]);
        policy.autoUpgradeInterval[1] = autoUpgradeInterval[1];

        char upgradeApp[FUZZ_CHAR_ARRAY_UPD_APP_NAME_LEN_DATA];
        FtGetCharArray(upgradeApp, FUZZ_CHAR_ARRAY_UPD_APP_NAME_LEN_DATA);
        ctx.upgradeApp = upgradeApp;

        char upgradeDevId[FUZZ_CHAR_ARRAY_DEV_ID_LEN_DATA];
        FtGetCharArray(upgradeDevId, FUZZ_CHAR_ARRAY_DEV_ID_LEN_DATA);
        ctx.upgradeDevId = upgradeDevId;

        char controlDevId[FUZZ_CHAR_ARRAY_DEV_ID_LEN_DATA];
        FtGetCharArray(controlDevId, FUZZ_CHAR_ARRAY_DEV_ID_LEN_DATA);
        ctx.controlDevId = controlDevId;

        VersionInfo versionInfo;
        memset_s(&versionInfo, sizeof(versionInfo), 0, sizeof(versionInfo));

        int32_t service;
        FtGetInt(service);

        g_index = FUZZ_HEAD_DATA;

        UpgradeInfo info = {UPDATE_STATE_INIT};

        (void)UpdateServiceKits::GetInstance().RegisterUpdateCallback(ctx, cb);
        (void)UpdateServiceKits::GetInstance().SetUpdatePolicy(policy);
        (void)UpdateServiceKits::GetInstance().GetUpdatePolicy(policy);
        (void)UpdateServiceKits::GetInstance().CheckNewVersion();
        (void)UpdateServiceKits::GetInstance().GetNewVersion(versionInfo);
        (void)UpdateServiceKits::GetInstance().DownloadVersion();
        (void)UpdateServiceKits::GetInstance().Cancel(service);
        int32_t ret = UpdateServiceKits::GetInstance().GetUpgradeStatus(info);
        (void)UpdateServiceKits::GetInstance().UnregisterUpdateCallback();

        return ret;
    }

    bool FuzzUpdateServiceGetUpgradeStatus(const uint8_t* data, size_t size)
    {
        if (size < FUZZ_DATA_LEN) {
            return false;
        }
        if (memcpy_s(g_data, sizeof(g_data), data, FUZZ_DATA_LEN) != 0) {
            return false;
        }
        return !FuzzUpdateServiceGetUpgradeStatusImpl();
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzUpdateServiceGetUpgradeStatus(data, size);
    return 0;
}
