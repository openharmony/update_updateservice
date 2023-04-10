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

#ifndef FIRMWARE_FILE_UTILS_H
#define FIRMWARE_FILE_UTILS_H

#include <string>

#include "constant.h"
#include "firmware_constant.h"
#include "file_utils.h"

namespace OHOS {
namespace UpdateEngine {
class FirmwareFileUtils {
public:
    static bool IsSpaceEnough(const int64_t requiredSpace)
    {
        return FileUtils::IsSpaceEnough(Constant::DUPDATE_ENGINE_PACKAGE_ROOT_PATH, requiredSpace);
    }

    static void DeleteDownloadFiles()
    {
        FileUtils::DeleteFile(Firmware::PACKAGE_PATH, false);
    }
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FIRMWARE_FILE_UTILS_H