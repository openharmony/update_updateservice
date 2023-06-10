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

#ifndef IDATABASE_H
#define IDATABASE_H

#include <vector>

#include "rdb_predicates.h"
#include "result_set.h"
#include "values_bucket.h"

namespace OHOS {
namespace UpdateEngine {
class IDataBase {
public:
    virtual ~IDataBase() = default;
    virtual bool DeleteDbStore() = 0;
    virtual bool Insert(const std::string &tableName, const std::vector<NativeRdb::ValuesBucket> &values) = 0;
    virtual bool Delete(int32_t &deletedRows, const NativeRdb::RdbPredicates &predicates) = 0;
    virtual std::shared_ptr<NativeRdb::ResultSet> Query(
        const NativeRdb::RdbPredicates &predicates, const std::vector<std::string> &columns) = 0;
    virtual bool Update(
        int &changedRows, const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicates) = 0;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // IDATABASE_H