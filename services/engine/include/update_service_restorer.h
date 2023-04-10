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

#ifndef UPDATE_SERVICE_RESTORER_H
#define UPDATE_SERVICE_RESTORER_H

#include "iservice_restorer.h"

#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceRestorer final : public IServiceRestorer {
public:
    UpdateServiceRestorer() = default;

    ~UpdateServiceRestorer() = default;

    DISALLOW_COPY_AND_MOVE(UpdateServiceRestorer);

    int32_t FactoryReset(BusinessError &businessError) override;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_RESTORER_H