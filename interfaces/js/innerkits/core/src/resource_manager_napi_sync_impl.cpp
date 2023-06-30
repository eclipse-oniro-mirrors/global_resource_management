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

#include <unordered_map>
#include "drawable_descriptor.h"
#include "js_drawable_descriptor.h"
#include "resource_manager_napi_utils.h"
namespace OHOS {
namespace Global {
namespace Resource {
using namespace std::placeholders;
constexpr int ARRAY_SUBCRIPTOR_ZERO = 0;
constexpr int ARRAY_SUBCRIPTOR_ONE = 1;
constexpr int PARAMS_NUM_TWO = 2;
ResourceManagerNapiSyncImpl::ResourceManagerNapiSyncImpl() 
{}

ResourceManagerNapiSyncImpl::~ResourceManagerNapiSyncImpl()
{}

std::unordered_map<std::string, std::function<napi_value(napi_env&, napi_callback_info&)>>
    ResourceManagerNapiSyncImpl::syncFuncMatch {
    {"GetRawFileList", std::bind(&ResourceManagerNapiSyncImpl::GetRawFileList, _1, _2)},
    {"GetStringSync", std::bind(&ResourceManagerNapiSyncImpl::GetStringSync, _1, _2)},
    {"GetStringByNameSync", std::bind(&ResourceManagerNapiSyncImpl::GetStringByNameSync, _1, _2)},
    {"GetBoolean", std::bind(&ResourceManagerNapiSyncImpl::GetBoolean, _1, _2)},
    {"GetBooleanByName", std::bind(&ResourceManagerNapiSyncImpl::GetBooleanByName, _1, _2)},
    {"GetNumber", std::bind(&ResourceManagerNapiSyncImpl::GetNumber, _1, _2)},
    {"GetNumberByName", std::bind(&ResourceManagerNapiSyncImpl::GetNumberByName, _1, _2)},
    {"GetDrawableDescriptor", std::bind(&ResourceManagerNapiSyncImpl::GetDrawableDescriptor, _1, _2)},
    {"GetDrawableDescriptorByName", std::bind(&ResourceManagerNapiSyncImpl::GetDrawableDescriptorByName, _1, _2)},
    {"GetColorSync", std::bind(&ResourceManagerNapiSyncImpl::GetColorSync, _1, _2)},
    {"GetColorByNameSync", std::bind(&ResourceManagerNapiSyncImpl::GetColorByNameSync, _1, _2)},
    {"AddResource", std::bind(&ResourceManagerNapiSyncImpl::AddResource, _1, _2)},
    {"RemoveResource", std::bind(&ResourceManagerNapiSyncImpl::RemoveResource, _1, _2)}
};

napi_value ResourceManagerNapiSyncImpl::GetResource(napi_env env, napi_callback_info info,
    const std::string &functionName)
{
    auto functionIndex = syncFuncMatch.find(functionName);
    if (functionIndex == syncFuncMatch.end()) {
        HiLog::Info(LABEL, "Invalid functionName, %{public}s", functionName.c_str());
        return nullptr;
    }
    return functionIndex->second(env, info);
}

napi_value ResourceManagerNapiSyncImpl::GetRawFileList(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        HiLog::Error(LABEL, "Failed to get addon in GetRawfileList");
        return nullptr;
    }

    dataContext->path_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);

    RState state = dataContext->addon_->GetResMgr()->GetRawFileList(dataContext->path_.c_str(),
        dataContext->arrayValue_);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to get rawfile list by %{public}s", dataContext->path_.c_str());
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    return ResourceManagerNapiUtils::CreateJsArray(env, *dataContext);
}

bool ResourceManagerNapiSyncImpl::InitParamsFromParamArray(napi_env env, napi_value value,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    napi_valuetype valuetype = ResourceManagerNapiUtils::GetType(env, value);
    if (valuetype == napi_number) {
        double param;
        if (napi_get_value_double(env, value, &param) != napi_ok) {
            HiLog::Error(LABEL, "Failed to get parameter value in InitParamsFromParamArray");
            return false;
        }
        jsParams.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(param)));
        return true;
    }
    if (valuetype == napi_string) {
        size_t len = 0;
        if (napi_get_value_string_utf8(env, value, nullptr, 0, &len) != napi_ok) {
            HiLog::Error(LABEL, "Failed to get parameter length in InitParamsFromParamArray");
            return false;
        }
        std::vector<char> buf(len + 1);
        if (napi_get_value_string_utf8(env, value, buf.data(), len + 1, &len) != napi_ok) {
            HiLog::Error(LABEL, "Failed to get parameter value in InitParamsFromParamArray");
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

int32_t ResourceManagerNapiSyncImpl::ProcessStrResourceById(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        HiLog::Error(LABEL, "Failed to get resMgr in GetStringSync");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }

    if (!InitNapiParameters(env, info, dataContext->jsParams_)) {
        HiLog::Error(LABEL, "GetStringSync formatting error");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }

    RState state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetStringSync failed state", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetStringSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to init para in GetStringSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessStrResourceById(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process string in GetStringSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessColorResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        HiLog::Error(LABEL, "Failed to get resMgr in ProcessColorResource");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }

    RState state = resMgr->GetColorById(resId, dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetColor failed state", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetColorSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to init para in GetColor");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessColorResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process string in GetColor");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsColor(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessNumResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        HiLog::Error(LABEL, "Failed to ResourceManagerNapiUtils::GetHapResourceManager in GetNumber");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
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

    int32_t state = InitIdResourceAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in InitIdResourceAddon");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessNumResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process string in GetStringSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsNumber(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessBoolResource(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret2 = ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret2) {
        HiLog::Error(LABEL, "Failed to get resMgr in GetBoolean");
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
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
        HiLog::Error(LABEL, "Failed to process para in GetBoolean");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessBoolResource(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process bool resource in GetBoolean");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsBool(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetDrawableDescriptor(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    auto dataContext = std::make_unique<ResMgrDataContext>();
    int32_t ret = ResourceManagerNapiSyncImpl::InitIdResourceAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in ProcessIdResourceDensityParam");
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }
    // density optional parameters
    napi_valuetype valuetype = ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]);
    if (valuetype != napi_valuetype::napi_undefined && valuetype != napi_valuetype::napi_null &&
        ResourceManagerNapiUtils::GetDensity(env, argv[ARRAY_SUBCRIPTOR_ONE], dataContext->density_) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    if (!ResourceManagerNapiUtils::GetHapResourceManager(dataContext.get(), resMgr, resId)) {
        HiLog::Error(LABEL, "Failed to get ResourceManagerNapiUtils::GetHapResourceManager in GetDrawableDescriptor");
        return nullptr;
    }
    RState state = SUCCESS;
    Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor = Ace::Napi::DrawableDescriptorFactory::Create(resId, resMgr,
        state, drawableType, dataContext->density_);
    if (state != SUCCESS) {
        HiLog::Error(LABEL, "Failed to Create drawableDescriptor by %{public}d", resId);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    return Ace::Napi::JsDrawableDescriptor::ToNapi(env, drawableDescriptor.release(), drawableType);
}

int32_t ResourceManagerNapiSyncImpl::InitNameAddon(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
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
        HiLog::Error(LABEL, "GetStringByNameSync formatting error");
        return ERROR_CODE_RES_NAME_FORMAT_ERROR;
    }

    RState state = dataContext->addon_->GetResMgr()->GetStringFormatByName(dataContext->resName_.c_str(),
        dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetStringByNameSync failed state", true);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetStringByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in GetStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessStrResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in GetStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    
    return ResourceManagerNapiUtils::CreateJsString(env, *dataContext);
}

int32_t ResourceManagerNapiSyncImpl::ProcessColorResourceByName(napi_env env, napi_callback_info info,
    std::unique_ptr<ResMgrDataContext> &dataContext)
{
    RState state = dataContext->addon_->GetResMgr()->GetColorByName(dataContext->resName_.c_str(),
        dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetStringByNameSync failed state", true);
        return state;
    }
    return SUCCESS;
}

napi_value ResourceManagerNapiSyncImpl::GetColorByNameSync(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();

    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in GetStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessColorResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in GetStringByNameSync");
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

    int32_t state = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in GetStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    state = ProcessNumResourceByName(env, info, dataContext);
    if (state != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process string in GetStringSync");
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsNumber(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetBooleanByName(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    auto dataContext = std::make_unique<ResMgrDataContext>();

    int32_t ret = ResourceManagerNapiSyncImpl::InitNameAddon(env, info, dataContext);
    if (ret != RState::SUCCESS) {
        HiLog::Error(LABEL, "Failed to process para in GetStringByNameSync");
        ResourceManagerNapiUtils::NapiThrow(env, ret);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetBooleanByName(dataContext->resName_.c_str(),
        dataContext->bValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetBooleanByName failed state", true);
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }

    return ResourceManagerNapiUtils::CreateJsBool(env, *dataContext);
}

napi_value ResourceManagerNapiSyncImpl::GetDrawableDescriptorByName(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);
    if (!ResourceManagerNapiUtils::IsNapiString(env, info)) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    auto dataContext = std::make_unique<ResMgrDataContext>();
    // density optional parameters
    napi_valuetype valuetype = ResourceManagerNapiUtils::GetType(env, argv[ARRAY_SUBCRIPTOR_ONE]);
    if (valuetype != napi_valuetype::napi_undefined && valuetype != napi_valuetype::napi_null &&
        ResourceManagerNapiUtils::GetDensity(env, argv[ARRAY_SUBCRIPTOR_ONE], dataContext->density_) != SUCCESS) {
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    dataContext->addon_ = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (dataContext->addon_ == nullptr) {
        HiLog::Error(LABEL, "Failed to get addon_ in GetDrawableDescriptorByName");
        return nullptr;
    }
    dataContext->resName_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    auto resMgr = dataContext->addon_->GetResMgr();
    RState state = SUCCESS;
    Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor = Ace::Napi::DrawableDescriptorFactory::Create(dataContext->resName_.c_str(),
        resMgr, state, drawableType, dataContext->density_);
    if (state != SUCCESS) {
        HiLog::Error(LABEL, "Failed to Create drawableDescriptor by %{public}s", dataContext->resName_.c_str());
        ResourceManagerNapiUtils::NapiThrow(env, state);
        return nullptr;
    }
    return Ace::Napi::JsDrawableDescriptor::ToNapi(env, drawableDescriptor.release(), drawableType);
}

std::shared_ptr<ResourceManager> GetNativeResoruceManager(napi_env env, napi_callback_info info)
{
    auto addon = ResMgrDataContext::GetResourceManagerAddon(env, info);
    if (addon == nullptr) {
        HiLog::Error(LABEL, "Failed to get addon_ in GetNativeResoruceManager");
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
    dataContext->path_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    auto resMgr = GetNativeResoruceManager(env, info);
    if (!resMgr->AddAppOverlay(dataContext->path_)) {
        HiLog::Error(LABEL, "Failed to add overlay path = %{public}s", dataContext->path_.c_str());
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
    dataContext->path_ = ResourceManagerNapiUtils::GetResNameOrPath(env, argc, argv);
    auto resMgr = GetNativeResoruceManager(env, info);
    if (!resMgr->RemoveAppOverlay(dataContext->path_)) {
        HiLog::Error(LABEL, "Failed to add overlay path = %{public}s", dataContext->path_.c_str());
        ResourceManagerNapiUtils::NapiThrow(env, ERROR_CODE_OVERLAY_RES_PATH_INVALID);
        return nullptr;
    }
    return nullptr;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS