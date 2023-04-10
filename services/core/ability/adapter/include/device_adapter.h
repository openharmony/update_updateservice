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

#ifndef DEVICE_ADAPTER_H
#define DEVICE_ADAPTER_H

#include <string>
#include <vector>

namespace OHOS {
namespace UpdateEngine {
class DeviceAdapter {
public:
    static std::string GetParameter(const std::string &key, const std::string &def);
    static std::string GetDeviceName();
    static std::string GetOsVersion();
    static std::string GetRealVersion();
    static std::string GetDisplayVersion();

    static std::string GetBootSlot();
private:
    static bool IsValidParamValue(const char *value, uint32_t len);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // DEVICE_ADAPTER_H