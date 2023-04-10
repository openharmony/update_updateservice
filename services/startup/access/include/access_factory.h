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

#ifndef ACCESS_FACTORY_H
#define ACCESS_FACTORY_H

#include <memory>

#include "i_access.h"
#include "startup_constant.h"

namespace OHOS {
namespace UpdateEngine {
class AccessFactory final {
public:
    static std::shared_ptr<IAccess> GetInstance(AccessType type);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ACCESS_FACTORY_H