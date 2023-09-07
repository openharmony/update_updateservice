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

#ifndef TABLE_BASE_OPERATOR_H
#define TABLE_BASE_OPERATOR_H

#include <memory>
#include <string>

#include "idatabase.h"
#include "itable.h"
#include "itable_operator.h"
#include "sqlite_db.h"
#include "update_define.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
template <typename Table, typename T>
class TableBaseOperator : public ITableOperator<T> {
public:
    explicit TableBaseOperator(std::shared_ptr<IDataBase> dataBase);
    virtual ~TableBaseOperator() = default;
    bool Insert(const T &value);
    bool Insert(const std::vector<T> &values) final;
    bool DeleteById(int id);
    bool DeleteAll();
    bool DeleteAll(int32_t &deletedRows) final;
    bool Delete(const NativeRdb::RdbPredicates &predicates);
    bool Delete(int32_t &deletedRows, const NativeRdb::RdbPredicates &predicates) final;
    bool QueryAll(std::vector<T> &results) final;
    bool Query(std::vector<T> &results, const NativeRdb::RdbPredicates &predicates) final;
    bool Update(const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicate);
    bool Update(
        int &changedRows, const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicate) final;

protected:
    std::string GetTableName();

private:
    std::shared_ptr<IDataBase> dataBase_ = nullptr;
    std::shared_ptr<ITable<T>> table_ = nullptr;
};

template <typename Table, typename T>
TableBaseOperator<Table, T>::TableBaseOperator(std::shared_ptr<IDataBase> dataBase)
{
    dataBase_ = dataBase;
    table_ = std::make_shared<Table>();
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::Insert(const T &value)
{
    std::vector<T> values{value};
    return Insert(values);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::Insert(const std::vector<T> &values)
{
    std::vector<NativeRdb::ValuesBucket> dbValues;
    ENGINE_CHECK(table_ != nullptr, return false, "TableBaseOperator Insert table is null");
    table_->BuildDbValues(values, dbValues);

    ENGINE_CHECK(dataBase_ != nullptr, return false, "TableBaseOperator Insert db is null");
    return dataBase_->Insert(GetTableName(), dbValues);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::DeleteAll()
{
    int32_t deletedRows = 0;
    return DeleteAll(deletedRows);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::DeleteAll(int32_t &deletedRows)
{
    NativeRdb::RdbPredicates predicates(GetTableName());
    return Delete(deletedRows, predicates);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::Delete(const NativeRdb::RdbPredicates &predicates)
{
    int32_t deletedRows = 0;
    return Delete(deletedRows, predicates);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::Delete(int32_t &deletedRows, const NativeRdb::RdbPredicates &predicates)
{
    ENGINE_CHECK(dataBase_ != nullptr, return false, "TableBaseOperator Delete db is null");
    return dataBase_->Delete(deletedRows, predicates);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::DeleteById(int id)
{
    ENGINE_CHECK(dataBase_ != nullptr, return false, "TableBaseOperator DeleteById db is null");
    OHOS::NativeRdb::RdbPredicates predicates(GetTableName());
    int32_t deletedRows = 0;
    predicates.EqualTo(COLUMN_ID, std::to_string(id));
    return dataBase_->Delete(deletedRows, predicates);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::QueryAll(std::vector<T> &results)
{
    OHOS::NativeRdb::RdbPredicates predicates(GetTableName());
    return Query(results, predicates);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::Query(std::vector<T> &results, const NativeRdb::RdbPredicates &predicates)
{
    std::vector<std::string> columns;
    ENGINE_CHECK(dataBase_ != nullptr, return false, "TableBaseOperator Query db is null");
    auto resultSet = dataBase_->Query(predicates, columns);
    ENGINE_CHECK(resultSet != nullptr, return false, "TableBaseOperator Query failed to get result");

    ENGINE_CHECK(table_ != nullptr, return false, "TableBaseOperator Query table is null");
    table_->ParseDbValues(std::move(resultSet), results);
    return true;
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::Update(
    const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicates)
{
    int changedRows = 0;
    return Update(changedRows, values, predicates);
}

template <typename Table, typename T>
bool TableBaseOperator<Table, T>::Update(
    int &changedRows, const NativeRdb::ValuesBucket &values, const NativeRdb::RdbPredicates &predicates)
{
    ENGINE_CHECK(dataBase_ != nullptr, return false, "TableBaseOperator Update db is null");
    return dataBase_->Update(changedRows, values, predicates);
}

template <typename Table, typename T>
std::string TableBaseOperator<Table, T>::GetTableName()
{
    if (table_ == nullptr) {
        ENGINE_LOGE("TableBaseOperator GetTableName table is null");
        return "default";
    }
    return table_->GetTableName();
}
} // namespace UpdateEngine
} // namespace OHOS
#endif // TABLE_BASE_OPERATOR_H