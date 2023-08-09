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

#include "firmware_component_operator.h"

#include <memory>

namespace OHOS {
namespace UpdateEngine {
bool FirmwareComponentOperator::QueryAll(const std::vector<FirmwareComponent> &results)
{
    return true;
}

bool FirmwareComponentOperator::Insert(const std::vector<FirmwareComponent> &values)
{
    return true;
}

bool FirmwareComponentOperator::DeleteAll()
{
    return true;
}

bool FirmwareComponentOperator::UpdateProgressByUrl(const std::string &url, UpgradeStatus status, int32_t progress)
{
    return true;
}

bool FirmwareComponentOperator::UpdateUrlByVersionId(const std::string &versionId, const std::string &url)
{
    return true;
}

bool FirmwareComponentOperator::QueryByVersionId(const std::string &versionId, FirmwareComponent &component)
{
    return true;
}

bool FirmwareComponentOperator::QueryByUrl(const std::string &url, FirmwareComponent &component)
{
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS