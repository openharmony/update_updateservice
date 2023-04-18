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
#ifndef RELATIONAL_STORE_NATIVE_RDB_ENABLE
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
#endif

bool FirmwareComponentOperator::UpdateProgressByUrl(const std::string &url, UpgradeStatus status, int32_t progress)
{
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_COMPONENT_STATUS, CAST_INT(status));
    values.PutInt(COLUMN_COMPONENT_PROGRESS, progress);

    OHOS::NativeRdb::RdbPredicates predicates(GetTableName());
    predicates.EqualTo(COLUMN_COMPONENT_DOWNLOAD_URL, url);
    return TableBaseOperator::Update(values, predicates);
#else
    return true;
#endif
}

bool FirmwareComponentOperator::UpdateUrlByVersionId(const std::string &versionId, const std::string &url)
{
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutString(COLUMN_COMPONENT_DOWNLOAD_URL, url);

    OHOS::NativeRdb::RdbPredicates predicates(GetTableName());
    predicates.EqualTo(COLUMN_COMPONENT_VERSION_ID, versionId);
    return TableBaseOperator::Update(values, predicates);
#else
    return true;
#endif
}

bool FirmwareComponentOperator::QueryByVersionId(const std::string &versionId, FirmwareComponent &component)
{
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    std::vector<FirmwareComponent> components;
    OHOS::NativeRdb::RdbPredicates predicates(GetTableName());
    predicates.EqualTo(COLUMN_COMPONENT_VERSION_ID, versionId);
    if (Query(components, predicates) && !components.empty()) {
        component = components[0];
        return true;
    }
    return false;
#else
    return true;
#endif
}

bool FirmwareComponentOperator::QueryByUrl(const std::string &url, FirmwareComponent &component)
{
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    std::vector<FirmwareComponent> components;
    OHOS::NativeRdb::RdbPredicates predicates(GetTableName());
    predicates.EqualTo(COLUMN_COMPONENT_DOWNLOAD_URL, url);
    if (Query(components, predicates) && !components.empty()) {
        component = components[0];
        return true;
    }
    return false;
#else
    return true;
#endif
}
} // namespace UpdateEngine
} // namespace OHOS
