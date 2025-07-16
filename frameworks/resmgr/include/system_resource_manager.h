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
#ifndef OHOS_RESOURCE_MANAGER_SYSTEMRESOURCEMANAGER_H
#define OHOS_RESOURCE_MANAGER_SYSTEMRESOURCEMANAGER_H

#include <memory>
#include "resource_manager_impl.h"

namespace OHOS {
namespace Global {
namespace Resource {
class SystemResourceManager {
public:
    SystemResourceManager();

    ~SystemResourceManager();

    /**
     * Get system resource manager, the added system resource is sandbox path. This method should call
     * after the sandbox mount.
     *
     * @return pointer of system resource manager
     */
    static ResourceManagerImpl *GetSystemResourceManager();

    /**
     * Get system resource manager, the added system resource is no sandbox path. This method should call
     * before the sandbox mount, for example appspawn.
     *
     * @return pointer of system resource manager
     */
    static ResourceManagerImpl *GetSystemResourceManagerNoSandBox();

    /**
     * Release system resource manager.
     */
    static void ReleaseSystemResourceManager();

    /**
     * Add system resource to hap resource vector.
     *
     * @param resourceManager the hap resource manager.
     */
    static bool AddSystemResource(ResourceManagerImpl *resourceManager);

    /**
     * create system resource manager
     *
     * @return the new system resource manager
     */
    EXPORT_FUNC static std::shared_ptr<ResourceManagerImpl> CreateSysResourceManager();

    /**
     * update system resource manager resconfig
     *
     * @param resConfig the current resconfig
     * @param isThemeSystemResEnable if true means is theme system res enable
     */
    static void UpdateSysResConfig(ResConfigImpl &resConfig, bool isThemeSystemResEnable);

#if defined(__ARKUI_CROSS__) || defined(__IDE_PREVIEW__)
    /**
     * add system resource for preview
     *
     * @param resMgr the system resource manager
     */
    static void AddSystemResourceForPreview(ResourceManagerImpl* resMgr);
#endif
private:
    static std::mutex mutex_;

    static ResourceManagerImpl *resourceManager_;

    static std::weak_ptr<ResourceManagerImpl> weakResourceManager_;

    static const std::string SYSTEM_RESOURCE_PATH;

    static const std::string SYSTEM_RESOURCE_PATH_COMPRESSED;

    static const std::string SYSTEM_RESOURCE_OVERLAY_PATH;

    static const std::string SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED;

    static const std::string SYSTEM_RESOURCE_NO_SAND_BOX_PKG_PATH;

    static const std::string SYSTEM_RESOURCE_NO_SAND_BOX_HAP_PATH;

    static const std::string SYSTEM_RESOURCE_EXT_NO_SAND_BOX_HAP_PATH;

    static bool LoadSystemResource(ResourceManagerImpl *impl, bool isSandbox = true);

    static ResourceManagerImpl *CreateSystemResourceManager(bool isSandbox);

    static std::shared_ptr<ResourceManagerImpl> CreateSystemResourceManager();

    static void SaveResConfig(ResConfigImpl &resConfig, bool isThemeSystemResEnable);

    static void UpdateResConfig(ResConfigImpl &resConfig, bool isThemeSystemResEnable);

    static bool CreateManagerInner(ResourceManagerImpl *impl);

    static std::mutex sysResMgrMutex_;

    static std::shared_ptr<ResourceManagerImpl> sysResMgr_;

    static std::shared_ptr<ResConfigImpl> resConfig_;

    static bool isUpdateAppConfig_;

    static bool isThemeSystemResEnable_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif