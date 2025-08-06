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

#include "hap_resource_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Global {
namespace Resource {

HapResourceManager& HapResourceManager::GetInstance()
{
    static HapResourceManager instance;
    return instance;
}

std::shared_ptr<HapResource> HapResourceManager::PutAndGetResource(const std::string &path,
    std::shared_ptr<HapResource> pResource)
{
    std::unique_lock<std::shared_mutex> lock(mutexRw_);
    auto iter = hapResourceMap_.find(path);
    if (iter != hapResourceMap_.end()) {
        auto res = iter->second.lock();
        if (res && res->GetLastModTime() == pResource->GetLastModTime()) {
            return res;
        }
    }
    hapResourceMap_[path] = pResource;
    return pResource;
}

bool HapResourceManager::PutPatchResource(const std::string &path, const std::string &patchPath)
{
    std::unique_lock<std::shared_mutex> lock(mutexRw_);
    if (hapResourceMap_.find(path) != hapResourceMap_.end()) {
        std::shared_ptr<HapResource> hapResource = hapResourceMap_[path].lock();
        if (hapResource) {
            hapResource->SetPatchPath(patchPath);
            hapResource->SetIsPatch(true);
            return true;
        }
    }
    return false;
}

std::shared_ptr<HapResource> HapResourceManager::GetHapResource(const std::string &path)
{
    std::shared_lock<std::shared_mutex> lock(mutexRw_);
    auto iter = hapResourceMap_.find(path);
    if (iter != hapResourceMap_.end()) {
        return iter->second.lock();
    }
    return nullptr;
}

#if defined(__ARKUI_CROSS__)
void HapResourceManager::RemoveHapResource(const std::string &path)
{
    std::unique_lock<std::shared_mutex> lock(mutexRw_);
    auto iter = hapResourceMap_.find(path);
    if (iter != hapResourceMap_.end()) {
        hapResourceMap_.erase(iter);
    }
}
#endif
} // namespace Resource
} // namespace Global
} // namespace OHOS