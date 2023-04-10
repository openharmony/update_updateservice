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

#include "update_service_restorer.h"

#include "fs_manager/mount.h"
#include "updaterkits/updaterkits.h"

#include "update_define.h"
#include "update_log.h"
#include "update_system_event.h"

namespace OHOS {
namespace UpdateEngine {
const std::string MISC_PATH = "/misc";
const std::string MISC_FILE = "/dev/block/by-name/misc";
const std::string CMD_WIPE_DATA = "--user_wipe_data";

int32_t UpdateServiceRestorer::FactoryReset(BusinessError &businessError)
{
#ifndef UPDATER_UT
    businessError.errorNum = CallResult::SUCCESS;
    auto miscBlockDev = Updater::GetBlockDeviceByMountPoint(MISC_PATH);
    ENGINE_LOGI("FactoryReset::misc path : %{public}s", miscBlockDev.c_str());
    ENGINE_CHECK(!miscBlockDev.empty(), miscBlockDev = MISC_FILE, "cannot get block device of partition");
    int32_t ret = RebootAndCleanUserData(miscBlockDev, CMD_WIPE_DATA) ? INT_CALL_SUCCESS : INT_CALL_FAIL;
    ENGINE_LOGI("FactoryReset result : %{public}d", ret);
    SYS_EVENT_SYSTEM_RESET(
        0, ret == INT_CALL_SUCCESS ? UpdateSystemEvent::EVENT_SUCCESS_RESULT : UpdateSystemEvent::EVENT_FAILED_RESULT);
    return ret;
#else
    return INT_CALL_SUCCESS;
#endif
}
} // namespace UpdateEngine
} // namespace OHOS
