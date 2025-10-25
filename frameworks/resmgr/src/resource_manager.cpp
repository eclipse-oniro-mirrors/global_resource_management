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
#include "resource_manager_ext_mgr.h"
#endif
#include "theme_pack_manager.h"
namespace OHOS {
namespace Global {
namespace Resource {
namespace {
constexpr int32_t INVALID_USER_ID = -1;
constexpr int32_t DEFAULT_USER_ID = 100;
}
static std::map<std::string, std::shared_ptr<ResourceManager>> resMgrMap;
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
static std::mutex resMgrExtLock;
static std::shared_ptr<ResourceManagerExtMgr> resMgrExtMgr = std::make_shared<ResourceManagerExtMgr>();
#endif

ResourceManager *CreateResourceManager(bool includeSystemRes)
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
    if (includeSystemRes) {
        SystemResourceManager::AddSystemResource(impl);
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
    if (userId == INVALID_USER_ID) {
        resourceManagerImpl->userId = DEFAULT_USER_ID;
    } else {
        resourceManagerImpl->userId = userId;
    }
    uint32_t currentId = resConfig.GetThemeId();
    auto themePackManager = ThemePackManager::GetThemePackManager();
    if (themePackManager->IsFirstLoadResource() || themePackManager->UpdateThemeId(currentId)
        || themePackManager->IsUpdateByUserId(resourceManagerImpl->userId)) {
        RESMGR_HILOGD(RESMGR_TAG, "CreateResourceManagerDef LoadThemeRes");
        themePackManager->LoadThemeRes(bundleName, moduleName, resourceManagerImpl->userId);
    }
    return resourceManagerImpl;
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::shared_ptr<ResourceManager> CreateResourceManagerExt(
    const std::string &bundleName, const int32_t appType, const int32_t userId)
{
    if (bundleName.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "bundleName is empty when CreateResourceManagerExt");
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(resMgrExtLock);
    std::shared_ptr<ResourceManager> resMgrExt;
    if (!resMgrExtMgr->Init(resMgrExt, bundleName, appType, userId) || resMgrExt == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "ResourceManagerExt init fail");
        return nullptr;
    }
    return resMgrExt;
}
#endif

std::shared_ptr<ResourceManager> CreateResourceManager(const std::string &bundleName, const std::string &moduleName,
    const std::string &hapPath, const std::vector<std::string> &overlayPath,
    ResConfig &resConfig, int32_t appType, int32_t userId)
{
    if (appType == 0) {
        return CreateResourceManagerDef(bundleName, moduleName, hapPath, overlayPath, resConfig, userId);
    } else {
    #if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
        return CreateResourceManagerExt(bundleName, appType, userId);
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

void ReleaseSystemResourceManager()
{
    return SystemResourceManager::ReleaseSystemResourceManager();
}

ResourceManager::~ResourceManager()
{}
} // namespace Resource
} // namespace Global
} // namespace OHOS
