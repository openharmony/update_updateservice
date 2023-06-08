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

#ifndef UPDATER_SA_IPC_INTERFACE_CODE_H
#define UPDATER_SA_IPC_INTERFACE_CODE_H

namespace OHOS {
namespace UpdateEngine {
/* SAID: 3006 */
enum class UpdaterSaInterfaceCode {
    CHECK_VERSION = 1,
    DOWNLOAD,
    PAUSE_DOWNLOAD,
    RESUME_DOWNLOAD,
    UPGRADE,
    CLEAR_ERROR,
    TERMINATE_UPGRADE,
    SET_POLICY,
    GET_POLICY,
    GET_NEW_VERSION,
    GET_NEW_VERSION_DESCRIPTION,
    GET_CURRENT_VERSION,
    GET_CURRENT_VERSION_DESCRIPTION,
    GET_TASK_INFO,
    REGISTER_CALLBACK,
    UNREGISTER_CALLBACK,
    CANCEL,
    FACTORY_RESET,
    APPLY_NEW_VERSION,
    VERIFY_UPGRADE_PACKAGE
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATER_SA_IPC_INTERFACE_CODE_H