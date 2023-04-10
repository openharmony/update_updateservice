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

#ifndef SHA256_UTILS_H
#define SHA256_UTILS_H

#include <string>

#include "securec.h"

#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
class Sha256Utils {
public:
    static std::string CalculateHashCode(std::string inputStr);
    static bool CheckFileSha256String(const std::string &fileName, const std::string &sha256String);

private:
    static bool GetFileSha256Str(const std::string &fileName, char *sha256Result, uint32_t len);
    static bool GetDigestFromFile(const char *fileName, unsigned char digest[]);
    static bool Sha256Calculate(const unsigned char *input, int len, char *componentId, int componentIdLen);
    static bool TransDigestToSha256Result(char *sha256Result, uint32_t componentIdLen, const unsigned char *digest);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // SHA256_UTILS_H