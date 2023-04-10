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

#ifndef FIRMWARE_COMBINE_VERSION_H
#define FIRMWARE_COMBINE_VERSION_H

#include <iostream>
#include <string>

namespace OHOS {
namespace UpdateEngine {
class CombinePackageVersionUtils {
public:
    static std::string GetPackageVersion(std::string &baseVersion, std::string &custVersion,
        std::string &preloadVersion);

private:
    static std::string HandleBaseVersion(std::string &baseVersion);
    static std::string HandleCustVersion(std::string &custVersion);
    static std::string HandlePreloadVersion(std::string &preloadVersion);
    static void HandleBaseVersionLog(std::string &baseVersion, std::string &base, std::string &log);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_COMBINE_VERSION_H