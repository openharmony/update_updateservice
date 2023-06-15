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

#ifndef ITABLE_H
#define ITABLE_H

#include <vector>

#include "rdb_errno.h"
#include "result_set.h"
#include "values_bucket.h"

#include "update_define.h"
#include "update_log.h"

using ResultSet = OHOS::NativeRdb::ResultSet;

namespace OHOS {
namespace UpdateEngine {
const std::string COLUMN_ID = "id";

template <typename T>
class ITable {
public:
    virtual ~ITable() = default;
    virtual std::string GetTableName() = 0;
    virtual std::string GetTableCreateSql() = 0;

    void ParseDbValues(std::shared_ptr<ResultSet> resultSet, std::vector<T> &values)
    {
        ENGINE_CHECK(resultSet != nullptr, return, "ParseDbValues resultSet is null");
        resultSet->GoToFirstRow();
        bool isEnded = true;
        resultSet->IsEnded(isEnded);
        while (!isEnded) {
            T value;
            ParseDbValue(resultSet.get(), value);
            values.push_back(value);
            resultSet->GoToNextRow();
            resultSet->IsEnded(isEnded);
        }
    }

    void BuildDbValues(const std::vector<T> &values, std::vector<NativeRdb::ValuesBucket> &dbValues)
    {
        for (T value : values) {
            NativeRdb::ValuesBucket genericValue;
            BuildDbValue(value, genericValue);
            dbValues.push_back(genericValue);
        }
    }

protected:
    template <typename ValueType>
    void GetColumnValue(ResultSet *resultSet, const std::string &columnName, ValueType &value)
    {
        ENGINE_CHECK(resultSet != nullptr, return, "GetColumnValue resultSet is null");
        int index;
        int ret = resultSet->GetColumnIndex(columnName, index);
        if (ret != NativeRdb::E_OK) {
            ENGINE_LOGE("GetColumnValue %{public}s column index fail, ret = %{public}d", columnName.c_str(), ret);
            return;
        }
        GetIndexValue(resultSet, index, value);
    }

    void PutColumnValue(NativeRdb::ValuesBucket &dbValue, const std::string &columnName, const std::string &value)
    {
        dbValue.PutString(columnName, value);
    }

    void PutColumnValue(NativeRdb::ValuesBucket &dbValue, const std::string &columnName, const int32_t &value)
    {
        dbValue.PutInt(columnName, value);
    }

    void PutColumnValue(NativeRdb::ValuesBucket &dbValue, const std::string &columnName, const int64_t &value)
    {
        dbValue.PutLong(columnName, value);
    }

    void PutColumnValue(NativeRdb::ValuesBucket &dbValue, const std::string &columnName, const double &value)
    {
        dbValue.PutDouble(columnName, value);
    }

    void PutColumnValue(NativeRdb::ValuesBucket &dbValue, const std::string &columnName, const bool &value)
    {
        int32_t intValue = value ? 1 : 0;
        dbValue.PutInt(columnName, intValue);
    }

private:
    virtual void ParseDbValue(ResultSet *resultSet, T &value) = 0;
    virtual void BuildDbValue(const T &value, NativeRdb::ValuesBucket &dbValue) = 0;

    int GetIndexValue(ResultSet *resultSet, int index, std::string &value)
    {
        return resultSet->GetString(index, value);
    }

    int GetIndexValue(ResultSet *resultSet, int index, int &value)
    {
        return resultSet->GetInt(index, value);
    }

    int GetIndexValue(ResultSet *resultSet, int index, int64_t &value)
    {
        return resultSet->GetLong(index, value);
    }

    int GetIndexValue(ResultSet *resultSet, int index, double &value)
    {
        return resultSet->GetDouble(index, value);
    }

    int GetIndexValue(ResultSet *resultSet, int index, bool &value)
    {
        int32_t intValue = 0;
        int ret = resultSet->GetInt(index, intValue);
        value = (intValue != 0);
        return ret;
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // ITABLE_H