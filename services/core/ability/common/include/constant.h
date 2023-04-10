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

#ifndef CONSTANT_H
#define CONSTANT_H

#include <stdint.h>
#include <string>
#include <vector>

#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
namespace Constant {
constexpr int32_t ONE_DAY_HOUR = 24;
constexpr int32_t ONE_HOUR_MINUTES = 60;
constexpr int32_t ONE_MINUTE_SECONDS = 60;
constexpr int32_t FIVE_MINUTES_SECONDS = 5 * Constant::ONE_MINUTE_SECONDS;

static const std::string DUPDATE_ENGINE_CONFIG_PATH = "/system/etc/update/dupdate_config.json";

// 存放升级引擎数据库、缓存等文件的加密路径
static const std::string UPDATE_ENCRYPTED_ROOT_PATH = "/data/service/el1/public/update";
static const std::string DUPDATE_ENGINE_ENCRYPTED_ROOT_PATH = UPDATE_ENCRYPTED_ROOT_PATH + "/dupdate_engine";
static const std::string DATABASES_ROOT_PATH = DUPDATE_ENGINE_ENCRYPTED_ROOT_PATH + "/databases";
static const std::string PREFERENCES_ROOT_PATH = DUPDATE_ENGINE_ENCRYPTED_ROOT_PATH + "/preferences";
static const std::string FILES_ROOT_PATH = DUPDATE_ENGINE_ENCRYPTED_ROOT_PATH + "/files";

// 存放ota升级包的非加密路径
static const std::string UPDATE_PACKAGE_ROOT_PATH = "/data/update";
static const std::string DUPDATE_ENGINE_PACKAGE_ROOT_PATH = UPDATE_PACKAGE_ROOT_PATH + "/ota_package";

static const std::string DUE_INIT_FLAG = "due_init_flag"; // DUE初始启动标记
static const std::string PROCESS_RESTART_REASON = "process_restart_reason"; // 进程重启原因
} // namespace Constant

enum class CommonEventType {
    AUTO_UPGRADE = 0,
    NET_CHANGED,
    TIME_CHANGED,
    TIME_ZONE_CHANGED,
    BOOT_COMPLETE,
    PROCESS_INIT,
    NIGHT_UPGRADE,
    SCREEN_OFF,
    UPGRADE_REMIND,
};

static const std::vector<DirInfo> BASE_DIR_INFOS {
    {Constant::UPDATE_ENCRYPTED_ROOT_PATH, 0751, false},
    {Constant::DUPDATE_ENGINE_ENCRYPTED_ROOT_PATH, 0700, false},
    {Constant::DATABASES_ROOT_PATH, 0700, false},
    {Constant::PREFERENCES_ROOT_PATH, 0700, false},
    {Constant::FILES_ROOT_PATH, 0700, true},
    {Constant::UPDATE_PACKAGE_ROOT_PATH, 0770, false},
    {Constant::DUPDATE_ENGINE_PACKAGE_ROOT_PATH, 0770, true}
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // CONSTANT_H