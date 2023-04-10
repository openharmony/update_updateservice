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

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "constant.h"
#include "update_helper.h"

namespace OHOS {
namespace UpdateEngine {
constexpr int32_t FOLDER_PERMISSION = 0750;

class FileUtils {
public:
    static bool IsFileExist(const std::string &fileName);
    static int64_t GetFileSize(const std::string &fileName);
    static bool IsSpaceEnough(const std::string &filePath, const int64_t requiredSpace);
    static bool SaveDataToFile(const std::string &filePath, const std::string &data);
    static void DeleteFile(const std::string &rootPath, bool isDeleteRootDir);
    static bool CreateMultiDirWithPermission(const std::string &fileDir, int32_t permission);
    static void InitAndCreateBaseDirs(const std::vector<DirInfo> &dirInfos);
    static void DestroyBaseDirectory(const std::vector<DirInfo> &dirInfos);
    static std::string ReadDataFromFile(const std::string &filePath);
    static std::string GetParentDir(const std::string &fileDir);

private:
    static std::string GetCurrentDir(const std::string &fileDir);
    static bool CreatDirWithPermission(const std::string &fileDir, int32_t dirPermission);

private:
    static std::map<std::string, int32_t> baseDirMap_;
    static void RemoveAll(const std::filesystem::path &path);
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // FILE_UTILS_H