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

#include "firmware_changelog_utils.h"

#include "firmware_component_operator.h"
#include "firmware_constant.h"
#include "firmware_log.h"
#include "firmware_preferences_utils.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareChangelogUtils::FirmwareChangelogUtils() {}

FirmwareChangelogUtils::~FirmwareChangelogUtils() {}
std::string FirmwareChangelogUtils::GetChangelogDir()
{
    return Firmware::CHANGELOG_PATH;
}

    std::vector<std::string> FirmwareChangelogUtils::GetNewVersionComponentIds()
{
    std::vector<std::string> componentIds;
    std::vector<FirmwareComponent> dbComponentList;
    FirmwareComponentOperator().QueryAll(dbComponentList);
    for (FirmwareComponent &component : dbComponentList) {
        if (std::find(componentIds.begin(), componentIds.end(), component.componentId) == componentIds.end()) {
            FIRMWARE_LOGI("new version componentId %{public}s", component.componentId.c_str());
            componentIds.push_back(component.componentId);
        }
    }

    return componentIds;
}

std::vector<std::string> FirmwareChangelogUtils::GetCurrentVersionComponentIds()
{
    std::vector<std::string> componentIds;
    std::shared_ptr<FirmwarePreferencesUtil> preferencesUtil = DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
    if (preferencesUtil->IsExist(Firmware::HOTA_CURRENT_COMPONENT_ID)) {
        componentIds.push_back(preferencesUtil->ObtainString(Firmware::HOTA_CURRENT_COMPONENT_ID, ""));
    }

    return componentIds;
}

void FirmwareChangelogUtils::SaveHotaCurrentVersionComponentId()
{
    std::shared_ptr<FirmwarePreferencesUtil> preferencesUtil = DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
    std::vector<FirmwareComponent> components;
    FirmwareComponentOperator().QueryAll(components);
    for (auto const &component : components) {
        if (component.versionPackageType == PackageType::NORMAL) {
            preferencesUtil->SaveString(Firmware::HOTA_CURRENT_COMPONENT_ID, component.componentId);
            return;
        }
    }
}

void FirmwareChangelogUtils::GetAllComponentIds(std::vector<std::string> &componentIds)
{
    std::vector<std::string> curComponentIds = GetCurrentVersionComponentIds();
    std::vector<std::string> newComponentIds = GetNewVersionComponentIds();

    componentIds.insert(componentIds.end(), curComponentIds.begin(), curComponentIds.end());
    componentIds.insert(componentIds.end(), newComponentIds.begin(), newComponentIds.end());
}
} // namespace UpdateEngine
} // namespace OHOS
