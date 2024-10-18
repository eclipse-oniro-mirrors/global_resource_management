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
#include "resource_manager_napi_context.h"

#include "resource_manager_napi_sync_impl.h"
#include "resource_manager_napi_async_impl.h"
namespace OHOS {
namespace Global {
namespace Resource {
ResourceManagerNapiContext::ResourceManagerNapiContext()
{}

ResourceManagerNapiContext::~ResourceManagerNapiContext()
{}

napi_value ResourceManagerNapiContext::ContextGetResource(napi_env env, napi_callback_info info,
    const std::string &methodName, FunctionType functionType)
{
    std::shared_ptr<ResourceManagerNapiBase> napiBase;
    if (functionType == FunctionType::SYNC) {
        napiBase = std::make_shared<ResourceManagerNapiSyncImpl>();
    } else {
        napiBase = std::make_shared<ResourceManagerNapiAsyncImpl>();
    }
    return napiBase->GetResource(env, info, methodName);
}
} // namespace Resource
} // namespace Global
} // namespace OHOS