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

private:
    static std::mutex mutex_;

    static ResourceManagerImpl *resourceManager_;

    static const std::string SYSTEM_RESOURCE_PATH;

    static const std::string SYSTEM_RESOURCE_PATH_COMPRESSED;

    static const std::string SYSTEM_RESOURCE_OVERLAY_PATH;

    static const std::string SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED;

    static const std::string SYSTEM_RESOURCE_NO_SAND_BOX_PKG_PATH;

    static const std::string SYSTEM_RESOURCE_NO_SAND_BOX_HAP_PATH;

    static const std::string SYSTEM_RESOURCE_EXT_NO_SAND_BOX_HAP_PATH;

    static bool LoadSystemResource(ResourceManagerImpl *impl, bool isSandbox = true);

    static bool CreateSystemResourceManager(bool isSandbox = true);
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif