/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "auto_mutex.h"
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

Lock g_rawDirLock;
Lock g_rawFileLock;
Lock g_rawFile64Lock;
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

struct ActualOffset {
    int64_t offset;
    explicit ActualOffset() : offset(0) {}
};

struct RawFile {
    const std::string filePath;
    int64_t offset;
    int64_t length;
    FILE* pf;
    uint8_t* buffer;
    const NativeResourceManager *resMgr;
    std::unique_ptr<ActualOffset> actualOffset;

    explicit RawFile(const std::string &path) : filePath(path), offset(0), length(0),
        pf(nullptr), buffer(nullptr), resMgr(nullptr), actualOffset(std::make_unique<ActualOffset>()) {}

    ~RawFile()
    {
        if (buffer != nullptr) {
            delete[] buffer;
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
        HiLog::Debug(LABEL, "failed to get RawDir dirName, %{public}s", dirName.c_str());
        return nullptr;
    }
    return result.release();
}

RawDir *OH_ResourceManager_OpenRawDir(const NativeResourceManager *mgr, const char *dirName)
{
    AutoMutex mutex(g_rawDirLock);
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
        HiLog::Debug(LABEL, "failed to get %{public}s rawfile", fileName);
        return nullptr;
    }
    auto result = std::make_unique<RawFile>(fileName);
    result->buffer = tmpBuf.release();
    if (result->buffer == nullptr) {
        HiLog::Error(LABEL, "failed get file buffer");
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
    AutoMutex mutex(g_rawFileLock);
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
    auto result = std::make_unique<RawFile>(filePath);
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
    AutoMutex mutex(g_rawDirLock);
    if (rawDir != nullptr) {
        delete rawDir;
        rawDir = nullptr;
    }
}

int OH_ResourceManager_ReadRawFile(const RawFile *rawFile, void *buf, size_t length)
{
    if (rawFile == nullptr || rawFile->actualOffset == nullptr || buf == nullptr || length == 0) {
        return 0;
    }
    if (rawFile->buffer != nullptr) {
        size_t len = static_cast<size_t>(OH_ResourceManager_GetRawFileRemainingLength(rawFile));
        if (length > len) {
            length = len;
        }
        int ret = memcpy_s(buf, length, rawFile->buffer + rawFile->actualOffset->offset, length);
        if (ret != 0) {
            HiLog::Error(LABEL, "failed to copy to buf");
            return 0;
        }
        rawFile->actualOffset->offset += static_cast<int64_t>(length);
        return static_cast<int>(length);
    } else {
        return std::fread(buf, 1, length, rawFile->pf);
    }
}

int OH_ResourceManager_SeekRawFile(const RawFile *rawFile, long offset, int whence)
{
    if (rawFile == nullptr || rawFile->actualOffset == nullptr || abs(offset) > rawFile->length) {
        return -1;
    }

    int origin = 0;
    switch (whence) {
        case SEEK_SET:
            origin = SEEK_SET;
            rawFile->actualOffset->offset = offset;
            break;
        case SEEK_CUR:
            origin = SEEK_CUR;
            rawFile->actualOffset->offset = rawFile->actualOffset->offset + offset;
            break;
        case SEEK_END:
            origin = SEEK_END;
            rawFile->actualOffset->offset = rawFile->length + offset;
            break;
        default:
            return -1;
    }

    if (rawFile->actualOffset->offset < 0 || rawFile->actualOffset->offset > rawFile->length) {
        return -1;
    }

    return std::fseek(rawFile->pf, rawFile->actualOffset->offset, origin);
}

long OH_ResourceManager_GetRawFileSize(RawFile *rawFile)
{
    if (rawFile == nullptr || rawFile->actualOffset == nullptr) {
        return 0;
    }

    return static_cast<long>(rawFile->length);
}

long OH_ResourceManager_GetRawFileRemainingLength(const RawFile *rawFile)
{
    if (rawFile == nullptr || rawFile->actualOffset == nullptr ||
        rawFile->length < rawFile->actualOffset->offset) {
        return 0;
    }

    return static_cast<long>(rawFile->length - rawFile->actualOffset->offset);
}

void OH_ResourceManager_CloseRawFile(RawFile *rawFile)
{
    AutoMutex mutex(g_rawFileLock);
    if (rawFile != nullptr) {
        delete rawFile;
        rawFile = nullptr;
    }
}

long OH_ResourceManager_GetRawFileOffset(const RawFile *rawFile)
{
    if (rawFile == nullptr || rawFile->actualOffset == nullptr) {
        return 0;
    }
    return static_cast<long>(rawFile->actualOffset->offset);
}

static bool GetRawFileDescriptorFromHap(const RawFile *rawFile, RawFileDescriptor &descriptor)
{
    ResourceManager::RawFileDescriptor resMgrDescriptor;
    RState state = rawFile->resMgr->resManager->
        GetRawFdNdkFromHap(rawFile->filePath, resMgrDescriptor);
    if (state != SUCCESS) {
        HiLog::Error(LABEL, "GetRawFileDescriptorFromHap failed to get rawFile descriptor");
        return false;
    }
    descriptor.fd = resMgrDescriptor.fd;
    descriptor.length = static_cast<long>(resMgrDescriptor.length);
    descriptor.start = static_cast<long>(resMgrDescriptor.offset);
    return true;
}

bool OH_ResourceManager_GetRawFileDescriptor(const RawFile *rawFile, RawFileDescriptor &descriptor)
{
    if (rawFile == nullptr || rawFile->actualOffset == nullptr) {
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
        descriptor.length = static_cast<long>(rawFile->length);
        descriptor.start = static_cast<long>(rawFile->actualOffset->offset);
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

struct Raw {
    const std::string filePath;
    int64_t offset; // offset base on the rawfile
    int64_t length;
    int64_t start; // offset base on the Hap
    FILE* pf;
    int fd;
    const NativeResourceManager *resMgr;

    explicit Raw(const std::string &path) : filePath(path), offset(0), length(0), start(0),
        pf(nullptr), fd(0), resMgr(nullptr) {}

    ~Raw()
    {
        if (pf != nullptr) {
            fclose(pf);
            pf = nullptr;
        }
        if (fd > 0) {
            close(fd);
            fd = 0;
        }
    }

    bool open()
    {
        pf = std::fopen(filePath.c_str(), "rb");
        return pf != nullptr;
    }
};

struct RawFile64 {
    std::unique_ptr<Raw> raw;
    explicit RawFile64(std::unique_ptr<Raw> raw) : raw(std::move(raw)) {}
};

RawFile64 *LoadRawFileFromHap64(const NativeResourceManager *mgr, const char *fileName, const std::string hapPath)
{
    ResourceManager::RawFileDescriptor resMgrDescriptor;
    RState state = mgr->resManager->GetRawFdNdkFromHap(fileName, resMgrDescriptor);
    if (state != SUCCESS) {
        HiLog::Error(LABEL, "failed to get %{public}s rawfile descriptor", fileName);
        return nullptr;
    }
    auto result = std::make_unique<Raw>(fileName);
    result->pf = fdopen(resMgrDescriptor.fd, "rb");
    result->fd = resMgrDescriptor.fd;
    result->length = resMgrDescriptor.length;
    result->start = resMgrDescriptor.offset;
    result->resMgr = mgr;
    if (result->pf == nullptr) {
        HiLog::Error(LABEL, "failed to open %{public}s rawfile descriptor", fileName);
        return nullptr;
    }
    std::fseek(result->pf, result->start, SEEK_SET);
    return new RawFile64(std::move(result));
}

RawFile64 *OH_ResourceManager_OpenRawFile64(const NativeResourceManager *mgr, const char *fileName)
{
    AutoMutex mutex(g_rawFile64Lock);
    if (mgr == nullptr || fileName == nullptr) {
        return nullptr;
    }

    std::string hapPath;
    if (IsLoadHap(mgr, hapPath)) {
        return LoadRawFileFromHap64(mgr, fileName, hapPath);
    }
    std::string filePath;
    RState state = mgr->resManager->GetRawFilePathByName(fileName, filePath);
    if (state != SUCCESS) {
        return nullptr;
    }
    auto result = std::make_unique<Raw>(filePath);
    if (!result->open()) {
        return nullptr;
    }

    std::fseek(result->pf, 0, SEEK_END);
    result->length = ftell(result->pf);
    std::fseek(result->pf, 0, SEEK_SET);
    return new RawFile64(std::move(result));
}

int64_t OH_ResourceManager_ReadRawFile64(const RawFile64 *rawFile, void *buf, int64_t length)
{
    if (rawFile == nullptr || rawFile->raw == nullptr || buf == nullptr || length == 0) {
        return 0;
    }
    int64_t len = OH_ResourceManager_GetRawFileRemainingLength64(rawFile);
    if (length > len) {
        length = len;
    }
    size_t ret = std::fread(buf, 1, length, rawFile->raw->pf);
    if (ret == 0) {
        HiLog::Error(LABEL, "failed to fread");
        return 0;
    }
    rawFile->raw->offset += length;
    return static_cast<int64_t>(ret);
}

int OH_ResourceManager_SeekRawFile64(const RawFile64 *rawFile, int64_t offset, int whence)
{
    if (rawFile == nullptr || rawFile->raw == nullptr || abs(offset) > rawFile->raw->length) {
        return -1;
    }

    int origin = 0;
    int64_t actualOffset = 0;
    switch (whence) {
        case SEEK_SET:
            origin = SEEK_SET;
            rawFile->raw->offset = offset;
            actualOffset = rawFile->raw->start + offset;
            break;
        case SEEK_CUR:
            origin = SEEK_CUR;
            rawFile->raw->offset = rawFile->raw->offset + offset;
            actualOffset = offset;
            break;
        case SEEK_END:
            origin = SEEK_SET;
            rawFile->raw->offset = rawFile->raw->length + offset;
            actualOffset = rawFile->raw->start + rawFile->raw->length + offset;
            break;
        default:
            return -1;
    }

    if (rawFile->raw->offset < 0 || rawFile->raw->offset > rawFile->raw->length) {
        return -1;
    }

    return std::fseek(rawFile->raw->pf, actualOffset, origin);
}

int64_t OH_ResourceManager_GetRawFileSize64(RawFile64 *rawFile)
{
    if (rawFile == nullptr || rawFile->raw == nullptr) {
        return 0;
    }

    return rawFile->raw->length;
}

int64_t OH_ResourceManager_GetRawFileRemainingLength64(const RawFile64 *rawFile)
{
    if (rawFile == nullptr || rawFile->raw == nullptr ||
        rawFile->raw->length < rawFile->raw->offset) {
        return 0;
    }

    return rawFile->raw->length - rawFile->raw->offset;
}

void OH_ResourceManager_CloseRawFile64(RawFile64 *rawFile)
{
    AutoMutex mutex(g_rawFile64Lock);
    if (rawFile != nullptr) {
        delete rawFile;
        rawFile = nullptr;
    }
}

int64_t OH_ResourceManager_GetRawFileOffset64(const RawFile64 *rawFile)
{
    if (rawFile == nullptr || rawFile->raw == nullptr) {
        return 0;
    }
    return rawFile->raw->offset;
}

static bool GetRawFileDescriptorFromHap64(const RawFile64 *rawFile, RawFileDescriptor64 *descriptor)
{
    ResourceManager::RawFileDescriptor resMgrDescriptor;
    RState state = rawFile->raw->resMgr->resManager->
        GetRawFdNdkFromHap(rawFile->raw->filePath, resMgrDescriptor);
    if (state != SUCCESS) {
        HiLog::Error(LABEL, "GetRawFileDescriptorFromHap64 failed to get rawFile descriptor");
        return false;
    }
    descriptor->fd = resMgrDescriptor.fd;
    descriptor->length = resMgrDescriptor.length;
    descriptor->start = resMgrDescriptor.offset;
    return true;
}

bool OH_ResourceManager_GetRawFileDescriptor64(const RawFile64 *rawFile, RawFileDescriptor64 *descriptor)
{
    if (rawFile == nullptr || rawFile->raw == nullptr) {
        return false;
    }
    if (rawFile->raw->resMgr != nullptr) {
        return GetRawFileDescriptorFromHap64(rawFile, descriptor);
    }
    char paths[PATH_MAX] = {0};
#ifdef __WINNT__
    if (!PathCanonicalizeA(paths, rawFile->raw->filePath.c_str())) {
        HiLog::Error(LABEL, "failed to PathCanonicalizeA the rawFile path");
    }
#else
    if (realpath(rawFile->raw->filePath.c_str(), paths) == nullptr) {
        HiLog::Error(LABEL, "failed to realpath the rawFile path");
    }
#endif
    int fd = open(paths, O_RDONLY);
    if (fd > 0) {
        descriptor->fd = fd;
        descriptor->length = rawFile->raw->length;
        descriptor->start = rawFile->raw->offset;
    } else {
        return false;
    }
    return true;
}

bool OH_ResourceManager_ReleaseRawFileDescriptor64(const RawFileDescriptor64 *descriptor)
{
    if (descriptor->fd > 0) {
        return close(descriptor->fd) == 0;
    }
    return true;
}

bool OH_ResourceManager_IsRawDir(const NativeResourceManager *mgr, const char *path)
{
    bool result = false;
    if (mgr == nullptr || path == nullptr) {
        return result;
    }
    RState state = mgr->resManager->IsRawDirFromHap(path, result);
    if (state != SUCCESS) {
        HiLog::Error(LABEL, "failed to determine whether the path is a directory");
        return result;
    }
    return result;
}