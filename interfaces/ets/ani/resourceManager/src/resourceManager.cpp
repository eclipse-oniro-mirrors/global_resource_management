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

#include "resourceManager.h"

#include "ani_signature.h"
#include "hilog_wrapper.h"
#include "resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {

ani_object ResMgrAddon::CreateResMgr(
    ani_env* env, const std::string& bundleName, const std::shared_ptr<ResourceManager>& resMgr,
    std::shared_ptr<AbilityRuntime::Context> context)
{
    return WrapResourceManager(env, resMgr);
}

ani_object ResMgrAddon::WrapResourceManager(ani_env* env, std::shared_ptr<ResourceManager> resMgr)
{
    ani_object nativeResMgr;
    ani_class cls;
    if (ANI_OK != env->FindClass(AniSignature::RESOURCE_MANAGER_INNER, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", AniSignature::RESOURCE_MANAGER_INNER);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    auto resMgrPtr = std::make_unique<std::shared_ptr<ResourceManager>>(resMgr);
    if (ANI_OK != env->Object_New(cls, ctor, &nativeResMgr, reinterpret_cast<ani_long>(resMgrPtr.get()))) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", AniSignature::RESOURCE_MANAGER_INNER);
        return nullptr;
    }
    resMgrPtr.release();
    return nativeResMgr;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS