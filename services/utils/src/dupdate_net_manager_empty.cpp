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
    ENGINE_LOGI("NetManager NetManager");
}

NetManager::~NetManager() {}

void NetManager::Init()
{
    ENGINE_LOGI("NetManager init");
}

bool NetManager::RegisterCallback(
    NetChangeCallbackType callbackType, const std::set<NetType> &netTypes, NetObserverCallback cb)
{
    ENGINE_LOGI("NetManager RegisterCallback");
    return true;
}

void NetManager::UnregisterCallback(NetChangeCallbackType callbackType)
{
    ENGINE_LOGI("NetManager UnregisterCallback");
}

NetType NetManager::GetNetType()
{
    return netType_;
}

bool NetManager::IsNetAvailable(NetType netType)
{
    return true;
}

bool NetManager::OnNetChange(NetType netType)
{
    ENGINE_LOGI("NetManager OnNetChange");
    return true;
}

bool NetManager::IsBaseNetType(NetType netType)
{
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS