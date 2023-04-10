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

#include "firmware_task_table.h"

#include "update_define.h"

namespace OHOS {
namespace UpdateEngine {
std::string FirmwareTaskTable::GetTableName()
{
    return FIRMWARE_TABLE_TASK;
}

std::string FirmwareTaskTable::GetTableCreateSql()
{
    return std::string("create table if not exists ")
        .append(FIRMWARE_TABLE_TASK + " (")
        .append(COLUMN_ID + " integer PRIMARY KEY autoincrement not null,")
        .append(COLUMN_TASK_TASK_ID + " varchar(128),")
        .append(COLUMN_TASK_STATUS + " integer,")
        .append(COLUMN_TASK_PROGRESS + " integer,")
        .append(COLUMN_TASK_DOWNLOAD_TASK_ID + " varchar(128),")
        .append(COLUMN_TASK_ERROR_CODE + " integer,")
        .append(COLUMN_TASK_ERROR_MSG + " varchar(512),")
        .append(COLUMN_TASK_COMBINATION_TYPE + " integer,")
        .append(COLUMN_TASK_DOWNLOAD_MODE + " integer,")
        .append(COLUMN_TASK_DOWNLOAD_ALLOW_NETWORK + " integer,")
        .append(COLUMN_TASK_DOWNLOAD_ORDER + " integer,")
        .append(COLUMN_TASK_UPGRADE_MODE + " integer,")
        .append(COLUMN_TASK_UPGRADE_ORDER + " integer,")
        .append(COLUMN_TASK_AUTH_RESULT + " varchar(10240),")
        .append(COLUMN_TASK_UPDATE_TOKEN + " varchar(256),")
        .append(COLUMN_TASK_DEVICE_TOKEN + " varchar(256),")
        .append(COLUMN_TASK_TOKEN_H_MAC + " varchar(256),")
        .append(COLUMN_TASK_CURR_VERSION_LIST + " varchar(1024),")
        .append(COLUMN_TASK_AUTH_TIME + " bigint,")
        .append(COLUMN_TASK_EXTENDS1 + " varchar(256),")
        .append(COLUMN_TASK_EXTENDS2 + " varchar(256),")
        .append(COLUMN_TASK_EXTENDS3 + " varchar(256),")
        .append(COLUMN_TASK_EXTENDS4 + " varchar(256),")
        .append(COLUMN_TASK_EXTENDS5 + " varchar(256),")
        .append(COLUMN_TASK_EXTENDS6 + " varchar(256)")
        .append(")");
}

void FirmwareTaskTable::ParseDbValue(ResultSet *resultSet, FirmwareTask &value)
{
    GetColumnValue(resultSet, COLUMN_TASK_TASK_ID, value.taskId);
    int32_t status = CAST_INT(UpgradeStatus::INIT);
    GetColumnValue(resultSet, COLUMN_TASK_STATUS, status);
    value.status = static_cast<UpgradeStatus>(status);
    GetColumnValue(resultSet, COLUMN_TASK_PROGRESS, value.progress);
    GetColumnValue(resultSet, COLUMN_TASK_DOWNLOAD_TASK_ID, value.downloadTaskId);
    GetColumnValue(resultSet, COLUMN_TASK_ERROR_CODE, value.errorCode);
    GetColumnValue(resultSet, COLUMN_TASK_ERROR_MSG, value.errorMsg);
    int32_t combinationType = CAST_INT(CombinationType::INVALLID_TYPE);
    GetColumnValue(resultSet, COLUMN_TASK_COMBINATION_TYPE, combinationType);
    value.combinationType = static_cast<CombinationType>(combinationType);
    int32_t downloadMode = CAST_INT(DownloadMode::MANUAL);
    GetColumnValue(resultSet, COLUMN_TASK_DOWNLOAD_MODE, downloadMode);
    value.downloadMode = static_cast<DownloadMode>(downloadMode);
    int32_t downloadAllowNetwork = CAST_INT(NetType::WIFI);
    GetColumnValue(resultSet, COLUMN_TASK_DOWNLOAD_ALLOW_NETWORK, downloadAllowNetwork);
    value.downloadAllowNetwork = static_cast<NetType>(downloadAllowNetwork);
    int32_t downloadOrder = CAST_INT(NetType::WIFI);
    GetColumnValue(resultSet, COLUMN_TASK_DOWNLOAD_ORDER, downloadOrder);
    value.downloadOrder = static_cast<Order>(downloadOrder);
    int32_t upgradeMode = CAST_INT(UpgradeMode::MANUAL);
    GetColumnValue(resultSet, COLUMN_TASK_UPGRADE_MODE, upgradeMode);
    value.upgradeMode = static_cast<UpgradeMode>(upgradeMode);
    int32_t upgradeOrder = CAST_INT(Order::INSTALL_AND_APPLY);
    GetColumnValue(resultSet, COLUMN_TASK_UPGRADE_ORDER, upgradeOrder);
    value.upgradeOrder = static_cast<Order>(upgradeOrder);
    GetColumnValue(resultSet, COLUMN_TASK_AUTH_RESULT, value.authResult);
    GetColumnValue(resultSet, COLUMN_TASK_UPDATE_TOKEN, value.updateToken);
    GetColumnValue(resultSet, COLUMN_TASK_DEVICE_TOKEN, value.deviceToken);
    GetColumnValue(resultSet, COLUMN_TASK_TOKEN_H_MAC, value.tokenHMac);
    GetColumnValue(resultSet, COLUMN_TASK_CURR_VERSION_LIST, value.currentVersionList);
    GetColumnValue(resultSet, COLUMN_TASK_AUTH_TIME, value.authTime);
}

void FirmwareTaskTable::BuildDbValue(const FirmwareTask &value, NativeRdb::ValuesBucket &dbValue)
{
    PutColumnValue(dbValue, COLUMN_TASK_TASK_ID, value.taskId);
    PutColumnValue(dbValue, COLUMN_TASK_STATUS, CAST_INT(value.status));
    PutColumnValue(dbValue, COLUMN_TASK_PROGRESS, value.progress);
    PutColumnValue(dbValue, COLUMN_TASK_DOWNLOAD_TASK_ID, value.downloadTaskId);
    PutColumnValue(dbValue, COLUMN_TASK_ERROR_CODE, value.errorCode);
    PutColumnValue(dbValue, COLUMN_TASK_ERROR_MSG, value.errorMsg);
    PutColumnValue(dbValue, COLUMN_TASK_COMBINATION_TYPE, CAST_INT(value.combinationType));
    PutColumnValue(dbValue, COLUMN_TASK_DOWNLOAD_MODE, CAST_INT(value.downloadMode));
    PutColumnValue(dbValue, COLUMN_TASK_DOWNLOAD_ALLOW_NETWORK, CAST_INT(value.downloadAllowNetwork));
    PutColumnValue(dbValue, COLUMN_TASK_DOWNLOAD_ORDER, CAST_INT(value.downloadOrder));
    PutColumnValue(dbValue, COLUMN_TASK_UPGRADE_MODE, CAST_INT(value.upgradeMode));
    PutColumnValue(dbValue, COLUMN_TASK_UPGRADE_ORDER, CAST_INT(value.upgradeOrder));
    PutColumnValue(dbValue, COLUMN_TASK_AUTH_RESULT, value.authResult);
    PutColumnValue(dbValue, COLUMN_TASK_UPDATE_TOKEN, value.updateToken);
    PutColumnValue(dbValue, COLUMN_TASK_DEVICE_TOKEN, value.deviceToken);
    PutColumnValue(dbValue, COLUMN_TASK_TOKEN_H_MAC, value.tokenHMac);
    PutColumnValue(dbValue, COLUMN_TASK_CURR_VERSION_LIST, value.currentVersionList);
    PutColumnValue(dbValue, COLUMN_TASK_AUTH_TIME, value.authTime);
}
} // namespace UpdateEngine
} // namespace OHOS