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
    static void FtGetCharArray(char *getCharArray, uint32_t size)
    {
        if (g_index + size > FUZZ_DATA_LEN) {
            g_index = FUZZ_HEAD_DATA;
        }
        for (uint32_t i = 0; i < size; i++) {
            getCharArray[i] = static_cast<char>(g_data[i + g_index]);
        }
        g_index += size;
    }

    static void FtGetInt(int32_t &getInt)
    {
        if (g_index + FUZZ_INT_LEN_DATA > FUZZ_DATA_LEN) {
            g_index = FUZZ_HEAD_DATA;
        }
        getInt = static_cast<int32_t>(
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX0]) << CHAR_TO_INT_MOVE_LEFT3) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX1]) << CHAR_TO_INT_MOVE_LEFT2) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX2]) << CHAR_TO_INT_MOVE_LEFT1) +
            (static_cast<uint32_t>(g_data[g_index + CHAR_TO_INT_INDEX3]) << CHAR_TO_INT_MOVE_LEFT0));
        g_index += FUZZ_INT_LEN_DATA;
    }

    static void FtGetUInt(uint32_t &getUInt)
    {
        if (g_index + FUZZ_INT_LEN_DATA > FUZZ_DATA_LEN) {
            g_index = FUZZ_HEAD_DATA;
        }
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

    int32_t FtBuildService(void)
    {
        int32_t service;

        FtGetInt(service);

        return service;
    }

    UpdateCallbackInfo FtBuildUpdateCallbackInfo(void)
    {
        UpdateCallbackInfo cb {};

        cb.checkNewVersionDone = FtCheckProcess;
        cb.downloadProgress = FtDownloadProgress;
        cb.upgradeProgress = FtUpgradeProgress;

        return cb;
    }

    UpdateContext FtBuildUpdateContext(void)
    {
        UpdateContext ctx {};

        char controlDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
        FtGetCharArray(controlDevId, FUZZ_CHAR_ARRAY_LEN_DATA);
        ctx.controlDevId = controlDevId;

        char type[FUZZ_CHAR_ARRAY_LEN_DATA];
        FtGetCharArray(type, FUZZ_CHAR_ARRAY_LEN_DATA);
        ctx.type = type;

        char upgradeApp[FUZZ_CHAR_ARRAY_LEN_DATA];
        FtGetCharArray(upgradeApp, FUZZ_CHAR_ARRAY_LEN_DATA);
        ctx.upgradeApp = upgradeApp;

        char upgradeDevId[FUZZ_CHAR_ARRAY_LEN_DATA];
        FtGetCharArray(upgradeDevId, FUZZ_CHAR_ARRAY_LEN_DATA);
        ctx.upgradeDevId = upgradeDevId;

        char upgradeFile[FUZZ_CHAR_ARRAY_LEN_DATA];
        FtGetCharArray(upgradeFile, FUZZ_CHAR_ARRAY_LEN_DATA);
        ctx.upgradeFile = upgradeFile;

        return ctx;
    }

    UpdatePolicy FtBuildUpdatePolicy(void)
    {
        UpdatePolicy updatePolicy {};

        uint32_t autoDownload;
        FtGetUInt(autoDownload);
        updatePolicy.autoDownload = static_cast<bool>(autoDownload % COUNT_BOOL_TYPE);

        uint32_t autoDownloadNet;
        FtGetUInt(autoDownloadNet);
        updatePolicy.autoDownloadNet = static_cast<bool>(autoDownloadNet % COUNT_BOOL_TYPE);

        updatePolicy.autoUpgradeCondition = static_cast<AutoUpgradeCondition>(0);

        uint32_t autoUpgradeInterval[2];
        FtGetUInt(autoUpgradeInterval[0]);
        updatePolicy.autoUpgradeInterval[0] = autoUpgradeInterval[0];
        FtGetUInt(autoUpgradeInterval[1]);
        updatePolicy.autoUpgradeInterval[1] = autoUpgradeInterval[1];

        uint32_t mode;
        FtGetUInt(mode);
        updatePolicy.mode = static_cast<InstallMode>(mode % COUNT_INSTALL_MODE_TYPE);

        return updatePolicy;
    }

    UpgradeInfo FtBuildUpgradeInfo(void)
    {
        UpgradeInfo upgradeInfo {};

        return upgradeInfo;
    }

    VersionInfo FtBuildVersionInfo(void)
    {
        VersionInfo versionInfo {};

        return versionInfo;
    }
} // namespace update_engine
} // namespace OHOS
