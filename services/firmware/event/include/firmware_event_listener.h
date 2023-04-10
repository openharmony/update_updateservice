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

#ifndef FIRMWARE_EVENT_LISTENER_H
#define FIRMWARE_EVENT_LISTENER_H

#include "singleton.h"

#include "dupdate_inet_observer.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareEventListener : public DelayedSingleton<FirmwareEventListener> {
    DECLARE_DELAYED_SINGLETON(FirmwareEventListener);

public:
    void RegisterNetChangedListener();
    void UnregisterNetChangedListener();
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_EVENT_LISTENER_H