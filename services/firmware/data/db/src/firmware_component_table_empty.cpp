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

#include "firmware_component_table.h"

namespace OHOS {
namespace UpdateEngine {
const std::string COLUMN_ID = "id";
std::string FirmwareComponentTable::GetTableName()
{
    return FIRMWARE_TABLE_COMPONENT;
}

std::string FirmwareComponentTable::GetTableCreateSql()
{
    return "";
}
} // namespace UpdateEngine
} // namespace OHOS