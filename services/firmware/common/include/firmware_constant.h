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

#ifndef FIRMWARE_CONSTANT_H
#define FIRMWARE_CONSTANT_H

#include <vector>

#include "constant.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
namespace Firmware {
constexpr double HALF_PROGRESS = 0.5;
constexpr int32_t NIGHT_UPGRADE_START_HOUR = 2;
constexpr int32_t NIGHT_UPGRADE_END_HOUR = 4;
constexpr int32_t VER_CARRY_DEVICE_ID = 2;

static const std::string HOTA_CURRENT_COMPONENT_ID = "hota_current_component_id";

static const std::string UPDATE_ACTION = "update_action"; // recovery upgrade

static const std::string PACKAGE_ROOT_PATH = Constant::DUPDATE_ENGINE_PACKAGE_ROOT_PATH + "/firmware";
static const std::string PACKAGE_PATH = PACKAGE_ROOT_PATH + "/versions";

static const std::string ENCRYPTED_ROOT_PATH = Constant::FILES_ROOT_PATH + "/firmware";
static const std::string CHANGELOG_PATH = ENCRYPTED_ROOT_PATH + "/changelogs";

static const std::string AUTO_DOWNLOAD_SWITCH = "auto_download_switch"; // 自动下载开关

constexpr float COMPRESSION_RATIO = 0.2;

constexpr int32_t ONE_HUNDRED = 100;

const std::string BOOTSLOTS_DEFAULT_VALUE = "0";
const std::string BOOTSLOTS_AB_UPDATE_VALUE = "2";
constexpr int32_t SLEEP_TIME_BEFORE_REBOOT = 2;

} // namespace Firmware

static const std::vector<DirInfo> FIRMWARE_DIR_INFOS {
    {Firmware::ENCRYPTED_ROOT_PATH, 0700, true},
    {Firmware::CHANGELOG_PATH, 0700, true},
    {Firmware::PACKAGE_ROOT_PATH, 0770, true},
    {Firmware::PACKAGE_PATH, 0770, true}
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_CONSTANT_H
