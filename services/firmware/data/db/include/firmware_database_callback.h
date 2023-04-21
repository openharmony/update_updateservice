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

#ifndef FIRMWARE_DATABASE_CALLBACK_H
#define FIRMWARE_DATABASE_CALLBACK_H

#include <string>
#include <vector>

#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
#include "rdb_open_callback.h"
#include "rdb_store.h"
#endif

namespace OHOS {
namespace UpdateEngine {
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
class FirmwareDatabaseCallback final : public NativeRdb::RdbOpenCallback {
#else
class FirmwareDatabaseCallback final {
#endif
public:
#ifdef RELATIONAL_STORE_NATIVE_RDB_ENABLE
    int OnCreate(NativeRdb::RdbStore &rdbStore) final;
    int OnUpgrade(NativeRdb::RdbStore &rdbStore, int oldVersion, int newVersion) final;
    int OnOpen(NativeRdb::RdbStore &rdbStore) final;
#endif

private:
    std::vector<std::string> GenerateDbCreateSqlVector();
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_DATABASE_CALLBACK_H
