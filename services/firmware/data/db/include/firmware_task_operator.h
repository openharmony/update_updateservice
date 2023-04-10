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

#ifndef FIRMWARE_TASK_OPERATOR_H
#define FIRMWARE_TASK_OPERATOR_H

#include "singleton.h"

#include "firmware_common.h"
#include "firmware_database.h"
#include "firmware_task_table.h"
#include "firmware_task.h"
#include "table_base_operator.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareTaskOperator final : public TableBaseOperator<FirmwareTaskTable, FirmwareTask> {
public:
    FirmwareTaskOperator() : TableBaseOperator(DelayedSingleton<FirmwareDatabase>::GetInstance()) {}
    ~FirmwareTaskOperator() = default;
    void QueryTask(FirmwareTask &task);
    bool UpdateProgressByTaskId(const std::string &taskId, UpgradeStatus status, int32_t progress);
    bool UpdateErrMsgByTaskId(const std::string &taskId, int errorCode, const std::string &errorMsg);
    bool UpdateDownloadTaskIdByTaskId(const std::string &taskId, const std::string &downloadTaskId);
    bool UpdateCombinationTypeByTaskId(const std::string &taskId,
        const CombinationType &combinationType);

    bool UpdateDownloadModeByTaskId(const std::string &taskId, DownloadMode downloadMode);
    bool UpdateDownloadAllowNetworkByTaskId(const std::string &taskId, NetType downloadAllowNetwork);
    bool UpdateDownloadOrderByTaskId(const std::string &taskId, Order downloadOrder);
    bool UpdateDownloadOptionByTaskId(const std::string &taskId,
        DownloadMode downloadMode, NetType downloadAllowNetwork, Order downloadOrder);

    bool UpdateUpgradeModeByTaskId(const std::string &taskId, UpgradeMode upgradeMode);
    bool UpdateUpgradeOrderByTaskId(const std::string &taskId, Order upgradeOrder);

private:
    bool UpdateByTaskId(const std::string &taskId, const NativeRdb::ValuesBucket &values);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_TASK_OPERATOR_H