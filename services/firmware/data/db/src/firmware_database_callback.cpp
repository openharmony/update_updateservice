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

#include "firmware_database_callback.h"

#include <vector>

#include "rdb_errno.h"

#include "itable.h"
#include "firmware_component_table.h"
#include "firmware_task_table.h"

namespace OHOS {
namespace UpdateEngine {
int FirmwareDatabaseCallback::OnCreate(NativeRdb::RdbStore &rdbStore)
{
    std::vector<std::string> dbCreateSqlVector = GenerateDbCreateSqlVector();
    for (auto &sql : dbCreateSqlVector) {
        int ret = rdbStore.ExecuteSql(sql);
        if (ret != NativeRdb::E_OK) {
            ENGINE_LOGE("FirmwareDatabaseCallback OnCreate create table %{public}s, ret=%{public}d", sql.c_str(), ret);
            return ret;
        }
    }
    ENGINE_LOGI("FirmwareDatabaseCallback OnCreate success");
    return NativeRdb::E_OK;
}

int FirmwareDatabaseCallback::OnUpgrade(NativeRdb::RdbStore &rdbStore, int oldVersion, int newVersion)
{
    ENGINE_LOGI("FirmwareDatabaseCallback OnUpgrade oldVersion=%{public}d newVersion=%{public}d", oldVersion,
        newVersion);
    return NativeRdb::E_OK;
}

int FirmwareDatabaseCallback::OnOpen(NativeRdb::RdbStore &rdbStore)
{
    ENGINE_LOGI("FirmwareDatabaseCallback OnOpen");
    return NativeRdb::E_OK;
}

std::vector<std::string> FirmwareDatabaseCallback::GenerateDbCreateSqlVector()
{
    std::vector<std::string> dbCreateSqlVector;
    dbCreateSqlVector.push_back(FirmwareComponentTable().GetTableCreateSql());
    dbCreateSqlVector.push_back(FirmwareTaskTable().GetTableCreateSql());
    return dbCreateSqlVector;
}
} // namespace UpdateEngine
} // namespace OHOS