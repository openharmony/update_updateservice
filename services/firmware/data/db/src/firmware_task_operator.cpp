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

#include "firmware_task_operator.h"

#include <memory>

#include "time_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
#ifndef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
bool FirmwareTaskOperator::Insert(const FirmwareTask &values)
{
    return true;
}

void FirmwareTaskOperator::DeleteAll()
{
    return true;
}
#endif

void FirmwareTaskOperator::QueryTask(FirmwareTask &task)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    std::vector<FirmwareTask> taskList;
    QueryAll(taskList);
    if (taskList.empty()) {
        FIRMWARE_LOGI("FirmwareTaskOperator QueryTask no task");
        task.isExistTask = false;
        return;
    }
    task = taskList[0];
    task.isExistTask = true;
#endif
}

bool FirmwareTaskOperator::UpdateProgressByTaskId(const std::string &taskId, UpgradeStatus status, int32_t progress)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_STATUS, CAST_INT(status));
    values.PutInt(COLUMN_TASK_PROGRESS, progress);
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateErrMsgByTaskId(const std::string &taskId, int errorCode, const std::string &errorMsg)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_ERROR_CODE, errorCode);
    values.PutString(COLUMN_TASK_ERROR_MSG, errorMsg);
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateDownloadTaskIdByTaskId(const std::string &taskId, const std::string &downloadTaskId)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutString(COLUMN_TASK_DOWNLOAD_TASK_ID, downloadTaskId);
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateCombinationTypeByTaskId(const std::string &taskId,
    const CombinationType &combinationType)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_COMBINATION_TYPE, CAST_INT(combinationType));
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateDownloadModeByTaskId(const std::string &taskId, DownloadMode downloadMode)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_DOWNLOAD_MODE, CAST_INT(downloadMode));
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateDownloadAllowNetworkByTaskId(const std::string &taskId, NetType downloadAllowNetwork)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_DOWNLOAD_ALLOW_NETWORK, CAST_INT(downloadAllowNetwork));
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateDownloadOrderByTaskId(const std::string &taskId, Order downloadOrder)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_DOWNLOAD_ORDER, CAST_INT(downloadOrder));
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateDownloadOptionByTaskId(
    const std::string &taskId, DownloadMode downloadMode, NetType downloadAllowNetwork, Order downloadOrder)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_DOWNLOAD_MODE, CAST_INT(downloadMode));
    values.PutInt(COLUMN_TASK_DOWNLOAD_ALLOW_NETWORK, CAST_INT(downloadAllowNetwork));
    values.PutInt(COLUMN_TASK_DOWNLOAD_ORDER, CAST_INT(downloadOrder));
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateUpgradeModeByTaskId(const std::string &taskId, UpgradeMode upgradeMode)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_UPGRADE_MODE, CAST_INT(upgradeMode));
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

bool FirmwareTaskOperator::UpdateUpgradeOrderByTaskId(const std::string &taskId, Order upgradeOrder)
{
#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
    NativeRdb::ValuesBucket values;
    values.PutInt(COLUMN_TASK_UPGRADE_ORDER, CAST_INT(upgradeOrder));
    return UpdateByTaskId(taskId, values);
#else
    return true;
#endif
}

#ifdef RELATIONAL_SOTRE_NATIVE_RDB_ENABLE
bool FirmwareTaskOperator::UpdateByTaskId(const std::string &taskId, const NativeRdb::ValuesBucket &values)
{
    OHOS::NativeRdb::RdbPredicates predicates(GetTableName());
    predicates.EqualTo(COLUMN_TASK_TASK_ID, taskId);
    return TableBaseOperator::Update(values, predicates);
}
#endif
} // namespace UpdateEngine
} // namespace OHOS
