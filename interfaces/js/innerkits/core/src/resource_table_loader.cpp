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

#include "resource_table_loader.h"

#include <unordered_set>

#include "ability_stage_context.h"
#include "ecmascript/napi/include/jsnapi.h"
#include "extractor.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Global {
namespace Resource {
constexpr char BUNDLE_INSTALL_PATH[] = "/data/storage/el1/bundle/";
constexpr char MERGE_ABC_PATH[] = "/ets/modules.abc";

static std::mutex mutex_;
static std::unordered_set<std::string> loadedHaps;

void ResourceTableLoader::LoadTable(napi_env env, const std::shared_ptr<ResourceManagerAddon> &addon)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string bundleName;
    std::string moduleName;
    std::string loadPath;
    GetHapInfo(addon, bundleName, moduleName, loadPath);
    if (bundleName.empty() || moduleName.empty() || loadPath.empty()) {
        RESMGR_HILOGD(RESMGR_JS_TAG, "HapInfo from context is empty");
        return;
    }
    if (loadedHaps.count(moduleName)) {
        return;
    }
    Load(env, bundleName, moduleName, loadPath);
    loadedHaps.insert(moduleName);
}

void ResourceTableLoader::GetHapInfo(const std::shared_ptr<ResourceManagerAddon> &addon, std::string &bundleName,
    std::string &moduleName, std::string &loadPath)
{
    auto context = addon->GetContext();
    if (!context) {
        return;
    }
    if (AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityStageContext>(context) != nullptr) {
        return;
    }
    auto hapModuleInfo = context->GetHapModuleInfo();
    if (!hapModuleInfo) {
        return;
    }
    bundleName = hapModuleInfo->bundleName;
    moduleName = hapModuleInfo->moduleName;
    loadPath = AbilityBase::ExtractorUtil::GetLoadFilePath(hapModuleInfo->hapPath);
}

void ResourceTableLoader::Load(napi_env env, const std::string &bundleName, const std::string &moduleName,
    const std::string &loadPath)
{
    NativeEngine *engine = reinterpret_cast<NativeEngine *>(env);
    if (!engine) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "NativeEngine is null");
        return;
    }
    auto vm = const_cast<EcmaVM *>(engine->GetEcmaVm());
    if (!vm) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "vm is null");
        return;
    }
    bool newCreate = false;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(loadPath, newCreate);
    if (!extractor) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetExtractor failed");
        return;
    }
    std::string abcPath = BUNDLE_INSTALL_PATH + moduleName + MERGE_ABC_PATH;
    auto safeData = extractor->GetSafeData(abcPath);
    if (!safeData) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetSafeData failed");
        return;
    }
    panda::JSExecutionScope executionScope(vm);
    panda::LocalScope scope(vm);
    panda::TryCatch trycatch(vm);
    std::string tablePath = "/build/generated/r/ResourceTable";
    std::string tableOhmurl = "@normalized:N&&&" + moduleName + tablePath + "&";
    auto data = safeData->GetDataPtr();
    auto size = safeData->GetDataLen();
    bool isTableExist = panda::JSNApi::IsExecuteModuleInAbcFileSecure(vm, data, size, abcPath, tableOhmurl);
    if (!isTableExist) {
        RESMGR_HILOGD(RESMGR_JS_TAG, "[%{public}s] normalized res table not exist", moduleName.c_str());
        // old format, unnormalized ohmurl
        tableOhmurl = "@bundle:" + bundleName + "/" + moduleName + tablePath;
        isTableExist = panda::JSNApi::IsExecuteModuleInAbcFileSecure(vm, data, size, abcPath, tableOhmurl);
        if (!isTableExist) {
            RESMGR_HILOGD(RESMGR_JS_TAG, "[%{public}s] unnormalized res table not exist", moduleName.c_str());
            return;
        }
    }
    panda::JSNApi::ExecuteSecureWithOhmUrl(vm, data, size, abcPath, tableOhmurl);
    panda::Local<panda::ObjectRef> exception = trycatch.GetAndClearException();
    if (!exception.IsEmpty() && !exception->IsHole()) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "[%{public}s] LoadTable failed", moduleName.c_str());
    }
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
