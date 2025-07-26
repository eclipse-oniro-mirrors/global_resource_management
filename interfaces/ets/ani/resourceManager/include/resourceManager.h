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

#include <ani.h>
#include <array>
#include <iostream>

#include "application_context.h"
#include "resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResMgrAddon {
public:
    ResMgrAddon(const std::string& bundleName, const std::shared_ptr<ResourceManager>& resMgr,
        const std::shared_ptr<OHOS::AbilityRuntime::Context>& context, bool isSystem = false);
    ResMgrAddon(const std::shared_ptr<ResourceManager>& resMgr, bool isSystem = false);
    ani_object CreateOverrideAddon(ani_env* env, const std::shared_ptr<ResourceManager>& resMgr);

    static ani_object CreateResMgr(ani_env* env, const std::string& bundleName,
        const std::shared_ptr<ResourceManager>& resMgr, std::shared_ptr<AbilityRuntime::Context> context);
    static ani_object GetSystemResourceManager(ani_env* env);

    static ani_string GetStringSyncById(ani_env* env, ani_object object, ani_double resId);
    static ani_string GetFormatStringSyncById(ani_env *env, ani_object object, ani_double resId, ani_object args);
    
    static ani_string GetStringByNameSync(ani_env* env, ani_object object, ani_string resName);
    static ani_string GetFormatStringByNameSync(ani_env *env, ani_object object, ani_string resName, ani_object args);

    static ani_boolean GetBooleanById(ani_env* env, ani_object object, ani_double resId);
    static ani_boolean GetBooleanByName(ani_env* env, ani_object object, ani_string resName);

    static ani_double GetNumberById(ani_env* env, ani_object object, ani_double resId);
    static ani_double GetNumberByName(ani_env* env, ani_object object, ani_string resName);

    static ani_double GetColorSyncById(ani_env* env, ani_object object, ani_double resId);

    static ani_string GetIntPluralStringValueSyncById(ani_env* env, ani_object object,
        ani_double resId, ani_double num, ani_object args);

    static ani_string GetIntPluralStringByNameSync(ani_env* env, ani_object object,
        ani_string resName, ani_double num, ani_object args);

    static ani_string GetDoublePluralStringValueSyncById(ani_env* env, ani_object object,
        ani_double resId, ani_double num, ani_object args);

    static ani_string GetDoublePluralStringByNameSync(ani_env* env, ani_object object,
        ani_string resName, ani_double num, ani_object args);

    static ani_double GetColorByNameSync(ani_env* env, ani_object object, ani_string resName);

    static void AddResource(ani_env* env, ani_object object, ani_string path);
    static void RemoveResource(ani_env* env, ani_object object, ani_string path);

    static ani_object GetRawFdSync(ani_env* env, ani_object object, ani_string path);
    static void CloseRawFdSync(ani_env* env, ani_object object, ani_string path);
    static ani_boolean IsRawDir(ani_env* env, ani_object object, ani_string path);

    static ani_object GetRawFileListSync(ani_env* env, ani_object object, ani_string path);
    static ani_object GetRawFileContentSync(ani_env* env, ani_object object, ani_string path);

    static ani_object GetMediaContentSyncById(ani_env* env, ani_object object,
        ani_double resId, ani_object density);


    static ani_string GetMediaContentBase64SyncById(ani_env* env, ani_object object,
        ani_double resId, ani_object density);


    static ani_object GetStringArrayValueSyncById(ani_env* env, ani_object object, ani_double resId);
    static ani_object GetStringArrayByNameSync(ani_env* env, ani_object object, ani_string resName);

    static ani_object GetMediaByNameSync(ani_env* env, ani_object object, ani_string resName, ani_object density);
    static ani_string GetMediaBase64ByNameSync(ani_env* env, ani_object object, ani_string resName, ani_object density);

    static ani_object GetConfigurationSync(ani_env* env, ani_object object);
    static ani_object GetDeviceCapabilitySync(ani_env* env, ani_object object);
    static ani_object GetLocales(ani_env* env, ani_object object, ani_object includeSystem);

    static ani_double GetSymbolById(ani_env* env, ani_object object, ani_double resId);
    static ani_double GetSymbolByName(ani_env* env, ani_object object, ani_string resName);

    static ani_object GetOverrideResourceManager(ani_env* env, ani_object object, ani_object configuration);
    static ani_object GetOverrideConfiguration(ani_env* env, ani_object object);
    static void UpdateOverrideConfiguration(ani_env* env, ani_object object, ani_object configuration);
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
    bool isSystem_ = false;
    bool isOverrideAddon_ = false;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
