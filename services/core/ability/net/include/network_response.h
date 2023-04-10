/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NETWORK_RESPONSE_H
#define NETWORK_RESPONSE_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace UpdateEngine {
enum class HttpConstant {
    DEFAULT = -1,
    SUCCESS = 200,
};

enum class RequestMethod {
    POST = 0,
    GET = 1,
};

class NetworkResponse {
public:
    int64_t status = static_cast<int64_t>(HttpConstant::DEFAULT);
    int32_t result = 0;
    std::string content;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // NETWORK_RESPONSE_H