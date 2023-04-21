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

#ifndef FIRMWARE_DATABASE_H
#define FIRMWARE_DATABASE_H

#include "singleton.h"

#include "firmware_database_callback.h"
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
#include "sqlite_db.h"
#endif

namespace OHOS {
namespace UpdateEngine {
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
class FirmwareDatabase final : public SqliteDb, DelayedSingleton<FirmwareDatabase> {
    DECLARE_DELAYED_SINGLETON(FirmwareDatabase);
#else
class FirmwareDatabase final {
#endif
public:
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    std::string GetDbName() final;
    int32_t GetDbVersion() final;
    void InitDbStoreDir() final;
    std::string GetDbStoreDir() final;
    NativeRdb::RdbOpenCallback &GetDbOpenCallback() final;
#else
    FirmwareDatabase();
    ~FirmwareDatabase();
    std::string GetDbName();
    int32_t GetDbVersion();
    void InitDbStoreDir();
    std::string GetDbStoreDir();
    bool DeleteDbStore();
#endif

private:
    std::string dbStoreDir_;
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    FirmwareDatabaseCallback dbOpenCallback_;
#endif
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_DATABASE_H
