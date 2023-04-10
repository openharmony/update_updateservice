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

#include "firmware_update_helper.h"

namespace OHOS {
namespace UpdateEngine {
std::string FirmwareUpdateHelper::BuildTaskId(const std::vector<FirmwareComponent> &componentList)
{
    if (componentList.empty()) {
        return "";
    }
    std::string taskId;
    for (const FirmwareComponent &component : componentList) {
        if (!taskId.empty()) {
            taskId.append("_");
        }
        taskId.append(component.versionId);
    }
    return Sha256Utils::CalculateHashCode(taskId);
}

void FirmwareUpdateHelper::ClearFirmwareInfo()
{
    FIRMWARE_LOGI("ClearFirmwareInfo");
    std::shared_ptr<FirmwarePreferencesUtil> preferencesUtil = DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();

    FirmwareTask task;
    FirmwareTaskOperator firmwareTaskOperator;
    firmwareTaskOperator.QueryTask(task);
    firmwareTaskOperator.DeleteAll();
    FirmwareComponentOperator().DeleteAll();
    FirmwareFileUtils::DeleteDownloadFiles();
}

// 此处为桩函数，默认电量足够。
bool FirmwareUpdateHelper::IsBatteryEnough(int32_t batteryLimit)
{
    return true;
}

InstallType FirmwareUpdateHelper::GetInstallType()
{
    InstallType installType = (DeviceAdapter::GetBootSlot()) == Firmware::BOOTSLOTS_AB_UPDATE_VALUE ?
        InstallType::SYS_INSTALLER : InstallType::UPDATER;
    FIRMWARE_LOGI("get installType(bootsloot) %{public}d", CAST_INT(installType));
    return installType;
}

VersionComponent FirmwareUpdateHelper::BuildHotaVersionComponent(std::vector<FirmwareComponent> &hotaComponents)
{
    VersionComponent hotaVersionComponent;
    hotaVersionComponent.componentId = hotaComponents[0].componentId;
    hotaVersionComponent.upgradeAction = UpgradeAction::UPGRADE;
    hotaVersionComponent.componentType = CAST_INT(ComponentType::OTA);
    hotaVersionComponent.effectiveMode = FirmwareUpdateHelper::GetInstallType() == InstallType::SYS_INSTALLER ?
        static_cast<size_t>(EffectiveMode::LIVE_AND_COLD): static_cast<size_t>(EffectiveMode::COLD);
    hotaVersionComponent.innerVersion = hotaComponents[0].targetBlVersionNumber;
    hotaVersionComponent.displayVersion = hotaComponents[0].targetBlDisplayVersionNumber;
    for (const auto &component : hotaComponents) {
        hotaVersionComponent.size += static_cast<size_t>(component.size);
    }
    return hotaVersionComponent;
}

void FirmwareUpdateHelper::BuildNewVersionInfo(const std::vector<FirmwareComponent> &components,
    std::vector<VersionComponent> &versionComponents)
{
    if (components.empty()) {
        FIRMWARE_LOGI("BuildNewVersionInfo component is null");
        return;
    }
    std::vector<FirmwareComponent> hotaComponents;
    for (auto &component : components) {
        if (component.blVersionType == CAST_INT(BlType::HOTA_TYPE)) {
            hotaComponents.push_back(component);
        }
        if (component.blVersionType == CAST_INT(BlType::DYNAMIC_TYPE)) {
            hotaComponents.push_back(component);
        }
    }

    std::string hotaVersionNumber;
    std::string hotaDisplayVersionNumber;
    if (!hotaComponents.empty()) {
        VersionComponent hotaVersionComponent = BuildHotaVersionComponent(hotaComponents);
        hotaVersionNumber = hotaVersionComponent.innerVersion;
        hotaDisplayVersionNumber = hotaVersionComponent.displayVersion;
        versionComponents.push_back(hotaVersionComponent);
    } else {
        hotaVersionNumber = DeviceAdapter::GetRealVersion();
        hotaDisplayVersionNumber = DeviceAdapter::GetDisplayVersion();
    }
}

void FirmwareUpdateHelper::BuildCurrentVersionInfo(CurrentVersionInfo &currentVersionInfo)
{
    currentVersionInfo.osVersion = DeviceAdapter::GetOsVersion();
    currentVersionInfo.deviceName = DeviceAdapter::GetDeviceName();

    std::shared_ptr<FirmwarePreferencesUtil> utils = DelayedSingleton<FirmwarePreferencesUtil>::GetInstance();
    VersionComponent hotaVersionComponent;
    hotaVersionComponent.displayVersion = DeviceAdapter::GetDisplayVersion();
    hotaVersionComponent.innerVersion = DeviceAdapter::GetRealVersion();
    hotaVersionComponent.descriptionInfo.descriptionType = DescriptionType::ID;
    hotaVersionComponent.componentId = utils->ObtainString(Firmware::HOTA_CURRENT_COMPONENT_ID, "");
    hotaVersionComponent.componentType = CAST_INT(ComponentType::OTA);
    currentVersionInfo.versionComponents.push_back(hotaVersionComponent);
}

bool FirmwareUpdateHelper::IsUpgradePackagesReady(const std::vector<FirmwareComponent> &componentList)
{
    if (componentList.empty()) {
        FIRMWARE_LOGE("IsUpgradePackagesReady: componentList is null!");
        return false;
    }

    return std::all_of(componentList.begin(), componentList.end(), [](const FirmwareComponent &component) {
        if (!FileUtils::IsFileExist(component.spath)) {
            FIRMWARE_LOGE("IsUpgradePackagesReady: package [%{public}s] is not exist!", component.spath.c_str());
            return false;
        }
        int64_t realSize = FileUtils::GetFileSize(component.spath);
        if (realSize != component.size) {
            FIRMWARE_LOGE("IsUpgradePackagesReady: calculate component size %{public}s != DB component size %{public}s",
                std::to_string(realSize).c_str(), std::to_string(component.size).c_str());
            return false;
        }
        return true;
    });
}
} // namespace UpdateEngine
} // namespace OHOS