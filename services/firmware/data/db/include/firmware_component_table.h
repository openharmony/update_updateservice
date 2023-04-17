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

#ifndef FIRMWARE_COMPONENT_TABLE_H
#define FIRMWARE_COMPONENT_TABLE_H

#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
#include "values_bucket.h"

#include "itable.h"
#endif
#include "firmware_component.h"

namespace OHOS {
namespace UpdateEngine {
const std::string FIRMWARE_TABLE_COMPONENT = "component";
const std::string COLUMN_COMPONENT_VERSION_ID = "versionId";
const std::string COLUMN_COMPONENT_VERSION_PACKAGE_TYPE = "versionPackageType";
const std::string COLUMN_COMPONENT_PACKAGE_INDEX = "packageIndex";
const std::string COLUMN_COMPONENT_TARGET_BL_DISPLAY_VERSION_NUMBER  = "targetBlDisplayVersionNumber";
const std::string COLUMN_COMPONENT_TARGET_BL_VERSION_NUMBER = "targetBlVersionNumber";
const std::string COLUMN_COMPONENT_VERSION_NUMBER = "versionNumber";
const std::string COLUMN_COMPONENT_BL_VERSION_INFO = "blVersionInfo";
const std::string COLUMN_COMPONENT_DESCRIPT_PACKAGE_ID = "descriptPackageId";
const std::string COLUMN_COMPONENT_COMPONENT_ID = "componentId";
const std::string COLUMN_COMPONENT_CHANGELOG_URL = "changelogUrl";
const std::string COLUMN_COMPONENT_BL_VERSION_TYPE = "blVersionType";
const std::string COLUMN_COMPONENT_IS_NEED_RESTART = "isNeedRestart";
const std::string COLUMN_COMPONENT_PATCH_TYPE = "patchType";
const std::string COLUMN_COMPONENT_DOWNLOAD_URL = "url";
const std::string COLUMN_COMPONENT_RESERVE_URL = "reserveUrl";
const std::string COLUMN_COMPONENT_STORAGE_TYPE = "storageType";
const std::string COLUMN_COMPONENT_VERIFY_INFO = "verifyInfo";
const std::string COLUMN_COMPONENT_SIZE = "size";
const std::string COLUMN_COMPONENT_FILENAME = "fileName";
const std::string COLUMN_COMPONENT_SPATH = "spath";
const std::string COLUMN_COMPONENT_STATUS = "status";
const std::string COLUMN_COMPONENT_PROGRESS = "progress";

// 预留6个字段
const std::string COLUMN_COMPONENT_EXTENDS1 = "extends1";
const std::string COLUMN_COMPONENT_EXTENDS2 = "extends2";
const std::string COLUMN_COMPONENT_EXTENDS3 = "extends3";
const std::string COLUMN_COMPONENT_EXTENDS4 = "extends4";
const std::string COLUMN_COMPONENT_EXTENDS5 = "extends5";
const std::string COLUMN_COMPONENT_EXTENDS6 = "extends6";

#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
class FirmwareComponentTable final: public ITable<FirmwareComponent> {
#else 
class FirmwareComponentTable final {
#endif
public:
    FirmwareComponentTable() = default;
    ~FirmwareComponentTable() = default;
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    std::string GetTableName() final;
    std::string GetTableCreateSql() final;
    void ParseDbValue(ResultSet *resultSet, FirmwareComponent &value) final;
    void BuildDbValue(const FirmwareComponent &value, NativeRdb::ValuesBucket &dbValue) final;
#else
    std::string GetTableName();
    std::string GetTableCreateSql();
#endif
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_COMPONENT_TABLE_H
