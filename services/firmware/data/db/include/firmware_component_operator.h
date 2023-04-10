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

#ifndef FIRMWARE_COMPONENT_OPERATOR_H
#define FIRMWARE_COMPONENT_OPERATOR_H

#include "singleton.h"

#include "firmware_component.h"
#include "firmware_component_table.h"
#include "firmware_database.h"
#include "table_base_operator.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareComponentOperator final : public TableBaseOperator<FirmwareComponentTable, FirmwareComponent> {
public:
    FirmwareComponentOperator() : TableBaseOperator(DelayedSingleton<FirmwareDatabase>::GetInstance()) {}
    ~FirmwareComponentOperator() = default;
    bool UpdateProgressByUrl(const std::string &url, UpgradeStatus status, int32_t progress);
    bool UpdateUrlByVersionId(const std::string &versionId, const std::string &url);
    bool QueryByVersionId(const std::string &versionId, FirmwareComponent &component);
    bool QueryByUrl(const std::string &url, FirmwareComponent &component);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_COMPONENT_OPERATOR_H