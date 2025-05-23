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

#include "resource_manager_napi_utils.h"

#include "hilog/log_cpp.h"
#include "securec.h"
#include "utils/string_utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
constexpr int ARRAY_SUBCRIPTOR_ZERO = 0;
constexpr int PARAMS_NUM_TWO = 2;

const std::unordered_map<int32_t, std::string> ResourceManagerNapiUtils::ErrorCodeToMsg {
    {ERROR_CODE_INVALID_INPUT_PARAMETER, "Invalid input parameter"},
    {ERROR_CODE_RES_ID_NOT_FOUND, "Invalid resource ID: %lu"},
    {ERROR_CODE_RES_NAME_NOT_FOUND, "Invalid resource name"},
    {ERROR_CODE_RES_NOT_FOUND_BY_ID, "No matching resource is found based on the resource ID"},
    {ERROR_CODE_RES_NOT_FOUND_BY_NAME, "No matching resource is found based on the resource name"},
    {ERROR_CODE_RES_PATH_INVALID, "Invalid relative path"},
    {ERROR_CODE_RES_REF_TOO_MUCH, "The resource is referenced cyclically"},
    {ERROR_CODE_RES_ID_FORMAT_ERROR, "Failed to format the resource obtained based on the resource ID"},
    {ERROR_CODE_RES_NAME_FORMAT_ERROR, "Failed to format the resource obtained based on the resource Name"},
    {ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED, "Failed to access the system resource"},
    {ERROR_CODE_OVERLAY_RES_PATH_INVALID, "Invalid overlay path"},
    {ERROR, "Unknow error"}
};

bool ResourceManagerNapiUtils::IsNapiString(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[ARRAY_SUBCRIPTOR_ZERO], &valueType);
    if (valueType != napi_string) {
        RESMGR_HILOGD(RESMGR_JS_TAG, "Parameter type is not napi_string");
        return false;
    }
    return true;
}

bool ResourceManagerNapiUtils::IsNapiNumber(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[ARRAY_SUBCRIPTOR_ZERO], &valueType);
    if (valueType != napi_number) {
        return false;
    }
    return true;
}

bool ResourceManagerNapiUtils::IsNapiObject(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, PARAMS_NUM_TWO);

    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[ARRAY_SUBCRIPTOR_ZERO], &valueType);
    if (valueType != napi_object) {
        RESMGR_HILOGI(RESMGR_JS_TAG, "Parameter type is not napi_object");
        return false;
    }
    return true;
}

napi_valuetype ResourceManagerNapiUtils::GetType(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, value, &valueType);
    return valueType;
}

std::string ResourceManagerNapiUtils::GetResNameOrPath(napi_env env, size_t argc, napi_value *argv)
{
    if (argc == 0 || argv == nullptr) {
        return "";
    }

    napi_valuetype valuetype;
    napi_typeof(env, argv[ARRAY_SUBCRIPTOR_ZERO], &valuetype);
    if (valuetype != napi_string) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Invalid param, not string");
        return "";
    }
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, argv[ARRAY_SUBCRIPTOR_ZERO], nullptr, 0, &len);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resName or rawfile path length");
        return "";
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv[ARRAY_SUBCRIPTOR_ZERO], buf.data(), len + 1, &len);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resName or raw file path");
        return "";
    }
    return buf.data();
}

uint32_t ResourceManagerNapiUtils::GetResId(napi_env env, size_t argc, napi_value *argv)
{
    if (argc == 0 || argv == nullptr) {
        return 0;
    }

    napi_valuetype valuetype;
    napi_status status = napi_typeof(env, argv[ARRAY_SUBCRIPTOR_ZERO], &valuetype);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get value type");
        return 0;
    }
    if (valuetype != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Invalid param, not number");
        return 0;
    }
    int64_t resId = 0;
    status = napi_get_value_int64(env, argv[ARRAY_SUBCRIPTOR_ZERO], &resId);
    if (status != napi_ok || resId < 0 || resId > UINT32_MAX) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get id number");
        return 0;
    }

    return static_cast<uint32_t>(resId);
}

std::string ResourceManagerNapiUtils::FindErrMsg(int32_t errCode)
{
    auto iter = ResourceManagerNapiUtils::ErrorCodeToMsg.find(errCode);
    std::string errMsg = iter != ResourceManagerNapiUtils::ErrorCodeToMsg.end() ? iter->second : "";
    return errMsg;
}

void ResourceManagerNapiUtils::NapiThrow(napi_env env, int32_t errCode, ...)
{
    napi_value code = nullptr;
    napi_create_string_latin1(env, std::to_string(errCode).c_str(), NAPI_AUTO_LENGTH, &code);

    napi_value message = nullptr;
    va_list args;
    va_start(args, errCode);
    std::string errMsg = FormatString(FindErrMsg(errCode).c_str(), args);
    va_end(args);
    napi_create_string_latin1(env, errMsg.c_str(), NAPI_AUTO_LENGTH, &message);
    if (errMsg != "") {
        napi_value error = nullptr;
        napi_create_error(env, code, message, &error);
        napi_throw(env, error);
    }
}

napi_value ResourceManagerNapiUtils::CreateJsArray(napi_env env, ResMgrDataContext &dataContext)
{
    napi_value result;
    napi_status status = napi_create_array_with_length(env, dataContext.arrayValue_.size(), &result);
    if (status != napi_ok) {
        dataContext.SetErrorMsg("Failed to create array");
        return nullptr;
    }
    for (size_t i = 0; i < dataContext.arrayValue_.size(); i++) {
        napi_value value;
        status = napi_create_string_utf8(env, dataContext.arrayValue_[i].c_str(), NAPI_AUTO_LENGTH, &value);
        if (status != napi_ok) {
            dataContext.SetErrorMsg("Failed to create string item");
            return nullptr;
        }
        status = napi_set_element(env, result, i, value);
        if (status != napi_ok) {
            dataContext.SetErrorMsg("Failed to set array item");
            return nullptr;
        }
    }
    return result;
}

napi_value ResourceManagerNapiUtils::CreateJsUint8Array(napi_env env, ResMgrDataContext &context)
{
    napi_value buffer;
    uint8_t *data;
    napi_status status = napi_create_arraybuffer(env, context.len_, reinterpret_cast<void **>(&data), &buffer);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create media array buffer");
        return nullptr;
    }

    napi_value result = nullptr;
    status = napi_create_typedarray(env, napi_uint8_array, context.len_, buffer, 0, &result);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create media typed array");
        return nullptr;
    }

    if (context.len_ == 0) {
        return result;
    }

    uint8_t *tempData = context.mediaData.release();
    int ret = memcpy_s(data, context.len_, tempData, context.len_);
    delete[] tempData;
    if (ret != 0) {
        context.SetErrorMsg("Failed to copy media to buffer");
        return result;
    }
    return result;
}

napi_value ResourceManagerNapiUtils::CreateJsRawFd(napi_env env, ResMgrDataContext &context)
{
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create result");
        return result;
    }

    napi_value fd;
    status = napi_create_int32(env, context.descriptor_.fd, &fd);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create fd");
        return result;
    }
    status = napi_set_named_property(env, result, "fd", fd);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to set fd");
        return result;
    }

    napi_value offset;
    status = napi_create_int64(env, context.descriptor_.offset, &offset);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create offset");
        return result;
    }
    status = napi_set_named_property(env, result, "offset", offset);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to set offset");
        return result;
    }

    napi_value length;
    status = napi_create_int64(env, context.descriptor_.length, &length);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create length");
        return result;
    }
    status = napi_set_named_property(env, result, "length", length);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to set length");
        return result;
    }
    return result;
}

napi_value ResourceManagerNapiUtils::CloseJsRawFd(napi_env env, ResMgrDataContext& context)
{
    napi_value undefined;
    if (napi_get_undefined(env, &undefined) != napi_ok) {
        return nullptr;
    }
    RState state = context.addon_->GetResMgr()->CloseRawFileDescriptor(context.path_);
    if (state != RState::SUCCESS) {
        context.SetErrorMsg("CloseRawFileDescriptor failed state", true, state);
        return nullptr;
    }
    return undefined;
}

napi_value ResourceManagerNapiUtils::CreateJsString(napi_env env, ResMgrDataContext& context)
{
    napi_value result;
    if (napi_create_string_utf8(env, context.value_.c_str(), NAPI_AUTO_LENGTH, &result) != napi_ok) {
        context.SetErrorMsg("Failed to create result");
        return result;
    }
    return result;
}

napi_value ResourceManagerNapiUtils::CreateJsNumber(napi_env env, ResMgrDataContext& context)
{
    napi_value jsValue = nullptr;
    napi_status status;
    if (context.iValue_) {
        status = napi_create_int32(env, context.iValue_, &jsValue);
    } else {
        status = napi_create_double(env, context.fValue_, &jsValue);
    }
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create js number", true);
    }
    return jsValue;
}

napi_value ResourceManagerNapiUtils::CreateJsBool(napi_env env, ResMgrDataContext& context)
{
    napi_value jsValue = nullptr;
    if (napi_get_boolean(env, context.bValue_, &jsValue) != napi_ok) {
        context.SetErrorMsg("Failed to create result", true);
    }
    return jsValue;
}

bool ResourceManagerNapiUtils::GetResourceObjectName(napi_env env,
    std::shared_ptr<ResourceManager::Resource> &resourcePtr, napi_value &value, int32_t type)
{
    std::string typeName("moduleName");
    if (type == 0) {
        typeName = std::string("bundleName");
    }
    napi_value name;
    napi_status status = napi_get_named_property(env, value, typeName.c_str(), &name);
    if (status != napi_ok || name == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resource name property");
        return false;
    }
    if (ResourceManagerNapiUtils::GetType(env, name) != napi_string) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resource name string");
        return false;
    }
    size_t len = 0;
    status = napi_get_value_string_utf8(env, name, nullptr, 0, &len);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resource len");
        return false;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, name, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resource name value");
        return false;
    }
    if (type == 0) {
        resourcePtr->bundleName = buf.data();
    } else {
        resourcePtr->moduleName = buf.data();
    }
    return true;
}

bool ResourceManagerNapiUtils::GetResourceObjectId(napi_env env,
    std::shared_ptr<ResourceManager::Resource> &resourcePtr, napi_value &value)
{
    napi_value id;
    napi_status status = napi_get_named_property(env, value, "id", &id);
    if (status != napi_ok || id == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resource id property");
        return false;
    }
    if (ResourceManagerNapiUtils::GetType(env, id) != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resource id number");
        return false;
    }
    int64_t resId = 0;
    status = napi_get_value_int64(env, id, &resId);
    if (resId == -1) {
        resourcePtr->id = 0;
        return true;
    }
    if (status != napi_ok || resId < 0 || resId > UINT32_MAX) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resource id value");
        return false;
    }
    resourcePtr->id = static_cast<uint32_t>(resId);
    return true;
}

int32_t ResourceManagerNapiUtils::GetResourceObject(napi_env env,
    std::shared_ptr<ResourceManager::Resource> &resourcePtr, napi_value &value)
{
    if (resourcePtr == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "resourcePtr == nullptr");
        return ERROR;
    }
    if (!GetResourceObjectName(env, resourcePtr, value, 0)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get bundleName");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!GetResourceObjectName(env, resourcePtr, value, 1)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get moduleName");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!GetResourceObjectId(env, resourcePtr, value)) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get id");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    return SUCCESS;
}

bool ResourceManagerNapiUtils::GetHapResourceManager(const ResMgrDataContext* dataContext,
    std::shared_ptr<ResourceManager> &resMgr, uint32_t &resId)
{
    std::shared_ptr<ResourceManager::Resource> resource = dataContext->resource_;
    // In fa module, resource is null.
    if (resource == nullptr) {
        resMgr = dataContext->addon_->GetResMgr();
        resId = dataContext->resId_;
        return true;
    }

    // In stage module and isSystem is true, resId is the resource object id.
    if (dataContext->addon_->IsSystem()) {
        resMgr = dataContext->addon_->GetResMgr();
        resId = resource->id;
        return true;
    }

    resId = resource->id;
    if (dataContext->addon_->isOverrideAddon()) {
        resMgr = dataContext->addon_->GetResMgr();
        return true;
    }
    auto context = dataContext->addon_->GetContext();
    if (context == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetHapResourceManager context == nullptr");
        return false;
    }
    std::string bundleName(resource->bundleName);
    if (bundleName.empty()) {
        auto applicationInfo = context->GetApplicationInfo();
        if (applicationInfo != nullptr) {
            bundleName = applicationInfo->name;
        }
    }
    auto moduleContext = context->CreateModuleContext(bundleName, resource->moduleName);
    if (moduleContext == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetHapResourceManager moduleContext == nullptr, bundleName = %{public}s," \
            "moduleName = %{public}s", bundleName.c_str(), resource->moduleName.c_str());
        return false;
    }
    resMgr = moduleContext->GetResourceManager();
    return true;
}

napi_value ResourceManagerNapiUtils::CreateJsDeviceCap(napi_env env, ResMgrDataContext& context)
{
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        context.SetErrorMsg("Failed to create ResConfig object.");
        return nullptr;
    }
    context.addon_->GetResMgr()->GetResConfig(*cfg);

    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create GetDeviceCapability object");
        return nullptr;
    }

    napi_value deviceType;
    status = napi_create_int32(env, static_cast<int>(cfg->GetDeviceType()), &deviceType);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create deviceType");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "deviceType", deviceType);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to set deviceType property");
        return nullptr;
    }

    napi_value screenDensity;
    status = napi_create_int32(env, static_cast<int>(cfg->ConvertDensity(cfg->GetScreenDensity())), &screenDensity);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create screenDensity");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "screenDensity", screenDensity);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to set screenDensity property");
        return nullptr;
    }
    return result;
}


std::string ResourceManagerNapiUtils::GetLocale(std::unique_ptr<ResConfig> &cfg)
{
    std::string result;
#ifdef SUPPORT_GRAPHICS
    const icu::Locale *localeInfo = cfg->GetLocaleInfo();
    if (localeInfo == nullptr) {
        return result;
    }
    const char *lang = localeInfo->getLanguage();
    if (lang == nullptr) {
        return result;
    }
    result = lang;

    const char *script = localeInfo->getScript();
    if (script != nullptr) {
        result += std::string("_") + script;
    }

    const char *region = localeInfo->getCountry();
    if (region != nullptr) {
        result += std::string("_") + region;
    }
#endif
    return result;
}

napi_value ResourceManagerNapiUtils::CreateJsConfig(napi_env env, ResMgrDataContext& context)
{
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        context.SetErrorMsg("Failed to create ResConfig object.");
        return nullptr;
    }
    context.addon_->GetResMgr()->GetResConfig(*cfg);
    return CreateConfig(env, context, cfg);
}

napi_value ResourceManagerNapiUtils::CreateOverrideJsConfig(napi_env env, ResMgrDataContext& context)
{
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        context.SetErrorMsg("Failed to create ResConfig object.");
        return nullptr;
    }
    context.addon_->GetResMgr()->GetOverrideResConfig(*cfg);
    return CreateConfig(env, context, cfg);
}

napi_value ResourceManagerNapiUtils::CreateConfig(napi_env env,
    ResMgrDataContext& context, std::unique_ptr<ResConfig> &cfg)
{
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create Configuration object");
        return nullptr;
    }

    // write locale
    napi_value locale;
    status = napi_create_string_utf8(env, ResourceManagerNapiUtils::GetLocale(cfg).c_str(), NAPI_AUTO_LENGTH, &locale);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create locale");
        return nullptr;
    }
    status = napi_set_named_property(env, result, "locale", locale);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to set locale property");
        return nullptr;
    }

    // write other int properties
    SetIntProperty(env, context, result, "direction", static_cast<int>(cfg->GetDirection()));
    SetIntProperty(env, context, result, "deviceType", static_cast<int>(cfg->GetDeviceType()));
    SetIntProperty(env, context, result, "screenDensity", static_cast<int>(cfg->GetScreenDensityDpi()));
    SetIntProperty(env, context, result, "colorMode", static_cast<int>(cfg->GetColorMode()));
    SetIntProperty(env, context, result, "mcc", static_cast<int>(cfg->GetMcc()));
    SetIntProperty(env, context, result, "mnc", static_cast<int>(cfg->GetMnc()));

    return result;
}

bool ResourceManagerNapiUtils::SetIntProperty(napi_env env,
    ResMgrDataContext& context, napi_value &object, const std::string &property, const int &value)
{
    napi_value napi_val;
    napi_status status = napi_create_int32(env, value, &napi_val);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to create %{public}s", property.c_str());
        return false;
    }
    status = napi_set_named_property(env, object, property.c_str(), napi_val);
    if (status != napi_ok) {
        context.SetErrorMsg("Failed to set %{public}s property", property.c_str());
        return false;
    }
    return true;
}

RState ResourceManagerNapiUtils::GetConfigObject(napi_env env,
    napi_value object, std::unique_ptr<ResMgrDataContext> &dataContext)
{
    napi_valuetype valueType;
    napi_typeof(env, object, &valueType);
    if (valueType == napi_undefined || valueType == napi_null) {
        RESMGR_HILOGD(RESMGR_JS_TAG, "GetConfigObject, no config");
        return SUCCESS;
    }
    if (valueType != napi_object) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetConfigObject, param not object");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    ResConfig *config = CreateDefaultResConfig();
    if (config == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetConfigObject, new config failed");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    dataContext->overrideResConfig_.reset(config);

    if (!GetEnumParamOfConfig(env, dataContext->overrideResConfig_, object)) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!GetLocaleOfConfig(env, dataContext->overrideResConfig_, object)) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    return SUCCESS;
}

bool ResourceManagerNapiUtils::GetEnumParamOfConfig(napi_env env,
    std::shared_ptr<ResConfig> configPtr, napi_value &object)
{
    std::vector<std::string> properties = {"direction", "deviceType", "screenDensity", "colorMode", "mcc", "mnc"};
    for (const auto& property : properties) {
        napi_value napi_val;
        napi_status status = napi_get_named_property(env, object, property.c_str(), &napi_val);
        if (status != napi_ok) {
            RESMGR_HILOGE(RESMGR_JS_TAG, "GetEnumParamOfConfig failed to get property %{public}s", property.c_str());
            return false;
        }
        if (napi_val == nullptr) {
            RESMGR_HILOGD(RESMGR_JS_TAG, "GetEnumParamOfConfig property %{public}s not set", property.c_str());
            continue;
        }
        if (ResourceManagerNapiUtils::GetType(env, napi_val) != napi_number) {
            RESMGR_HILOGE(RESMGR_JS_TAG,
                "GetEnumParamOfConfig type of property %{public}s is not number", property.c_str());
            return false;
        }
        int value;
        status = napi_get_value_int32(env, napi_val, &value);
        if (status != napi_ok) {
            RESMGR_HILOGE(RESMGR_JS_TAG,
                "GetEnumParamOfConfig failed to get value of property %{public}s", property.c_str());
            return false;
        }

        RESMGR_HILOGD(RESMGR_JS_TAG, "GetEnumParamOfConfig, %{public}s = %{public}d", property.c_str(), value);
        if (property == "direction") {
            configPtr->SetDirection(static_cast<Direction>(value));
        } else if (property == "deviceType") {
            configPtr->SetDeviceType(static_cast<DeviceType>(value));
        } else if (property == "screenDensity") {
            configPtr->SetScreenDensityDpi(static_cast<ScreenDensity>(value));
        } else if (property == "colorMode") {
            configPtr->SetColorMode(static_cast<ColorMode>(value));
        } else if (property == "mcc") {
            configPtr->SetMcc(value);
        } else if (property == "mnc") {
            configPtr->SetMnc(value);
        }
    }
    return true;
}

bool ResourceManagerNapiUtils::GetLocaleOfConfig(
    napi_env env, std::shared_ptr<ResConfig> configPtr, napi_value &object)
{
#ifdef SUPPORT_GRAPHICS
    napi_value locale;
    napi_status status = napi_get_named_property(env, object, "locale", &locale);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetLocaleOfConfig failed to get property locale");
        return false;
    }
    if (locale == nullptr) {
        RESMGR_HILOGD(RESMGR_JS_TAG, "GetLocaleOfConfig property locale not set");
        return true;
    }
    if (ResourceManagerNapiUtils::GetType(env, locale) != napi_string) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetLocaleOfConfig type of property locale is not string");
        return false;
    }

    size_t len = 0;
    status = napi_get_value_string_utf8(env, locale, nullptr, 0, &len);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetLocaleOfConfig failed to get locale len");
        return false;
    }
    if (len == 0) {
        RESMGR_HILOGD(RESMGR_JS_TAG, "GetLocaleOfConfig locale not set or empty");
        return true;
    }

    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, locale, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetLocaleOfConfig failed to get locale value");
        return false;
    }

    if (configPtr->SetLocaleInfo(buf.data()) != SUCCESS) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "GetLocaleOfConfig failed to SetLocaleInfo");
        return false;
    }
#endif
    return true;
}

napi_value ResourceManagerNapiUtils::CreateJsColor(napi_env env, ResMgrDataContext& context)
{
    napi_value jsColorValue;
    if (napi_create_uint32(env, context.colorValue_, &jsColorValue) != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get density");
        return nullptr;
    }
    return jsColorValue;
}

napi_value ResourceManagerNapiUtils::CreateJsSymbol(napi_env env, ResMgrDataContext& context)
{
    napi_value jsSymbolValue;
    if (napi_create_uint32(env, context.symbolValue_, &jsSymbolValue) != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get symbol");
        return nullptr;
    }
    return jsSymbolValue;
}

RState ResourceManagerNapiUtils::GetDataType(napi_env env, napi_value value, uint32_t& density)
{
    napi_valuetype valuetype;
    napi_typeof(env, value, &valuetype);
    if (valuetype == napi_undefined || valuetype == napi_null) {
        return SUCCESS;
    }
    if (valuetype != napi_number) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Invalid param, not number");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    if (napi_get_value_uint32(env, value, &density) != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get density");
        return NOT_FOUND;
    }
    return SUCCESS;
}

RState ResourceManagerNapiUtils::GetIncludeSystem(napi_env env, napi_value value, bool &includeSystem)
{
    napi_valuetype valuetype;
    napi_typeof(env, value, &valuetype);
    if (valuetype == napi_undefined || valuetype == napi_null) {
        return SUCCESS;
    }
    if (valuetype != napi_boolean) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Invalid param, not boolean");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    if (napi_get_value_bool(env, value, &includeSystem) != napi_ok) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get includeSystem");
        return NOT_FOUND;
    }
    return SUCCESS;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS