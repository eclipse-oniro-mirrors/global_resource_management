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
#ifndef OHOS_RESOURCE_MANAGER_RESOURCEMANAGEREXTMGR_H
#define OHOS_RESOURCE_MANAGER_RESOURCEMANAGEREXTMGR_H
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)

#include "resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {

typedef int (*IResMgrExt) (std::shared_ptr<ResourceManager> &resMgrExt, const std::string &bundleName,
    const int32_t appType);
class ResourceManagerExtMgr {
public:
    ResourceManagerExtMgr();
    virtual ~ResourceManagerExtMgr();
    bool Init(std::shared_ptr<ResourceManager> &resMgrExt, const std::string &bundleName, const int32_t appType);

private:
    void *handle_ = nullptr;
    std::map<std::string, std::shared_ptr<ResourceManager>> resMgrExtMap_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
#endif