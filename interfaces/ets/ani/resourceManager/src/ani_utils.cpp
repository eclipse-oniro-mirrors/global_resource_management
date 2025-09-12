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

#include "ani_utils.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "hilog_wrapper.h"
#include "ani_signature_builder.h"
#include "ani_signature.h"
#include "rstate.h"

namespace OHOS {
namespace Global {
namespace Resource {

constexpr int SCREEN_DENSITY_SDPI_ORDER = 0;
constexpr int SCREEN_DENSITY_MDPI_ORDER = 1;
constexpr int SCREEN_DENSITY_LDPI_ORDER = 2;
constexpr int SCREEN_DENSITY_XLDPI_ORDER = 3;
constexpr int SCREEN_DENSITY_XXLDPI_ORDER = 4;
constexpr int SCREEN_DENSITY_XXXLDPI_ORDER = 5;

const std::unordered_map<int32_t, std::string> errorMsg {
    {ERROR_CODE_INVALID_INPUT_PARAMETER, "Invalid input parameter"},
    {ERROR_CODE_RES_ID_NOT_FOUND, "Invalid resource ID"},
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

std::string AniUtils::FindErrMsg(int32_t errCode)
{
    auto iter = errorMsg.find(errCode);
    if (iter == errorMsg.end()) {
        return "";
    }
    return iter->second;
}

void AniUtils::ThrowAniError(ani_env *env, ani_int code, const std::string &message)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::BUSINESSERROR, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class %{public}s failed, status %{public}d",
            AniSignature::BUSINESSERROR, status);
        return;
    }
    ani_method ctor;
    arkts::ani_signature::SignatureBuilder sb;
    status = env->Class_FindMethod(cls, "<ctor>", sb.BuildSignatureDescriptor().c_str(), &ctor);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method BusinessError constructor failed, status: %{public}d.", status);
        return;
    }
    ani_object error;
    status = env->Object_New(cls, ctor, &error);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Create BusinessError failed, status: %{public}d.", status);
        return;
    }
    status = env->Object_SetPropertyByName_Int(error, "code", code);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set BusinessError.code = %{public}d failed, status: %{public}d.", code, status);
        return;
    }
    ani_string messageRef;
    status = env->String_NewUTF8(message.c_str(), message.size(), &messageRef);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Create message string failed, status: %{public}d.", status);
        return;
    }
    status = env->Object_SetPropertyByName_Ref(error, "message", static_cast<ani_ref>(messageRef));
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set BusinessError message failed, status:%{public}d.", status);
        return;
    }
    status = env->ThrowError(static_cast<ani_error>(error));
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "ThrowError BusinessError failed, status:%{public}d.", status);
    }
}

void AniUtils::AniThrow(ani_env *env, int32_t errCode)
{
    std::string errMsg = FindErrMsg(errCode);
    if (errMsg != "") {
        ThrowAniError(env, errCode, errMsg);
    }
}

std::shared_ptr<ResourceManager> AniUtils::GetResourceManager(ani_env* env, ani_object jsResMgr)
{
    ani_long nativePtr;
    ani_status status = env->Object_GetFieldByName_Long(jsResMgr, "nativeResMgr", &nativePtr);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get nativeResMgr failed from etsResMgr, status: %{public}d.", status);
        return nullptr;
    }
    std::shared_ptr<ResourceManager>* resMgr = reinterpret_cast<std::shared_ptr<ResourceManager>*>(nativePtr);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "NativePtr is nullptr, unknown error.");
        return nullptr;
    }
    return *reinterpret_cast<std::shared_ptr<ResourceManager>*>(nativePtr);
}

ani_string AniUtils::CreateAniString(ani_env *env, const std::string &str)
{
    ani_string result;
    ani_status status = env->String_NewUTF8(str.c_str(), str.size(), &result);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to create ani_string, status: %{public}d", status);
        return nullptr;
    }
    return result;
}

std::string AniUtils::AniStrToString(ani_env *env, ani_string aniStr)
{
    ani_size size = 0;
    ani_status status = env->String_GetUTF8Size(aniStr, &size);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get string size failed, status: %{public}d.", status);
        return "";
    }
    std::string result;
    result.resize(size, 0);
    ani_size written;
    status = env->String_GetUTF8(aniStr, result.data(), size + 1, &written);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get string data failed, status: %{public}d.", status);
        return "";
    }
    return result;
}

ani_object AniUtils::CreateAniRawFileDescriptor(ani_env *env, const ResourceManager::RawFileDescriptor& descriptor)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::RAWFILE_DESCRIPTOR_INNER, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class RawFileDescriptor failed, status: %{public}d.", status);
        return nullptr;
    }

    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method RawFileDescriptor.constructor failed, status: %{public}d.", status);
        return nullptr;
    }

    ani_object obj;
    status = env->Object_New(cls, ctor, &obj);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Create RawFileDescriptor failed, status: %{public}d.", status);
        return nullptr;
    }
    status = env->Object_SetPropertyByName_Int(obj, "fd", descriptor.fd);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set RawFileDescriptor property 'fd' failed, status: %{public}d.", status);
        return nullptr;
    }
    status = env->Object_SetPropertyByName_Long(obj, "offset", descriptor.offset);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set RawFileDescriptor property 'offset' failed, status: %{public}d.", status);
        return nullptr;
    }
    status = env->Object_SetPropertyByName_Long(obj, "length", descriptor.length);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set RawFileDescriptor property 'length' failed, status: %{public}d.", status);
        return nullptr;
    }
    return obj;
}

RState AniUtils::ConfigurationToResConfig(ani_env *env, ani_object configuration,
    std::shared_ptr<ResConfig> &resConfig)
{
    ani_boolean isUndefined;
    ani_status status = env->Reference_IsUndefined(configuration, &isUndefined);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get configuration is undefined failed , status: %{public}d.", status);
        return ERROR;
    }

    if (isUndefined) {
        return SUCCESS;
    }

    ResConfig *config = CreateDefaultResConfig();
    if (config == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Create resConfig failed.");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    resConfig.reset(config);

    if (!GetEnumParamOfConfig(env, resConfig, configuration)) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!GetLocaleOfConfig(env, resConfig, configuration)) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    return SUCCESS;
}

ani_object AniUtils::CreateConfig(ani_env* env, std::unique_ptr<ResConfig> &cfg)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::CONFIGURATION, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class Configuration failed, status: %{public}d.", status);
        return nullptr;
    }

    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method <ctor> in Configuration failed, status: %{public}d.", status);
        return nullptr;
    }

    ani_object obj;
    status = env->Object_New(cls, ctor, &obj);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Create Configuration failed, status: %{public}d.", status);
        return nullptr;
    }

    if (!SetEnumMember(env, obj, "direction", AniSignature::DIRECTION, static_cast<int>(cfg->GetDirection()))) {
        return nullptr;
    }
    if (!SetEnumMember(env, obj, "deviceType", AniSignature::DEVICE_TYPE, static_cast<int>(cfg->GetDeviceType()))) {
        return nullptr;
    }
    if (!SetEnumMember(env, obj, "colorMode", AniSignature::COLOR_MODE, static_cast<int>(cfg->GetColorMode()))) {
        return nullptr;
    }
    if (!SetEnumMember(env, obj, "screenDensity", AniSignature::SCREEN_DENSITY,
        GetScreenDensityIndex(cfg->GetScreenDensityDpi()))) {
        return nullptr;
    }
    
    status = env->Object_SetPropertyByName_Int(obj, "mcc", static_cast<int>(cfg->GetMcc()));
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set Configuration property 'mcc' failed, status: %{public}d.", status);
        return nullptr;
    }
    status = env->Object_SetPropertyByName_Int(obj, "mnc", static_cast<int>(cfg->GetMnc()));
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set Configuration property 'mnc' failed, status: %{public}d.", status);
        return nullptr;
    }

    std::string value = GetLocale(cfg);
    status = env->Object_SetPropertyByName_Ref(obj, "locale", CreateAniString(env, value));
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set Configuration property 'locale' failed, status: %{public}d.", status);
        return nullptr;
    }
    return obj;
}

bool AniUtils::GetEnumParamOfConfig(ani_env* env, std::shared_ptr<ResConfig> configPtr, ani_object configuration)
{
    int intEnum;
    if (!GetEnumMember(env, configuration, "direction", intEnum)) {
        return false;
    }
    configPtr->SetDirection(static_cast<Direction>(intEnum));

    if (!GetEnumMember(env, configuration, "deviceType", intEnum)) {
        return false;
    }
    configPtr->SetDeviceType(static_cast<DeviceType>(intEnum));

    if (!GetEnumMember(env, configuration, "screenDensity", intEnum)) {
        return false;
    }
    configPtr->SetScreenDensityDpi(static_cast<ScreenDensity>(intEnum));

    if (!GetEnumMember(env, configuration, "colorMode", intEnum)) {
        return false;
    }
    configPtr->SetColorMode(static_cast<ColorMode>(intEnum));

    int value;
    if (!GetNumberMember(env, configuration, "mcc", value)) {
        return false;
    }
    configPtr->SetMcc(value);
    if (!GetNumberMember(env, configuration, "mnc", value)) {
        return false;
    }
    configPtr->SetMnc(value);
    return true;
}

bool AniUtils::GetEnumMember(ani_env *env, ani_object options, const std::string name, int& member)
{
    ani_ref ref;
    ani_status status = env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get Configuration property '%{public}s' failed, status: %{public}d.",
            name.c_str(), status);
        return false;
    }
    status = env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(ref), &member);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get Configuration property '%{public}s' value failed, status: %{public}d.",
            name.c_str(), status);
        return false;
    }
    return true;
}

bool AniUtils::GetLocaleOfConfig(ani_env* env, std::shared_ptr<ResConfig> configPtr, ani_object configuration)
{
#ifdef SUPPORT_GRAPHICS
    ani_ref ret;
    ani_status status = env->Object_GetPropertyByName_Ref(configuration, "locale", &ret);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get Configuration property 'locale' failed, status: %{public}d.", status);
        return false;
    }
    if (configPtr->SetLocaleInfo(AniStrToString(env, static_cast<ani_string>(ret)).c_str()) != SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetLocaleOfConfig failed to SetLocaleInfo.");
        return false;
    }
#endif
    return true;
}

bool AniUtils::SetEnumMember(ani_env *env, ani_object obj, const char* memberName, const char* enumName,
    const int index)
{
    int realIndex = index;
    if (index < 0) {
        RESMGR_HILOGW(RESMGR_TAG, "Set Enum %{public}s invalid index %{public}d, reset index to zero.",
            enumName, index);
        realIndex = 0;
    }

    ani_enum aniEnum;
    ani_status status = env->FindEnum(enumName, &aniEnum);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find enum '%{public}s' failed, status: %{public}d.", enumName, status);
        return false;
    }

    ani_enum_item enumItem;
    status = env->Enum_GetEnumItemByIndex(aniEnum, realIndex, &enumItem);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get enumItem '%{public}s' failed, status: %{public}d.", enumName, status);
        return false;
    }
    status = env->Object_SetPropertyByName_Ref(obj, memberName, enumItem);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set Configuration property '%{public}s' failed, status: %{public}d.",
            memberName, status);
        return false;
    }
    return true;
}

std::string AniUtils::GetLocale(std::unique_ptr<ResConfig> &cfg)
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

ani_object AniUtils::CreateAniArray(ani_env *env, const std::vector<std::string> &strs)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::ANI_ARRAY, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class %{public}s failed, status: %{public}d.",
            AniSignature::ANI_ARRAY, status);
        return nullptr;
    }

    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>", "i:", &ctor);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method <ctor> in %{public}s failed, status: %{public}d",
            AniSignature::ANI_ARRAY, status);
        return nullptr;
    }

    ani_object ret;
    status = env->Object_New(cls, ctor, &ret, strs.size());
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object %{public}s failed, status: %{public}d",
            AniSignature::ANI_ARRAY, status);
        return nullptr;
    }

    ani_method set;
    status = env->Class_FindMethod(cls, "$_set", "iC{std.core.Object}:", &set);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method set in array failed, status %{public}d.", status);
        return nullptr;
    }

    for (size_t i = 0; i < strs.size(); i++) {
        status = env->Object_CallMethod_Void(ret, set, i, CreateAniString(env, strs[i]));
        if (ANI_OK != status) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Call method '$_set' failed, status :%{}d, currentIndex: %{}d.", status, i);
            return nullptr;
        }
    }
    return ret;
}

ani_object AniUtils::CreateAniUint8Array(ani_env* env, std::unique_ptr<uint8_t[]> &mediaData, size_t len)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::ANI_UINT8ARRAY, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class %{public}s failed, status: %{public}d",
            AniSignature::ANI_UINT8ARRAY, status);
        return nullptr;
    }

    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>", "i:", &ctor);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method <ctor> in %{public}s failed, status: %{public}d.",
            AniSignature::ANI_UINT8ARRAY, status);
        return nullptr;
    }

    ani_object ret;
    status = env->Object_New(cls, ctor, &ret, len);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object %{public}s failed, status: %{public}d",
            AniSignature::ANI_UINT8ARRAY, status);
        return nullptr;
    }

    ani_method set;
    status = env->Class_FindMethod(cls, "$_set", "ii:", &set);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method 'set' in %{public}s failed, status :%{public}d.",
            AniSignature::ANI_UINT8ARRAY, status);
        return nullptr;
    }

    for (size_t i = 0; i < len; i++) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, mediaData[i])) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Call method 'set' in %{public}s failed, status :%{public}d.",
                AniSignature::ANI_UINT8ARRAY, status);
            return nullptr;
        }
    }
    return ret;
}

int AniUtils::GetScreenDensityIndex(ScreenDensity value)
{
    switch (value) {
        case ScreenDensity::SCREEN_DENSITY_SDPI: return SCREEN_DENSITY_SDPI_ORDER;
        case ScreenDensity::SCREEN_DENSITY_MDPI: return SCREEN_DENSITY_MDPI_ORDER;
        case ScreenDensity::SCREEN_DENSITY_LDPI: return SCREEN_DENSITY_LDPI_ORDER;
        case ScreenDensity::SCREEN_DENSITY_XLDPI: return SCREEN_DENSITY_XLDPI_ORDER;
        case ScreenDensity::SCREEN_DENSITY_XXLDPI: return SCREEN_DENSITY_XXLDPI_ORDER;
        case ScreenDensity::SCREEN_DENSITY_XXXLDPI: return SCREEN_DENSITY_XXXLDPI_ORDER;
        default: return -1;
    }
}

bool AniUtils::GetNumberMember(ani_env *env, ani_object options, const std::string name, int& value)
{
    ani_status status = env->Object_GetPropertyByName_Int(options, name.c_str(), &value);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get property '%{public}s' failed, status: %{public}d.", name.c_str(), status);
        return false;
    }
    return true;
}

bool AniUtils::InitAniParameters(ani_env *env, ani_object args,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &params)
{
    ani_size len;
    ani_status status = env->Array_GetLength(static_cast<ani_array>(args), &len);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get args Array length failed, status: %{public}d.", status);
        return false;
    }

    if (len == 0) {
        return true;
    }
    ani_class stringClass;
    status = env->FindClass(AniSignature::ANI_STRING, &stringClass);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class %{public}s failed, status: %{public}d.",
            AniSignature::ANI_STRING, status);
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        ani_ref value;
        status = env->Array_Get(static_cast<ani_array>(args), i, &value);
        if (ANI_OK != status) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Call get args array value failed, index: %{public}d, status: %{public}d",
                i, status);
            return false;
        }

        ani_boolean isString;
        status = env->Object_InstanceOf(static_cast<ani_object>(value), stringClass, &isString);
        if (ANI_OK != status) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Check args value isString failed, index: %{public}d, status: %{public}d",
                i, status);
            return false;
        }

        if (isString) {
            params.emplace_back(ResourceManager::NapiValueType::NAPI_STRING,
                AniStrToString(env, static_cast<ani_string>(value)));
        } else {
            ani_double param;
            ani_status status =
                env->Object_CallMethodByName_Double(static_cast<ani_object>(value), "unboxed", ":d", &param);
            if (ANI_OK != status) {
                return false;
            }
            params.emplace_back(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(param));
        }
    }
    return true;
}

ani_object AniUtils::CreateDeviceCapability(ani_env *env, std::unique_ptr<ResConfig> &cfg)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::DEVICE_CAPABILITY, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class %{public}s failed, status: %{public}d.",
            AniSignature::DEVICE_CAPABILITY, status);
        return nullptr;
    }

    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method <ctor> in %{public}s failed, status: %{public}d.",
            AniSignature::DEVICE_CAPABILITY, status);
        return nullptr;
    }

    ani_object obj;
    status = env->Object_New(cls, ctor, &obj);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object %{public}s failed, status: %{public}d.",
            AniSignature::DEVICE_CAPABILITY, status);
        return nullptr;
    }
    int index = GetScreenDensityIndex(cfg->GetScreenDensityDpi());
    if (!SetEnumMember(env, obj, "screenDensity", AniSignature::SCREEN_DENSITY, index)) {
        return nullptr;
    };
    index = static_cast<int>(cfg->GetDeviceType());
    if (!SetEnumMember(env, obj, "deviceType", AniSignature::DEVICE_TYPE, index)) {
        return nullptr;
    };
    return obj;
}

bool AniUtils::GetOptionalInt(ani_env *env, ani_object optionalInt, int& result)
{
    ani_boolean isUndefined;
    ani_status status = env->Reference_IsUndefined(optionalInt, &isUndefined);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to judge int is undefined, status: %{public}d.", status);
        return false;
    }
    ani_int value = 0;
    if (!isUndefined) {
        status = env->Object_CallMethodByName_Int(optionalInt, "unboxed", ":i", &value);
        if (ANI_OK != status) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get int, status: %{public}d.", status);
            return false;
        }
    }
    result = value;
    return true;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS