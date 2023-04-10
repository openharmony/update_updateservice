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

#ifndef ITABLE_OPERATOR_H
#define ITABLE_OPERATOR_H

#include <vector>

#include "rdb_predicates.h"
#include "values_bucket.h"

namespace OHOS {
namespace UpdateEngine {
template <typename T>
class ITableOperator {
public:
    virtual ~ITableOperator() = default;
    virtual bool Insert(const std::vector<T> &value) = 0;
    virtual bool DeleteAll(int32_t &deletedRows) = 0;
    virtual bool Delete(int32_t &deletedRows, const NativeRdb::RdbPredicates &predicates) = 0;
    virtual bool QueryAll(std::vector<T> &result) = 0;
    virtual bool Query(std::vector<T> &results, const NativeRdb::RdbPredicates &predicates) = 0;
    virtual bool Update(
        int &changedRows, const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicates) = 0;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ITABLE_OPERATOR_H