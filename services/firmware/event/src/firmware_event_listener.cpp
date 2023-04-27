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

#include "firmware_event_listener.h"

#include "constant.h"
#include "dupdate_net_manager.h"
#include "firmware_log.h"
#include "firmware_manager.h"
#include "update_define.h"

namespace OHOS {
namespace UpdateEngine {
FirmwareEventListener::FirmwareEventListener()
{
    FIRMWARE_LOGD("FirmwareEventListener::FirmwareEventListener");
}

FirmwareEventListener::~FirmwareEventListener()
{
    FIRMWARE_LOGD("FirmwareEventListener::~FirmwareEventListener");
}

void FirmwareEventListener::RegisterNetChangedListener()
{
    FIRMWARE_LOGI("FirmwareEventListener RegisterNetChangedListener");
    DelayedSingleton<NetManager>::GetInstance()->RegisterCallback(NetChangeCallbackType::HOTA_DOWNLOAD,
        {NetType::CELLULAR, NetType::METERED_WIFI, NetType::NOT_METERED_WIFI, NetType::NO_NET},
        [=](NetType netType) {
            FIRMWARE_LOGI("FirmwareEventListener on NetChanged type: %{public}d", CAST_INT(netType));
            DelayedSingleton<FirmwareManager>::GetInstance()->HandleEvent(CommonEventType::NET_CHANGED);
        });
}

void FirmwareEventListener::UnregisterNetChangedListener()
{
    FIRMWARE_LOGI("FirmwareEventListener UnregisterNetChangedListener");
    DelayedSingleton<NetManager>::GetInstance()->UnregisterCallback(NetChangeCallbackType::HOTA_DOWNLOAD);
}
} // namespace UpdateEngine
} // namespace OHOS