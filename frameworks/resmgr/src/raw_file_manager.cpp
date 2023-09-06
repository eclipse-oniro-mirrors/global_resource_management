/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "raw_file_manager.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <securec.h>
#include <unistd.h>

#include "raw_dir.h"
#include "raw_file.h"
#include "resource_manager.h"
#include "resource_manager_addon.h"
#include "resource_manager_impl.h"
#include "hilog/log_c.h"
#include "hilog/log_cpp.h"

#ifdef __WINNT__
#include <shlwapi.h>
#include <windows.h>
#endif

using namespace OHOS::Global::Resource;
using namespace OHOS::HiviewDFX;

namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, 0xD001E00, "RawFile"};
}

struct NativeResourceManager {
    std::shared_ptr<ResourceManager> resManager = nullptr;
};

struct FileNameCache {
    std::vector<std::string> names;
};

struct RawDir {
    std::shared_ptr<ResourceManager> resManager = nullptr;
    struct FileNameCache fileNameCache;
};

struct RawFile {
    const std::string filePath;
    long offset;
    long length;
    FILE* pf;
    uint8_t* buffer;
    const NativeResourceManager *resMgr;

    explicit RawFile(const std::string &path) : filePath(path), offset(0L), length(0L),
        pf(nullptr), buffer(nullptr), resMgr{nullptr} {}

    ~RawFile()
    {
        if (buffer != nullptr) {
            free(buffer);
            buffer = nullptr;
        }
        if (pf != nullptr) {
            fclose(pf);
            pf = nullptr;
        }
    }

    bool open()
    {
        pf = std::fopen(filePath.c_str(), "rb");
        return pf != nullptr;
    }
};

NativeResourceManager *OH_ResourceManager_InitNativeResourceManager(napi_env env, napi_value jsResMgr)
{
    napi_valuetype valueType;
    napi_typeof(env, jsResMgr, &valueType);
    if (valueType != napi_object) {
        HiLog::Error(LABEL, "jsResMgr is not an object");
        return nullptr;
    }
    std::shared_ptr<ResourceManagerAddon> *addonPtr = nullptr;
    napi_status status = napi_unwrap(env, jsResMgr, reinterpret_cast<void **>(&addonPtr));
    if (status != napi_ok) {
        HiLog::Error(LABEL, "Failed to get native resourcemanager");
        return nullptr;
    }
    std::unique_ptr<NativeResourceManager> result = std::make_unique<NativeResourceManager>();
    result->resManager = (*addonPtr)->GetResMgr();
    return result.release();
}

void OH_ResourceManager_ReleaseNativeResourceManager(NativeResourceManager *resMgr)
{
    if (resMgr != nullptr) {
        delete resMgr;
        resMgr = nullptr;
    }
}

static bool IsLoadHap(const NativeResourceManager *mgr, std::string &hapPath)
{
    return mgr->resManager->IsLoadHap(hapPath) == RState::SUCCESS ? true : false;
}

RawDir *LoadRawDirFromHap(const NativeResourceManager *mgr, const std::string dirName)
{
    std::unique_ptr<RawDir> result = std::make_unique<RawDir>();
    RState state = mgr->resManager->GetRawFileList(dirName, result->fileNameCache.names);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "failed to get RawDir dirName, %{public}s", dirName.c_str());
        return nullptr;
    }
    return result.release();
}

RawDir *OH_ResourceManager_OpenRawDir(const NativeResourceManager *mgr, const char *dirName)
{
    if (mgr == nullptr || dirName == nullptr) {
        return nullptr;
    }
    std::string hapPath;
    if (IsLoadHap(mgr, hapPath)) {
        return LoadRawDirFromHap(mgr, dirName);
    }
    ResourceManagerImpl* impl = static_cast<ResourceManagerImpl *>(mgr->resManager.get());
    std::string tempName = dirName;
    const std::string rawFileDirName = tempName.empty() ? "rawfile" : "rawfile/";
    if (tempName.length() < rawFileDirName.length()
        || (tempName.compare(0, rawFileDirName.length(), rawFileDirName) != 0)) {
        tempName = rawFileDirName + tempName;
    }
    std::unique_ptr<RawDir> result = std::make_unique<RawDir>();
    std::vector<std::string> resourcesPaths = impl->GetResourcePaths();
    for (auto iter = resourcesPaths.begin(); iter != resourcesPaths.end(); iter++) {
        std::string currentPath = *iter + tempName;
        DIR* dir = opendir(currentPath.c_str());
        if (dir == nullptr) {
            continue;
        }
        struct dirent *dirp = readdir(dir);
        while (dirp != nullptr) {
            if (std::strcmp(dirp->d_name, ".") == 0 ||
                std::strcmp(dirp->d_name, "..") == 0) {
                dirp = readdir(dir);
                continue;
            }
            if (dirp->d_type == DT_REG || dirp->d_type == DT_DIR) {
                result->fileNameCache.names.push_back(tempName + "/" + dirp->d_name);
            }

            dirp = readdir(dir);
        }
        closedir(dir);
    }
    return result.release();
}

RawFile *LoadRawFileFromHap(const NativeResourceManager *mgr, const char *fileName, const std::string hapPath)
{
    size_t len;
    std::unique_ptr<uint8_t[]> tmpBuf;
    RState state = mgr->resManager->GetRawFileFromHap(fileName, len, tmpBuf);
    if (state != SUCCESS) {
        HiLog::Error(LABEL, "failed to get %{public}s rawfile", fileName);
        return nullptr;
    }
    auto result = std::make_unique<RawFile>(fileName);
    result->buffer = reinterpret_cast<uint8_t*>(malloc(len));
    if (result->buffer == nullptr) {
        HiLog::Error(LABEL, "failed to malloc");
        return nullptr;
    }
    int ret = memcpy_s(result->buffer, len, tmpBuf.get(), len);
    if (ret != 0) {
        HiLog::Error(LABEL, "failed to memcpy_s");
        return nullptr;
    }

    int zipFd = open(hapPath.c_str(), O_RDONLY);
    if (zipFd < 0) {
        HiLog::Error(LABEL, "failed open file %{public}s", hapPath.c_str());
        return nullptr;
    }
    result->pf = fdopen(zipFd, "r");
    result->length = static_cast<long>(len);
    result->resMgr = mgr;
    return result.release();
}

RawFile *OH_ResourceManager_OpenRawFile(const NativeResourceManager *mgr, const char *fileName)
{
    if (mgr == nullptr || fileName == nullptr) {
        return nullptr;
    }

    std::string hapPath;
    if (IsLoadHap(mgr, hapPath)) {
        return LoadRawFileFromHap(mgr, fileName, hapPath);
    }

    std::string filePath;
    RState state = mgr->resManager->GetRawFilePathByName(fileName, filePath);
    if (state != SUCCESS) {
        return nullptr;
    }
    std::unique_ptr<RawFile> result = std::make_unique<RawFile>(filePath);
    if (!result->open()) {
        return nullptr;
    }

    std::fseek(result->pf, 0, SEEK_END);
    result->length = ftell(result->pf);
    std::fseek(result->pf, 0, SEEK_SET);
    return result.release();
}

int OH_ResourceManager_GetRawFileCount(RawDir *rawDir)
{
    if (rawDir == nullptr) {
        return 0;
    }
    return rawDir->fileNameCache.names.size();
}

const char *OH_ResourceManager_GetRawFileName(RawDir *rawDir, int index)
{
    if (rawDir == nullptr || index < 0) {
        return nullptr;
    }
    uint32_t rawFileCount = rawDir->fileNameCache.names.size();
    if (rawFileCount == 0 || index >= static_cast<int>(rawFileCount)) {
        return nullptr;
    }
    return rawDir->fileNameCache.names[index].c_str();
}

void OH_ResourceManager_CloseRawDir(RawDir *rawDir)
{
    if (rawDir != nullptr) {
        delete rawDir;
    }
}

int OH_ResourceManager_ReadRawFile(const RawFile *rawFile, void *buf, size_t length)
{
    if (rawFile == nullptr || buf == nullptr || length == 0) {
        return 0;
    }
    if (rawFile->buffer != nullptr) {
        int ret = memcpy_s(buf, length, rawFile->buffer, rawFile->length);
        if (ret != 0) {
            HiLog::Error(LABEL, "failed to copy to buf");
            return 0;
        }
        return rawFile->length;
    } else {
        return std::fread(buf, 1, length, rawFile->pf);
    }
}

int OH_ResourceManager_SeekRawFile(const RawFile *rawFile, long offset, int whence)
{
    if (rawFile == nullptr) {
        return 0;
    }

    int origin = 0;
    int start = 0;
    switch (whence) {
        case SEEK_SET:
            origin = SEEK_SET;
            start = rawFile->offset + offset;
            break;
        case SEEK_CUR:
            origin = SEEK_CUR;
            start = offset;
            break;
        case SEEK_END:
            start = rawFile->offset + rawFile->length + offset;
            origin = SEEK_SET;
            break;
        default:
            return -1;
    }

    return std::fseek(rawFile->pf, start, origin);
}

long OH_ResourceManager_GetRawFileSize(RawFile *rawFile)
{
    if (rawFile == nullptr) {
        return 0;
    }

    return rawFile->length;
}

void OH_ResourceManager_CloseRawFile(RawFile *rawFile)
{
    if (rawFile != nullptr) {
        delete rawFile;
    }
}

long OH_ResourceManager_GetRawFileOffset(const RawFile *rawFile)
{
    if (rawFile == nullptr) {
        return 0;
    }
    return ftell(rawFile->pf) - rawFile->offset;
}

static bool GetRawFileDescriptorFromHap(const RawFile *rawFile, RawFileDescriptor &descriptor)
{
    ResourceManager::RawFileDescriptor resMgrDescriptor;
    int32_t ret = rawFile->resMgr->resManager->GetRawFdNdkFromHap(rawFile->filePath, resMgrDescriptor);
    if (ret != 0) {
        HiLog::Error(LABEL, "failed to get rawFile descriptor");
        return false;
    }
    descriptor.fd = resMgrDescriptor.fd;
    descriptor.length = resMgrDescriptor.length;
    descriptor.start = resMgrDescriptor.offset;
    return true;
}

bool OH_ResourceManager_GetRawFileDescriptor(const RawFile *rawFile, RawFileDescriptor &descriptor)
{
    if (rawFile == nullptr) {
        return false;
    }
    if (rawFile->resMgr != nullptr) {
        return GetRawFileDescriptorFromHap(rawFile, descriptor);
    }
    char paths[PATH_MAX] = {0};
#ifdef __WINNT__
    if (!PathCanonicalizeA(paths, rawFile->filePath.c_str())) {
        HiLog::Error(LABEL, "failed to PathCanonicalizeA the rawFile path");
    }
#else
    if (realpath(rawFile->filePath.c_str(), paths) == nullptr) {
        HiLog::Error(LABEL, "failed to realpath the rawFile path");
    }
#endif
    int fd = open(paths, O_RDONLY);
    if (fd > 0) {
        descriptor.fd = fd;
        descriptor.length = rawFile->length;
        descriptor.start = rawFile->offset;
    } else {
        return false;
    }
    return true;
}

bool OH_ResourceManager_ReleaseRawFileDescriptor(const RawFileDescriptor &descriptor)
{
    if (descriptor.fd > 0) {
        return close(descriptor.fd) == 0;
    }
    return true;
}
