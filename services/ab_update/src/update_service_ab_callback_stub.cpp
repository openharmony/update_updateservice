/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "update_service_ab_callback_stub.h"

namespace OHOS {
namespace UpdateEngine {
int32_t UpdateServiceAbCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ENGINE_LOGE("UpdateServiceAbCallbackStub ReadInterfaceToken fail");
        return -1;
    }
    switch (code) {
        case OHOS::SysInstaller::ISysInstallerCallback::UPDATE_RESULT: {
            int32_t updateStatus = data.ReadInt32();
            int32_t percent  = data.ReadInt32();
            ENGINE_LOGI("UpdateServiceAbCallbackStub OnUpgradeProgress progress %d percent %d", updateStatus, percent);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}
} // namespace UpdateEngine
} // namespace OHOS
