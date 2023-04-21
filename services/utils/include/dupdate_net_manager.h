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

#ifndef DUPDATE_NET_MANAGER_H
#define DUPDATE_NET_MANAGER_H

#include "singleton.h"

#include <set>

#include "dupdate_inet_observer.h"
#include "dupdate_net_observer.h"

namespace OHOS {
namespace UpdateEngine {
struct NetChangeCallback {
    std::set<NetType> netTypes;
    NetObserverCallback function;
};

class NetManager final : public INetObserverCallback,
                         public std::enable_shared_from_this<NetManager>,
                         public DelayedSingleton<NetManager> {
    DECLARE_DELAYED_SINGLETON(NetManager);

public:
    void Init();
    NetType GetNetType();
    bool IsNetAvailable(NetType netType = NetType::CELLULAR_AND_WIFI);

    bool RegisterCallback(
        NetChangeCallbackType callbackType, const std::set<NetType> &netTypes, NetObserverCallback cb);
    void UnregisterCallback(NetChangeCallbackType callbackType);

#ifdef NETMANAGER_BASE_ENABLE
    bool OnNetChange(NetType netType) final;
#else
    bool OnNetChange(NetType netType);
#endif

private:
    bool IsBaseNetType(NetType netType);

#ifdef NETMANAGER_BASE_ENABLE
    sptr<NetObserver> observer_ = nullptr;
#endif
    std::mutex netChangeMutex_;
    NetType netType_ = NetType::NO_NET;
    std::map<NetChangeCallbackType, NetChangeCallback> netChangeCallbackMap_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // DUPDATE_NET_MANAGER_H