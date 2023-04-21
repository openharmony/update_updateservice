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

#ifndef FIRMWARE_TASK_TABLE_H
#define FIRMWARE_TASK_TABLE_H

#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
#include "values_bucket.h"

#include "itable.h"
#endif
#include "firmware_task.h"

namespace OHOS {
namespace UpdateEngine {
const std::string FIRMWARE_TABLE_TASK = "task";
const std::string COLUMN_TASK_TASK_ID = "taskId";
const std::string COLUMN_TASK_STATUS = "status";
const std::string COLUMN_TASK_PROGRESS = "progress";
const std::string COLUMN_TASK_DOWNLOAD_TASK_ID = "downloadTaskId";
const std::string COLUMN_TASK_ERROR_CODE = "errorCode";
const std::string COLUMN_TASK_ERROR_MSG = "errorMsg";
const std::string COLUMN_TASK_COMBINATION_TYPE = "combinationType";

const std::string COLUMN_TASK_DOWNLOAD_MODE = "downloadMode";
const std::string COLUMN_TASK_DOWNLOAD_ALLOW_NETWORK = "downloadAllowNetwork";
const std::string COLUMN_TASK_DOWNLOAD_ORDER = "downloadOrder";
const std::string COLUMN_TASK_UPGRADE_MODE = "upgradeMode";
const std::string COLUMN_TASK_UPGRADE_ORDER = "upgradeOrder";

const std::string COLUMN_TASK_AUTH_RESULT = "authResult";
const std::string COLUMN_TASK_UPDATE_TOKEN = "updateToken";
const std::string COLUMN_TASK_DEVICE_TOKEN = "deviceToken";
const std::string COLUMN_TASK_TOKEN_H_MAC = "tokenHMac";
const std::string COLUMN_TASK_CURR_VERSION_LIST = "currentVersionList";
const std::string COLUMN_TASK_AUTH_TIME = "authTime";

// 预留6个字段
const std::string COLUMN_TASK_EXTENDS1 = "extends1";
const std::string COLUMN_TASK_EXTENDS2 = "extends2";
const std::string COLUMN_TASK_EXTENDS3 = "extends3";
const std::string COLUMN_TASK_EXTENDS4 = "extends4";
const std::string COLUMN_TASK_EXTENDS5 = "extends5";
const std::string COLUMN_TASK_EXTENDS6 = "extends6";

#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
class FirmwareTaskTable : public ITable<FirmwareTask> {
#else
class FirmwareTaskTable {
#endif
public:
    FirmwareTaskTable() = default;
    ~FirmwareTaskTable() = default;
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    std::string GetTableName() final;
    std::string GetTableCreateSql() final;
    void ParseDbValue(ResultSet *resultSet, FirmwareTask &value) final;
    void BuildDbValue(const FirmwareTask &value, NativeRdb::ValuesBucket &dbValue) final;
#else
    std::string GetTableName();
    std::string GetTableCreateSql();
#endif
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_TASK_TABLE_H
