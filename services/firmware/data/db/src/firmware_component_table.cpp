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
std::string FirmwareComponentTable::GetTableName()
{
    return FIRMWARE_TABLE_COMPONENT;
}

std::string FirmwareComponentTable::GetTableCreateSql()
{
    return std::string("create table if not exists ")
        .append(FIRMWARE_TABLE_COMPONENT + " (")
        .append(COLUMN_ID + " integer PRIMARY KEY autoincrement not null,")
        .append(COLUMN_COMPONENT_VERSION_ID + " varchar(256),")
        .append(COLUMN_COMPONENT_VERSION_PACKAGE_TYPE + " integer,")
        .append(COLUMN_COMPONENT_PACKAGE_INDEX + " integer,")
        .append(COLUMN_COMPONENT_TARGET_BL_DISPLAY_VERSION_NUMBER  + " varchar(256),")
        .append(COLUMN_COMPONENT_TARGET_BL_VERSION_NUMBER + " varchar(256),")
        .append(COLUMN_COMPONENT_VERSION_NUMBER + " varchar(256),")
        .append(COLUMN_COMPONENT_BL_VERSION_INFO + " varchar(1024),")
        .append(COLUMN_COMPONENT_DESCRIPT_PACKAGE_ID + " varchar(128),")
        .append(COLUMN_COMPONENT_COMPONENT_ID + " varchar(128),")
        .append(COLUMN_COMPONENT_CHANGELOG_URL + " varchar(512),")
        .append(COLUMN_COMPONENT_BL_VERSION_TYPE + " integer,")
        .append(COLUMN_COMPONENT_IS_NEED_RESTART + " integer,")
        .append(COLUMN_COMPONENT_PATCH_TYPE + " integer,")
        .append(COLUMN_COMPONENT_DOWNLOAD_URL + " varchar(512),")
        .append(COLUMN_COMPONENT_RESERVE_URL + " varchar(512),")
        .append(COLUMN_COMPONENT_STORAGE_TYPE + " integer,")
        .append(COLUMN_COMPONENT_VERIFY_INFO + " varchar(256),")
        .append(COLUMN_COMPONENT_SIZE + " bigint,")
        .append(COLUMN_COMPONENT_FILENAME + " varchar(256),")
        .append(COLUMN_COMPONENT_SPATH + " varchar(256),")
        .append(COLUMN_COMPONENT_STATUS + " integer,")
        .append(COLUMN_COMPONENT_PROGRESS + " integer,")
        .append(COLUMN_COMPONENT_EXTENDS1 + " varchar(256),")
        .append(COLUMN_COMPONENT_EXTENDS2 + " varchar(256),")
        .append(COLUMN_COMPONENT_EXTENDS3 + " varchar(256),")
        .append(COLUMN_COMPONENT_EXTENDS4 + " varchar(256),")
        .append(COLUMN_COMPONENT_EXTENDS5 + " varchar(256),")
        .append(COLUMN_COMPONENT_EXTENDS6 + " varchar(256)")
        .append(")");
}

void FirmwareComponentTable::ParseDbValue(ResultSet *resultSet, FirmwareComponent &value)
{
    GetColumnValue(resultSet, COLUMN_ID, value.id);
    GetColumnValue(resultSet, COLUMN_COMPONENT_VERSION_ID, value.versionId);
    int32_t versionPackageType = CAST_INT(PackageType::DYNAMIC);
    GetColumnValue(resultSet, COLUMN_COMPONENT_VERSION_PACKAGE_TYPE, versionPackageType);
    value.versionPackageType = static_cast<PackageType>(versionPackageType);
    GetColumnValue(resultSet, COLUMN_COMPONENT_PACKAGE_INDEX, value.packageIndex);
    GetColumnValue(resultSet, COLUMN_COMPONENT_TARGET_BL_DISPLAY_VERSION_NUMBER, value.targetBlDisplayVersionNumber);
    GetColumnValue(resultSet, COLUMN_COMPONENT_TARGET_BL_VERSION_NUMBER, value.targetBlVersionNumber);
    GetColumnValue(resultSet, COLUMN_COMPONENT_VERSION_NUMBER, value.versionNumber);
    GetColumnValue(resultSet, COLUMN_COMPONENT_BL_VERSION_INFO, value.blVersionInfo);
    GetColumnValue(resultSet, COLUMN_COMPONENT_DESCRIPT_PACKAGE_ID, value.descriptPackageId);
    GetColumnValue(resultSet, COLUMN_COMPONENT_COMPONENT_ID, value.componentId);
    GetColumnValue(resultSet, COLUMN_COMPONENT_CHANGELOG_URL, value.changelogUrl);
    GetColumnValue(resultSet, COLUMN_COMPONENT_BL_VERSION_TYPE, value.blVersionType);
    GetColumnValue(resultSet, COLUMN_COMPONENT_IS_NEED_RESTART, value.isNeedRestart);
    GetColumnValue(resultSet, COLUMN_COMPONENT_PATCH_TYPE, value.patchType);
    GetColumnValue(resultSet, COLUMN_COMPONENT_DOWNLOAD_URL, value.url);
    GetColumnValue(resultSet, COLUMN_COMPONENT_RESERVE_URL, value.reserveUrl);
    GetColumnValue(resultSet, COLUMN_COMPONENT_STORAGE_TYPE, value.storageType);
    GetColumnValue(resultSet, COLUMN_COMPONENT_VERIFY_INFO, value.verifyInfo);
    GetColumnValue(resultSet, COLUMN_COMPONENT_SIZE, value.size);
    GetColumnValue(resultSet, COLUMN_COMPONENT_FILENAME, value.fileName);
    GetColumnValue(resultSet, COLUMN_COMPONENT_SPATH, value.spath);
    int32_t status = CAST_INT(UpgradeStatus::INIT);
    GetColumnValue(resultSet, COLUMN_COMPONENT_STATUS, status);
    value.status = static_cast<UpgradeStatus>(status);
    GetColumnValue(resultSet, COLUMN_COMPONENT_PROGRESS, value.progress);
}

void FirmwareComponentTable::BuildDbValue(const FirmwareComponent &value, NativeRdb::ValuesBucket &dbValue)
{
    PutColumnValue(dbValue, COLUMN_COMPONENT_VERSION_ID, value.versionId);
    PutColumnValue(dbValue, COLUMN_COMPONENT_VERSION_PACKAGE_TYPE, CAST_INT(value.versionPackageType));
    PutColumnValue(dbValue, COLUMN_COMPONENT_PACKAGE_INDEX, value.packageIndex);
    PutColumnValue(dbValue, COLUMN_COMPONENT_TARGET_BL_DISPLAY_VERSION_NUMBER, value.targetBlDisplayVersionNumber);
    PutColumnValue(dbValue, COLUMN_COMPONENT_TARGET_BL_VERSION_NUMBER, value.targetBlVersionNumber);
    PutColumnValue(dbValue, COLUMN_COMPONENT_VERSION_NUMBER, value.versionNumber);
    PutColumnValue(dbValue, COLUMN_COMPONENT_BL_VERSION_INFO, value.blVersionInfo);
    PutColumnValue(dbValue, COLUMN_COMPONENT_DESCRIPT_PACKAGE_ID, value.descriptPackageId);
    PutColumnValue(dbValue, COLUMN_COMPONENT_COMPONENT_ID, value.componentId);
    PutColumnValue(dbValue, COLUMN_COMPONENT_CHANGELOG_URL, value.changelogUrl);
    PutColumnValue(dbValue, COLUMN_COMPONENT_BL_VERSION_TYPE, value.blVersionType);
    PutColumnValue(dbValue, COLUMN_COMPONENT_IS_NEED_RESTART, value.isNeedRestart);
    PutColumnValue(dbValue, COLUMN_COMPONENT_PATCH_TYPE, value.patchType);
    PutColumnValue(dbValue, COLUMN_COMPONENT_DOWNLOAD_URL, value.url);
    PutColumnValue(dbValue, COLUMN_COMPONENT_RESERVE_URL, value.reserveUrl);
    PutColumnValue(dbValue, COLUMN_COMPONENT_STORAGE_TYPE, value.storageType);
    PutColumnValue(dbValue, COLUMN_COMPONENT_VERIFY_INFO, value.verifyInfo);
    PutColumnValue(dbValue, COLUMN_COMPONENT_SIZE, value.size);
    PutColumnValue(dbValue, COLUMN_COMPONENT_FILENAME, value.fileName);
    PutColumnValue(dbValue, COLUMN_COMPONENT_SPATH, value.spath);
    PutColumnValue(dbValue, COLUMN_COMPONENT_STATUS, CAST_INT(value.status));
    PutColumnValue(dbValue, COLUMN_COMPONENT_PROGRESS, value.progress);
}
} // namespace UpdateEngine
} // namespace OHOS