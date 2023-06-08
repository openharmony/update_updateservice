/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef IUPDATE_SERVICE_H
#define IUPDATE_SERVICE_H

#include "iremote_broker.h"
#include "iremote_proxy.h"

#include "iservice_local_updater.h"
#include "iservice_online_updater.h"
#include "iservice_restorer.h"
#include "iupdate_callback.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
class IUpdateService : public OHOS::IRemoteBroker, public IServiceOnlineUpdater, public IServiceRestorer,
    public IServiceLocalUpdater {
public:
    virtual int32_t RegisterUpdateCallback(const UpgradeInfo &info, const sptr<IUpdateCallback>& updateCallback) = 0;

    virtual int32_t UnregisterUpdateCallback(const UpgradeInfo &info) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Updater.IUpdateService");
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // IUPDATE_SERVICE_H
