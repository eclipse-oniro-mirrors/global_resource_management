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
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "resource_manager_impl_ext.h"
#endif
#include "theme_pack_manager.h"
namespace OHOS {
namespace Global {
namespace Resource {
static std::map<std::string, std::shared_ptr<ResourceManager>> resMgrMap;

ResourceManager *CreateResourceManager()
{
    ResourceManagerImpl *impl = new (std::nothrow) ResourceManagerImpl;
    if (impl == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResourceManagerImpl failed when CreateResourceManager");
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

std::shared_ptr<ResourceManager> CreateResourceManagerDef(
    const std::string &bundleName, const std::string &moduleName,
    const std::string &hapPath, const std::vector<std::string> &overlayPath,
    ResConfig &resConfig, int32_t userId)
{
    if (bundleName.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "bundleName or hapPath is empty when CreateResourceManagerDef");
        return nullptr;
    }
    std::shared_ptr<ResourceManager> resourceManagerImpl(CreateResourceManager());
    if (resourceManagerImpl == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "CreateResourceManagerDef failed bundleName = %{public}s moduleName = %{public}s",
            bundleName.c_str(), moduleName.c_str());
        return nullptr;
    }
    resourceManagerImpl->bundleInfo.first = bundleName;
    resourceManagerImpl->bundleInfo.second = moduleName;
    resourceManagerImpl->userId = userId;
    ThemePackManager::GetThemePackManager()->LoadThemeRes(bundleName, moduleName, userId);
    return resourceManagerImpl;
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__) && defined(RESMGR_BROKER_ENABLE)
std::shared_ptr<ResourceManager> CreateResourceManagerExt(const std::string &bundleName, const int32_t appType)
{
    if (bundleName.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "bundleName is empty when CreateResourceManagerExt");
        return nullptr;
    }
    ResourceManagerImplExt *impl = new (std::nothrow) ResourceManagerImplExt;
    if (impl == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResourceManagerImpl failed when CreateResourceManager");
        return nullptr;
    }
    if (!impl->Init(appType, bundleName)) {
        delete (impl);
        return nullptr;
    }
    std::shared_ptr<ResourceManager> resMgrImplExt(impl);
    if (resMgrImplExt == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "CreateResourceManagerExt failed bundleName = %{public}s",
                      bundleName.c_str());
        return nullptr;
    }
    return resMgrImplExt;
}
#endif

std::shared_ptr<ResourceManager> CreateResourceManager(const std::string &bundleName, const std::string &moduleName,
    const std::string &hapPath, const std::vector<std::string> &overlayPath,
    ResConfig &resConfig, int32_t appType, int32_t userId)
{
    if (appType == 0) {
        return CreateResourceManagerDef(bundleName, moduleName, hapPath, overlayPath, resConfig, userId);
    } else {
    #if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__) && defined(RESMGR_BROKER_ENABLE)
        return CreateResourceManagerExt(bundleName, appType);
    #else
        return nullptr;
    #endif
    }
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
