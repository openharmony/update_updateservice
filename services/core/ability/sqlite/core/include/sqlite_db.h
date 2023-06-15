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

#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <memory>
#include <mutex>
#include <string>

#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "result_set.h"

#include "idatabase.h"

namespace OHOS {
namespace UpdateEngine {
class SqliteDb : public IDataBase {
public:
    SqliteDb() = default;
    virtual ~SqliteDb() = default;
    bool DeleteDbStore() final;
    bool Insert(const std::string &tableName, const std::vector<NativeRdb::ValuesBucket> &values) final;
    bool Delete(int32_t &deletedRows, const NativeRdb::RdbPredicates &predicates) final;
    std::shared_ptr<NativeRdb::ResultSet> Query(
        const NativeRdb::RdbPredicates &predicates, const std::vector<std::string> &columns) final;
    bool Update(
        int &changedRows, const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicates) final;

protected:
    virtual std::string GetDbName() = 0;
    virtual int32_t GetDbVersion() = 0;
    virtual std::string GetDbStoreDir() = 0;
    virtual void InitDbStoreDir() = 0;
    virtual NativeRdb::RdbOpenCallback &GetDbOpenCallback() = 0;

private:
    std::shared_ptr<NativeRdb::RdbStore> GetDbStore();
    std::shared_ptr<NativeRdb::RdbStore> CreateDbStore();

private:
    std::shared_ptr<NativeRdb::RdbStore> dbStore_ = nullptr;
    std::mutex dbMutex_;
    std::mutex writeMutex_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // SQLITE_DB_H