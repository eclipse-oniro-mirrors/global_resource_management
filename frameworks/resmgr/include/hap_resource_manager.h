/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
     * Put hapResource into the map of HapResourceManager
     *
     * @param path the resources.index file path
     * @param hapResource  hapResource
     * @return th resource
     */
    std::shared_ptr<HapResource> PutAndGetResource(const std::string path, std::shared_ptr<HapResource> hapResource);

    /**
     * Put patch resource
     *
     * @return Whether patch resource is put successfully
     */
    bool PutPatchResource(const std::string path, std::string patchPath);

    /**
     * get resource from map
     *
     * @return the resource
     */
    std::shared_ptr<HapResource> getHapResource(const std::string path);

private:
    static std::recursive_mutex mutex_;
    static std::shared_ptr<HapResourceManager> instance_;
    std::shared_mutex mutexRw_;
    std::unordered_map<std::string, std::weak_ptr<HapResource>> hapResourceMap_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif