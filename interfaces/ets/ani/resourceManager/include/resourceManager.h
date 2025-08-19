/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include <memory>

#include "ani.h"
#include "application_context.h"

#include "resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResMgrAddon {
public:
    static ani_object CreateResMgr(ani_env* env, const std::string& bundleName,
        const std::shared_ptr<ResourceManager>& resMgr, std::shared_ptr<AbilityRuntime::Context> context);
    static ani_object WrapResourceManager(ani_env* env, std::shared_ptr<ResourceManager> resMgr);
private:
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
