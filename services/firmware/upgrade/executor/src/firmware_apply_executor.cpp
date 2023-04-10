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

#include "firmware_apply_executor.h"

#include <unistd.h>

#include "init_reboot.h"

#include "firmware_callback_utils.h"
#include "firmware_constant.h"
#include "firmware_component_operator.h"
#include "firmware_log.h"
#include "firmware_task_operator.h"
#include "firmware_update_helper.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
void FirmwareApplyExecutor::Execute()
{
    FIRMWARE_LOGI("FirmwareApplyExecutor::Execute");
    std::thread installThread(&FirmwareApplyExecutor::DoInstall, this);
    installThread.detach();
}

void FirmwareApplyExecutor::DoInstall()
{
    FIRMWARE_LOGI("FirmwareApplyExecutor DoInstall");
    FirmwareComponentOperator firmwareComponentOperator;
    firmwareComponentOperator.QueryAll(components_);
    FIRMWARE_LOGI("Execute size %{public}d", CAST_INT(components_.size()));
    if (components_.size() == 0) {
        firmwareApplyCallback_.applyCallback(false);
        return;
    }
    GetTask();
    FirmwareTaskOperator().UpdateProgressByTaskId(tasks_.taskId, UpgradeStatus::UPDATING, 0);
    sleep(Firmware::SLEEP_TIME_BEFORE_REBOOT);
    DoReboot(NULL); // ab apply
}

void FirmwareApplyExecutor::GetTask()
{
    if (!tasks_.isExistTask) {
        FirmwareTaskOperator().QueryTask(tasks_);
    }
}
} // namespace UpdateEngine
} // namespace OHOS
