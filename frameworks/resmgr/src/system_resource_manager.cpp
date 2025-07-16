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
#include "resource_manager.h"
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

std::weak_ptr<ResourceManagerImpl> SystemResourceManager::weakResourceManager_;

std::mutex SystemResourceManager::mutex_;

std::shared_ptr<ResourceManagerImpl> SystemResourceManager::sysResMgr_ = nullptr;

std::mutex SystemResourceManager::sysResMgrMutex_;

std::shared_ptr<ResConfigImpl> SystemResourceManager::resConfig_ = nullptr;

bool SystemResourceManager::isUpdateAppConfig_ = true;

bool SystemResourceManager::isThemeSystemResEnable_ = false;

SystemResourceManager::SystemResourceManager()
{}

SystemResourceManager::~SystemResourceManager()
{}

ResourceManagerImpl *SystemResourceManager::GetSystemResourceManager()
{
    // SystemAbility is not forked from appspawn, so SystemAbility should load sandbox system resource.
    return CreateSystemResourceManager(true);
}

ResourceManagerImpl *SystemResourceManager::GetSystemResourceManagerNoSandBox()
{
    RESMGR_HILOGI(RESMGR_TAG, "GetSystemResourceManagerNoSandBox");
    // appspawn can only add no sandbox system resource path, so all app that forked from appspawn have
    // one global resourceManager.
    return CreateSystemResourceManager(false);
}

bool SystemResourceManager::InitResourceManager(ResourceManagerImpl *impl)
{
    if (!impl->Init(true)) {
        RESMGR_HILOGE(RESMGR_TAG, "init failed");
        return false;
    }
    std::shared_ptr<ResourceManagerImpl> sysResMgr = weakResourceManager_.lock();
    if (!sysResMgr) {
        sysResMgr = CreateSystemResourceManager();
    }
    if (!impl->AddSystemResource(sysResMgr)) {
        RESMGR_HILOGE(RESMGR_TAG, "add system resource failed");
        return false;
    }
    return true;
}

ResourceManagerImpl *SystemResourceManager::CreateSystemResourceManager(bool isSandbox)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (resourceManager_ == nullptr) {
        ResourceManagerImpl *impl = new (std::nothrow) ResourceManagerImpl;
        if (impl == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new ResourceManagerImpl failed when CreateSystemResourceManager");
            return nullptr;
        }
        if (!InitResourceManager(impl)) {
            delete impl;
            return nullptr;
        }
        resourceManager_ = impl;
    }
    return resourceManager_;
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

void SystemResourceManager::ReleaseSystemResourceManager()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (resourceManager_ != nullptr) {
        delete resourceManager_;
        resourceManager_ = nullptr;
        RESMGR_HILOGI(RESMGR_TAG, "ReleaseSystemResourceManager success");
    } else {
        RESMGR_HILOGI(RESMGR_TAG, "SystemResourceManager has been released");
    }
}

bool SystemResourceManager::AddSystemResource(ResourceManagerImpl *appResMgr)
{
    if (!appResMgr) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    std::shared_ptr<ResourceManagerImpl> sysResMgr = weakResourceManager_.lock();
    if (!sysResMgr) {
        sysResMgr = CreateSystemResourceManager();
    }
    appResMgr->AddSystemResource(sysResMgr);
    return true;
}

std::shared_ptr<ResourceManagerImpl> SystemResourceManager::CreateSystemResourceManager()
{
    std::shared_ptr<ResourceManagerImpl> sysResMgr = std::make_shared<ResourceManagerImpl>();
    if (!sysResMgr) {
        return nullptr;
    }
    if (!sysResMgr->Init(true)) {
        return nullptr;
    }
#if !defined(__ARKUI_CROSS__) && !defined(__IDE_PREVIEW__)
    if (!LoadSystemResource(sysResMgr.get(), false) && !LoadSystemResource(sysResMgr.get(), true)) {
        return nullptr;
    }
#endif
    weakResourceManager_ = sysResMgr;
    return sysResMgr;
}

std::shared_ptr<ResourceManagerImpl> SystemResourceManager::CreateSysResourceManager()
{
    std::lock_guard<std::mutex> lock(sysResMgrMutex_);
    if (sysResMgr_ != nullptr) {
        return sysResMgr_;
    }
    sysResMgr_ = std::make_shared<ResourceManagerImpl>();
    if (sysResMgr_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResourceManagerImpl failed when CreateSysResourceManager");
        return nullptr;
    }

    if (!InitResourceManager(sysResMgr_.get())) {
        RESMGR_HILOGE(RESMGR_TAG, "init sys resource manager failed");
        return nullptr;
    }

#if defined(__ARKUI_CROSS__) || defined(__IDE_PREVIEW__)
    AddSystemResourceForPreview(resourceManager_);
#endif
    if (resConfig_ != nullptr) {
        UpdateResConfig(*resConfig_, isThemeSystemResEnable_);
    }
    return sysResMgr_;
}

void SystemResourceManager::SaveResConfig(ResConfigImpl &resConfig, bool isThemeSystemResEnable)
{
    if (resConfig_ == nullptr) {
        resConfig_ = std::make_shared<ResConfigImpl>();
    }

    if (resConfig_ == nullptr) {
        return;
    }

    resConfig_->Copy(resConfig, true);
    if (isUpdateAppConfig_) {
        isUpdateAppConfig_ = false;
        isThemeSystemResEnable_ = isThemeSystemResEnable;
    }
}

void SystemResourceManager::UpdateResConfig(ResConfigImpl &resConfig, bool isThemeSystemResEnable)
{
    if (sysResMgr_ == nullptr) {
        return;
    }
    auto hapManager = sysResMgr_->GetHapManager();
    if (hapManager == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "sys resource manager hapManager is null");
        return;
    }
    hapManager->UpdateResConfig(resConfig);
    hapManager->UpdateAppConfigForSysResManager(resConfig.GetAppDarkRes(), isThemeSystemResEnable);
}

void SystemResourceManager::UpdateSysResConfig(ResConfigImpl &resConfig, bool isThemeSystemResEnable)
{
    std::lock_guard<std::mutex> lock(sysResMgrMutex_);
    if (resConfig.IsInvalidResConfig()) {
        return;
    }

    if (sysResMgr_ == nullptr) {
        SaveResConfig(resConfig, isThemeSystemResEnable);
        return;
    }
    UpdateResConfig(resConfig, isThemeSystemResEnable);
}

#if defined(__ARKUI_CROSS__) || defined(__IDE_PREVIEW__)
void SystemResourceManager::AddSystemResourceForPreview(ResourceManagerImpl* resMgr)
{
    if (resMgr == nullptr || sysResMgr_ == nullptr) {
        return;
    }
    sysResMgr_->AddSystemResource(resMgr);
}
#endif
} // namespace Resource
} // namespace Global
} // namespace OHOS
