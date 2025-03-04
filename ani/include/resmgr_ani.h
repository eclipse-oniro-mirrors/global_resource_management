/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef RESMGR_ANI_H
#define RESMGR_ANI_H

#include <ani.h>
#include <array>
#include <iostream>

#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/context.h"
#include "resource_manager.h"
#include "resource_manager_napi_context.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResMgrAddon {
public:
    ResMgrAddon(const std::string& bundleName,
        const std::shared_ptr<ResourceManager>& resMgr,
        const std::shared_ptr<OHOS::AbilityRuntime::Context>& context, bool isSystem = false);
    ResMgrAddon(const std::shared_ptr<ResourceManager>& resMgr, bool isSystem = false);
    static ani_object CreateResMgr(ani_env* env, const std::string& bundleName,
        const std::shared_ptr<ResourceManager>& resMgr, std::shared_ptr<AbilityRuntime::Context> context);
    static std::string AniStrToString(ani_env* env, ani_ref aniStr);
    static ani_object getSystemResourceManager([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object);
    static ani_string getStringSync0([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_double resId);
    static ani_string getStringSync2([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_object resource);
    static ani_double getNumber0([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_double resId);
    static ani_double getNumber1([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_object resource);
    static ani_double getColorSync0([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_double resId);
    static ani_double getColorSync1([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_object resource);
    static ani_object getRawFileContentSync([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_string path);
    static ani_string getPluralStringValueSync0([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_double resId, ani_double num);
    static ani_string getPluralStringValueSync1([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
        ani_object resource, ani_double num);
    static ani_object create([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_class clazz);
    static ani_status BindContext(ani_env* env);
private:
    static ani_object WrapResourceManager(ani_env* env, std::shared_ptr<ResMgrAddon> &addon);

    std::string bundleName_;
    std::shared_ptr<ResourceManager> resMgr_;
    std::shared_ptr<AbilityRuntime::Context> context_;
    [[maybe_unused]] bool isSystem_;
    std::shared_ptr<ResourceManagerNapiContext> napiContext_;
    [[maybe_unused]] bool isOverrideAddon_ = false;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
