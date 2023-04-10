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
#include "sqlite_db.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareDatabase final : public SqliteDb, DelayedSingleton<FirmwareDatabase> {
    DECLARE_DELAYED_SINGLETON(FirmwareDatabase);

public:
    std::string GetDbName() final;
    int32_t GetDbVersion() final;
    void InitDbStoreDir() final;
    std::string GetDbStoreDir() final;
    NativeRdb::RdbOpenCallback &GetDbOpenCallback() final;

private:
    std::string dbStoreDir_;
    FirmwareDatabaseCallback dbOpenCallback_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_DATABASE_H
