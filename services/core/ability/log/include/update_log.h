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

#ifndef UPDATE_LOG_H
#define UPDATE_LOG_H

#include <string>

#include "hilog/log.h"

namespace OHOS {
namespace UpdateEngine {
#ifdef UPDATE_SERVICE
static constexpr OHOS::HiviewDFX::HiLogLabel UPDATE_LABEL = {LOG_CORE, 0xD002E00, "UPDATE_SA"};
#else
static constexpr OHOS::HiviewDFX::HiLogLabel UPDATE_LABEL = {LOG_CORE, 0xD002E00, "UPDATE_KITS"};
#endif

enum class UpdateLogLevel {
    UPDATE_DEBUG = 0,
    UPDATE_INFO,
    UPDATE_WARN,
    UPDATE_ERROR,
    UPDATE_FATAL
};

struct UpdateLogContent {
    HiviewDFX::HiLogLabel label;
    UpdateLogLevel level;
    std::string log;
    std::string args;
    std::string fileName;
    int32_t line;

    UpdateLogContent BuildWithArgs(const std::string &argsInput) const
    {
        return {label, level, log, argsInput, fileName, line};
    };

    UpdateLogContent BuildWithFmtAndArgs(const std::string &logInput, const std::string &argsInput) const
    {
        return {label, level, logInput, argsInput, fileName, line};
    };
};

class UpdateLog {
public:
    static bool JudgeLevel(const UpdateLogLevel &level);
    static void SetLogLevel(const UpdateLogLevel &level);
    static const UpdateLogLevel &GetLogLevel();
    static std::string GetBriefFileName(const std::string &file);
    static void PrintLongLog(const UpdateLogContent &logContent);

private:
    static void PrintLog(const UpdateLogContent &logContent);
    static void PrintSingleLine(const UpdateLogContent &logContent);
    static std::pair<std::string, std::string> SplitLogByFmtLabel(const std::string &log, const std::string &fmtLabel);
    static std::string GetFmtLabel(const std::string &log);
    static int32_t FindSubStrCount(const std::string &str, const std::string &subStr);

private:
    static UpdateLogLevel level_;
};

// 暂时记录两边日志
#define BASE_PRINT_LOG(label, level, hilogMethod, fileName, line, fmt, ...)                                       \
    if (UpdateLog::JudgeLevel((level)))                                                                           \
        OHOS::HiviewDFX::HiLog::hilogMethod((label), ("[%{public}s(%{public}d)] " +                               \
            std::string(fmt)).c_str(), fileName, line, ##__VA_ARGS__)

#define PRINT_LOG(label, level, hilogMethod, fmt, ...) BASE_PRINT_LOG(label, level,                               \
    hilogMethod, UpdateLog::GetBriefFileName(std::string(__FILE__)).c_str(), __LINE__, fmt, ##__VA_ARGS__)

#define PRINT_LOGD(label, fmt, ...) PRINT_LOG(label, UpdateLogLevel::UPDATE_DEBUG, Debug, fmt, ##__VA_ARGS__)
#define PRINT_LOGI(label, fmt, ...) PRINT_LOG(label, UpdateLogLevel::UPDATE_INFO, Info, fmt, ##__VA_ARGS__)
#define PRINT_LOGE(label, fmt, ...) PRINT_LOG(label, UpdateLogLevel::UPDATE_ERROR, Error, fmt, ##__VA_ARGS__)

#define ENGINE_LOGD(fmt, ...) PRINT_LOGD(UPDATE_LABEL, fmt, ##__VA_ARGS__)
#define ENGINE_LOGI(fmt, ...) PRINT_LOGI(UPDATE_LABEL, fmt, ##__VA_ARGS__)
#define ENGINE_LOGE(fmt, ...) PRINT_LOGE(UPDATE_LABEL, fmt, ##__VA_ARGS__)

#define PRINT_LONG_LOGD(label, fmt, args) UpdateLog::PrintLongLog({label,                                         \
    UpdateLogLevel::UPDATE_DEBUG, std::string(fmt), std::string(args), std::string(__FILE__), __LINE__})
#define PRINT_LONG_LOGI(label, fmt, args) UpdateLog::PrintLongLog({label,                                         \
    UpdateLogLevel::UPDATE_INFO, std::string(fmt), std::string(args), std::string(__FILE__), __LINE__})
#define PRINT_LONG_LOGE(label, fmt, args) UpdateLog::PrintLongLog({label,                                         \
    UpdateLogLevel::UPDATE_ERROR, std::string(fmt), std::string(args), std::string(__FILE__), __LINE__})

#define ENGINE_LONG_LOGD(fmt, args) PRINT_LONG_LOGD(UPDATE_LABEL, fmt, args)
#define ENGINE_LONG_LOGI(fmt, args) PRINT_LONG_LOGI(UPDATE_LABEL, fmt, args)
#define ENGINE_LONG_LOGE(fmt, args) PRINT_LONG_LOGE(UPDATE_LABEL, fmt, args)
} // namespace UpdateEngine
} // namespace OHOS
#endif // UPDATE_LOG_H