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

#ifndef ENCRYPT_UTILS_H
#define ENCRYPT_UTILS_H

#include <chrono>
#include <cstdio>
#include <stdlib.h>
#include <string>

#include "update_log.h"
namespace OHOS {
namespace UpdateEngine {
static const int32_t ENCRYPT_LENGTH = 4; // 需要替换*的长度
static const int32_t ENCRYPT_TOTAL_LENGTH = 8; // 敏感数据匿名化后最长长度
static const std::string ENCRYPT_STR = "****";

class EncryptUtils {
public:
    static int64_t GetRand(int32_t min, int32_t max)
    {
        // 随机 min ~ max值
        if (max < min) {
            return min;
        }
        srand(time(nullptr));
        return min + rand() % (max - min);
    }

    static std::string EncryptUrl(const std::string &url)
    {
        std::string encryptUrl = url;
        std::string httpsPrefix = "https://";
        std::string httpPrefix = "http://";

        // 从https:// 或者 http:// 开始后面4位替换为 xxxx
        if (encryptUrl.compare(0, httpsPrefix.size(), httpsPrefix) == 0) {
            encryptUrl.replace(httpsPrefix.size(), ENCRYPT_LENGTH, ENCRYPT_STR);
            return encryptUrl;
        }
        if (encryptUrl.compare(0, httpPrefix.size(), httpPrefix) == 0) {
            encryptUrl.replace(httpPrefix.size(), ENCRYPT_LENGTH, ENCRYPT_STR);
            return encryptUrl;
        }
        return encryptUrl;
    }

    static std::string EncryptString(std::string inputStr)
    {
        if (inputStr.empty()) {
            return inputStr;
        }
        std::string result;
        size_t length = inputStr.length();
        if (length >= ENCRYPT_TOTAL_LENGTH) {
            std::string sequence = inputStr.substr(0, ENCRYPT_LENGTH);
            result = sequence + ENCRYPT_STR;
        } else if (length > ENCRYPT_LENGTH) {
            std::string sequence = inputStr.substr(0, length - ENCRYPT_LENGTH);
            result = sequence + ENCRYPT_STR;
        } else {
            result = ENCRYPT_STR;
        }
        return result;
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ENCRYPT_UTILS_H