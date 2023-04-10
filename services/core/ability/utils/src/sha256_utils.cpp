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

#include "sha256_utils.h"

#include <cstdlib>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <unistd.h>

#include "file_utils.h"
#include "mbedtls/sha256.h"

namespace OHOS {
namespace UpdateEngine {
constexpr unsigned int SHA256_STRING_LEN = 65;
constexpr unsigned int SHA256_LENGTH = 32;
constexpr unsigned int MAX_BUFFER_LENGTH = 1024;
constexpr unsigned int SHA256_TO_STRING_STEP = 2;

std::string Sha256Utils::CalculateHashCode(std::string inputStr)
{
    char result[SHA256_STRING_LEN] = {0};
    if (!Sha256Calculate((unsigned char *)(inputStr.c_str()), inputStr.length(), result, SHA256_STRING_LEN)) {
        ENGINE_LOGE("CalculateHashCode fail, src = %s", inputStr.c_str());
        return "";
    }
    return result;
}

bool Sha256Utils::CheckFileSha256String(const std::string &fileName, const std::string &sha256String)
{
    if (!FileUtils::IsFileExist(fileName)) {
        ENGINE_LOGE("check file sha256 failed, fileName = %{pubilc}s is not exist", fileName.c_str());
        return false;
    }
    char sha256Result[SHA256_STRING_LEN] = {0}; // sha256Result len is 65
    if (!GetFileSha256Str(fileName, sha256Result, sizeof(sha256Result))) {
        ENGINE_LOGE("get file sha256 failed");
        return false;
    }
    if (strcasecmp(sha256Result, sha256String.c_str()) != 0) {
        ENGINE_LOGE("sha256 not same! input=%{public}s, cal=%{public}s", sha256Result, sha256String.c_str());
        return false;
    }
    return true;
}

bool Sha256Utils::GetDigestFromFile(const char *fileName, unsigned char digest[])
{
    char *canonicalPath = realpath(fileName, NULL);
    if (canonicalPath == NULL) {
        ENGINE_LOGI("%s is not exist or invalid", fileName);
        return false;
    }
    FILE *fp = fopen(canonicalPath, "rb");
    free(canonicalPath);
    if (fp == NULL) {
        return false;
    }
    (void)fseek(fp, 0, SEEK_END);
    long fLen = ftell(fp);
    (void)fseek(fp, 0, SEEK_SET);

    unsigned char buffer[MAX_BUFFER_LENGTH]; /* buffer len 1024 */
    mbedtls_sha256_context context;
    mbedtls_sha256_init(&context);
    mbedtls_sha256_starts(&context, 0);
    while (!feof(fp) && fLen > 0) {
        int readCount = (sizeof(buffer) > (uint32_t)fLen) ? fLen : sizeof(buffer);
        int count = (int)fread(buffer, 1, readCount, fp);
        if (count != readCount) {
            ENGINE_LOGE("read file %{public}s error", fileName);
            break;
        }
        fLen -= count;
        mbedtls_sha256_update(&context, buffer, count);
    }
    mbedtls_sha256_finish(&context, digest);
    mbedtls_sha256_free(&context);
    (void)fclose(fp);
    return true;
}

bool Sha256Utils::GetFileSha256Str(const std::string &fileName, char *sha256Result, uint32_t len)
{
    unsigned char digest[SHA256_LENGTH] = {0};
    GetDigestFromFile(fileName.c_str(), digest);
    return TransDigestToSha256Result(sha256Result, len, digest);
}

bool Sha256Utils::Sha256Calculate(const unsigned char *input, int len, char *componentId, int componentIdLen)
{
    unsigned char digest[SHA256_LENGTH] = {0};
    mbedtls_sha256_context ctx;
    int ret = memset_s(&ctx, sizeof(ctx), 0, sizeof(ctx));
    if (ret != 0) {
        ENGINE_LOGE("init mbedtls_sha256_context failed");
        return false;
    }
    mbedtls_sha256_init(&ctx);
    int startRet = mbedtls_sha256_starts(&ctx, 0);
    if (startRet != 0) {
        mbedtls_sha256_free(&ctx);
        ENGINE_LOGE("mbedtls_sha256_starts_ret failed");
        return false;
    }

    int updateRet = mbedtls_sha256_update(&ctx, input, len);
    if (updateRet != 0) {
        mbedtls_sha256_free(&ctx);
        ENGINE_LOGE("mbedtls_sha256_update_ret failed");
        return false;
    }

    int finishRet = mbedtls_sha256_finish(&ctx, digest);
    if (finishRet != 0) {
        mbedtls_sha256_free(&ctx);
        ENGINE_LOGE("mbedtls_sha256_finish_ret failed");
        return false;
    }
    mbedtls_sha256_free(&ctx);
    return TransDigestToSha256Result(componentId, componentIdLen, digest);
}

bool Sha256Utils::TransDigestToSha256Result(char *sha256Result, uint32_t componentIdLen, const unsigned char *digest)
{
    for (unsigned int i = 0; i < SHA256_LENGTH; i++) {
        unsigned int deviation = i * SHA256_TO_STRING_STEP;
        if (deviation >= componentIdLen) {
            ENGINE_LOGE("deviation len illegal");
            return false;
        }
        int result = sprintf_s(sha256Result + deviation, (componentIdLen - deviation), "%02x", digest[i]);
        if (result <= 0) {
            ENGINE_LOGE("generated sha256 failed");
            return false;
        }
    }
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS