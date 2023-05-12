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

ResourceManager *GetSystemResourceManager()
{
    return SystemResourceManager::GetSystemResourceManager();
}

ResourceManager::~ResourceManager()
{}
} // namespace Resource
} // namespace Global
} // namespace OHOS
