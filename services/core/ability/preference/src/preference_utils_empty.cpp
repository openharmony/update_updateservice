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
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
bool PreferencesUtil::DeletePreference()
{
    return true;
}

bool PreferencesUtil::SaveString(const std::string &key, const std::string &value)
{
    return true;
}

bool PreferencesUtil::SaveInt(const std::string &key, int value)
{
    return true;
}

bool PreferencesUtil::SaveBool(const std::string &key, bool value)
{
    return true;
}

bool PreferencesUtil::SaveLong(const std::string &key, int64_t value)
{
    return true;
}

bool PreferencesUtil::SaveFloat(const std::string &key, float value)
{
    return true;
}

template <typename T>
bool PreferencesUtil::Save(const std::string &key, const T &value)
{
    return true;
}

std::string PreferencesUtil::ObtainString(const std::string &key, const std::string &defValue)
{
    return defValue;
}

int PreferencesUtil::ObtainInt(const std::string &key, int defValue)
{
    return defValue;
}

bool PreferencesUtil::ObtainBool(const std::string &key, bool defValue)
{
    return defValue;
}

int64_t PreferencesUtil::ObtainLong(const std::string &key, int64_t defValue)
{
    return defValue;
}

float PreferencesUtil::ObtainFloat(const std::string &key, float defValue)
{
    return defValue;
}

template <typename T>
T PreferencesUtil::Obtain(const std::string &key, const T &defValue)
{
    return defValue;
}

bool PreferencesUtil::IsExist(const std::string &key)
{
    return true;
}

bool PreferencesUtil::Remove(const std::string &key)
{
    return true;
}

bool PreferencesUtil::RemoveAll()
{
    return true;
}

bool PreferencesUtil::RefreshSync()
{
    return true;
}
} // namespace UpdateEngine
} // namespace OHOS