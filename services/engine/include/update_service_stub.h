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

#ifndef UPDATE_SERVICE_STUB_H
#define UPDATE_SERVICE_STUB_H

#include <functional>
#include <iostream>
#include "iremote_stub.h"
#include "iupdate_service.h"
#include "message_parcel.h"
#include "parcel.h"

namespace OHOS {
namespace UpdateEngine {
class UpdateServiceStub : public IRemoteStub<IUpdateService> {
public:
    UpdateServiceStub();
    ~UpdateServiceStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption &option) override;

    using UpdateServiceStubPtr = UpdateServiceStub *;
    using RequestFuncType = int32_t (UpdateServiceStub::*)(UpdateServiceStubPtr service,
        MessageParcel &data, MessageParcel &reply, MessageOption &option);

private:
    int32_t GetNewVersionStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetNewVersionDescriptionStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetCurrentVersionStub(UpdateServiceStubPtr service,
        MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t GetCurrentVersionDescriptionStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetTaskInfoStub(UpdateServiceStubPtr service,
        MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t CheckNewVersionStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t DownloadVersionStub(UpdateServiceStubPtr service,
        MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t PauseDownloadStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ResumeDownloadStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t DoUpdateStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ClearErrorStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t TerminateUpgradeStub(UpdateServiceStubPtr service,
        MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t SetUpgradePolicyStub(UpdateServiceStubPtr service,
        MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t GetUpgradePolicyStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t RegisterUpdateCallbackStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t UnregisterUpdateCallbackStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t CancelStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t FactoryResetStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ApplyNewVersionStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t VerifyUpgradePackageStub(UpdateServiceStubPtr service,
        MessageParcel& data, MessageParcel& reply, MessageOption &option);
    bool IsCallerValid();
    bool IsPermissionGranted(uint32_t code);

private:
    std::map<uint32_t, RequestFuncType> requestFuncMap_;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_SERVICE_STUB_H