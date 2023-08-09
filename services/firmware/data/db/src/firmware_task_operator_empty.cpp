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

#include "firmware_task_operator.h"

#include <memory>

#include "time_utils.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
bool FirmwareTaskOperator::Insert(const FirmwareTask &values)
{
    return true;
}

bool FirmwareTaskOperator::DeleteAll()
{
    return true;
}

void FirmwareTaskOperator::QueryTask(FirmwareTask &task)
{
    FIRMWARE_LOGI("FirmwareTaskOperator QueryTask");
}

bool FirmwareTaskOperator::UpdateProgressByTaskId(const std::string &taskId, UpgradeStatus status, int32_t progress)
{
    return true;
}

bool FirmwareTaskOperator::UpdateErrMsgByTaskId(const std::string &taskId, int errorCode, const std::string &errorMsg)
{
    return true;
}

bool FirmwareTaskOperator::UpdateDownloadTaskIdByTaskId(const std::string &taskId, const std::string &downloadTaskId)
{
    return true;
}

bool FirmwareTaskOperator::UpdateCombinationTypeByTaskId(const std::string &taskId,
    const CombinationType &combinationType)
{
    return true;
}

bool FirmwareTaskOperator::UpdateDownloadModeByTaskId(const std::string &taskId, DownloadMode downloadMode)
{
    return true;
}

bool FirmwareTaskOperator::UpdateDownloadAllowNetworkByTaskId(const std::string &taskId, NetType downloadAllowNetwork)
{
    return true;
}

bool FirmwareTaskOperator::UpdateDownloadOrderByTaskId(const std::string &taskId, Order downloadOrder)
{
    return true;
}

bool FirmwareTaskOperator::UpdateDownloadOptionByTaskId(
    const std::string &taskId, DownloadMode downloadMode, NetType downloadAllowNetwork, Order downloadOrder)
{
    return true;
}

bool FirmwareTaskOperator::UpdateUpgradeModeByTaskId(const std::string &taskId, UpgradeMode upgradeMode)
{
    return true;
}

bool FirmwareTaskOperator::UpdateUpgradeOrderByTaskId(const std::string &taskId, Order upgradeOrder)
{
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS
