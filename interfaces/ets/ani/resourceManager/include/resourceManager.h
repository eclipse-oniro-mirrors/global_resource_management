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

namespace OHOS {
namespace Global {
namespace Resource {
class ResMgrAddon {
public:
    ResMgrAddon(const std::string& bundleName, const std::shared_ptr<ResourceManager>& resMgr,
        const std::shared_ptr<OHOS::AbilityRuntime::Context>& context, bool isSystem = false);
    ResMgrAddon(const std::shared_ptr<ResourceManager>& resMgr, bool isSystem = false);
    static ani_object CreateResMgr(ani_env* env, const std::string& bundleName,
        const std::shared_ptr<ResourceManager>& resMgr, std::shared_ptr<AbilityRuntime::Context> context);
    static ani_object getSystemResourceManager(ani_env* env);

    static ani_string getStringSyncById(ani_env* env, ani_object object, ani_double resId);
    static ani_string getFormatStringSyncById(ani_env *env, ani_object object, ani_double resId, ani_object args);
    static ani_string getStringSync(ani_env* env, ani_object object, ani_object resource);
    static ani_string getFormatStringSync(ani_env *env, ani_object object, ani_object resource, ani_object args);
    static ani_double getNumberById(ani_env* env, ani_object object, ani_double resId);
    static ani_double getNumber(ani_env* env, ani_object object, ani_object resource);
    static ani_double getColorSyncById(ani_env* env, ani_object object, ani_double resId);
    static ani_double getColorSync(ani_env* env, ani_object object, ani_object resource);
    static ani_object getRawFileContentSync(ani_env* env, ani_object object, ani_string path);

    static ani_string GetIntPluralStringValueSyncById(ani_env* env, ani_object object,
        ani_double resId, ani_double num, ani_object args);
    static ani_string GetIntPluralStringValueSync(ani_env* env, ani_object object,
        ani_object resource, ani_double num, ani_object args);
    static ani_string GetIntPluralStringByNameSync(ani_env* env, ani_object object,
        ani_string resName, ani_double num, ani_object args);

    static ani_string GetDoublePluralStringValueSyncById(ani_env* env, ani_object object,
        ani_double resId, ani_double num, ani_object args);
    static ani_string GetDoublePluralStringValueSync(ani_env* env, ani_object object,
        ani_object resource, ani_double num, ani_object args);
    static ani_string GetDoublePluralStringByNameSync(ani_env* env, ani_object object,
        ani_string resName, ani_double num, ani_object args);
    static ani_status BindContext(ani_env* env);

    inline std::shared_ptr<ResourceManager> GetResMgr()
    {
        return resMgr_;
    }

    inline std::shared_ptr<AbilityRuntime::Context> GetContext()
    {
        return context_;
    }

    inline bool IsSystem()
    {
        return isSystem_;
    }

    bool isOverrideAddon()
    {
        return isOverrideAddon_;
    }

private:
    static ani_object WrapResourceManager(ani_env* env, std::shared_ptr<ResMgrAddon> &addon);

    std::string bundleName_;
    std::shared_ptr<ResourceManager> resMgr_;
    std::shared_ptr<AbilityRuntime::Context> context_;
    bool isSystem_;
    bool isOverrideAddon_ = false;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
