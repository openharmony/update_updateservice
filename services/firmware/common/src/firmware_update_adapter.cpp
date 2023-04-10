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

#include "firmware_update_adapter.h"

#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ohos_types.h>
#include <securec.h>
#include <sstream>

#include "securec.h"

#include "constant.h"
#include "config_parse.h"
#include "device_adapter.h"
#include "firmware_constant.h"
#include "firmware_file_utils.h"
#include "firmware_log.h"
#include "firmware_preferences_utils.h"
#include "firmware_task.h"
#include "time_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
constexpr int32_t REQUEST_ID_LEN = 21;
constexpr int32_t TIME_STAMP_LEN = 15;
constexpr int32_t TIME_START_YEAR = 1900;
constexpr int32_t RANDOM_BOUND_SIX = 1000000;

std::string FirmwareUpdateAdapter::GetRequestId()
{
    char requestId[REQUEST_ID_LEN] = {0};
    char timeStamp[TIME_STAMP_LEN] = {0};
    struct tm timeInfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeInfo);
    int res = sprintf_s(timeStamp, sizeof(timeStamp), "%d%02d%02d%02d%02d%02d", timeInfo.tm_year + TIME_START_YEAR,
        timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    if (res <= 0) {
        FIRMWARE_LOGE("CreateTimeStamp error: %{public}d", res);
        return "";
    }

    srand((unsigned int)now);
    int32_t randomInt = rand() % RANDOM_BOUND_SIX; /* random len is 6 */
    res = sprintf_s(requestId, REQUEST_ID_LEN, "%s%d", timeStamp, randomInt);
    if (res <= 0) {
        FIRMWARE_LOGE("CreateRequestId error: %{public}d", res);
        return "";
    }

    return std::string(requestId);
}

std::string FirmwareUpdateAdapter::GetBusinessDomain()
{
    return DelayedSingleton<ConfigParse>::GetInstance()->GetBusinessDomain();
}

std::string FirmwareUpdateAdapter::GetUpdateAction()
{
    std::string action =
        DelayedSingleton<FirmwarePreferencesUtil>::GetInstance()->ObtainString(Firmware::UPDATE_ACTION, "");
    if (action.compare("recovery") == 0) {
        return "recovery";
    }
    return "upgrade";
}
} // namespace UpdateEngine
} // namespace OHOS
