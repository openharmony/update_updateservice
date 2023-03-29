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
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "client_helper.h"
#include "local_updater.h"
#include "napi/native_common.h"
#include "napi_util.h"
#include "restorer.h"
#include "update_client.h"

namespace OHOS {
namespace UpdateEngine {
struct NativeClass {
    std::string className;
    napi_callback constructor;
    napi_ref* constructorRef;
    napi_property_descriptor *desc;
    int descSize;
};

// class name
const std::string CLASS_NAME_UPDATE_CLIENT = "UpdateClient";
const std::string CLASS_NAME_RESTORER = "Restorer";
const std::string CLASS_NAME_LOCAL_UPDATER = "LocalUpdater";

// constructor reference
static thread_local napi_ref g_updateClientConstructorRef = nullptr;
static thread_local napi_ref g_restorerConstructorRef = nullptr;
static thread_local napi_ref g_localUpdaterConstructorRef = nullptr;

std::shared_ptr<Restorer> g_restorer = nullptr;
std::shared_ptr<LocalUpdater> g_localUpdater = nullptr;
std::map<UpgradeInfo, std::shared_ptr<UpdateClient>> g_onlineUpdater;

template<typename T, typename Initializer, typename Finalizer>
napi_value JsConstructor(napi_env env, napi_callback_info info, Initializer initializer, Finalizer finalizer)
{
    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, args, &thisVar, &data);

    T* object = initializer(env, thisVar, args[0]);
    if (object == nullptr) {
        return thisVar;
    }
    napi_wrap(env, thisVar, object, finalizer, nullptr, nullptr);
    return thisVar;
}

template<typename T>
napi_value JsConstructor(napi_env env, napi_callback_info info)
{
    auto initializer = [](napi_env env, napi_value value, const napi_value arg) {
        UpgradeInfo upgradeInfo;
        ClientStatus ret = ClientHelper::GetUpgradeInfoFromArg(env, arg, upgradeInfo);
        if (ret != ClientStatus::CLIENT_SUCCESS) {
            T* object = NULL;
            return object;
        }
        if (g_onlineUpdater.count(upgradeInfo) == 0) {
            CLIENT_LOGI("JsConstructor new UpdateClient subtype: %{public}d", upgradeInfo.businessType.subType);
            std::shared_ptr<UpdateClient> updateClient = std::make_shared<UpdateClient>(env, value);
            g_onlineUpdater[upgradeInfo] = updateClient;
        } else {
            CLIENT_LOGI("JsConstructor UpdateClient use cache");
        }
        return g_onlineUpdater[upgradeInfo].get();
    };

    auto finalizer = [](napi_env env, void* data, void* hint) {
        CLIENT_LOGI("delete js object");
    };

    return JsConstructor<T>(env, info, initializer, finalizer);
}

napi_value JsConstructorRestorer(napi_env env, napi_callback_info info)
{
    auto initializer = [](napi_env env, napi_value value, const napi_value arg) {
        if (g_restorer == nullptr) {
            CLIENT_LOGI("JsConstructorRestorer, create native object");
            g_restorer = std::make_shared<Restorer>(env, value);
        }
        return g_restorer.get();
    };
    auto finalizer = [](napi_env env, void* data, void* hint) {};
    return JsConstructor<Restorer>(env, info, initializer, finalizer);
}

napi_value JsConstructorLocalUpdater(napi_env env, napi_callback_info info)
{
    auto initializer = [](napi_env env, napi_value value, const napi_value arg) {
        if (g_localUpdater == nullptr) {
            CLIENT_LOGI("JsConstructorLocalUpdater, create native object");
            g_localUpdater = std::make_shared<LocalUpdater>(env, value);
        }
        return g_localUpdater.get();
    };
    auto finalizer = [](napi_env env, void* data, void* hint) {};
    return JsConstructor<LocalUpdater>(env, info, initializer, finalizer);
}

template<typename T>
T* CreateJsObject(napi_env env, napi_callback_info info, napi_ref constructorRef, napi_value& jsObject)
{
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructorRef, &constructor);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr,
        "CreateJsObject error, napi_get_reference_value fail");

    size_t argc = MAX_ARGC;
    napi_value args[MAX_ARGC] = {0};
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        CLIENT_LOGI("CreateJsObject, napi_get_cb_info error");
    }
    status = napi_new_instance(env, constructor, argc, args, &jsObject);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return nullptr, "CreateJsObject error, napi_new_instance fail");

    T *nativeObject = nullptr;
    status = napi_unwrap(env, jsObject, (void**)&nativeObject);
    if (status != napi_ok) {
        CLIENT_LOGE("CreateJsObject error, napi_unwrap fail");
        napi_remove_wrap(env, jsObject, (void**)&nativeObject);
        jsObject = nullptr;
        return nullptr;
    }
    return nativeObject;
}

napi_value GetOnlineUpdater(napi_env env, napi_callback_info info)
{
    napi_value jsObject = nullptr;
    UpdateClient *client = CreateJsObject<UpdateClient>(env, info, g_updateClientConstructorRef, jsObject);
    if (client != nullptr) {
        napi_value result = client->GetOnlineUpdater(env, info);
        if (result != nullptr) {
            return jsObject;
        }
    }
    return nullptr;
}

napi_value GetRestorer(napi_env env, napi_callback_info info)
{
    napi_value jsObject = nullptr;
    Restorer* restorer = CreateJsObject<Restorer>(env, info, g_restorerConstructorRef, jsObject);
    if (restorer == nullptr) {
        return nullptr;
    }
    return jsObject;
}

napi_value GetLocalUpdater(napi_env env, napi_callback_info info)
{
    napi_value jsObject = nullptr;
    LocalUpdater* localUpdater = CreateJsObject<LocalUpdater>(env, info, g_localUpdaterConstructorRef, jsObject);
    if (localUpdater == nullptr) {
        return nullptr;
    }
    localUpdater->Init();
    return jsObject;
}

napi_value CheckNewVersion(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->CheckNewVersion(env, info);
}

napi_value SetUpgradePolicy(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->SetUpgradePolicy(env, info);
}

napi_value GetUpgradePolicy(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->GetUpgradePolicy(env, info);
}

napi_value Download(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->Download(env, info);
}

napi_value PauseDownload(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("PauseDownload");
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->PauseDownload(env, info);
}

napi_value ResumeDownload(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("ResumeDownload");
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->ResumeDownload(env, info);
}

napi_value CancelUpgrade(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->CancelUpgrade(env, info);
}

napi_value Upgrade(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("Upgrade");
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->Upgrade(env, info);
}

napi_value ClearError(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("ClearError");
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->ClearError(env, info);
}

napi_value TerminateUpgrade(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("TerminateUpgrade");
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->TerminateUpgrade(env, info);
}

napi_value GetNewVersionInfo(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->GetNewVersionInfo(env, info);
}

napi_value GetNewVersionDescription(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->GetNewVersionDescription(env, info);
}

napi_value GetCurrentVersionInfo(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("GetCurrentVersionInfo");
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->GetCurrentVersionInfo(env, info);
}

napi_value GetCurrentVersionDescription(napi_env env, napi_callback_info info)
{
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->GetCurrentVersionDescription(env, info);
}

napi_value GetTaskInfo(napi_env env, napi_callback_info info)
{
    CLIENT_LOGI("GetTaskInfo");
    UpdateClient *client = UnwrapJsObject<UpdateClient>(env, info);
    PARAM_CHECK_NAPI_CALL(env, client != nullptr, return nullptr, "Error get client");
    return client->GetTaskInfo(env, info);
}

static bool DefineClass(napi_env env, napi_value exports, const NativeClass& nativeClass)
{
    const std::string& className = nativeClass.className;
    napi_value result = nullptr;
    napi_status status = napi_define_class(env, className.c_str(), className.size(),
        nativeClass.constructor, nullptr, nativeClass.descSize, nativeClass.desc, &result);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return false, "DefineClass error, napi_define_class fail");

    status = napi_set_named_property(env, exports, className.c_str(), result);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return false, "DefineClass error, napi_set_named_property fail");

    constexpr int32_t refCount = 1;
    status = napi_create_reference(env, result, refCount, nativeClass.constructorRef);
    PARAM_CHECK_NAPI_CALL(env, status == napi_ok, return false, "DefineClass error, napi_create_reference fail");
    return true;
}

static bool DefineRestorer(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION(Restorer::Napi::FUNCTION_FACTORY_RESET, Restorer::Napi::FactoryReset)
    };

    NativeClass nativeClass = {
        .className = CLASS_NAME_RESTORER,
        .constructor = JsConstructorRestorer,
        .constructorRef = &g_restorerConstructorRef,
        .desc = desc,
        .descSize = COUNT_OF(desc)
    };
    return DefineClass(env, exports, nativeClass);
}

static bool DefineLocalUpdater(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION(LocalUpdater::Napi::FUNCTION_VERIFY_UPGRADE_PACKAGE,
            LocalUpdater::Napi::NapiVerifyUpgradePackage),
        DECLARE_NAPI_FUNCTION(LocalUpdater::Napi::FUNCTION_APPLY_NEW_VERSION, LocalUpdater::Napi::NapiApplyNewVersion),
        DECLARE_NAPI_FUNCTION(LocalUpdater::Napi::FUNCTION_ON, LocalUpdater::Napi::NapiOn),
        DECLARE_NAPI_FUNCTION(LocalUpdater::Napi::FUNCTION_OFF, LocalUpdater::Napi::NapiOff)
    };

    NativeClass nativeClass = {
        .className = CLASS_NAME_LOCAL_UPDATER,
        .constructor = JsConstructorLocalUpdater,
        .constructorRef = &g_localUpdaterConstructorRef,
        .desc = desc,
        .descSize = COUNT_OF(desc)
    };
    return DefineClass(env, exports, nativeClass);
}

static bool DefineUpdateClient(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("checkNewVersion", CheckNewVersion),
        DECLARE_NAPI_FUNCTION("getNewVersionInfo", GetNewVersionInfo),
        DECLARE_NAPI_FUNCTION("getNewVersionDescription", GetNewVersionDescription),
        DECLARE_NAPI_FUNCTION("getCurrentVersionInfo", GetCurrentVersionInfo),
        DECLARE_NAPI_FUNCTION("getCurrentVersionDescription", GetCurrentVersionDescription),
        DECLARE_NAPI_FUNCTION("getTaskInfo", GetTaskInfo),
        DECLARE_NAPI_FUNCTION("setUpgradePolicy", SetUpgradePolicy),
        DECLARE_NAPI_FUNCTION("getUpgradePolicy", GetUpgradePolicy),
        DECLARE_NAPI_FUNCTION("cancel", CancelUpgrade),
        DECLARE_NAPI_FUNCTION("download", Download),
        DECLARE_NAPI_FUNCTION("pauseDownload", PauseDownload),
        DECLARE_NAPI_FUNCTION("resumeDownload", ResumeDownload),
        DECLARE_NAPI_FUNCTION("upgrade", Upgrade),
        DECLARE_NAPI_FUNCTION("clearError", ClearError),
        DECLARE_NAPI_FUNCTION("terminateUpgrade", TerminateUpgrade),
        DECLARE_NAPI_FUNCTION(UpdateClient::Napi::FUNCTION_ON, UpdateClient::Napi::NapiOn),
        DECLARE_NAPI_FUNCTION(UpdateClient::Napi::FUNCTION_OFF, UpdateClient::Napi::NapiOff)
    };

    NativeClass nativeClass = {
        .className = CLASS_NAME_UPDATE_CLIENT,
        .constructor = JsConstructor<UpdateClient>,
        .constructorRef = &g_updateClientConstructorRef,
        .desc = desc,
        .descSize = COUNT_OF(desc)
    };
    return DefineClass(env, exports, nativeClass);
}

#ifdef UPDATER_UT
napi_value UpdateClientInit(napi_env env, napi_value exports)
#else
static napi_value UpdateClientInit(napi_env env, napi_value exports)
#endif
{
    CLIENT_LOGI("UpdateClientInit");
    // Registration function
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getOnlineUpdater", GetOnlineUpdater),
        DECLARE_NAPI_FUNCTION("getRestorer", GetRestorer),
        DECLARE_NAPI_FUNCTION("getLocalUpdater", GetLocalUpdater)
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    bool ret = DefineUpdateClient(env, exports);
    PARAM_CHECK_NAPI_CALL(env, ret, return nullptr, "DefineUpdateClient fail");
    CLIENT_LOGI("DefineUpdateClient success");

    ret = DefineRestorer(env, exports);
    PARAM_CHECK_NAPI_CALL(env, ret, return nullptr, "DefineRestorer fail");
    CLIENT_LOGI("DefineRestorer success");

    ret = DefineLocalUpdater(env, exports);
    PARAM_CHECK_NAPI_CALL(env, ret, return nullptr, "DefineLocalUpdater fail");
    CLIENT_LOGI("DefineLocalUpdater success");

    return exports;
}

/*
 * Module definition
 */
static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = UpdateClientInit,
    .nm_modname = "update",
    .nm_priv = (reinterpret_cast<void*>(0)),
    .reserved = { 0 }
};

/*
 * Module registration function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_module);
}
} // namespace UpdateEngine
} // namespace OHOS
