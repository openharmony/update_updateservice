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

#ifndef FIRMWARE_RESULT_PROCESS_H
#define FIRMWARE_RESULT_PROCESS_H

#include <map>
#include <string>
#include <vector>
#include <unistd.h>

#include "firmware_component.h"

namespace OHOS {
namespace UpdateEngine {
static const std::string UPDATER_RESULT_SUCCESS = "pass";
static const std::string UPDATER_RESULT_FAILURE = "fail";
static const std::string UPDATER_FILE_ERROR = "fileError";

enum class UpdateResultCode {
    SUCCESS = 0,
    FAILURE,
    FILE_ERROR,
    PART_SUCCESS
};

enum class UpdateComponentResultCode {
    SUCCESS = 0,
    FAILURE,
    FILE_ERROR,
    UNEXECUTED
};

struct UpdateResult {
    std::string spath;
    std::string result;
    std::string reason;

    UpdateComponentResultCode GetUpdateResultCode() const
    {
        if (result == UPDATER_RESULT_SUCCESS) {
            return UpdateComponentResultCode::SUCCESS;
        }
        if (result == UPDATER_RESULT_FAILURE) {
            return UpdateComponentResultCode::FAILURE;
        }
        if (result == UPDATER_FILE_ERROR) {
            return UpdateComponentResultCode::FILE_ERROR;
        }
        return UpdateComponentResultCode::UNEXECUTED;
    };
};

class FirmwareResultProcess {
public:
    UpdateResultCode GetUpdaterResult(const std::vector<FirmwareComponent> &components,
        std::map<std::string, UpdateResult> &resultMap);

private:
    void ParseUpdaterResultRecord(const std::string &resultLine, std::map<std::string, UpdateResult> &resultMap);
    void HandleFileError(std::map<std::string, UpdateResult> &resultMap,
        const std::vector<FirmwareComponent> &components);
    UpdateResultCode HandleFileResults(std::map<std::string, UpdateResult> &resultMap,
        const std::vector<FirmwareComponent> &components);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_RESULT_PROCESS_H