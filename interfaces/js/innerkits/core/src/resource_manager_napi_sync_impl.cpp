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

#include "resource_manager_napi_sync_impl.h"

#ifdef __IDE_PREVIEW__
#include <limits.h>
#endif
#include <unordered_map>
#include "drawable_descriptor/drawable_descriptor.h"
#include "drawable_descriptor/js_drawable_descriptor.h"
#include "resource_manager_napi_utils.h"
#include "resource_manager_addon.h"
namespace OHOS {
namespace Global {
namespace Resource {
using namespace std::placeholders;
constexpr int ARRAY_SUBCRIPTOR_ZERO = 0;
constexpr int ARRAY_SUBCRIPTOR_ONE = 1;
constexpr int ARRAY_SUBCRIPTOR_TWO = 2;
constexpr int PARAMS_NUM_TWO = 2;
constexpr int PARAMS_NUM_THREE = 3;
ResourceManagerNapiSyncImpl::ResourceManagerNapiSyncImpl()
{}

ResourceManagerNapiSyncImpl::~ResourceManagerNapiSyncImpl()
{}

std::unordered_map<std::string, std::function<napi_value(napi_env&, napi_callback_info&)>>
    ResourceManagerNapiSyncImpl::syncFuncMatch {
    {"GetStringSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetStringSync(env, info);}},
    {"GetStringByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetStringByNameSync(env, info);}},
    {"GetBoolean", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetBoolean(env, info);}},
    {"GetBooleanByName", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetBooleanByName(env, info);}},
    {"GetNumber", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetNumber(env, info);}},
    {"GetNumberByName", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetNumberByName(env, info);}},
    {"GetDrawableDescriptor", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetDrawableDescriptor(env, info);}},
    {"GetDrawableDescriptorByName", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetDrawableDescriptorByName(env, info);}},
    {"GetColorSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetColorSync(env, info);}},
    {"GetColorByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetColorByNameSync(env, info);}},
    {"AddResource", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::AddResource(env, info);}},
    {"RemoveResource", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::RemoveResource(env, info);}},
    {"GetMediaContentBase64Sync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetMediaContentBase64Sync(env, info);}},
    {"GetMediaContentSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetMediaContentSync(env, info);}},
    {"GetPluralStringValueSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetPluralStringValueSync(env, info);}},
    {"GetStringArrayValueSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetStringArrayValueSync(env, info);}},
    {"GetRawFileContentSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetRawFileContentSync(env, info);}},
    {"GetRawFdSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetRawFdSync(env, info);}},
    {"CloseRawFdSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::CloseRawFdSync(env, info);}},
    {"GetRawFileListSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetRawFileListSync(env, info);}},
    {"GetPluralStringByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetPluralStringByNameSync(env, info);}},
    {"GetMediaBase64ByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetMediaBase64ByNameSync(env, info);}},
    {"GetMediaByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetMediaByNameSync(env, info);}},
    {"GetStringArrayByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetStringArrayByNameSync(env, info);}},
    {"GetConfigurationSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetConfigurationSync(env, info);}},
    {"GetDeviceCapabilitySync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetDeviceCapabilitySync(env, info);}},
    {"GetLocales", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetLocales(env, info);}},
    {"GetSymbol", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetSymbol(env, info);}},
    {"GetSymbolByName", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetSymbolByName(env, info);}},
    {"IsRawDir", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::IsRawDir(env, info);}},
    {"GetOverrideResourceManager", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetOverrideResourceManager(env, info);}},
    {"GetOverrideConfiguration", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetOverrideConfiguration(env, info);}},
    {"UpdateOverrideConfiguration", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::UpdateOverrideConfiguration(env, info);}},
    {"GetIntPluralStringValueSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetIntPluralStringValueSync(env, info);}},
    {"GetDoublePluralStringValueSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetDoublePluralStringValueSync(env, info);}},
    {"GetIntPluralStringByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetIntPluralStringByNameSync(env, info);}},
    {"GetDoublePluralStringByNameSync", [](napi_env& env, napi_callback_info& info) -> napi_value {
        return ResourceManagerNapiSyncImpl::GetDoublePluralStringByNameSync(env, info);}},
};

napi_value ResourceManagerNapiSyncImpl::GetResource(napi_env env, napi_callback_info info,
    const std::string &functionName)
{
    auto functionIndex = syncFuncMatch.find(functionName);
    if (functionIndex == syncFuncMatch.end()) {
        RESMGR_HILOGI(RESMGR_JS_TAG, "Invalid functionName, %{public}s", functionName.c_str());
        return nullptr;
    }
    return functionIndex->second(env, info);
}

int32_t ResourceManagerNapiSyncImpl::InitPathAddon(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get addon in InitPathAddon");
        return NOT_FOUND;
    }
    if (ResourceManagerNapiUtils::IsNapiString(env, info)) {
        dataContext->path_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    } else {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetRawFileListSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t ret = InitPathAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to init para in GetRawFileListSync by rawfile");
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetRawFileList(dataContext->path_.c_str(),
        dataContext->arrayValue_);
    if (state != RState::SUCCESS || dataContext->arrayValue_.empty()) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get rawfile list");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_RES_PATH_INVALID);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsArray(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetRawFileContentSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t ret = InitPathAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to init para in GetRawFileContentSync");
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetRawFileFromHap(dataContext->path_,
        dataContext->len_, dataContext->mediaData);
    if (state != SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get rawfile");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsUint8Array(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetRawFdSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t ret = InitPathAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to init para in GetRawFdSync");
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetRawFileDescriptorFromHap(dataContext->path_,
        dataContext->descriptor_);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get rawfd");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    return ResourceManagerNapiUtils::CreateJsRawFd(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::CloseRawFdSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t ret = InitPathAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to init para in CloseRawFdSync");
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->CloseRawFileDescriptor(dataContext->path_);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to close rawfd");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    napi_value undefined;
    if (napi_get_undefined(env, &undefined) != napi_ok) {
        return nullptr;
    }
    return undefined;
}

bool ResourceManagerNapiSyncImpl::InitParamsFromParamArray(napi_env env, napi_value value,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    napi_valuetype valuetype = ResourceManagerNapiUtils::GetType(env, value);
    if (valuetype == napi_number) {
        double param;
        if (napi_get_value_double(env, value, &param) != napi_ok) {
            RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get parameter value in InitParamsFromParamArray");
            return false;
        }
        jsParams.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(param)));
        return true;
    }
    if (valuetype == napi_string) {
        size_t len = 0;
        if (napi_get_value_string_utf8(env, value, nullptr, 0, &len) != napi_ok) {
            RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get parameter length in InitParamsFromParamArray");
            return false;
        }
        std::vector<char> buf(len + 1);
        if (napi_get_value_string_utf8(env, value, buf.data(), len + 1, &len) != napi_ok) {
            RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get parameter value in InitParamsFromParamArray");
            return false;
        }
        jsParams.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_STRING, buf.data()));
        return true;
    }
    return false;
}

bool ResourceManagerNapiSyncImpl::InitNapiParameters(napi_env env, napi_callback_info info,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    size_t size = 1;
    napi_get_cb_info(env, info, &size, nullptr, nullptr, nullptr);
    // one parameter: resId or resource or Name
    if (size == 1) {
        return true;
    }
    napi_value paramArray[size];
    napi_get_cb_info(env, info, &size, paramArray, nullptr, nullptr);

    for (size_t i = 1; i < size; ++i) {
        if (!InitParamsFromParamArray(env, paramArray[i], jsParams)) {
            return false;
        }
    }
    return true;
}

int32_t ResourceManagerNapiSyncImpl::InitIdResourceAddon(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get addon in InitIdResourceAddon");
        return NOT_FOUND;
    }
    if (ResourceManagerNapiUtils::IsNapiNumber(env, info)) {
        dataContext->resId_ = ResourceManagerNapiUtils::GetResId(env, argc, argv);
    } else if (ResourceManagerNapiUtils::IsNapiObject(env, info)) {
        auto resourcePtr = std::make_shared<ResourceManager::Resource>();
        int32_t retCode = ResourceManagerNapiUtils::GetResourceObject(env, resourcePtr, argv[ARRAY_SUBCRIPTOR_ZERO]);
        dataContext->resource_ = resourcePtr;
        return retCode;
    } else {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    return SUCCESS;
}

int32_t ResourceManagerNapiSyncImpl::ProcessStrResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetStringSync");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }

    if (!InitNapiParameters(env, info, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetStringSync formatting error");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }

    RState state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetStringSync failed state", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetStringSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetStringSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessStrResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process string in GetStringSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessSymbolResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetSymbol");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }

    RState state = resMgr->GetSymbolById(resId, dataContext->symbolValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetSymbol failed state", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
    }
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetSymbol(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetSymbol", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessSymbolResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process symbol in GetSymbol", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsSymbol(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessColorResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in ProcessColorResource");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }

    RState state = resMgr->GetColorById(resId, dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetColor failed state", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetColorSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetColorSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessColorResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process string in GetColorSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsColor(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessNumResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to ResourceManagerNapiUtils::GetHapResourceManager in GetNumber");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }

    RState state = resMgr->GetIntegerById(resId, dataContext->iValue_);
    if (state != RState::SUCCESS) {
        state = resMgr->GetFloatById(resId, dataContext->fValue_);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetNumber", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessNumResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process string in GetNumber", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsNumber(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessBoolResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret2 = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret2) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetBoolean");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    RState state = resMgr->GetBooleanById(resId, dataContext->bValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetBoolean state", true);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetBoolean(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetBoolean", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessBoolResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process bool resource in GetBoolean", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsBool(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcesstMediaContentBase64Resource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret2 = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret2) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetMediaContentBase64Sync");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    RState state = resMgr->GetMediaBase64DataById(resId, dataContext->value_, dataContext->density_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetMediaContentBase64Sync state", true);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetMediaContentBase64Sync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetMediaContentBase64Sync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    // density optional parameters
    if (ResourceManagerNapiUtils::GetDataType(env, argv[ARRAY_SUBCRIPTOR_ONE], dataContext->density_) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    state = ProcesstMediaContentBase64Resource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process media base64 resource in GetMediaContentBase64Sync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessMediaContentResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret2 = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret2) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetMediaContentSync");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    RState state = resMgr->GetMediaDataById(resId, dataContext->len_, dataContext->mediaData,
        dataContext->density_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetMediaContentSync state", true);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetMediaContentSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetMediaContentSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    // density optional parameters
    if (ResourceManagerNapiUtils::GetDataType(env, argv[ARRAY_SUBCRIPTOR_ONE], dataContext->density_) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    state = ProcessMediaContentResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process media resource in GetMediaContentSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsUint8Array(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessPluralStringValueResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret2 = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret2) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetPluralStringValueSync");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    if (!InitNapiParameters(env, info, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetPluralStringValueSync formatting error");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }
    RState state = resMgr->GetFormatPluralStringById(dataContext->value_, resId, dataContext->param_,
        dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetPluralStringValueSync state", true, state);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetPluralStringValueSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetPluralStringValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    if (ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]) != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Parameter type is not napi_number in GetPluralStringValueSync");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_SUBCRIPTOR_ONE], &dataContext->param_);

    state = ProcessPluralStringValueResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process plural string resource in GetPluralStringValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessStringArrayValueResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret2 = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret2) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetStringArrayValueSync");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    RState state = resMgr->GetStringArrayById(resId, dataContext->arrayValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetStringArrayValueSync state", true);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetStringArrayValueSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetStringArrayValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessStringArrayValueResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process string array resource in GetStringArrayValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsArray(env, *dataContext);
}

RState ResourceManagerNapiSyncImpl::GetParamsFromArgv(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    GET_PARAMS(env, info, PARAMS_NUM_THREE);
    // density optional parameters
    if (ResourceManagerNapiUtils::GetDataType(env, argv[ARRAY_SUBCRIPTOR_ONE], dataContext->density_) != SUCCESS) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    // data type optional parameters
    if (ResourceManagerNapiUtils::GetDataType(env, argv[ARRAY_SUBCRIPTOR_TWO], dataContext->iconType_) != SUCCESS) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetDrawableDescriptor(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_THREE);
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t ret = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetDrawableDescriptor", true);
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    if (GetParamsFromArgv(env, info, dataContext) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId)) {
        dataContext->SetErrorMsg("Failed to get GetHapResourceManager in GetDrawableDescriptor", true);
        return nullptr;
    }
    RState state = SUCCESS;
    Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    if (dataContext->iconType_ == 1) {
        std::string themeMask = resMgr->GetThemeMask();
        std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
        std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
        state = resMgr->GetThemeIcons(resId, foregroundInfo, backgroundInfo, dataContext->density_);
        if (state == SUCCESS) {
            auto drawableDescriptor = Ace::Napi::DrawableDescriptorFactory::Create(foregroundInfo, backgroundInfo,
                themeMask, drawableType, resMgr);
            return Ace::Napi::JsDrawableDescriptor::ToNapi(env, drawableDescriptor.release(), drawableType);
        }
    }
    auto drawableDescriptor = Ace::Napi::DrawableDescriptorFactory::Create(resId, resMgr,
        state, drawableType, dataContext->density_);
    if (state != SUCCESS) {
        dataContext->SetErrorMsg("Failed to Create drawableDescriptor", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, resId);
        return nullptr;
    }
    return Ace::Napi::JsDrawableDescriptor::ToNapi(env, drawableDescriptor.release(), drawableType);
}

int32_t ResourceManagerNapiSyncImpl::InitNameAddon(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get addon in InitNameAddon");
        return NOT_FOUND;
    }
    if (ResourceManagerNapiUtils::IsNapiString(env, info)) {
        dataContext->resName_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    } else {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    return SUCCESS;
}

int32_t ResourceManagerNapiSyncImpl::ProcessStrResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    if (!InitNapiParameters(env, info, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetStringByNameSync formatting error");
        return ERROR_CODE_RES_NAME_FORMAT_ERROR;
    }

    RState state = dataContext->addon_->GetResMgr()->GetStringFormatByName(dataContext->resName_.c_str(),
        dataContext->value_, dataContext->jsParams_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetStringByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessStrResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process string in GetStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    
    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessSymbolResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    RState state = dataContext->addon_->GetResMgr()->GetSymbolByName(dataContext->resName_.c_str(),
        dataContext->symbolValue_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetSymbolByName(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetSymbolByName", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessSymbolResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process symbol in GetSymbolByName", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsSymbol(env, *dataContext);
}


int32_t ResourceManagerNapiSyncImpl::ProcessColorResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    RState state = dataContext->addon_->GetResMgr()->GetColorByName(dataContext->resName_.c_str(),
        dataContext->colorValue_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetColorByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetColorByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessColorResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process color in GetColorByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    
    return ResourceManagerNapiUtils::CreateJsColor(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessNumResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    auto resMgr = dataContext->addon_->GetResMgr();
    RState state = resMgr->GetIntegerByName(dataContext->resName_.c_str(),
        dataContext->iValue_);
    if (state != RState::SUCCESS) {
        state = resMgr->GetFloatByName(dataContext->resName_.c_str(), dataContext->fValue_);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetNumberByName(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetNumberByName", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessNumResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process number in GetNumberByName", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsNumber(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetBooleanByName(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t ret = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetBooleanByName", false);
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetBooleanByName(dataContext->resName_.c_str(),
        dataContext->bValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetBooleanByName failed state", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsBool(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetDrawableDescriptorByName(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_THREE);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }

    if (GetParamsFromArgv(env, info, dataContext) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        return nullptr;
    }
    dataContext->resName_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    auto resMgr = dataContext->addon_->GetResMgr();
    RState state = SUCCESS;
    Ace::Napi::DrawableDescriptor::DrawableType drawableType = Ace::Napi::DrawableDescriptor::DrawableType::BASE;
    if (dataContext->iconType_ == 1) {
        std::string themeMask = resMgr->GetThemeMask();
        std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
        std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
        if (resMgr->GetThemeIcons(0, foregroundInfo, backgroundInfo, dataContext->density_) == SUCCESS) {
            auto drawableDescriptor = Ace::Napi::DrawableDescriptorFactory::Create(foregroundInfo, backgroundInfo,
                themeMask, drawableType, resMgr);
            return Ace::Napi::JsDrawableDescriptor::ToNapi(env, drawableDescriptor.release(), drawableType);
        }
    }

    if (dataContext->iconType_ == 2) { // 2 means get the dynamic icon from theme
        std::pair<std::unique_ptr<uint8_t[]>, size_t> iconInfo;
        if (resMgr->GetDynamicIcon(dataContext->resName_, iconInfo, dataContext->density_) == SUCCESS) {
            auto drawableDescriptor = std::make_unique<Ace::Napi::DrawableDescriptor>(std::move(iconInfo.first),
                iconInfo.second);
            return Ace::Napi::JsDrawableDescriptor::ToNapi(env, drawableDescriptor.release(), drawableType);
        }
    }

    auto drawableDescriptor = Ace::Napi::DrawableDescriptorFactory::Create(dataContext->resName_.c_str(),
        resMgr, state, drawableType, dataContext->density_);
    if (state != SUCCESS) {
        dataContext->SetErrorMsg("Failed to Create drawableDescriptor ", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    return Ace::Napi::JsDrawableDescriptor::ToNapi(env, drawableDescriptor.release(), drawableType);
}

std::shared_ptr<ResourceManager> GetNativeResoruceManager(napi_env env, napi_callback_info info)
{
    auto addon = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (addon == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get addon_ in GetNativeResoruceManager");
        return nullptr;
    }
    return addon->GetResMgr();
}

napi_value ResourceManagerNapiSyncImpl::AddResource(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    dataContext->path_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    auto resMgr = GetNativeResoruceManager(env, info);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "resMgr is null, add overlay fail");
        return nullptr;
    }
    if (!resMgr->AddAppOverlay(dataContext->path_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to add overlay");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_OVERLAY_RES_PATH_INVALID);
        return nullptr;
    }
    return nullptr;
}

napi_value ResourceManagerNapiSyncImpl::RemoveResource(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    dataContext->path_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    auto resMgr = GetNativeResoruceManager(env, info);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "resMgr is null, remove overlay fail");
        return nullptr;
    }
    if (!resMgr->RemoveAppOverlay(dataContext->path_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to remove overlay");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_OVERLAY_RES_PATH_INVALID);
        return nullptr;
    }
    return nullptr;
}

int32_t ResourceManagerNapiSyncImpl::ProcessPluralStrResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    if (!InitNapiParameters(env, info, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetPluralStringByNameSync formatting error");
        return ERROR_CODE_RES_NAME_FORMAT_ERROR;
    }
    RState state = dataContext->addon_->GetResMgr()->GetFormatPluralStringByName(dataContext->value_,
        dataContext->resName_.c_str(), dataContext->param_, dataContext->jsParams_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetPluralStringByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetPluralStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    if (ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]) != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Parameter type is not napi_number in GetPluralStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    napi_get_value_int32(env, argv[ARRAY_SUBCRIPTOR_ONE], &dataContext->param_);

    state = ProcessPluralStrResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process plural string in GetPluralStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessMediaBase64ResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    RState state = dataContext->addon_->GetResMgr()->GetMediaBase64DataByName(dataContext->resName_.c_str(),
        dataContext->value_, dataContext->density_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetMediaBase64ByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetMediaBase64ByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    // density optional parameters
    if (ResourceManagerNapiUtils::GetDataType(env, argv[ARRAY_SUBCRIPTOR_ONE], dataContext->density_) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    state = ProcessMediaBase64ResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process media base64 resource in GetMediaBase64ByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessMediaResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    RState state = dataContext->addon_->GetResMgr()->GetMediaDataByName(dataContext->resName_.c_str(),
        dataContext->len_, dataContext->mediaData, dataContext->density_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetMediaByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetMediaByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    // density optional parameters
    if (ResourceManagerNapiUtils::GetDataType(env, argv[ARRAY_SUBCRIPTOR_ONE], dataContext->density_) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    state = ProcessMediaResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process media resource in GetMediaByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsUint8Array(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessStringArrayResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    RState state = dataContext->addon_->GetResMgr()->GetStringArrayByName(dataContext->resName_.c_str(),
        dataContext->arrayValue_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetStringArrayByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetStringArrayByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessStringArrayResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process string array resource in GetStringArrayByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    return ResourceManagerNapiUtils::CreateJsArray(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetConfigurationSync(napi_env env, napi_callback_info info)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get addon in GetConfigurationSync");
        return nullptr;
    }
    return ResourceManagerNapiUtils::CreateJsConfig(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetDeviceCapabilitySync(napi_env env, napi_callback_info info)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get addon in GetDeviceCapabilitySync");
        return nullptr;
    }
    return ResourceManagerNapiUtils::CreateJsDeviceCap(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetLocales(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get addon in GetLocales");
        return nullptr;
    }

    // includeSystem optional parameters
    if (ResourceManagerNapiUtils::GetIncludeSystem(env, argv[ARRAY_SUBCRIPTOR_ZERO],
        dataContext->bValue_) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    dataContext->addon_->GetResMgr()->GetLocales(dataContext->arrayValue_, dataContext->bValue_);
    return ResourceManagerNapiUtils::CreateJsArray(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::IsRawDir(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t ret = InitPathAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to init para in IsRawDir");
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->IsRawDirFromHap(dataContext->path_,
        dataContext->bValue_);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to determine the raw file is directory");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    return ResourceManagerNapiUtils::CreateJsBool(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetOverrideResourceManager(napi_env env, napi_callback_info info)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = getAddonAndConfig(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to get param in GetOverrideResourceManager", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> resMgr = dataContext->addon_->GetResMgr();
    std::shared_ptr<ResourceManager> overrideResMgr = resMgr->GetOverrideResourceManager(
        dataContext->overrideResConfig_);
    if (overrideResMgr == nullptr) {
        dataContext->SetErrorMsg("GetOverrideResourceManager, overrideResMgr is null", false);
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    return dataContext->addon_->CreateOverrideAddon(env, overrideResMgr);
}

RState ResourceManagerNapiSyncImpl::getAddonAndConfig(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "getAddonAndConfig failed to get addon");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (ResourceManagerNapiUtils::GetConfigObject(env, argv[ARRAY_SUBCRIPTOR_ZERO], dataContext) != SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "getAddonAndConfig failed to get resConfig");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetOverrideConfiguration(napi_env env, napi_callback_info info)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetOverrideConfiguration failed to get addon");
        return nullptr;
    }
    return ResourceManagerNapiUtils::CreateOverrideJsConfig(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::UpdateOverrideConfiguration(napi_env env, napi_callback_info info)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = getAddonAndConfig(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to get param in GetOverrideResourceManager", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> resMgr = dataContext->addon_->GetResMgr();
    state = resMgr->UpdateOverrideResConfig(*dataContext->overrideResConfig_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("UpdateOverrideConfiguration failed due to invalid config", false);
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
    }
    return nullptr;
}

bool ResourceManagerNapiSyncImpl::InitOptionalParameters(napi_env env, napi_callback_info info, uint32_t startIndex,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    size_t size = startIndex;
    napi_get_cb_info(env, info, &size, nullptr, nullptr, nullptr);
    // one parameter: resId or resource or Name
    if (size <= startIndex || size > SIZE_MAX) {
        return true;
    }
    napi_value paramArray[size];
    napi_get_cb_info(env, info, &size, paramArray, nullptr, nullptr);
    if (size > SIZE_MAX) {
        return true;
    }

    for (size_t i = startIndex; i < size; ++i) {
        if (!InitParamsFromParamArray(env, paramArray[i], jsParams)) {
            return false;
        }
    }
    return true;
}

int32_t ResourceManagerNapiSyncImpl::ProcessPluralStringResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret2 = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret2) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in GetHapResourceManager");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    uint32_t startIndex = 2;
    if (!InitOptionalParameters(env, info, startIndex, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "InitOptionalParameters formatting error");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }
    RState state = resMgr->GetFormatPluralStringById(dataContext->value_, resId, dataContext->quantity_,
        dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetFormatPluralStringById state", true, state);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetIntPluralStringValueSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetIntPluralStringValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    if (ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]) != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Parameter type is not napi_number in GetIntPluralStringValueSync");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    double num =  0;
    napi_get_value_double(env, argv[ARRAY_SUBCRIPTOR_ONE], &num);
    if (num > INT_MAX) {
        num = INT_MAX;
    } else if (num < INT_MIN) {
        num = INT_MIN;
    }
    dataContext->quantity_ = { true, num, 0.0 };
    
    state = ProcessPluralStringResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process plural string resource in GetIntPluralStringValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetDoublePluralStringValueSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetDoublePluralStringValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    if (ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]) != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Parameter type is not napi_number in GetDoublePluralStringValueSync");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    double num =  0;
    napi_get_value_double(env, argv[ARRAY_SUBCRIPTOR_ONE], &num);
    dataContext->quantity_ = { false, 0, num };

    state = ProcessPluralStringResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process plural string resource in GetDoublePluralStringValueSync", true);
        ResourceManagerNapiUtils::NapiThrow(env, state, dataContext->resId_);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessPluralStringResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    uint32_t startIndex = 2;
    if (!InitOptionalParameters(env, info, startIndex, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "InitOptionalParameters formatting error");
        return ERROR_CODE_RES_NAME_FORMAT_ERROR;
    }
    RState state = dataContext->addon_->GetResMgr()->GetFormatPluralStringByName(dataContext->value_,
        dataContext->resName_.c_str(), dataContext->quantity_, dataContext->jsParams_);
    return state;
}

napi_value ResourceManagerNapiSyncImpl::GetIntPluralStringByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetIntPluralStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    if (ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]) != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Parameter type is not napi_number in GetIntPluralStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    double num =  0;
    napi_get_value_double(env, argv[ARRAY_SUBCRIPTOR_ONE], &num);
    if (num > INT_MAX) {
        num = INT_MAX;
    } else if (num < INT_MIN) {
        num = INT_MIN;
    }
    dataContext->quantity_ = { true, num, 0.0 };

    state = ProcessPluralStringResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process plural string in GetIntPluralStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetDoublePluralStringByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    auto dataContext = std::make_unique<ResMgrDataContext>();
    if (dataContext == nullptr) {
        return nullptr;
    }
    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in GetDoublePluralStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    if (ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]) != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Parameter type is not napi_number in GetDoublePluralStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    double num = 0.0;
    napi_get_value_double(env, argv[ARRAY_SUBCRIPTOR_ONE], &num);
    dataContext->quantity_ = { false, 0, num };

    state = ProcessPluralStringResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process plural string in GetDoublePluralStringByNameSync", false);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}
} // namespace Resource
} // namespace Global
} // namespace OHOS