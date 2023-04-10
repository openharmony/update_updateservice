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

#include "firmware_flow_manager.h"

#include "firmware_apply_executor.h"
#include "firmware_check_executor.h"
#include "firmware_common.h"
#include "firmware_download_executor.h"
#include "firmware_iexecute_mode.h"
#include "firmware_install_executor.h"
#include "firmware_log.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareFlowManager::FirmwareFlowManager()
{
    FIRMWARE_LOGD("FirmwareFlowManager::FirmwareFlowManager()");
}

FirmwareFlowManager::~FirmwareFlowManager()
{
    FIRMWARE_LOGD("FirmwareFlowManager::~FirmwareFlowManager()");
}

void FirmwareFlowManager::SetExecuteMode(std::shared_ptr<FirmwareIExecuteMode> executeMode)
{
    executeMode_ = executeMode;
}

void FirmwareFlowManager::Start()
{
    if (executeMode_ == nullptr) {
        FIRMWARE_LOGE("FirmwareFlowManager executeMode is null");
        return;
    }
    Execute();
}

void FirmwareFlowManager::Execute()
{
    if (executeMode_ == nullptr) {
        FIRMWARE_LOGE("FirmwareFlowManager executeMode is null");
        return;
    }
    nextStep_ = executeMode_->GetNextStep(nextStep_);
    FIRMWARE_LOGI("FirmwareFlowManager::Execute %{public}d", static_cast<int>(nextStep_));
    if (nextStep_ == FirmwareStep::COMPLETE) {
        Complete();
    } else {
        executor_ = CreateInstance(nextStep_);
        if (executor_ != nullptr) {
            executor_->Execute();
        } else {
            FIRMWARE_LOGI("FirmwareFlowManager::Execute null");
        }
    }
}

void FirmwareFlowManager::Complete()
{
    FIRMWARE_LOGI("FirmwareFlowManager::complete");
    if (executeMode_ == nullptr) {
        FIRMWARE_LOGE("FirmwareFlowManager executeMode is null");
        return;
    }
    executeMode_->HandleComplete();
}

void FirmwareFlowManager::OnCheckCallback(CheckStatus status, const Duration &duration,
    const std::vector<FirmwareComponent> &componentList, const CheckAndAuthInfo &checkAndAuthInfo)
{
    FIRMWARE_LOGI("FirmwareFlowManager::OnCheckCallback");
    if (executeMode_ == nullptr) {
        FIRMWARE_LOGE("FirmwareFlowManager executeMode is null");
        return;
    }
    executeMode_->SetCheckResult(status, duration, componentList, checkAndAuthInfo);
    Execute();
}

void FirmwareFlowManager::OnDownloadCallback(const Progress &progress)
{
    FIRMWARE_LOGI("FirmwareFlowManager::OnDownloadCallback");
    if (executeMode_ == nullptr) {
        FIRMWARE_LOGE("FirmwareFlowManager executeMode is null");
        return;
    }
    executeMode_->SetDownloadProgress(progress);
    if (progress.status != UpgradeStatus::DOWNLOADING) {
        Execute();
    }
}

void FirmwareFlowManager::OnInstallCallback(const InstallCallbackInfo &installCallbackInfo)
{
    FIRMWARE_LOGI("FirmwareFlowManager::OnInstallCallback");
    if (executeMode_ == nullptr) {
        FIRMWARE_LOGE("FirmwareFlowManager executeMode is null");
        return;
    }
    executeMode_->SetInstallResult(installCallbackInfo);
    if (installCallbackInfo.progress.status != UpgradeStatus::INSTALLING) {
        Execute();
    }
}

void FirmwareFlowManager::OnApplyCallback(bool isSuccess)
{
    FIRMWARE_LOGI("ParamFlowManager::apply %{public}s", isSuccess ? "success" : "fail");
    executeMode_->SetApplyResult(isSuccess);
    Execute();
}

std::shared_ptr<FirmwareIExecutor> FirmwareFlowManager::CreateInstance(FirmwareStep step)
{
    std::shared_ptr<FirmwareIExecutor> executor = nullptr;
    switch (step) {
        case FirmwareStep::CHECK_STEP: {
            FirmwareCheckComponentCallback checkCallback {[=](CheckStatus status, const Duration &duration,
                const std::vector<FirmwareComponent> &componentList, const CheckAndAuthInfo &checkAndAuthInfo) {
                OnCheckCallback(status, duration, componentList, checkAndAuthInfo);
            }};
            executor = std::make_shared<FirmwareCheckExecutor>(checkCallback);
            break;
        }
        case FirmwareStep::DOWNLOAD_STEP: {
            FirmwareProgressCallback downloadCallback{[=](const Progress &progress) { OnDownloadCallback(progress); }};
            executor = std::make_shared<FirmwareDownloadExecutor>(executeMode_->GetDownloadOptions(), downloadCallback);
            break;
        }
        case FirmwareStep::INSTALL_STEP: {
            FirmwareInstallExecutorCallback installCallback {[=](const InstallCallbackInfo &installCallbackInfo) {
                OnInstallCallback(installCallbackInfo);
            }};
            executor = std::make_shared<FirmwareInstallExecutor>(executeMode_->GetInstallType(), installCallback);
            break;
        }
        case FirmwareStep::APPLY_STEP: {
            FirmwareApplyCallback firmwareApplyCallback{[=](bool isSuccess) { OnApplyCallback(isSuccess); }};
            executor = std::make_shared<FirmwareApplyExecutor>(firmwareApplyCallback);
            break;
        }
        default:
            FIRMWARE_LOGI("FirmwareFlowManager::CreateInstance NULL");
            break;
    }
    return executor;
}
} // namespace UpdateEngine
} // namespace OHOS