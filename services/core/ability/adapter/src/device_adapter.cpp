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

#include "device_adapter.h"

#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ohos_types.h>
#include <securec.h>
#include <sstream>

#include "param/init_param.h"
#include "parameter.h"
#include "securec.h"

#include "constant.h"
#include "config_parse.h"
#include "encrypt_utils.h"
#include "update_log.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
constexpr int32_t MAX_PARAM_VALUE_LEN = 128;

bool DeviceAdapter::IsValidParamValue(const char *value, uint32_t len)
{
    return (value != NULL) && (strlen(value) + 1 <= len);
}

std::string DeviceAdapter::GetParameter(const std::string &key, const std::string &def)
{
    uint32_t size = 0;
    int ret = SystemReadParam(key.c_str(), NULL, &size);
    if (ret == 0) {
        std::vector<char> value(size + 1);
        ret = SystemReadParam(key.c_str(), value.data(), &size);
        if (ret == 0) {
            return std::string(value.data());
        }
    }
    if (IsValidParamValue(def.c_str(), MAX_PARAM_VALUE_LEN)) {
        return std::string(def);
    }
    return "";
}

std::string DeviceAdapter::GetDeviceName()
{
    return GetParameter("const.product.model", "");
}

std::string DeviceAdapter::GetOsVersion()
{
    return GetParameter("const.ohos.fullname", "");
}

    std::string DeviceAdapter::GetDisplayVersion()
{
    return GetParameter("const.product.software.version", "");
}

std::string DeviceAdapter::GetRealVersion()
{
    return GetParameter("const.build.ver.physical", "");
}

std::string DeviceAdapter::GetBootSlot()
{
    return GetParameter("ohos.boot.bootslots", "0"); // 0:not ab,2:ab; default 0
}
} // namespace UpdateEngine
} // namespace OHOS
