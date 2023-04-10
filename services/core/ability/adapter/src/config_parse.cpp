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

#include "config_parse.h"

#include <ohos_types.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "constant.h"
#include "firmware_constant.h"
#include "json_utils.h"

namespace OHOS {
namespace UpdateEngine {
ConfigParse::ConfigParse()
{
    ENGINE_LOGI("ConfigParse::ConfigParse");
}

ConfigParse::~ConfigParse()
{
    ENGINE_LOGI("ConfigParse::~ConfigParse");
}

    uint32_t ConfigParse::GetAbInstallerTimeout()
{
    return configInfo_.abInstallTimeout;
}

void ConfigParse::LoadConfigInfo()
{
    std::ifstream readFile;
    readFile.open(Constant::DUPDATE_ENGINE_CONFIG_PATH);
    if (readFile.fail()) {
        ENGINE_LOGE("open config fail");
        return;
    }
    std::stringstream streambuffer;
    streambuffer << readFile.rdbuf();
    std::string rawJson(streambuffer.str());
    readFile.close();

    nlohmann::json root = nlohmann::json::parse(rawJson, nullptr, false);
    if (root.is_discarded()) {
        ENGINE_LOGE("json Create error!");
        return;
    }

    JsonUtils::GetValueAndSetTo(root, "abInstallTimeout", configInfo_.abInstallTimeout);
}
} // namespace UpdateEngine
} // namespace OHOS
