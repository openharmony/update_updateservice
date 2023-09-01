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

#ifndef FIRMWARE_ICHECK_H
#define FIRMWARE_ICHECK_H

#include <arpa/inet.h>
#include <ohos_types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "cJSON.h"
#include "nlohmann/json.hpp"
#include "openssl/err.h"
#include "openssl/ssl.h"
#include "parameter.h"
#include "parameters.h"

#include "encrypt_utils.h"
#include "firmware_check_analyze_utils.h"
#include "firmware_common.h"
#include "firmware_component.h"
#include "firmware_constant.h"
#include "device_adapter.h"
#include "firmware_update_helper.h"
#include "network_response.h"
#include "update_helper.h"
#include "update_service_util.h"

constexpr int32_t PORT_NUMBER = 5022;
constexpr int32_t JSON_MAX_SIZE = 4096;
const std::string DEFAULT_SERVER_IP = "127.0.0.1";
const std::string PARAM_NAME_FOR_SEARCH = "update.serverip.search";
#ifndef UPDATER_UT
constexpr int32_t TIMEOUT_FOR_CONNECT = 10;
#else
constexpr int32_t TIMEOUT_FOR_CONNECT = 1;
#endif

namespace OHOS {
namespace UpdateEngine {
using CheckCallback = std::function<void(CheckStatus status, const Duration &duration,
    const std::vector<FirmwareComponent> &firmwareCheckResultList, const CheckAndAuthInfo &checkAndAuthInfo)>;

struct FirmwareCheckCallback {
    CheckCallback callback;
};

class FirmwareICheck {
public:
    explicit FirmwareICheck(const RequestType requestType)
    {
        requestType_ = requestType;
    }
    virtual ~FirmwareICheck() = default;

    void DoAction(FirmwareCheckCallback checkCallback)
    {
        FIRMWARE_LOGI("FirmwareCheck::DoAction");
        int32_t engineSocket = socket(AF_INET, SOCK_STREAM, 0);
        ENGINE_CHECK(engineSocket >= 0,
            checkCallback.callback(CheckStatus::CHECK_FAIL, duration_, checkResultList_, checkAndAuthInfo_);
            return, "socket error !");

        std::string serverIp = OHOS::system::GetParameter(PARAM_NAME_FOR_SEARCH, DEFAULT_SERVER_IP);
        FIRMWARE_LOGI("CheckNewVersion serverIp: %s ", serverIp.c_str());
        sockaddr_in engineSin {};
        engineSin.sin_family = AF_INET;
        engineSin.sin_port = htons(PORT_NUMBER);
        int32_t ret = inet_pton(AF_INET, serverIp.c_str(), &engineSin.sin_addr);
        ENGINE_CHECK(ret > 0, close(engineSocket);
            checkCallback.callback(CheckStatus::CHECK_FAIL, duration_, checkResultList_, checkAndAuthInfo_);
            return, "socket error");

        struct timeval tv = {TIMEOUT_FOR_CONNECT, 0};
        setsockopt(engineSocket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));
        ret = connect(engineSocket, reinterpret_cast<sockaddr *>(&engineSin), sizeof(engineSin));
        ENGINE_CHECK(ret == 0,
            close(engineSocket);
            checkCallback.callback(CheckStatus::CHECK_FAIL, duration_, checkResultList_, checkAndAuthInfo_);
            return, "connect error");
        NetworkResponse response {};
        ret = ReadDataFromSSL(engineSocket, response);
        ENGINE_CHECK(ret == 0,
            close(engineSocket);
            checkCallback.callback(CheckStatus::CHECK_FAIL, duration_, checkResultList_, checkAndAuthInfo_);
            return, "SSL ReadData error");
        close(engineSocket);
        CheckStatus checkStatus;
        if (response.status != static_cast<int64_t>(HttpConstant::SUCCESS) || response.content.empty()) {
            checkStatus = CheckStatus::CHECK_FAIL;
        } else {
            nlohmann::json root = nlohmann::json::parse(response.content, nullptr, false);
            if (!root.is_discarded()) {
                FIRMWARE_LONG_LOGI("FirmwareCheck response: %{public}s", root.dump().c_str());
            }
            FirmwareCheckAnalyzeUtils().DoAnalyze(response.content, checkResultList_, duration_, checkAndAuthInfo_);
            checkStatus = CheckStatus::CHECK_SUCCESS;
        }
        checkCallback.callback(checkStatus, duration_, checkResultList_, checkAndAuthInfo_);
    }

private:
    int32_t ParseJsonFile(const std::vector<char> &buffer, NetworkResponse &response)
    {
        response.content.assign(buffer.begin(), buffer.end());
        response.status = static_cast<int64_t>(HttpConstant::SUCCESS);
        cJSON *root = cJSON_Parse(buffer.data());
        ENGINE_CHECK(root != nullptr, return -1, "Error get root");
        cJSON *item = cJSON_GetObjectItem(root, "searchStatus");
        ENGINE_CHECK(item != nullptr, cJSON_Delete(root);
            return -1, "Error get searchStatus");
        return CAST_INT(static_cast<SearchStatus>(item->valueint));
    }

    int32_t ReadDataFromSSL(int32_t engineSocket, NetworkResponse &response)
    {
        SearchStatus result = SearchStatus::SERVER_BUSY;
        std::string errMsg = "Couldn't connect to server";
        std::vector<char> buffer(JSON_MAX_SIZE);

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        SSL_CTX *sslCtx = SSL_CTX_new(SSLv23_client_method());
        ENGINE_CHECK(sslCtx != nullptr, return -1, "sslCtx is nullptr");
        SSL *ssl = SSL_new(sslCtx);
        ENGINE_CHECK(ssl != nullptr,
            SSL_CTX_free(sslCtx);
            return -1,
            "ssl is nullptr");
        SSL_set_fd(ssl, engineSocket);
        int32_t ret = SSL_connect(ssl);
        if (ret != -1) {
            int32_t len = SSL_read(ssl, buffer.data(), JSON_MAX_SIZE);
            if (len > 0 && ParseJsonFile(buffer, response) == 0) {
                result = SearchStatus::HAS_NEW_VERSION;
                errMsg = "";
            } else {
                result = SearchStatus::SYSTEM_ERROR;
                errMsg = "Couldn't read data";
            }
        } else {
            result = SearchStatus::SYSTEM_ERROR;
            errMsg = "Couldn't connect to server";
        }
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(sslCtx);
        FIRMWARE_LOGI("ReadDataFromSSL errMsg: %s, result: %d", errMsg.c_str(), result);
        return result == SearchStatus::HAS_NEW_VERSION ? 0 : -1;
    }

    RequestType requestType_ = RequestType::CHECK;
    Duration duration_ {};
    CheckAndAuthInfo checkAndAuthInfo_ {};
    std::vector<FirmwareComponent> checkResultList_ {};
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_ICHECK_H