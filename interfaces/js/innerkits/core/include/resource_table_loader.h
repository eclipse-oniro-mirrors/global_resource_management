/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef RESOURCE_TABLE_LOADER_H
#define RESOURCE_TABLE_LOADER_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "resource_manager_addon.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResourceTableLoader {
public:
    static void LoadTable(napi_env env, const std::shared_ptr<ResourceManagerAddon> &addon);
private:
    static void GetHapInfo(const std::shared_ptr<ResourceManagerAddon> &addon, std::string &bundleName,
        std::string &moduleName, std::string &loadPath);
    static void Load(napi_env env, const std::string &bundleName, const std::string &moduleName,
        const std::string &loadPath);
    static bool CheckModuleLoaded(const std::string &moduleName);
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
