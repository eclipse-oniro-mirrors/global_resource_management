/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "hap_resource_manager.h"

#include <sys/stat.h>
#include "hap_parser.h"
#include "hap_parser_v1.h"
#include "hap_parser_v2.h"
#include "hilog_wrapper.h"
#include "utils/errors.h"
#include "utils/utils.h"

#if defined(__WINNT__)
#include <cstring>
#else
#include "securec.h"
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#endif

namespace OHOS {
namespace Global {
namespace Resource {
HapResourceManager& HapResourceManager::GetInstance()
{
    static HapResourceManager instance;
    return instance;
}

bool IsNewModule(const char *path)
{
    std::unique_ptr<uint8_t[]> buffer = nullptr;
    size_t bufLen = 0;
    if (!HapParser::GetIndexData(path, buffer, bufLen)) {
        return false;
    }
    if (ResHeader::RES_HEADER_LEN > bufLen) {
        return false;
    }

    ResHeader resHeader;
    errno_t eret = memcpy_s(&resHeader, sizeof(ResHeader), buffer.get(), ResHeader::RES_HEADER_LEN);
    if (eret != OK) {
        return false;
    }

    std::string version = std::string(resHeader.version_);
    if (version.substr(0, version.find(" ")) == "Restool") {
        return false;
    }
    return true;
}

const std::shared_ptr<HapResource> HapResourceManager::Load(const char *path,
    std::shared_ptr<ResConfigImpl> &defaultConfig, bool isSystem, bool isOverlay, uint32_t selectedTypes)
{
    std::shared_ptr<HapResource> pResource = nullptr;
    struct stat fileStat {};
    int ret = stat(path, &fileStat);
    if (ret != 0) {
        fileStat.st_mtime = 0;
    }
    if (selectedTypes == SELECT_ALL) {
        pResource = this->GetHapResource(path);
        if (pResource != nullptr && ret == 0 && fileStat.st_mtime == pResource->GetLastModTime()) {
            pResource->Update(defaultConfig);
            return pResource;
        }
    }

    std::unique_ptr<HapParser> hapParser = nullptr;
    bool isNewModule = IsNewModule(path);
    if (isNewModule) {
        hapParser = std::make_unique<HapParserV2>();
    } else {
        hapParser = std::make_unique<HapParserV1>(defaultConfig, selectedTypes, isSystem || isOverlay);
    }
    if (hapParser == nullptr || !hapParser->Init(path)) {
        return nullptr;
    }
    
    pResource = hapParser->GetHapResource(path, isSystem, isOverlay);
    if (pResource == nullptr) {
        return nullptr;
    }

    if (selectedTypes == SELECT_ALL) {
        pResource->SetLastModTime(fileStat.st_mtime);
        pResource = this->PutAndGetResource(path, pResource);
    }
    pResource->Update(defaultConfig);
    return pResource;
}

const std::unordered_map<std::string, std::shared_ptr<HapResource>> HapResourceManager::LoadOverlays(
    const std::string &path, const std::vector<std::string> &overlayPaths,
    std::shared_ptr<ResConfigImpl> &defaultConfig, bool isSystem)
{
    std::unordered_map<std::string, std::shared_ptr<HapResource>> result;
    do {
        const std::shared_ptr<HapResource> targetResource = this->Load(path.c_str(), defaultConfig, isSystem);
        if (targetResource == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "load target failed");
            break;
        }
        result[path] = targetResource;
        bool success = true;
        std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> mapping =
            targetResource->BuildNameTypeIdMapping();
        for (auto iter = overlayPaths.begin(); iter != overlayPaths.end(); iter++) {
            // load overlay hap, the isOverlay flag set true.
            const std::shared_ptr<HapResource> overlayResource =
                this->Load(iter->c_str(), defaultConfig, isSystem, true);
            if (overlayResource == nullptr) {
                success = false;
                break;
            }
            result[*iter] = overlayResource;
        }

        if (!success) {
            RESMGR_HILOGE(RESMGR_TAG, "load overlay failed");
            break;
        }

        for (auto iter = result.begin(); iter != result.end(); iter++) {
            auto index = iter->first.find(path);
            if (index == std::string::npos) {
                iter->second->UpdateOverlayInfo(mapping);
            }
        }
        return result;
    } while (false);

    result.clear();
    return std::unordered_map<std::string, std::shared_ptr<HapResource>>();
}

std::shared_ptr<HapResource> HapResourceManager::PutAndGetResource(const std::string &path,
    std::shared_ptr<HapResource> pResource)
{
    std::unique_lock<std::shared_mutex> lock(mutexRw_);
    auto iter = hapResourceMap_.find(path);
    if (iter != hapResourceMap_.end()) {
        auto res = iter->second.lock();
        if (res != nullptr && res->GetLastModTime() == pResource->GetLastModTime()) {
            return res;
        }
    }
    hapResourceMap_[path] = pResource;
    return pResource;
}

bool HapResourceManager::PutPatchResource(const std::string &path, const std::string &patchPath)
{
    std::unique_lock<std::shared_mutex> lock(mutexRw_);
    if (hapResourceMap_.find(path) != hapResourceMap_.end()) {
        std::shared_ptr<HapResource> pResource = hapResourceMap_[path].lock();
        if (pResource) {
            pResource->SetPatchPath(patchPath);
            pResource->SetHasPatch(true);
            return true;
        }
    }
    return false;
}

std::shared_ptr<HapResource> HapResourceManager::GetHapResource(const std::string &path)
{
    std::shared_lock<std::shared_mutex> lock(mutexRw_);
    auto iter = hapResourceMap_.find(path);
    if (iter != hapResourceMap_.end()) {
        return iter->second.lock();
    }
    return nullptr;
}

#if defined(__ARKUI_CROSS__)
void HapResourceManager::RemoveHapResource(const std::string &path)
{
    std::unique_lock<std::shared_mutex> lock(mutexRw_);
    auto iter = hapResourceMap_.find(path);
    if (iter != hapResourceMap_.end()) {
        hapResourceMap_.erase(iter);
    }
}
#endif
} // namespace Resource
} // namespace Global
} // namespace OHOS