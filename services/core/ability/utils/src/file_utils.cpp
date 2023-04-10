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

#include "file_utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <unistd.h>
#include <vector>

#include "constant.h"
#include "update_helper.h"
#include "update_log.h"

namespace OHOS {
namespace UpdateEngine {
std::map<std::string, int32_t> FileUtils::baseDirMap_;

int64_t FileUtils::GetFileSize(const std::string &fileName)
{
    std::error_code errorCode;
    int64_t fileSize = static_cast<int64_t>(std::filesystem::file_size(fileName, errorCode));
    if (errorCode.operator bool()) {
        ENGINE_LOGE("get file size error, file = %{public}s", fileName.c_str());
        return 0;
    }
    return fileSize;
}

bool FileUtils::IsFileExist(const std::string &fileName)
{
    std::ifstream f(fileName.c_str());
    return f.good();
}

bool FileUtils::IsSpaceEnough(const std::string &filePath, const int64_t requiredSpace)
{
    std::error_code errorCode;
    uint64_t freeSpace = 0;
    const std::filesystem::space_info spaceInfo = std::filesystem::space(filePath, errorCode);
    if (errorCode.operator bool()) {
        ENGINE_LOGE("get disk free error, error code = %d", errorCode.value());
    } else {
        freeSpace = static_cast<std::uint64_t>(spaceInfo.free);
        ENGINE_LOGI("free space of [%s] is [%lu]", filePath.c_str(), (unsigned long long)freeSpace);
    }
    ENGINE_LOGI("free space=%{public}lu, required space=%{public}ld", static_cast<uint64_t>(freeSpace),
        static_cast<int64_t>(requiredSpace));
    return freeSpace >= static_cast<uint64_t>(requiredSpace);
}

bool FileUtils::SaveDataToFile(const std::string &filePath, const std::string &data)
{
    std::ofstream os;
    os.open(filePath, std::ios::trunc);
    if (os.is_open()) {
        ENGINE_LOGI("SaveDataToFile success, file = %{public}s", filePath.c_str());
        os << data;
        os.close();
        return true;
    }
    ENGINE_LOGE("SaveDataToFile fail, file = %{public}s", filePath.c_str());
    os.close();
    return false;
}

void FileUtils::DeleteFile(const std::string &rootPath, bool isDeleteRootDir)
{
    if (!IsFileExist(rootPath)) {
        ENGINE_LOGE("dir[%{public}s] is not exist", rootPath.c_str());
        return;
    }

    auto myPath = std::filesystem::path(rootPath);
    if (isDeleteRootDir) {
        RemoveAll(myPath);
        return;
    }

    for (auto const &dirEntry : std::filesystem::directory_iterator { myPath }) {
        RemoveAll(dirEntry.path());
    }
}

void FileUtils::RemoveAll(const std::filesystem::path &path)
{
    std::error_code errorCode;
    std::filesystem::remove_all(path, errorCode);
    if (errorCode.operator bool()) {
        ENGINE_LOGE("remove dir[%{public}s] fail, error message : %{public}s", path.c_str(),
            errorCode.message().c_str());
    } else {
        ENGINE_LOGI("remove dir[%{public}s] success", path.c_str());
    }
}

// 此函数功能为创建多层目录，支持a/b和a/b/这两种形式，最终的结果为创建a目录以及b目录
bool FileUtils::CreateMultiDirWithPermission(const std::string &fileDir, int32_t permission)
{
    std::string curDir = GetCurrentDir(fileDir);
    if (IsFileExist(curDir)) {
        return true;
    }
    std::stack<std::string> dirStack;
    do {
        dirStack.push(curDir);
        curDir = GetParentDir(curDir);
    } while (!curDir.empty() && !IsFileExist(curDir));

    while (!dirStack.empty()) {
        if (!CreatDirWithPermission(dirStack.top(), baseDirMap_.count(fileDir) ? baseDirMap_[fileDir] : permission)) {
            return false;
        };
        dirStack.pop();
    }
    return true;
}

std::string FileUtils::GetParentDir(const std::string &fileDir)
{
    auto curDir = std::filesystem::path(fileDir);
    ENGINE_LOGI("dirPath = %s", curDir.parent_path().string().c_str());
    return curDir.parent_path().string();
}

std::string FileUtils::GetCurrentDir(const std::string &fileDir)
{
    // 兼容传递的文件夹路径，末尾带0-N个'/'均ok
    if (fileDir[fileDir.length() - 1] == '/') {
        return GetParentDir(fileDir);
    }
    return fileDir;
}

// 按需创建基础目录，并且将需要创建的基础目录放在baseDirMap中，用于后续创建目录判断权限
void FileUtils::InitAndCreateBaseDirs(const std::vector<DirInfo> &dirInfos)
{
    if (dirInfos.empty()) {
        ENGINE_LOGE("dirInfo is empty");
        return;
    }
    for (const auto &dirInfo : dirInfos) {
        baseDirMap_[dirInfo.dirName] = dirInfo.dirPermissions;
        if (!IsFileExist(dirInfo.dirName)) {
            CreatDirWithPermission(dirInfo.dirName, dirInfo.dirPermissions);
        }
    }
}

void FileUtils::DestroyBaseDirectory(const std::vector<DirInfo> &dirInfos)
{
    ENGINE_LOGI("destroy base directory");
    if (dirInfos.empty()) {
        ENGINE_LOGE("dirInfo is empty");
        return;
    }

    for (const auto &dir : dirInfos) {
        if (dir.isAllowDestroyContents) {
            DeleteFile(dir.dirName, false);
        }
    }
}

bool FileUtils::CreatDirWithPermission(const std::string &fileDir, int32_t dirPermission)
{
    if (fileDir.empty() || strstr(fileDir.c_str(), "/.") != NULL || strstr(fileDir.c_str(), "./") != NULL) {
        ENGINE_LOGE("dirName %{public}s is invalid", fileDir.c_str());
        return false;
    }
    std::error_code errorCode;
    std::filesystem::create_directory(fileDir, errorCode);
    if (errorCode.operator bool()) {
        ENGINE_LOGE("Create directory dir[%s] fail, error message : %{public}s", fileDir.c_str(),
            errorCode.message().c_str());
        return false;
    }
    std::filesystem::permissions(fileDir, static_cast<std::filesystem::perms>(dirPermission),
        std::filesystem::perm_options::replace, errorCode);
    if (errorCode.operator bool()) {
        ENGINE_LOGE("Assign permissions failed, path = %{public}s,error message : %{public}s", fileDir.c_str(),
            errorCode.message().c_str());
        return false;
    }
    return true;
}

std::string FileUtils::ReadDataFromFile(const std::string &filePath)
{
    std::ifstream readFile;
    readFile.open(filePath);
    if (readFile.fail()) {
        ENGINE_LOGI("open file from %{public}s err", filePath.c_str());
        return "";
    }
    std::stringstream streamBuffer;
    streamBuffer << readFile.rdbuf();
    std::string fileRaw(streamBuffer.str());
    readFile.close();
    return fileRaw;
}
} // namespace UpdateEngine
} // namespace OHOS
