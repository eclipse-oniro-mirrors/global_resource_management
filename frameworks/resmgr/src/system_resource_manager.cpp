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

const std::string SystemResourceManager::SYSTEM_RESOURCE_PATH_COMPRESSED = "/data/global/" \
    "systemResources/SystemResources.hap";

const std::string SystemResourceManager::SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED = "/sys_prod/app/" \
    "SystemResourcesOverlay/SystemResourcesOverlay.hap";

const std::string SystemResourceManager::SYSTEM_RESOURCE_NO_SAND_BOX_PKG_PATH = "/system/app/ohos.global.systemres" \
    "/SystemResources.hap";

const std::string SystemResourceManager::SYSTEM_RESOURCE_NO_SAND_BOX_HAP_PATH = "/system/app/SystemResources" \
    "/SystemResources.hap";

const std::string SystemResourceManager::SYSTEM_RESOURCE_EXT_NO_SAND_BOX_HAP_PATH = "/system/app/SystemResources" \
    "/SystemResourcesExt.hap";

ResourceManagerImpl *SystemResourceManager::resourceManager_ = nullptr;

std::mutex SystemResourceManager::mutex_;

SystemResourceManager::SystemResourceManager()
{}

SystemResourceManager::~SystemResourceManager()
{}

ResourceManagerImpl *SystemResourceManager::GetSystemResourceManager()
{
    // SystemAbility is not forked from appspawn, so SystemAbility should load sandbox system resource.
    bool isCreated = CreateSystemResourceManager(true);
    if (!isCreated) {
        return nullptr;
    }
    return resourceManager_;
}

ResourceManagerImpl *SystemResourceManager::GetSystemResourceManagerNoSandBox()
{
    HILOG_INFO("GetSystemResourceManagerNoSandBox");
    // appspawn can only add no sandbox system resource path, so all app that forked from appspawn have
    // one global resourceManager.
    bool isCreated = CreateSystemResourceManager(false);
    if (!isCreated) {
        HILOG_WARN("CreateSystemResourceManager failed when GetSystemResourceManagerNoSandBox");
        return nullptr;
    }
    return resourceManager_;
}

bool SystemResourceManager::CreateSystemResourceManager(bool isSandbox)
{
    if (resourceManager_ != nullptr) {
        return true;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (resourceManager_ == nullptr) {
        ResourceManagerImpl *impl = new (std::nothrow) ResourceManagerImpl;
        if (impl == nullptr) {
            HILOG_ERROR("new ResourceManagerImpl failed when CreateSystemResourceManager");
            return false;
        }
        if (!impl->Init(true)) {
            delete impl;
            return false;
        }
        if (!LoadSystemResource(impl, isSandbox)) {
            delete impl;
            return false;
        }
        resourceManager_ = impl;
    }
    return true;
}

bool SystemResourceManager::LoadSystemResource(ResourceManagerImpl *impl, bool isSandbox)
{
    std::string sysPkgNamePath = SystemResourceManager::SYSTEM_RESOURCE_PATH;
    std::string sysHapNamePath = SystemResourceManager::SYSTEM_RESOURCE_PATH_COMPRESSED;
    std::string sysHapExtNamePath = SystemResourceManager::SYSTEM_RESOURCE_EXT_NO_SAND_BOX_HAP_PATH;
    if (!isSandbox) {
        sysPkgNamePath = SystemResourceManager::SYSTEM_RESOURCE_NO_SAND_BOX_PKG_PATH;
        sysHapNamePath = SystemResourceManager::SYSTEM_RESOURCE_NO_SAND_BOX_HAP_PATH;
    }
    if (Utils::IsFileExist(sysPkgNamePath)) {
        if (Utils::IsFileExist(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED)) {
            vector<string> overlayPaths;
            overlayPaths.push_back(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED);
            return impl->AddResource(sysPkgNamePath.c_str(), overlayPaths);
        }
        return impl->AddResource(sysPkgNamePath.c_str());
    }

    if (Utils::IsFileExist(sysHapNamePath)) {
        bool result = false;
        if (Utils::IsFileExist(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED)) {
            vector<string> overlayPaths;
            overlayPaths.push_back(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED);
            result = impl->AddResource(sysHapNamePath.c_str(), overlayPaths);
        } else {
            result = impl->AddResource(sysHapNamePath.c_str());
        }
        if (result && Utils::IsFileExist(sysHapExtNamePath)) {
            result = impl->AddResource(sysHapExtNamePath.c_str());
        }
        return result;
    }
    return false;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
