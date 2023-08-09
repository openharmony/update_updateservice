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

#include "dupdate_net_manager.h"

#include <thread>
#include <unistd.h>

#include "update_define.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
NetManager::NetManager()
{
    observer_ = new NetObserver();
}

NetManager::~NetManager() {}

void NetManager::Init()
{
    ENGINE_LOGI("NetManager init");
    if (observer_ == nullptr) {
        ENGINE_LOGE("net ovserver is nullptr");
        return;
    }
    observer_->SetCallback(weak_from_this());
    observer_->StartObserver();
}

bool NetManager::RegisterCallback(
    NetChangeCallbackType callbackType, const std::set<NetType> &netTypes, NetObserverCallback cb)
{
    ENGINE_LOGI("RegisterCallback callbackType = %{public}d", callbackType);
    if (netTypes.empty()) {
        ENGINE_LOGE("RegisterCallback netTypes is empty");
        return false;
    }
    for (auto netType : netTypes) {
        if (!IsBaseNetType(netType)) {
            // 只允许注册基础网络类型
            ENGINE_LOGE("RegisterCallback unSurport netType = %{public}d", netType);
            return false;
        }
    }
    std::lock_guard<std::mutex> guard(netChangeMutex_);
    netChangeCallbackMap_[callbackType] = {netTypes, cb};
    return true;
}

void NetManager::UnregisterCallback(NetChangeCallbackType callbackType)
{
    std::lock_guard<std::mutex> guard(netChangeMutex_);
    for (auto iter = netChangeCallbackMap_.begin(); iter != netChangeCallbackMap_.end();) {
        if (iter->first == callbackType) {
            iter = netChangeCallbackMap_.erase(iter);
            break;
        }
        iter++;
    }
}

NetType NetManager::GetNetType()
{
    return netType_;
}

bool NetManager::IsNetAvailable(NetType netType)
{
    return (CAST_UINT(netType_) & CAST_UINT(netType)) > 0;
}

bool NetManager::OnNetChange(NetType netType)
{
    if (!IsBaseNetType(netType)) {
        ENGINE_LOGE("OnNetChange illegal netType = %{public}d", netType);
        return false;
    }
    if (netType == netType_) {
        return true;
    }
    netType_ = netType;
    ENGINE_LOGI("OnNetChange type = %{public}d", netType);
    std::lock_guard<std::mutex> guard(netChangeMutex_);
    for (auto iter = netChangeCallbackMap_.begin(); iter != netChangeCallbackMap_.end();) {
        NetChangeCallback netChangeCallback = iter->second;
        if (netChangeCallback.netTypes.count(netType) != 0) {
            netChangeCallback.function(netType);
        }
        iter++;
    }
    return true;
}

bool NetManager::IsBaseNetType(NetType netType)
{
    return netType == NetType::NO_NET || netType == NetType::CELLULAR || netType == NetType::METERED_WIFI ||
           netType == NetType::NOT_METERED_WIFI;
}
} // namespace UpdateEngine
} // namespace OHOS