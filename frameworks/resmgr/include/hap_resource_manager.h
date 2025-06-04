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
#ifndef OHOS_RESOURCE_MANAGER_HAPRESOURCEMANAGER_H
#define OHOS_RESOURCE_MANAGER_HAPRESOURCEMANAGER_H

#include <shared_mutex>
#include "hap_resource.h"
#include "res_config_impl.h"
#include "res_desc.h"
#include "resource_manager.h"

#ifdef SUPPORT_GRAPHICS
#include <unicode/plurrule.h>
#endif

namespace OHOS {
namespace Global {
namespace Resource {
class HapResourceManager {
public:
    HapResourceManager() {}
    ~HapResourceManager() {}

    static std::shared_ptr<HapResourceManager> GetInstance();

    /**
     * Load a HapResource.
     *
     * @param path hap file path
     * @param defaultConfig  match defaultConfig to keys of index file, only parse the matched keys.
     *     'null' means parse all keys.
     * @param isSystem If `isSystem` is true, the package is marked as a system package and allows some functions to
     *     filter out this package when computing what configurations/resources are available.
     * @param isOverlay If 'isOverlay' is true, the package is marked as an overlay package and overlay resource can
     *     replace non-overlay resource.
     * @return the resource if create load success, else nullptr
     */
    const std::shared_ptr<HapResource> Load(const char* path, std::shared_ptr<ResConfigImpl> &defaultConfig,
        bool isSystem = false, bool isOverlay = false, uint32_t selectedTypes = SELECT_ALL);
    
    /**
     * Load overlay resources
     * @param path the resources.index file path
     * @param overlayPath  the resource overlay path
     * @param defaultConfig the resource config
     * @param isSystem judge the overlay is system or not
     * @return the map of overlay resource path and resource info if success, else null
     */
    const std::unordered_map<std::string, std::shared_ptr<HapResource>> LoadOverlays(
        const std::string &path, const std::vector<std::string> &overlayPath,
        std::shared_ptr<ResConfigImpl> &defaultConfig, bool isSystem = false);

    /**
     * Put hapResource into the map of HapResourceManager
     *
     * @param path the resources.index file path
     * @param pResource hapResource
     * @return th resource
     */
    std::shared_ptr<HapResource> PutAndGetResource(const std::string &path, std::shared_ptr<HapResource> pResource);

    /**
     * Put patch resource
     *
     * @param path the resources.index file path
     * @param patchPath the patch path
     * @return Whether patch resource is put successfully
     */
    bool PutPatchResource(const std::string &path, const std::string &patchPath);

#if defined(__ARKUI_CROSS__)
    /**
     * remove resource from map
     *
     * @param path the resources.index file path
     */
    void RemoveHapResource(const std::string &path);
#endif

private:
    /**
     * get resource from map
     * @param path hap file path
     * @return the resource if map contain the hap, else nullptr
     */
    std::shared_ptr<HapResource> GetHapResource(const std::string &path);

    static std::recursive_mutex mutex_;
    static std::shared_ptr<HapResourceManager> instance_;
    std::shared_mutex mutexRw_;
    std::unordered_map<std::string, std::weak_ptr<HapResource>> hapResourceMap_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif