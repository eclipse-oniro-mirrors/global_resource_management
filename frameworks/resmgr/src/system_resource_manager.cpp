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

#include "system_resource_manager.h"

#include "hilog_wrapper.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
const std::string SystemResourceManager::SYSTEM_RESOURCE_PATH = "/data/storage/el1/bundle/ohos.global.systemres" \
    "/ohos.global.systemres/assets/entry/resources.index";

const std::string SystemResourceManager::SYSTEM_RESOURCE_PATH_COMPRESSED = "/data/storage/el1/bundle/" \
    "systemResources/SystemResources.hap";

const std::string SystemResourceManager::SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED = "/sys_prod/app/" \
    "SystemResourcesOverlay/SystemResourcesOverlay.hap";

ResourceManagerImpl *SystemResourceManager::resourceManager_ = nullptr;

std::mutex SystemResourceManager::mutex_;

SystemResourceManager::SystemResourceManager()
{}

SystemResourceManager::~SystemResourceManager()
{}

ResourceManagerImpl *SystemResourceManager::GetSystemResourceManager()
{
    if (resourceManager_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (resourceManager_ == nullptr) {
            HILOG_INFO("create system resource manager when GetSystemResourceManager");
            ResourceManagerImpl *impl = new (std::nothrow) ResourceManagerImpl;
            if (impl == nullptr) {
                HILOG_ERROR("new ResourceManagerImpl failed when GetSystemResourceManager");
                return nullptr;
            }
            if (!impl->Init(true)) {
                delete impl;
                return nullptr;
            }
            if (!LoadSystemResource(impl)) {
                HILOG_WARN("load system resource failed when GetSystemResourceManager");
                delete impl;
                return nullptr;
            }
            resourceManager_ = impl;
        }
    }
    return resourceManager_;
}

bool SystemResourceManager::LoadSystemResource(ResourceManagerImpl *impl)
{
    HILOG_INFO("load system resource when GetSystemResourceManager");
    if (Utils::IsFileExist(SYSTEM_RESOURCE_PATH)) {
        if (Utils::IsFileExist(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED)) {
            vector<string> overlayPaths;
            overlayPaths.push_back(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED);
            return impl->AddResource(SYSTEM_RESOURCE_PATH.c_str(), overlayPaths);
        }
        return impl->AddResource(SYSTEM_RESOURCE_PATH.c_str());
    }
    
    if (Utils::IsFileExist(SYSTEM_RESOURCE_PATH_COMPRESSED)) {
        if (Utils::IsFileExist(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED)) {
            vector<string> overlayPaths;
            overlayPaths.push_back(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED);
            return impl->AddResource(SYSTEM_RESOURCE_PATH_COMPRESSED.c_str(), overlayPaths);
        }
        return impl->AddResource(SYSTEM_RESOURCE_PATH_COMPRESSED.c_str());
    }
    return false;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
