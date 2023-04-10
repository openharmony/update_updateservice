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

#include "schedule_config.h"

#include "file_utils.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
uint64_t ScheduleConfig::pullupInterval_ = Startup::PULLUP_INTERVAL;
uint64_t ScheduleConfig::idleCheckInterval_ = Startup::IDLE_CHECK_INTERVAL;

void ScheduleConfig::InitConfig()
{
    ENGINE_LOGI("InitConfig");
    nlohmann::json root =
        nlohmann::json::parse(FileUtils::ReadDataFromFile(Constant::DUPDATE_ENGINE_CONFIG_PATH), nullptr, false);
    if (root.is_discarded()) {
        ENGINE_LOGI("InitConfig load fail");
        return;
    }
    pullupInterval_ = ParseConfig(root, Startup::PULLUP_INTERVAL_CONFIG, Startup::PULLUP_INTERVAL);
    idleCheckInterval_ = ParseConfig(root, Startup::IDLE_CHECK_INTERVAL_CONFIG, Startup::IDLE_CHECK_INTERVAL);
    ENGINE_LOGI("InitConfig pullupInterval: %{public}s, idleCheckInterval: %{public}s",
        std::to_string(pullupInterval_).c_str(), std::to_string(idleCheckInterval_).c_str());
}

uint64_t ScheduleConfig::GetPullupInterval()
{
    return pullupInterval_;
}

uint64_t ScheduleConfig::GetIdleCheckInterval()
{
    return idleCheckInterval_;
}

uint64_t ScheduleConfig::ParseConfig(nlohmann::json &root, const std::string &key, uint64_t defaultValue)
{
    uint64_t value = 0;
    int32_t ret = JsonUtils::GetValueAndSetTo(root, key, value);
    if ((ret == CAST_INT(JsonParseError::ERR_OK)) && (value > 0)) {
        return value;
    }
    return defaultValue;
}
} // namespace UpdateEngine
} // namespace OHOS