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

#include "firmware_database.h"

#include "constant.h"

namespace OHOS {
namespace UpdateEngine {
const std::string FIRMWARE_DB_NAME = "/firmware_sqlite.db";
constexpr int32_t FIRMWARE_DB_VERSION = 1;
FirmwareDatabase::FirmwareDatabase()
{
    InitDbStoreDir();
}

FirmwareDatabase::~FirmwareDatabase() {}

std::string FirmwareDatabase::GetDbName()
{
    return FIRMWARE_DB_NAME;
}

int32_t FirmwareDatabase::GetDbVersion()
{
    return FIRMWARE_DB_VERSION;
}

void FirmwareDatabase::InitDbStoreDir()
{
    dbStoreDir_ = Constant::DATABASES_ROOT_PATH + FIRMWARE_DB_NAME;
}

std::string FirmwareDatabase::GetDbStoreDir()
{
    return dbStoreDir_;
}

NativeRdb::RdbOpenCallback &FirmwareDatabase::GetDbOpenCallback()
{
    return dbOpenCallback_;
}
} // namespace UpdateEngine
} // namespace OHOS
