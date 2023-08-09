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

#ifndef DUPDATE_NET_OBSERVER_H
#define DUPDATE_NET_OBSERVER_H

#include "net_all_capabilities.h"
#include "net_conn_callback_stub.h"
#include "net_specifier.h"

#include "dupdate_inet_observer.h"

namespace OHOS {
namespace UpdateEngine {
class NetObserver final : public DupdateINetObserver, public NetManagerStandard::NetConnCallbackStub {
public:
    void SetCallback(const std::weak_ptr<INetObserverCallback> &callback) final;
    void StartObserver() final;
    int32_t NetCapabilitiesChange(sptr<NetManagerStandard::NetHandle> &netHandle,
        const sptr<NetManagerStandard::NetAllCapabilities> &netAllCap) final;
    int32_t NetLost(sptr<NetManagerStandard::NetHandle> &netHandle) final;

private:
    void OnNetChange(NetType netType);

    std::weak_ptr<INetObserverCallback> callback_; // 弱引用
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // DUPDATE_NET_OBSERVER_H