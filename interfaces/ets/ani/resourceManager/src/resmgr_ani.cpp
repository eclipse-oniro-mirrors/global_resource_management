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

#include "resmgr_ani.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <array>

#include "application_context.h"
#include "application_context.h"
#include "drawable_descriptor_ani.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "ani_signature_builder.h"

#include "ani_utils.h"
#include "ani_signature.h"
#include "hilog_wrapper.h"
#include "resourceManager.h"
#include "utils/utils.h"
#include "resource_manager_impl.h"
#include "resource_manager_addon.h"
#include "system_resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {

constexpr ani_int ABNORMAL_NUMBER_RETURN_VALUE = -1;

constexpr int NUM_NAPI_VALUES_TO_WRAP = 1;

constexpr int LAYER_ICON = 1;
constexpr int DYNAMIC_ICON = 2;

static std::array methods = {
    ani_native_function { "getStringSync", "l:C{std.core.String}",
        reinterpret_cast<void*>(ResMgrAni::GetStringSyncById) },
    ani_native_function { "getStringSync", "lC{escompat.Array}:C{std.core.String}",
        reinterpret_cast<void *>(ResMgrAni::GetFormatStringSyncById) },
    ani_native_function { "getStringByNameSync", "C{std.core.String}:C{std.core.String}",
        reinterpret_cast<void*>(ResMgrAni::GetStringByNameSync) },
    ani_native_function { "getStringByNameSync", "C{std.core.String}C{escompat.Array}:C{std.core.String}",
        reinterpret_cast<void*>(ResMgrAni::GetFormatStringByNameSync) },

    ani_native_function { "getBoolean", nullptr, reinterpret_cast<void*>(ResMgrAni::GetBooleanById) },
    ani_native_function { "getBooleanByName", nullptr, reinterpret_cast<void*>(ResMgrAni::GetBooleanByName) },

    ani_native_function { "getInt", nullptr, reinterpret_cast<void*>(ResMgrAni::GetIntById) },
    ani_native_function { "getIntByName", nullptr, reinterpret_cast<void*>(ResMgrAni::GetIntByName) },
    ani_native_function { "getDouble", nullptr, reinterpret_cast<void*>(ResMgrAni::GetDoubleById) },
    ani_native_function { "getDoubleByName", nullptr, reinterpret_cast<void*>(ResMgrAni::GetDoubleByName) },
    
    ani_native_function { "getIntPluralStringValueSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetIntPluralStringValueSyncById) },
    ani_native_function { "getIntPluralStringByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetIntPluralStringByNameSync) },

    ani_native_function { "getDoublePluralStringValueSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetDoublePluralStringValueSyncById) },
    ani_native_function { "getDoublePluralStringByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetDoublePluralStringByNameSync) },

    ani_native_function { "getColorSync", nullptr, reinterpret_cast<void*>(ResMgrAni::GetColorSyncById) },
    ani_native_function { "getColorByNameSync", nullptr, reinterpret_cast<void*>(ResMgrAni::GetColorByNameSync) },
    
    ani_native_function { "addResource", nullptr, reinterpret_cast<void*>(ResMgrAni::AddResource) },
    ani_native_function { "removeResource", nullptr, reinterpret_cast<void*>(ResMgrAni::RemoveResource) },

    ani_native_function { "getRawFdSync", nullptr, reinterpret_cast<void*>(ResMgrAni::GetRawFdSync) },
    ani_native_function { "closeRawFdSync", nullptr, reinterpret_cast<void*>(ResMgrAni::CloseRawFdSync) },
    ani_native_function { "isRawDir", nullptr, reinterpret_cast<void*>(ResMgrAni::IsRawDir) },

    ani_native_function { "getRawFileListSync", nullptr, reinterpret_cast<void*>(ResMgrAni::GetRawFileListSync) },
    ani_native_function { "getRawFileContentSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetRawFileContentSync) },
    
    ani_native_function { "getMediaContentSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetMediaContentSyncById) },

    ani_native_function { "getMediaContentBase64Sync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetMediaContentBase64SyncById) },
    
    ani_native_function { "getStringArrayValueSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetStringArrayValueSyncById) },

    ani_native_function { "getStringArrayByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetStringArrayByNameSync) },
    
    ani_native_function { "getMediaByNameSync", nullptr, reinterpret_cast<void*>(ResMgrAni::GetMediaByNameSync) },
    ani_native_function { "getMediaBase64ByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetMediaBase64ByNameSync) },
    
    ani_native_function { "getDrawableDescriptor", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetDrawableDescriptorById) },
    ani_native_function { "getDrawableDescriptorByName", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetDrawableDescriptorByName) },
        
    ani_native_function { "getConfigurationSync", nullptr, reinterpret_cast<void*>(ResMgrAni::GetConfigurationSync) },
    ani_native_function { "getDeviceCapabilitySync", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetDeviceCapabilitySync) },
    ani_native_function { "getLocales", nullptr, reinterpret_cast<void*>(ResMgrAni::GetLocales) },

    ani_native_function { "getSymbol", nullptr, reinterpret_cast<void*>(ResMgrAni::GetSymbolById) },
    ani_native_function { "getSymbolByName", nullptr, reinterpret_cast<void*>(ResMgrAni::GetSymbolByName) },

    ani_native_function { "getOverrideResourceManager", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetOverrideResourceManager) },
    ani_native_function { "getOverrideConfiguration", nullptr,
        reinterpret_cast<void*>(ResMgrAni::GetOverrideConfiguration) },
    ani_native_function { "updateOverrideConfiguration", nullptr,
        reinterpret_cast<void*>(ResMgrAni::UpdateOverrideConfiguration) },
};

ani_string ResMgrAni::GetStringSyncById(ani_env* env, ani_object object, ani_long resId)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetStringById.");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    std::string result;
    RState state = resMgr->GetStringById(resId, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetStringById failed, state: %{public}d, id: %{public}lld", state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_string ResMgrAni::GetFormatStringSyncById(ani_env *env, ani_object object, ani_long resId, ani_object args)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetFormatStringSyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (!AniUtils::InitAniParameters(env, args, params)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetFormatStringSyncById get params failed");
        AniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }
    std::string result;
    RState state = resMgr->GetStringFormatById(resId, result, params);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetFormatStringSyncById failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_string ResMgrAni::GetStringByNameSync(ani_env* env, ani_object object, ani_string aniName)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }
    std::string name = AniUtils::AniStrToString(env, aniName);
    std::string result;
    RState state = resMgr->GetStringByName(name.c_str(), result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetStringByNameSync failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_string ResMgrAni::GetFormatStringByNameSync(ani_env *env, ani_object object, ani_string aniName, ani_object args)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (!AniUtils::InitAniParameters(env, args, params)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "getFormatStringByNameSync get params failed");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NAME_FORMAT_ERROR);
        return nullptr;
    }
    std::string name = AniUtils::AniStrToString(env, aniName);
    std::string result;
    RState state = resMgr->GetStringFormatByName(name.c_str(), result, params);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetStringFormatByName failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_boolean ResMgrAni::GetBooleanById(ani_env* env, ani_object object, ani_long resId)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getBooleanById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return false;
    }
    bool result;
    RState state = resMgr->GetBooleanById(resId, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetBooleanById failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return false;
    }
    return result;
}

ani_boolean ResMgrAni::GetBooleanByName(ani_env* env, ani_object object, ani_string aniName)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getBooleanByName");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return false;
    }
    std::string name = AniUtils::AniStrToString(env, aniName);
    bool result;
    RState state = resMgr->GetBooleanByName(name.c_str(), result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetBooleanByName failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return false;
    }
    return result;
}

ani_int ResMgrAni::GetIntById(ani_env* env, ani_object object, ani_long resId)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetIntById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    int result;
    RState state = resMgr->GetIntegerById(resId, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetIntById failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

ani_int ResMgrAni::GetIntByName(ani_env* env, ani_object object, ani_string aniName)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetIntByName");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    std::string name = AniUtils::AniStrToString(env, aniName);
    int result;
    RState state = resMgr->GetIntegerByName(name.c_str(), result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetIntByName failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

ani_double ResMgrAni::GetDoubleById(ani_env* env, ani_object object, ani_long resId)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetDoubleById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    float result;
    RState state = resMgr->GetFloatById(resId, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetDoubleById failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

ani_double ResMgrAni::GetDoubleByName(ani_env* env, ani_object object, ani_string aniName)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetDoubleByName");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    std::string name = AniUtils::AniStrToString(env, aniName);
    float result;
    RState state = resMgr->GetFloatByName(name.c_str(), result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetDoubleByName failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

ani_string ResMgrAni::GetIntPluralStringValueSyncById(ani_env* env, ani_object object,
    ani_long resId, ani_int num, ani_object args)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getIntPluralStringValueSyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (!AniUtils::InitAniParameters(env, args, params)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        AniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }
    std::string result;
    RState state = resMgr->GetFormatPluralStringById(result, resId, { true, num, 0.0 }, params);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetFormatPluralStringById failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_string ResMgrAni::GetIntPluralStringByNameSync(ani_env* env, ani_object object,
    ani_string aniName, ani_int num, ani_object args)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getIntPluralStringByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }

    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (!AniUtils::InitAniParameters(env, args, params)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        AniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }

    std::string name = AniUtils::AniStrToString(env, aniName);
    std::string result;
    RState state = resMgr->GetFormatPluralStringByName(result, name.c_str(), { true, num, 0.0 }, params);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetFormatPluralStringByName failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_string ResMgrAni::GetDoublePluralStringValueSyncById(ani_env* env, ani_object object,
    ani_long resId, ani_double num, ani_object args)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getIntPluralStringValueSyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (!AniUtils::InitAniParameters(env, args, params)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        AniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }
    std::string result;
    RState state = resMgr->GetFormatPluralStringById(result, resId, { false, 0, num }, params);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetFormatPluralStringById failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_string ResMgrAni::GetDoublePluralStringByNameSync(ani_env* env, ani_object object,
    ani_string aniName, ani_double num, ani_object args)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getDoublePluralStringByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }
    
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (!AniUtils::InitAniParameters(env, args, params)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        AniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }
    
    std::string resName = AniUtils::AniStrToString(env, aniName);
    std::string result;
    RState state = resMgr->GetFormatPluralStringByName(result, resName.c_str(), { false, 0, num }, params);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetStringByNameSync failed, state: %{public}d, name: %{public}s",
            state, resName.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_long ResMgrAni::GetColorSyncById(ani_env* env, ani_object object, ani_long resId)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getColorSyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    uint32_t result;
    RState state = resMgr->GetColorById(resId, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetColorSyncById failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

ani_long ResMgrAni::GetColorByNameSync(ani_env* env, ani_object object, ani_string aniName)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getColorByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    std::string name = AniUtils::AniStrToString(env, aniName);
    uint32_t result;
    RState state = resMgr->GetColorByName(name.c_str(), result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "getColorByNameSync failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

void ResMgrAni::AddResource(ani_env* env, ani_object object, ani_string aniPath)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in AddResource.");
        return;
    }
    
    std::string path = AniUtils::AniStrToString(env, aniPath);
    if (!resMgr->AddAppOverlay(path)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to add overlay path.");
        AniUtils::AniThrow(env, ERROR_CODE_OVERLAY_RES_PATH_INVALID);
        return;
    }
}

void ResMgrAni::RemoveResource(ani_env* env, ani_object object, ani_string aniPath)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "Failed to get resMgr in RemoveResource.");
        return;
    }
    std::string path = AniUtils::AniStrToString(env, aniPath);
    if (!resMgr->RemoveAppOverlay(path)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to remove overlay path.");
        AniUtils::AniThrow(env, ERROR_CODE_OVERLAY_RES_PATH_INVALID);
        return;
    }
}

ani_object ResMgrAni::GetRawFdSync(ani_env* env, ani_object object, ani_string aniPath)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getRawFdSync");
        AniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }
    std::string path = AniUtils::AniStrToString(env, aniPath);
    ResourceManager::RawFileDescriptor descriptor;
    RState state = resMgr->GetRawFileDescriptorFromHap(path, descriptor);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to GetRawFileDescriptorFromHap.");
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniRawFileDescriptor(env, descriptor);
}

void ResMgrAni::CloseRawFdSync(ani_env* env, ani_object object, ani_string aniPath)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);

    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in closeRawFdSync");
        AniUtils::AniThrow(env, NOT_FOUND);
        return;
    }

    std::string path = AniUtils::AniStrToString(env, aniPath);
    RState state = resMgr->CloseRawFileDescriptor(path);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to close rawfd.");
        AniUtils::AniThrow(env, state);
        return;
    }
}

ani_object ResMgrAni::GetRawFileListSync(ani_env* env, ani_object object, ani_string aniPath)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getRawFileListSync");
        AniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }
    std::string path = AniUtils::AniStrToString(env, aniPath);
    std::vector<std::string> result;
    RState state = resMgr->GetRawFileList(path.c_str(), result);
    if (state != RState::SUCCESS || result.empty()) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get rawfile list.");
        AniUtils::AniThrow(env, ERROR_CODE_RES_PATH_INVALID);
        return nullptr;
    }
    
    return AniUtils::CreateAniArray(env, result);
}

ani_object ResMgrAni::GetRawFileContentSync(ani_env* env, ani_object object, ani_string aniPath)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getRawFileContentSync");
        AniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }
    std::string path = AniUtils::AniStrToString(env, aniPath);
    size_t len = 0;
    std::unique_ptr<uint8_t[]> mediaData;
    RState state = resMgr->GetRawFileFromHap(path, len, mediaData);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get rawfile.");
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniUint8Array(env, mediaData, len);
}

ani_object ResMgrAni::GetMediaContentSyncById(ani_env* env, ani_object object,
    ani_long resId, ani_object aniDensity)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getMediaContentSyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    int density = 0;
    if (!AniUtils::GetOptionalInt(env, aniDensity, density)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get density in getMediaContentSyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    size_t len = 0;
    std::unique_ptr<uint8_t[]> mediaData;
    RState state = resMgr->GetMediaDataById(resId, len, mediaData, density);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetMediaContentSync failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniUint8Array(env, mediaData, len);
}

ani_string ResMgrAni::GetMediaContentBase64SyncById(ani_env* env, ani_object object,
    ani_long resId, ani_object aniDensity)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getMediaContentBase64SyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    int density = 0;
    if (!AniUtils::GetOptionalInt(env, aniDensity, density)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get density in GetMediaContentBase64Sync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    
    std::string result;
    RState state = resMgr->GetMediaBase64DataById(resId, result, density);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetMediaContentBase64Sync failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

ani_object ResMgrAni::GetStringArrayValueSyncById(ani_env* env, ani_object object, ani_long resId)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetStringArrayValueSyncById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    std::vector<std::string> result;
    RState state = resMgr->GetStringArrayById(resId, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetStringArrayValueSync failed, state: %{public}d, id: %{public}lld",
            state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniArray(env, result);
}

ani_object ResMgrAni::GetMediaByNameSync(ani_env* env, ani_object object, ani_string aniName, ani_object aniDensity)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetMediaByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }
    int density = 0;
    if (!AniUtils::GetOptionalInt(env, aniDensity, density)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get density in GetMediaByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }

    std::string name = AniUtils::AniStrToString(env, aniName);
    size_t len = 0;
    std::unique_ptr<uint8_t[]> mediaData;
    RState state = resMgr->GetMediaDataByName(name.c_str(), len, mediaData, density);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetMediaByNameSync failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniUint8Array(env, mediaData, len);
}

ani_string ResMgrAni::GetMediaBase64ByNameSync(ani_env* env, ani_object object,
    ani_string aniName, ani_object aniDensity)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetMediaBase64ByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }

    int density = 0;
    if (!AniUtils::GetOptionalInt(env, aniDensity, density)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get density in GetMediaBase64ByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }

    std::string name = AniUtils::AniStrToString(env, aniName);
    std::string result;
    RState state = resMgr->GetMediaBase64DataByName(name.c_str(), result, density);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetMediaBase64ByNameSync failed, state: %{public}d, name: %{public}s",
            state, name.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniString(env, result);
}

RState CreateThemeIcon(ani_env *env, Ace::Ani::DrawableInfo &drawableInfo, ani_object *drawable)
{
    std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
    RState state = drawableInfo.manager->GetThemeIcons(0, foregroundInfo, backgroundInfo, drawableInfo.density);
    if (state == SUCCESS) {
        drawableInfo.firstBuffer.data = std::move(foregroundInfo.first);
        drawableInfo.firstBuffer.len = foregroundInfo.second;
        if (Utils::SupportSingleLayerThemeIcon() && backgroundInfo.first == nullptr) {
            *drawable = Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
            return SUCCESS;
        }
        drawableInfo.secondBuffer.data = std::move(backgroundInfo.first);
        drawableInfo.secondBuffer.len = backgroundInfo.second;
        drawableInfo.type = "layered";
        *drawable = Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
    }
    return state;
}

ani_object CreateDrawableDescriptorbyId(ani_env* env, std::shared_ptr<ResourceManager> resMgr,
    uint32_t resId, uint32_t density, uint32_t iconType)
{
    Ace::Ani::DrawableInfo drawableInfo;
    drawableInfo.density = density;
    drawableInfo.manager = resMgr;
    RState state = SUCCESS;
    if (iconType == LAYER_ICON) {
        ani_object result;
        if (SUCCESS == CreateThemeIcon(env, drawableInfo, &result)) {
            return result;
        }
    }
    state = resMgr->GetDrawableInfoById(resId, drawableInfo.type,
        drawableInfo.firstBuffer.len, drawableInfo.firstBuffer.data, density);
    if (SUCCESS != state) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "drawableDescriptor failed, state: %{public}d, id: %{public}u", state, resId);
        AniUtils::AniThrow(env, state);
        return nullptr;
    };
    return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
}

ani_object ResMgrAni::GetDrawableDescriptorById(ani_env* env, ani_object object,
    ani_long resId, ani_object aniDensity, ani_object aniType)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetDrawableDescriptorById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    int density = 0;
    if (!AniUtils::GetOptionalInt(env, aniDensity, density)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get density in GetDrawableDescriptorById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    int type = 0;
    if (!AniUtils::GetOptionalInt(env, aniType, type)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get type in GetDrawableDescriptorById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    return CreateDrawableDescriptorbyId(env, resMgr, resId, density, type);
}

ani_object CreateDrawableDescriptorbyName(ani_env* env, std::shared_ptr<ResourceManager> resMgr,
    const std::string& resName, uint32_t density, uint32_t iconType)
{
    Ace::Ani::DrawableInfo drawableInfo;
    drawableInfo.density = density;
    drawableInfo.manager = resMgr;
    RState state = SUCCESS;
    if (iconType == LAYER_ICON) {
        ani_object result;
        if (SUCCESS == CreateThemeIcon(env, drawableInfo, &result)) {
            return result;
        }
    }

    if (iconType == DYNAMIC_ICON) {
        std::pair<std::unique_ptr<uint8_t[]>, size_t> iconInfo;
        if (resMgr->GetDynamicIcon(resName, iconInfo, density) == SUCCESS) {
            drawableInfo.firstBuffer.data = std::move(iconInfo.first);
            drawableInfo.firstBuffer.len = iconInfo.second;
            return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
        }
    }

    state = resMgr->GetDrawableInfoByName(resName.c_str(), drawableInfo.type,
        drawableInfo.firstBuffer.len, drawableInfo.firstBuffer.data, density);
    if (SUCCESS != state) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetDrawableInfoByName failed, state: %{public}d, name: %{public}s",
            state, resName.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    };
    return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
}

ani_object ResMgrAni::GetDrawableDescriptorByName(ani_env* env, ani_object object,
    ani_string aniName, ani_object aniDensity, ani_object aniType)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetDrawableDescriptorByName");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }
    int density = 0;
    if (!AniUtils::GetOptionalInt(env, aniDensity, density)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get density in GetDrawableDescriptorByName");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }
    int type = 0;
    if (!AniUtils::GetOptionalInt(env, aniType, type)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get type in GetDrawableDescriptorByName");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }

    std::string resName = AniUtils::AniStrToString(env, aniName);
    return CreateDrawableDescriptorbyName(env, resMgr, resName, density, type);
}

ani_object ResMgrAni::GetStringArrayByNameSync(ani_env* env, ani_object object, ani_string aniName)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringArrayByNameSync");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return nullptr;
    }
    std::string resName = AniUtils::AniStrToString(env, aniName);
    std::vector<std::string> result;
    RState state = resMgr->GetStringArrayByName(resName.c_str(), result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetStringArrayByName failed, state: %{public}d, name: %{public}s",
            state, resName.c_str());
        AniUtils::AniThrow(env, state);
        return nullptr;
    }
    return AniUtils::CreateAniArray(env, result);
}


ani_long ResMgrAni::GetSymbolById(ani_env* env, ani_object object, ani_long resId)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getSymbolById");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    uint32_t result;
    RState state = resMgr->GetSymbolById(resId, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetSymbolById failed, state: %{public}d, id: %{public}lld", state, resId);
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

ani_long ResMgrAni::GetSymbolByName(ani_env* env, ani_object object, ani_string aniName)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getSymbolByName");
        AniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    std::string resName = AniUtils::AniStrToString(env, aniName);
    uint32_t result;
    RState state = resMgr->GetSymbolByName(resName.c_str(), result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetSymbolByName failed, state: %{public}d, name: %{public}s",
            state, resName.c_str());
        AniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return result;
}

ani_object ResMgrAni::GetConfigurationSync(ani_env* env, ani_object object)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in GetConfigurationSync");
        return nullptr;
    }

    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (cfg == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to create ResConfig object.");
        return nullptr;
    }
    resMgr->GetResConfig(*cfg);
    return AniUtils::CreateConfig(env, cfg);
}

ani_object ResMgrAni::GetOverrideResourceManager(ani_env* env, ani_object object, ani_object configuration)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetOverrideResourceManager");
        AniUtils::AniThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    std::shared_ptr<ResConfig> overrideResConfig;
    int32_t state = AniUtils::ConfigurationToResConfig(env, configuration, overrideResConfig);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resconfig in GetOverrideResourceManager");
        AniUtils::AniThrow(env, state);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> overrideResMgr = resMgr->GetOverrideResourceManager(overrideResConfig);
    if (overrideResMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get overrideResMgr in GetOverrideResourceManager");
        AniUtils::AniThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    return ResMgrAddon::WrapResourceManager(env, overrideResMgr);
}


ani_object ResMgrAni::GetOverrideConfiguration(ani_env* env, ani_object object)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetOverrideConfiguration");
        return nullptr;
    }

    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to create ResConfig object.");
        return nullptr;
    }
    resMgr->GetOverrideResConfig(*cfg);
    return AniUtils::CreateConfig(env, cfg);
}

void ResMgrAni::UpdateOverrideConfiguration(ani_env* env, ani_object object, ani_object configuration)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in updateOverrideConfiguration");
        AniUtils::AniThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return;
    }
    std::shared_ptr<ResConfig> overrideResConfig;
    RState state = AniUtils::ConfigurationToResConfig(env, configuration, overrideResConfig);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get config in GetOverrideResourceManager");
        AniUtils::AniThrow(env, state);
        return;
    }

    state = resMgr->UpdateOverrideResConfig(*overrideResConfig);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to update override configuration failed, status: %{public}d.", state);
        AniUtils::AniThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return;
    }
}

ani_object ResMgrAni::GetDeviceCapabilitySync(ani_env* env, ani_object object)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetConfigurationSync");
        return nullptr;
    }

    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to create ResConfig object.");
        return nullptr;
    }
    resMgr->GetResConfig(*cfg);
    return AniUtils::CreateDeviceCapability(env, cfg);
}

ani_object ResMgrAni::GetLocales(ani_env* env, ani_object object, ani_object includeSystem)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetConfigurationSync");
        return nullptr;
    }
    
    ani_boolean isUndefined;
    ani_status status = env->Reference_IsUndefined(includeSystem, &isUndefined);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to judge includeSystem is undefined, status: %{public}d.", status);
        return nullptr;
    }
    ani_boolean includeSys = false;
    if (!isUndefined) {
        status = env->Object_CallMethodByName_Boolean(includeSystem, "unboxed", ":z", &includeSys);
        if (ANI_OK != status) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get includeSystem, status: %{public}d.", status);
            return nullptr;
        }
    }
    std::vector<std::string> result;
    resMgr->GetLocales(result, includeSys);
    return AniUtils::CreateAniArray(env, result);
}

ani_boolean ResMgrAni::IsRawDir(ani_env* env, ani_object object, ani_string aniPath)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in isRawDir");
        AniUtils::AniThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return false;
    }
    std::string path = AniUtils::AniStrToString(env, aniPath);
    bool result = false;
    RState state = resMgr->IsRawDirFromHap(path, result);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to determine the raw file is directory.");
        AniUtils::AniThrow(env, state);
        return false;
    }
    return result;
}

ani_object ResMgrAni::GetSysResourceManager(ani_env* env)
{
    std::shared_ptr<Global::Resource::ResourceManager> sysResMgr(SystemResourceManager::CreateSysResourceManager());
    return ResMgrAddon::WrapResourceManager(env, sysResMgr);
}

void ResMgrAni::DestoryResMgr(ani_env* env, ani_object jsResMgr)
{
    ani_long nativeResMgrPtr;
    ani_status status = env->Object_GetFieldByName_Long(jsResMgr, "nativeResMgr", &nativeResMgrPtr);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get nativeResMgr in DestoryResMgr, status: %{public}d.", status);
        return;
    }
    std::shared_ptr<ResourceManager>* resMgr = reinterpret_cast<std::shared_ptr<ResourceManager>*>(nativeResMgrPtr);
    if (resMgr != nullptr) {
        delete resMgr;
        status = env->Object_SetPropertyByName_Long(jsResMgr, "nativeResMgr", 0);
        if (ANI_OK != status) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to reset nativeResMgr in DestoryResMgr, status: %{public}d.", status);
            return;
        }
    }
}

ani_ref ResMgrAni::TransferToDynamicResource(ani_env *env, ani_object etsResMgr)
{
    std::shared_ptr<ResourceManager> resMgr = AniUtils::GetResourceManager(env, etsResMgr);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in TransferToDynamicResource.");
        return nullptr;
    }
    std::shared_ptr<ResourceManagerImpl> impl = std::static_pointer_cast<ResourceManagerImpl>(resMgr);
    std::shared_ptr<ResourceManagerAddon> resourceManagerAddon = std::make_shared<ResourceManagerAddon>(resMgr,
        AbilityRuntime::ApplicationContext::GetInstance(), impl->IsSystem(), impl->IsOverride());

    ani_ref resAny;
    {
        napi_env jsenv;
        if (!arkts_napi_scope_open(env, &jsenv)) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to open napi js env.");
            return nullptr;
        }
        napi_value resNapi = ResourceManagerAddon::WrapResourceManager(jsenv, resourceManagerAddon);
        if (!arkts_napi_scope_close_n(jsenv, NUM_NAPI_VALUES_TO_WRAP, &resNapi, &resAny)) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to close napi js env.");
            return nullptr;
        }
    }
    return resAny;
}

ani_object ResMgrAni::TransferToStaticResource(ani_env *env, ani_object esValue)
{
    void *nativePtr = nullptr;
    if (!arkts_esvalue_unwrap(env, esValue, &nativePtr) || nativePtr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "arkts_esvalue_unwrap failed.");
        return nullptr;
    }
    std::shared_ptr<ResourceManagerAddon> addon = *reinterpret_cast<std::shared_ptr<ResourceManagerAddon>*>(nativePtr);
    if (addon == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "reinterpret_cast failed.");
        return nullptr;
    }
    std::shared_ptr<ResourceManager> resMgr = addon->GetResMgr();
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in TransferToStaticResource.");
        return nullptr;
    }
    return ResMgrAddon::WrapResourceManager(env, resMgr);
}

ani_status ResMgrAni::BindResMgr(ani_env* env)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::RESOURCE_MANAGER_INNER, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed, status: %{public}d",
            AniSignature::RESOURCE_MANAGER_INNER, status);
        return status;
    }
    status = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Can't bind native methods to %{public}s, status: %{public}d.",
            AniSignature::RESOURCE_MANAGER_INNER, status);
        return status;
    };
    return ANI_OK;
}

ani_status ResMgrAni::BindResMgrCleaner(ani_env* env)
{
    ani_class cls;
    ani_status status = env->FindClass(AniSignature::RESMGR_CLEANER, &cls);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed, status: %{public}d.",
            AniSignature::RESMGR_CLEANER, status);
        return status;
    }
    std::array methods = {
        ani_native_function { "destoryResMgr", nullptr, reinterpret_cast<void*>(DestoryResMgr) },
    };
    status = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Can't bind native methods to '%{public}s', status: %{public}d",
            AniSignature::RESMGR_CLEANER, status);
        return status;
    };
    return ANI_OK;
}

ani_status ResMgrAni::BindStaticFunc(ani_env* env)
{
    ani_namespace ns;
    ani_status status = env->FindNamespace(AniSignature::NAMESPACE, &ns);
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find namespace '%{public}s' failed, status: %{public}d.",
            AniSignature::NAMESPACE, status);
        return status;
    }

    std::array nsMethods = {
        ani_native_function { "getSysResourceManager", nullptr, reinterpret_cast<void*>(GetSysResourceManager) },
        ani_native_function{ "transferToDynamicResource", nullptr,
            reinterpret_cast<void *>(TransferToDynamicResource) },
        ani_native_function{ "transferToStaticResource", nullptr, reinterpret_cast<void *>(TransferToStaticResource) },
    };
    status = env->Namespace_BindNativeFunctions(ns, nsMethods.data(), nsMethods.size());
    if (ANI_OK != status) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Cannot bind native methods to '%{public}s', status: %{public}d.",
            AniSignature::NAMESPACE, status);
        return status;
    };
    return ANI_OK;
}

ani_status ResMgrAni::BindContext(ani_env* env)
{
    ani_status status = BindStaticFunc(env);
    if (ANI_OK != status) {
        return status;
    }

    status = BindResMgr(env);
    if (ANI_OK != status) {
        return status;
    }

    status = BindResMgrCleaner(env);
    if (ANI_OK != status) {
        return status;
    }

    return ANI_OK;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS

ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Unsupported ANI_VERSION_1");
        return (ani_status)ANI_ERROR;
    }

    auto status = OHOS::Global::Resource::ResMgrAni::BindContext(env);
    if (status != ANI_OK) {
        return status;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
