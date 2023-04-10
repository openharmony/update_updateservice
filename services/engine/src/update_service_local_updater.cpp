/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "update_service_local_updater.h"

#include "package/package.h"
#include "updaterkits/updaterkits.h"

#include "update_log.h"
#include "update_service_util.h"
#include "update_system_event.h"

namespace OHOS {
namespace UpdateEngine {
int32_t UpdateServiceLocalUpdater::ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile,
    const std::string &packageName, BusinessError &businessError)
{
#ifndef UPDATER_UT
    SYS_EVENT_SYSTEM_UPGRADE(0, UpdateSystemEvent::UPGRADE_START);
    businessError.errorNum = CallResult::SUCCESS;
    std::vector<std::string> packageNames;
    packageNames.push_back(packageName);
    int32_t ret = RebootAndInstallSdcardPackage(miscFile, packageNames) ? INT_CALL_SUCCESS : INT_CALL_FAIL;
    ENGINE_LOGI("ApplyNewVersion result : %{public}d", ret);
    SYS_EVENT_SYSTEM_UPGRADE(
        0, ret == INT_CALL_SUCCESS ? UpdateSystemEvent::EVENT_SUCCESS_RESULT : UpdateSystemEvent::EVENT_FAILED_RESULT);
    return ret;
#else
    return INT_CALL_SUCCESS;
#endif
}

int32_t UpdateServiceLocalUpdater::VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
    BusinessError &businessError)
{
    businessError.errorNum = CallResult::SUCCESS;
    int32_t ret = ::VerifyPackageWithCallback(packagePath.c_str(), keyPath.c_str(),
        [](int32_t result, uint32_t percent) {});
    ENGINE_LOGI("VerifyUpgradePackage %s, %s, %{public}d", packagePath.c_str(), keyPath.c_str(), ret);
    return (ret == 0) ? INT_CALL_SUCCESS : INT_CALL_FAIL;
}
} // namespace UpdateEngine
} // namespace OHOS
