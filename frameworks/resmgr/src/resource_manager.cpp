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

#include "resource_manager.h"

#include "hilog_wrapper.h"
#include "resource_manager_impl.h"
#include "system_resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
static std::map<std::string, std::shared_ptr<ResourceManager>> resMgrMap;
static std::mutex resMgrLock;

ResourceManager *CreateResourceManager()
{
    ResourceManagerImpl *impl = new (std::nothrow) ResourceManagerImpl;
    if (impl == nullptr) {
        HILOG_ERROR("new ResourceManagerImpl failed when CreateResourceManager");
        return nullptr;
    }
    if (!impl->Init()) {
        delete (impl);
        return nullptr;
    }
    ResourceManagerImpl *systemResourceManager = SystemResourceManager::GetSystemResourceManager();
    if (systemResourceManager != nullptr) {
        impl->AddSystemResource(systemResourceManager);
    }
    return impl;
}

std::shared_ptr<ResourceManager> CreateResourceManager(std::string &bundleName, std::string &moduleName,
    std::string &hapPath, std::vector<std::string> overlayPath, ResConfig &resConfig)
{
    if (bundleName.size() == 0 || hapPath.size() == 0) {
        HILOG_ERROR("bundleName or hapPath is empty when CreateResourceManager");
        return nullptr;
    }
    std::string resMgrKey(bundleName);
    if (moduleName.size() != 0) {
        resMgrKey.append("/").append(moduleName);
    }
    std::lock_guard<std::mutex> lock(resMgrLock);
    auto iter = resMgrMap.find(resMgrKey);
    if (iter != resMgrMap.end()) {
        return resMgrMap[resMgrKey];
    }
    std::shared_ptr<ResourceManager> resourceManagerImpl(CreateResourceManager());
    if (resourceManagerImpl == nullptr) {
        HILOG_ERROR("CreateResourceManager failed");
        return nullptr;
    }
    bool result = false;
    if (overlayPath.size() != 0) {
        result = resourceManagerImpl->AddResource(hapPath, overlayPath);
    } else {
        result = resourceManagerImpl->AddResource(hapPath.c_str());
    }
    if (!result) {
        HILOG_ERROR("AddResource failed when CreateResourceManager");
        return nullptr;
    }
    resourceManagerImpl->UpdateResConfig(resConfig);
    resMgrMap[resMgrKey] = resourceManagerImpl;
    return resourceManagerImpl;
}

ResourceManager *GetSystemResourceManager()
{
    return SystemResourceManager::GetSystemResourceManager();
}

ResourceManager *GetSystemResourceManagerNoSandBox()
{
    return SystemResourceManager::GetSystemResourceManagerNoSandBox();
}

ResourceManager::~ResourceManager()
{}
} // namespace Resource
} // namespace Global
} // namespace OHOS
