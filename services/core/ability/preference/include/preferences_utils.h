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

#ifndef PREFERENCES_UTILS_H
#define PREFERENCES_UTILS_H

#include <ohos_types.h>
#include <string>
#include <vector>

#include "preferences.h"
#include "preferences_errno.h"

namespace OHOS {
namespace UpdateEngine {
class PreferencesUtil {
public:
    PreferencesUtil() = default;
    virtual ~PreferencesUtil() = default;

    bool SaveString(const std::string &key, const std::string &value);
    bool SaveInt(const std::string &key, int value);
    bool SaveBool(const std::string &key, bool value);
    bool SaveLong(const std::string &key, int64_t value);
    bool SaveFloat(const std::string &key, float value);

    std::string ObtainString(const std::string &key, const std::string &defValue);
    int ObtainInt(const std::string &key, int defValue);
    bool ObtainBool(const std::string &key, bool defValue);
    int64_t ObtainLong(const std::string &key, int64_t defValue);
    float ObtainFloat(const std::string &key, float defValue);

    bool IsExist(const std::string &key);
    bool Remove(const std::string &key);
    bool RemoveAll();

    bool DeletePreference();

protected:
    virtual std::string GetPath() = 0;

private:
    std::shared_ptr<NativePreferences::Preferences> GetPreference();

    bool RefreshSync();

    template <typename T>
    bool Save(const std::string &key, const T &value);

    bool SaveInner(
        std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const std::string &value);
    bool SaveInner(std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int &value);
    bool SaveInner(std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const bool &value);
    bool SaveInner(std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int64_t &value);
    bool SaveInner(std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const float &value);

    template <typename T>
    T Obtain(const std::string &key, const T &defValue);

    std::string ObtainInner(
        std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const std::string &defValue);
    int ObtainInner(std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int &defValue);
    bool ObtainInner(std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const bool &defValue);
    int64_t ObtainInner(
        std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const int64_t &defValue);
    float ObtainInner(
        std::shared_ptr<NativePreferences::Preferences> ptr, const std::string &key, const float &defValue);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // PREFERENCES_UTILS_H