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
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "resource_manager_ext_mgr.h"

#include <dlfcn.h>
#include "hilog_wrapper.h"

namespace OHOS {
namespace Global {
namespace Resource {
ResourceManagerExtMgr::ResourceManagerExtMgr()
{
    HILOG_INFO("ResourceManagerExtMgr::ResourceManagerExtMgr");
}

ResourceManagerExtMgr::~ResourceManagerExtMgr()
{
    HILOG_INFO("ResourceManagerExtMgr::~ResourceManagerExtMgr()");
    for (auto entry : resMgrExtMap_) {
        entry.second.reset();
        entry.second = nullptr;
    }
    resMgrExtMap_.clear();
    if (handle_ != nullptr) {
        dlclose(handle_);
    }
    handle_ = nullptr;
}

void ResourceManagerExtMgr::Init(std::shared_ptr<ResourceManager> &resMgrExt, const std::string &bundleName)
{
    if (handle_ == nullptr) {
        std::string pluginPath = "system/lib64/libglobal_resmgr_broker.z.so";
        handle_ = dlopen(pluginPath.c_str(), RTLD_LAZY);
        if (handle_ == nullptr) {
            HILOG_ERROR("open so fail");
            return;
        }
    }
    std::string resMgrExtKey(bundleName);
    auto iter = resMgrExtMap_.find(resMgrExtKey);
    if (iter != resMgrExtMap_.end()) {
        resMgrExt = resMgrExtMap_[resMgrExtKey];
        return;
    }
    IResMgrExt iResMgrExt = (IResMgrExt)dlsym(handle_, "CreateResMgrExt");
    int ret = (*iResMgrExt)(resMgrExt, bundleName);
    if (ret) {
        HILOG_ERROR("CreateResMgrExt fail.");
        return;
    }
    resMgrExtMap_[bundleName] = resMgrExt;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif