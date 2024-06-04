/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohresmgr.h"

#include <securec.h>
#include "drawable_descriptor/drawable_descriptor.h"
#include "hilog_wrapper.h"
#include "resource_manager_impl.h"
#include "rstate.h"

using namespace OHOS::Global::Resource;
using namespace OHOS::Ace::Napi;

struct NativeResourceManager {
    std::shared_ptr<ResourceManager> resManager = nullptr;
};

ResourceManager_ErrorCode copyStringArray(char ***resultValue, uint32_t *resultLen,
    vector<string> tempResultValue, string apiName)
{
    size_t len = tempResultValue.size();
    *resultValue = new char* [tempResultValue.size()];
    if (*resultValue == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "%{public}s malloc error", apiName.c_str());
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    for (int i = 0; i < len; i++) {
        int strLen = tempResultValue[i].size();
        (*resultValue)[i] = new char[strLen + 1];
        if ((*resultValue)[i] == nullptr) {
            RESMGR_HILOGE(RESMGR_NATIVE_TAG, "%{public}s malloc error", apiName.c_str());
            OH_ResourceManager_ReleaseStringArray(resultValue, len);
            return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
        }
        if (strncpy_s((*resultValue)[i], strLen + 1, tempResultValue[i].c_str(), strLen) != 0) {
            RESMGR_HILOGE(RESMGR_NATIVE_TAG, "%{public}s strncpy_s error", apiName.c_str());
            OH_ResourceManager_ReleaseStringArray(resultValue, len);
            return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
        }
    }
    *resultLen = static_cast<uint32_t>(len);
    return ResourceManager_ErrorCode::SUCCESS;
}

ResourceManager_ErrorCode copyLocalInfo(const icu::Locale *localeInfo, ResourceManager_Configuration *configuration)
{
    std::string locale;
    if (localeInfo == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get configuration localeInfo errorCode = %{public}d",
            ResourceManager_ErrorCode::ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED);
        return ResourceManager_ErrorCode::ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED;
    }
    const char *lang = localeInfo->getLanguage();
    if (lang == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get configuration language errorCode = %{public}d",
            ResourceManager_ErrorCode::ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED);
        return ResourceManager_ErrorCode::ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED;
    }
    locale = lang;
    const char *script = localeInfo->getScript();
    if (script != nullptr) {
        locale += std::string("_") + script;
    }
    const char *region = localeInfo->getCountry();
    if (region != nullptr) {
        locale += std::string("_") + region;
    }
    if (configuration->locale != nullptr) {
        free(configuration->locale);
    }
    configuration->locale = (char*)malloc(locale.size() + 1);
    if (configuration->locale == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetConfiguration malloc error");
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    if (strncpy_s(configuration->locale, locale.size() + 1, locale.c_str(), locale.size()) != 0) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetConfiguration locale strncpy_s error");
        free(configuration->locale);
        configuration->locale = nullptr;
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    return ResourceManager_ErrorCode::SUCCESS;
}

ResourceManager_ErrorCode copyString(char **resultValue, string tempResultValue, string apiName)
{
    size_t len = tempResultValue.size();
    *resultValue = (char*)malloc(tempResultValue.size() + 1);
    if (*resultValue == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "%{public}s malloc error", apiName.c_str());
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    if (strncpy_s(*resultValue, len + 1, tempResultValue.c_str(), len) != 0) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "%{public}s strncpy_s error", apiName.c_str());
        free(*resultValue);
        *resultValue = nullptr;
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    return ResourceManager_ErrorCode::SUCCESS;
}

ResourceManager_ErrorCode OH_ResourceManager_GetMediaBase64(const NativeResourceManager *mgr, uint32_t resId,
    char **resultValue, uint64_t *resultLen, uint32_t density)
{
    if (mgr == nullptr || resultValue == nullptr || resultLen == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    std::string tempResultValue;
    RState state = mgr->resManager->GetMediaBase64DataById(resId, tempResultValue, density);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get media base64 id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    *resultValue = (char*)malloc(tempResultValue.size() + 1);
    if (*resultValue == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetMediaBase64 malloc error");
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    if (strncpy_s(*resultValue, tempResultValue.size() + 1, tempResultValue.c_str(), tempResultValue.size()) != 0) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetMediaBase64 strncpy_s error");
        free(*resultValue);
        *resultValue = nullptr;
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    *resultLen = static_cast<uint64_t>(tempResultValue.size());
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetMediaBase64ByName(const NativeResourceManager *mgr,
    const char *resName, char **resultValue, uint64_t *resultLen, uint32_t density)
{
    if (mgr == nullptr || resultValue == nullptr || resultLen == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    std::string tempResultValue;
    RState state = mgr->resManager->GetMediaBase64DataByName(resName, tempResultValue, density);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get media base64 name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    *resultValue = (char*)malloc(tempResultValue.size() + 1);
    if (*resultValue == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetMediaBase64Byname malloc error");
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    if (strncpy_s(*resultValue, tempResultValue.size() + 1, tempResultValue.c_str(), tempResultValue.size()) != 0) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetMediaBase64Byname strncpy_s error");
        free(*resultValue);
        *resultValue = nullptr;
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    *resultLen = static_cast<uint64_t>(tempResultValue.size());
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetMedia(const NativeResourceManager *mgr, uint32_t resId,
    uint8_t **resultValue, uint64_t *resultLen, uint32_t density)
{
    if (mgr == nullptr || resultValue == nullptr || resultLen == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    std::unique_ptr<uint8_t[]> tempResultValue;
    size_t len;
    RState state = mgr->resManager->GetMediaDataById(resId, len, tempResultValue, density);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get media resource id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    uint8_t *temPtr = tempResultValue.get();
    *resultValue = static_cast<uint8_t*>(malloc(len));
    if (*resultValue == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetMedia malloc error");
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    std::copy(temPtr, temPtr + len, *resultValue);
    *resultLen = static_cast<uint64_t>(len);
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetMediaByName(const NativeResourceManager *mgr, const char *resName,
    uint8_t **resultValue, uint64_t *resultLen, uint32_t density)
{
    if (mgr == nullptr || resultValue == nullptr || resultLen == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    std::unique_ptr<uint8_t[]> tempResultValue;
    size_t len;
    RState state = mgr->resManager->GetMediaDataByName(resName, len, tempResultValue, density);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get media resource name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    uint8_t *temPtr = tempResultValue.get();
    *resultValue = static_cast<uint8_t*>(malloc(len));
    if (*resultValue == nullptr) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "GetMediaByName malloc error");
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    std::copy(temPtr, temPtr + len, *resultValue);
    *resultLen = static_cast<uint64_t>(len);
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetDrawableDescriptor(const NativeResourceManager *mgr,
    uint32_t resId, ArkUI_DrawableDescriptor **drawableDescriptor, uint32_t density, uint32_t type)
{
    if (mgr == nullptr || drawableDescriptor == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    DrawableDescriptor::DrawableType drawableType;
    if (type == 1) {
        std::string themeMask = mgr->resManager->GetThemeMask();
        std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
        std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
        state = mgr->resManager->GetThemeIcons(resId, foregroundInfo, backgroundInfo, density);
        if (state == RState::SUCCESS) {
            auto descriptor = DrawableDescriptorFactory::Create(foregroundInfo, backgroundInfo,
            themeMask, drawableType, mgr->resManager);
            *drawableDescriptor = OH_ArkUI_CreateFromNapiDrawable(descriptor.get());
            return (*drawableDescriptor != nullptr) ? ResourceManager_ErrorCode::SUCCESS :
                ResourceManager_ErrorCode::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        }
    }
    auto descriptor = DrawableDescriptorFactory::Create(resId, mgr->resManager, state, drawableType, density);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "Failed to Create drawableDescriptor");
        return static_cast<ResourceManager_ErrorCode>(state);
    }
    *drawableDescriptor = OH_ArkUI_CreateFromNapiDrawable(descriptor.get());
    return (*drawableDescriptor != nullptr) ? ResourceManager_ErrorCode::SUCCESS :
        ResourceManager_ErrorCode::ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

ResourceManager_ErrorCode OH_ResourceManager_GetDrawableDescriptorByName(const NativeResourceManager *mgr,
    const char *resName, ArkUI_DrawableDescriptor **drawableDescriptor, uint32_t density, uint32_t type)
{
    if (mgr == nullptr || drawableDescriptor == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    DrawableDescriptor::DrawableType drawableType;
    if (type == 1) {
        std::string themeMask = mgr->resManager->GetThemeMask();
        std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
        std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
        state = mgr->resManager->GetThemeIcons(0, foregroundInfo, backgroundInfo, density);
        if (state == RState::SUCCESS) {
            auto descriptor = DrawableDescriptorFactory::Create(foregroundInfo, backgroundInfo,
            themeMask, drawableType, mgr->resManager);
            *drawableDescriptor = OH_ArkUI_CreateFromNapiDrawable(descriptor.get());
            return (*drawableDescriptor != nullptr) ? ResourceManager_ErrorCode::SUCCESS :
                ResourceManager_ErrorCode::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        }
    }
    if (type == 2) { // 2 means get the dynamic icon from theme
        std::pair<std::unique_ptr<uint8_t[]>, size_t> iconInfo;
        state = mgr->resManager->GetDynamicIcon(resName, iconInfo, density);
        if (state == RState::SUCCESS) {
            auto descriptor = std::make_unique<DrawableDescriptor>(std::move(iconInfo.first), iconInfo.second);
            *drawableDescriptor = OH_ArkUI_CreateFromNapiDrawable(descriptor.get());
            return (*drawableDescriptor != nullptr) ? ResourceManager_ErrorCode::SUCCESS :
                ResourceManager_ErrorCode::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        }
    }

    auto descriptor = DrawableDescriptorFactory::Create(resName, mgr->resManager, state, drawableType, density);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "Failed to Create drawableDescriptor");
        return static_cast<ResourceManager_ErrorCode>(state);
    }
    *drawableDescriptor = OH_ArkUI_CreateFromNapiDrawable(descriptor.get());
    return (*drawableDescriptor != nullptr) ? ResourceManager_ErrorCode::SUCCESS :
        ResourceManager_ErrorCode::ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

ResourceManager_ErrorCode OH_ResourceManager_GetSymbol(const NativeResourceManager *mgr, uint32_t resId,
    uint32_t *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    uint32_t tempResultValue;
    state = mgr->resManager->GetSymbolById(resId, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get symbol resource id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetSymbolByName(const NativeResourceManager *mgr, const char *resName,
    uint32_t *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    uint32_t tempResultValue;
    state = mgr->resManager->GetSymbolByName(resName, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get symbol resource name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetLocales(const NativeResourceManager *mgr, char ***resultValue,
    uint32_t *resultLen, bool includeSystem)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    std::vector<std::string> tempResultValue;
    mgr->resManager->GetLocales(tempResultValue, includeSystem);
    return copyStringArray(resultValue, resultLen, tempResultValue, "GetLocales");
}

ResourceManager_ErrorCode OH_ResourceManager_GetConfiguration(const NativeResourceManager *mgr,
    ResourceManager_Configuration *configuration)
{
    if (mgr == nullptr || configuration == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    ResConfigImpl resConfig;
    mgr->resManager->GetResConfig(resConfig);
    
    const icu::Locale *localeInfo = resConfig.GetLocaleInfo();
    ResourceManager_ErrorCode errCode = copyLocalInfo(localeInfo, configuration);
    if (errCode != ResourceManager_ErrorCode::SUCCESS) {
        return errCode;
    }
    {
        configuration->direction = static_cast<::ResourceManager_Direction>(resConfig.GetDirection());
        configuration->deviceType = static_cast<::ResourceManager_DeviceType>(resConfig.GetDeviceType());
        configuration->screenDensity = static_cast<::ScreenDensity>(resConfig.GetScreenDensity());
        configuration->colorMode = static_cast<::ResourceManager_ColorMode>(resConfig.GetColorMode());
        configuration->mcc = resConfig.GetMcc();
        configuration->mnc = resConfig.GetMnc();
    }
    return ResourceManager_ErrorCode::SUCCESS;
}

ResourceManager_ErrorCode OH_ResourceManager_GetStringArray(const NativeResourceManager *mgr,
    uint32_t resId, char ***resultValue, uint32_t *resultValueLen)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    std::vector<std::string> tempResultValue;
    state = mgr->resManager->GetStringArrayById(resId, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get string array id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    return copyStringArray(resultValue, resultValueLen, tempResultValue, "GetStringArray");
}

ResourceManager_ErrorCode OH_ResourceManager_GetStringArrayByName(const NativeResourceManager *mgr,
    const char *resName, char ***resultValue, uint32_t *resultLen)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    std::vector<std::string> tempResultValue;
    state = mgr->resManager->GetStringArrayByName(resName, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get string array resName = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    return copyStringArray(resultValue, resultLen, tempResultValue, "GetStringArrayByName");
}

ResourceManager_ErrorCode OH_ResourceManager_GetPluralStringByName(const NativeResourceManager *mgr,
    const char *resName, uint32_t num, char **resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    std::string tempResultValue;
    state = mgr->resManager->GetPluralStringByNameFormat(tempResultValue, resName, num, num);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get plural string name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    return copyString(resultValue, tempResultValue, "GetPluralStringByName");
}

ResourceManager_ErrorCode OH_ResourceManager_GetPluralString(const NativeResourceManager *mgr,
    uint32_t resId, uint32_t num, char **resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    std::string tempResultValue;
    state = mgr->resManager->GetPluralStringByIdFormat(tempResultValue, resId, num, num);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get plural string id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    return copyString(resultValue, tempResultValue, "GetPluralString");
}

ResourceManager_ErrorCode OH_ResourceManager_GetColor(const NativeResourceManager *mgr, uint32_t resId,
    uint32_t *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    uint32_t tempResultValue;
    state = mgr->resManager->GetColorById(resId, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "failed get color id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetColorByName(const NativeResourceManager *mgr, const char *resName,
    uint32_t *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    uint32_t tempResultValue;
    state = mgr->resManager->GetColorByName(resName, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get color name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetInt(const NativeResourceManager *mgr, uint32_t resId,
    int *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    int tempResultValue;
    state = mgr->resManager->GetIntegerById(resId, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get integer id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetIntByName(const NativeResourceManager *mgr, const char *resName,
    int *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    int tempResultValue;
    state = mgr->resManager->GetIntegerByName(resName, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get integer name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetFloat(const NativeResourceManager *mgr, uint32_t resId,
    float *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    float tempResultValue;
    state = mgr->resManager->GetFloatById(resId, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "failed get float id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetFloatByName(const NativeResourceManager *mgr, const char *resName,
    float *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    float tempResultValue;
    state = mgr->resManager->GetFloatByName(resName, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get float name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetBool(const NativeResourceManager *mgr, uint32_t resId,
    bool *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    bool tempResultValue;
    state = mgr->resManager->GetBooleanById(resId, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "failed get bool id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_GetBoolByName(const NativeResourceManager *mgr, const char *resName,
    bool *resultValue)
{
    if (mgr == nullptr || resultValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    RState state = RState::SUCCESS;
    bool tempResultValue;
    state = mgr->resManager->GetBooleanByName(resName, tempResultValue);
    ResourceManager_ErrorCode errorCode = static_cast<ResourceManager_ErrorCode>(state);
    if (errorCode != ResourceManager_ErrorCode::SUCCESS) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG,
            "failed get bool name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    *resultValue = tempResultValue;
    return errorCode;
}

ResourceManager_ErrorCode OH_ResourceManager_AddResource(const NativeResourceManager *mgr, const char *path)
{
    if (mgr == nullptr || path == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!mgr->resManager->AddAppOverlay(path)) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "Failed to add overlay path = %{public}s", path);
        return ResourceManager_ErrorCode::ERROR_CODE_OVERLAY_RES_PATH_INVALID;
    }
    return ResourceManager_ErrorCode::SUCCESS;
}

ResourceManager_ErrorCode OH_ResourceManager_RemoveResource(const NativeResourceManager *mgr, const char *path)
{
    if (mgr == nullptr || path == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!mgr->resManager->RemoveAppOverlay(path)) {
        RESMGR_HILOGE(RESMGR_NATIVE_TAG, "Failed to remove overlay path = %{public}s", path);
        return ResourceManager_ErrorCode::ERROR_CODE_OVERLAY_RES_PATH_INVALID;
    }
    return ResourceManager_ErrorCode::SUCCESS;
}

ResourceManager_ErrorCode OH_ResourceManager_ReleaseConfiguration(ResourceManager_Configuration *configuration)
{
    if (configuration == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (configuration->locale != nullptr) {
        free(configuration->locale);
    }
    return ResourceManager_ErrorCode::SUCCESS;
}

ResourceManager_ErrorCode OH_ResourceManager_ReleaseStringArray(char ***resValue, uint32_t len)
{
    if (resValue == nullptr || *resValue == nullptr) {
        return ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    for (int i = 0; i < len; i++) {
        if ((*resValue)[i] != nullptr) {
            delete [] (*resValue)[i];
            (*resValue)[i] = nullptr;
        }
    }
    delete [] *resValue;
    *resValue = nullptr;
    return ResourceManager_ErrorCode::SUCCESS;
}