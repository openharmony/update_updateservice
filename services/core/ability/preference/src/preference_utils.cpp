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

#include "preferences_utils.h"

#include <ohos_types.h>

#include "preferences_helper.h"

#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
std::shared_ptr<NativePreferences::Preferences> PreferencesUtil::GetPreference()
{
    std::string path = GetPath();
    if (path.empty()) {
        return nullptr;
    }
    int errCode = OHOS_FAILURE;
    std::shared_ptr<NativePreferences::Preferences> ptr =
        NativePreferences::PreferencesHelper::GetPreferences(path, errCode);
    if (ptr == nullptr) {
        ENGINE_LOGI("GetPreference error code is %{public}d", errCode);
    }
    return ptr;
}

bool PreferencesUtil::DeletePreference()
{
    std::string path = GetPath();
    if (path.empty()) {
        return false;
    }
    return NativePreferences::PreferencesHelper::DeletePreferences(path) == NativePreferences::E_OK;
}

bool PreferencesUtil::SaveString(const std::string &key, const std::string &value)
{
    return Save(key, value);
}

bool PreferencesUtil::SaveInt(const std::string &key, int value)
{
    return Save(key, value);
}

bool PreferencesUtil::SaveBool(const std::string &key, bool value)
{
    return Save(key, value);
}

bool PreferencesUtil::SaveLong(const std::string &key, int64_t value)
{
    return Save(key, value);
}

bool PreferencesUtil::SaveFloat(const std::string &key, float value)
{
    return Save(key, value);
}

template <typename T>
bool PreferencesUtil::Save(const std::string &key, const T &value)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetPreference();
    if (ptr == nullptr) {
        return false;
    }
    if (!SaveInner(ptr, key, value)) {
        ENGINE_LOGE("SaveInner error");
        return false;
    }
    return RefreshSync();
}

bool PreferencesUtil::SaveInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const std::string &value)
{
    return ptr->PutString(key, value) == NativePreferences::E_OK;
}

bool PreferencesUtil::SaveInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int &value)
{
    return ptr->PutInt(key, value) == NativePreferences::E_OK;
}

bool PreferencesUtil::SaveInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const bool &value)
{
    return ptr->PutBool(key, value) == NativePreferences::E_OK;
}

bool PreferencesUtil::SaveInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int64_t &value)
{
    return ptr->PutLong(key, value) == NativePreferences::E_OK;
}

bool PreferencesUtil::SaveInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const float &value)
{
    return ptr->PutFloat(key, value) == NativePreferences::E_OK;
}

std::string PreferencesUtil::ObtainString(const std::string &key, const std::string &defValue)
{
    return Obtain(key, defValue);
}

int PreferencesUtil::ObtainInt(const std::string &key, int defValue)
{
    return Obtain(key, defValue);
}

bool PreferencesUtil::ObtainBool(const std::string &key, bool defValue)
{
    return Obtain(key, defValue);
}

int64_t PreferencesUtil::ObtainLong(const std::string &key, int64_t defValue)
{
    return Obtain(key, defValue);
}

float PreferencesUtil::ObtainFloat(const std::string &key, float defValue)
{
    return Obtain(key, defValue);
}

template <typename T>
T PreferencesUtil::Obtain(const std::string &key, const T &defValue)
{
    int errCode = OHOS_FAILURE;
    std::shared_ptr<NativePreferences::Preferences> ptr = GetPreference();
    if (ptr == nullptr) {
        ENGINE_LOGI("GetPreferences error code is %{public}d", errCode);
        return defValue;
    }
    return ObtainInner(ptr, key, defValue);
}

std::string PreferencesUtil::ObtainInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const std::string &defValue)
{
    return ptr->GetString(key, defValue);
}

int PreferencesUtil::ObtainInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int &defValue)
{
    return ptr->GetInt(key, defValue);
}

bool PreferencesUtil::ObtainInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const bool &defValue)
{
    return ptr->GetBool(key, defValue);
}

int64_t PreferencesUtil::ObtainInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int64_t &defValue)
{
    return ptr->GetLong(key, defValue);
}

float PreferencesUtil::ObtainInner(
    std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const float &defValue)
{
    return ptr->GetFloat(key, defValue);
}

bool PreferencesUtil::IsExist(const std::string &key)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetPreference();
    if (ptr == nullptr) {
        return false;
    }
    return ptr->HasKey(key);
}

bool PreferencesUtil::Remove(const std::string &key)
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetPreference();
    if (ptr == nullptr) {
        return false;
    }
    if (ptr->Delete(key) != NativePreferences::E_OK) {
        return false;
    }
    return RefreshSync();
}

bool PreferencesUtil::RemoveAll()
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetPreference();
    if (ptr == nullptr) {
        return false;
    }
    if (ptr->Clear() != NativePreferences::E_OK) {
        return false;
    }
    return RefreshSync();
}

bool PreferencesUtil::RefreshSync()
{
    std::shared_ptr<NativePreferences::Preferences> ptr = GetPreference();
    if (ptr == nullptr) {
        ENGINE_LOGE("RefreshSync error ptr is null");
        return false;
    }
    if (ptr->FlushSync() != NativePreferences::E_OK) {
        ENGINE_LOGE("RefreshSync error");
        return false;
    }
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS
