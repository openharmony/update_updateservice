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

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#define GET_ENUM_VALUE_STRING(item) StringUtils::GetEnumValueString(#item)

namespace OHOS {
namespace UpdateEngine {
class StringUtils {
public:
    // trim from start (in place)
    static inline void LTrim(std::string &inputStr)
    {
        inputStr.erase(inputStr.begin(),
            std::find_if(inputStr.begin(), inputStr.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    }

    // trim from end (in place)
    static inline void RTrim(std::string &inputStr)
    {
        inputStr.erase(
            std::find_if(inputStr.rbegin(), inputStr.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
            inputStr.end());
    }

    // trim from both ends (in place)
    static inline void Trim(std::string &inputStr)
    {
        LTrim(inputStr);
        RTrim(inputStr);
    }

    static inline std::vector<std::string> Split(const std::string &str, char delimiter)
    {
        std::vector<std::string> tokens;
        size_t start;
        size_t end = 0;
        while ((start = str.find_first_not_of(delimiter, end)) != std::string::npos) {
            end = str.find(delimiter, start);
            tokens.push_back(str.substr(start, end - start));
        }
        return tokens;
    }

    static inline std::string GetBoolStr(bool isTrue)
    {
        return isTrue ? "true" : "false";
    }

    static std::string SafeSubString(const std::string &sourceStr, int beginIndex, int endIndex,
        std::string defaultStr)
    {
        if (sourceStr.empty()) {
            return defaultStr;
        }
        if (beginIndex < 0 || static_cast<size_t>(endIndex) > sourceStr.size() || beginIndex > endIndex) {
            return defaultStr;
        }
        return sourceStr.substr(beginIndex, endIndex);
    }

    static std::string GetEnumValueString(const std::string &enumItem)
    {
        std::string enumSplit = "::";
        size_t split = enumItem.find(enumSplit);
        if (split == std::string::npos) {
            return "";
        }
        return enumItem.substr(split + enumSplit.length(), enumItem.length());
    }

    static std::string GetLastSplitString(const std::string &stringName, const std::string &splitStr)
    {
        std::size_t found = stringName.find_last_of(splitStr);
        std::string fileString = "";
        if (found != std::string::npos) {
            fileString = stringName.substr(found + 1, stringName.size());
        }
        return fileString;
    }

    static void ReplaceStringAll(std::string &srcString, const std::string &subString, const std::string &newString)
    {
        std::string resultString = srcString;
        for (std::string::size_type pos = 0; pos != std::string::npos; pos += newString.length()) {
            pos = resultString.find(subString, pos);
            if (pos != std::string::npos) {
                resultString.replace(pos, subString.length(), newString);
            } else {
                break;
            }
        }
        srcString = resultString;
    }

    static void StringRemove(std::string &sourceString, const std::string &startString,
        const std::string &endString)
    {
        if (sourceString.empty()) {
            return;
        }
        std::string::size_type indexStart = sourceString.find(startString);
        if (indexStart != std::string::npos) {
            std::string::size_type indexEnd = sourceString.find(endString, indexStart);
            if (indexEnd != std::string::npos) {
                std::string tmpString = sourceString.substr(0, indexStart) +
                    sourceString.substr(indexEnd + endString.length(), sourceString.length());
                sourceString = tmpString;
            }
        }
        return;
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // STRING_UTILS_H