/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FIRMWARE_UPDATE_HELPER_H
#define FIRMWARE_UPDATE_HELPER_H

#include <algorithm>

#include "battery_srv_client.h"
#include "config_policy_utils.h"
#include "nlohmann/json.hpp"
#include <power_mgr_client.h>

#include "firmware_changelog_utils.h"
#include "firmware_constant.h"
#include "firmware_combine_version_utils.h"
#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_component_operator.h"
#include "firmware_file_utils.h"
#include "firmware_log.h"
#include "firmware_preferences_utils.h"
#include "firmware_task_operator.h"
#include "device_adapter.h"
#include "sha256_utils.h"
#include "time_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareUpdateHelper {
public:
    static bool IsBatteryEnough(int32_t batteryLimit);
    static bool IsUpgradePackagesReady(const std::vector<FirmwareComponent> &componentList);

    static void ClearFirmwareInfo();
    static void BuildCurrentVersionInfo(CurrentVersionInfo &currentVersionInfo);
    static void BuildNewVersionInfo(const std::vector<FirmwareComponent> &components,
        std::vector<VersionComponent> &versionComponents);

    static std::string BuildTaskId(const std::vector<FirmwareComponent> &componentList);
    static InstallType GetInstallType();

private:
    static VersionComponent BuildHotaVersionComponent(std::vector<FirmwareComponent> &hotaComponents);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_UPDATE_HELPER_H