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

#include "sqlite_db.h"

#include <string>
#include <vector>

#include "rdb_errno.h"
#include "rdb_helper.h"

#include "update_define.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
std::shared_ptr<NativeRdb::RdbStore> SqliteDb::GetDbStore()
{
    if (dbStore_ == nullptr) {
        std::lock_guard<std::mutex> lockGuard(dbMutex_);
        if (dbStore_ == nullptr) {
            dbStore_ = CreateDbStore();
        }
    }
    return dbStore_;
}

std::shared_ptr<NativeRdb::RdbStore> SqliteDb::CreateDbStore()
{
    std::string dbStoreDir = GetDbStoreDir();
    ENGINE_CHECK(!dbStoreDir.empty(), return nullptr, "SqliteDb failed to create: path is empty");

    NativeRdb::RdbStoreConfig config(dbStoreDir);
    config.SetSecurityLevel(NativeRdb::SecurityLevel::S1);
    int ret = NativeRdb::E_OK;
    std::shared_ptr<NativeRdb::RdbStore> dbStore =
        NativeRdb::RdbHelper::GetRdbStore(config, GetDbVersion(), GetDbOpenCallback(), ret);
    if (ret != NativeRdb::E_OK || dbStore == nullptr) {
        ENGINE_LOGE("SqliteDb failed to create db store %{public}s, ret=%{public}d", dbStoreDir.c_str(), ret);
        return nullptr;
    }
    return dbStore;
}

bool SqliteDb::DeleteDbStore()
{
    std::lock_guard<std::mutex> lockGuard(dbMutex_);
    int ret = NativeRdb::RdbHelper::DeleteRdbStore(GetDbStoreDir());
    dbStore_ = nullptr;
    ENGINE_LOGE("SqliteDb DeleteDbStore ret=%{public}d", ret);
    return ret == NativeRdb::E_OK;
}

bool SqliteDb::Insert(const std::string &tableName, const std::vector<NativeRdb::ValuesBucket> &values)
{
    std::lock_guard<std::mutex> lockGuard(writeMutex_);
    std::shared_ptr<NativeRdb::RdbStore> dbStore = GetDbStore();
    ENGINE_CHECK(dbStore != nullptr, return false, "SqliteDb failed to create: db is null");

    int64_t insertNum = 0;
    int ret = dbStore->BatchInsert(insertNum, tableName, values);
    ENGINE_CHECK(ret == NativeRdb::E_OK, return false, "SqliteDb failed to insert ret=%{public}d", ret);
    return true;
}

bool SqliteDb::Delete(int32_t &deletedRows, const NativeRdb::RdbPredicates &predicates)
{
    std::shared_ptr<NativeRdb::RdbStore> dbStore = GetDbStore();
    ENGINE_CHECK(dbStore != nullptr, return false, "SqliteDb failed to create: db is null");

    int32_t ret = dbStore->Delete(deletedRows, predicates);
    ENGINE_CHECK(ret == NativeRdb::E_OK, return false, "SqliteDb failed to delete ret=%{public}d", ret);
    return true;
}

std::shared_ptr<NativeRdb::ResultSet> SqliteDb::Query(
    const NativeRdb::RdbPredicates &predicates, const std::vector<std::string> &columns)
{
    std::shared_ptr<NativeRdb::RdbStore> dbStore = GetDbStore();
    ENGINE_CHECK(dbStore != nullptr, return nullptr, "SqliteDb failed to create: db is null");
    return dbStore->Query(predicates, columns);
}

bool SqliteDb::Update(
    int &changedRows, const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicates)
{
    std::shared_ptr<NativeRdb::RdbStore> dbStore = GetDbStore();
    ENGINE_CHECK(dbStore != nullptr, return false, "SqliteDb failed to create: db is null");
    int ret = dbStore->Update(changedRows, values, predicates);
    ENGINE_CHECK(ret == NativeRdb::E_OK, return false, "SqliteDb failed to update ret=%{public}d", ret);
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS