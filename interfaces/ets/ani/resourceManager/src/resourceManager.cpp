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
#include "drawable_descriptor_ani.h"
#include "hilog_wrapper.h"
#include "resource_manager_ani_utils.h"
#include "resource_manager_data_context.h"
#include "resource_manager.h"

using namespace OHOS;
using namespace Global;
using namespace Resource;

constexpr ani_int ABNORMAL_NUMBER_RETURN_VALUE = -1;

enum ScreenDensityIndex {
    SCREEN_DENSITY_ONE = 0,
    SCREEN_DENSITY_TWO = 1,
    SCREEN_DENSITY_THREE = 2,
    SCREEN_DENSITY_FOUR = 3,
    SCREEN_DENSITY_FIVE = 4,
    SCREEN_DENSITY_SIX = 5,
};

struct ArrayElement {
    enum class ElementType { NUMBER, STRING } type;
    std::variant<double, std::string> value;

    ArrayElement(ElementType type, double number) : type(type), value(number) {}
    ArrayElement(ElementType type, const std::string& str) : type(type), value(str) {}
};

static std::shared_ptr<ResourceManager> sysResMgr = nullptr;
static std::mutex sysMgrMutex;
static std::array methods = {
    ani_native_function { "getStringSync", "J:Lstd/core/String;",
        reinterpret_cast<void*>(ResMgrAddon::GetStringSyncById) },
    ani_native_function { "getStringSync", "JLescompat/Array;:Lstd/core/String;",
        reinterpret_cast<void *>(ResMgrAddon::GetFormatStringSyncById) },
    ani_native_function { "getStringByNameSync", "Lstd/core/String;:Lstd/core/String;",
        reinterpret_cast<void*>(ResMgrAddon::GetStringByNameSync) },
    ani_native_function { "getStringByNameSync", "Lstd/core/String;Lescompat/Array;:Lstd/core/String;",
        reinterpret_cast<void*>(ResMgrAddon::GetFormatStringByNameSync) },

    ani_native_function { "getBoolean", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetBooleanById) },
    ani_native_function { "getBooleanByName", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetBooleanByName) },

    ani_native_function { "getInt", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetIntById) },
    ani_native_function { "getIntByName", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetIntByName) },
    ani_native_function { "getDouble", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetDoubleById) },
    ani_native_function { "getDoubleByName", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetDoubleByName) },
    
    ani_native_function { "getIntPluralStringValueSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetIntPluralStringValueSyncById) },
    ani_native_function { "getIntPluralStringByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetIntPluralStringByNameSync) },

    ani_native_function { "getDoublePluralStringValueSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetDoublePluralStringValueSyncById) },
    ani_native_function { "getDoublePluralStringByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetDoublePluralStringByNameSync) },

    ani_native_function { "getColorSync", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetColorSyncById) },
    ani_native_function { "getColorByNameSync", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetColorByNameSync) },
    
    ani_native_function { "addResource", nullptr, reinterpret_cast<void*>(ResMgrAddon::AddResource) },
    ani_native_function { "removeResource", nullptr, reinterpret_cast<void*>(ResMgrAddon::RemoveResource) },

    ani_native_function { "getRawFdSync", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetRawFdSync) },
    ani_native_function { "closeRawFdSync", nullptr, reinterpret_cast<void*>(ResMgrAddon::CloseRawFdSync) },
    ani_native_function { "isRawDir", nullptr, reinterpret_cast<void*>(ResMgrAddon::IsRawDir) },

    ani_native_function { "getRawFileListSync", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetRawFileListSync) },
    ani_native_function { "getRawFileContentSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetRawFileContentSync) },
    
    ani_native_function { "getMediaContentSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetMediaContentSyncById) },

    ani_native_function { "getMediaContentBase64Sync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetMediaContentBase64SyncById) },
    
    ani_native_function { "getStringArrayValueSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetStringArrayValueSyncById) },

    ani_native_function { "getStringArrayByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetStringArrayByNameSync) },
    
    ani_native_function { "getMediaByNameSync", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetMediaByNameSync) },
    ani_native_function { "getMediaBase64ByNameSync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetMediaBase64ByNameSync) },
    
    ani_native_function { "getDrawableDescriptor", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetDrawableDescriptorById) },
    ani_native_function { "getDrawableDescriptorByName", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetDrawableDescriptorByName) },
        
    ani_native_function { "getConfigurationSync", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetConfigurationSync) },
    ani_native_function { "getDeviceCapabilitySync", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetDeviceCapabilitySync) },
    ani_native_function { "getLocales", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetLocales) },

    ani_native_function { "getSymbol", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetSymbolById) },
    ani_native_function { "getSymbolByName", nullptr, reinterpret_cast<void*>(ResMgrAddon::GetSymbolByName) },

    ani_native_function { "getOverrideResourceManager", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetOverrideResourceManager) },
    ani_native_function { "getOverrideConfiguration", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::GetOverrideConfiguration) },
    ani_native_function { "updateOverrideConfiguration", nullptr,
        reinterpret_cast<void*>(ResMgrAddon::UpdateOverrideConfiguration) },
};

ResMgrAddon::ResMgrAddon(
    const std::string& bundleName, const std::shared_ptr<ResourceManager>& resMgr,
    const std::shared_ptr<AbilityRuntime::Context>& context, bool isSystem)
    : bundleName_(bundleName), resMgr_(resMgr), context_(context), isSystem_(isSystem)
{
    RESMGR_HILOGD(RESMGR_ANI_TAG, "create ResMgrAddon %{public}s", bundleName_.c_str());
}

ResMgrAddon::ResMgrAddon(const std::shared_ptr<ResourceManager>& resMgr, bool isSystem)
    : resMgr_(resMgr), isSystem_(isSystem)
{
    RESMGR_HILOGD(RESMGR_ANI_TAG, "create ResMgrAddon.");
}

ani_object ResMgrAddon::CreateResMgr(
    ani_env* env, const std::string& bundleName, const std::shared_ptr<ResourceManager>& resMgr,
    std::shared_ptr<AbilityRuntime::Context> context)
{
    std::shared_ptr<ResMgrAddon> addon = std::make_shared<ResMgrAddon>(bundleName, resMgr, context);
    return WrapResourceManager(env, addon);
}

ani_object ResMgrAddon::WrapResourceManager(ani_env* env, std::shared_ptr<ResMgrAddon> &addon)
{
    ani_object nativeResMgr;
    static const char *className = "@ohos.resourceManager.resourceManager.ResourceManagerInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    auto addonPtr = std::make_unique<std::shared_ptr<ResMgrAddon>>(addon);
    if (ANI_OK != env->Object_New(cls, ctor, &nativeResMgr, reinterpret_cast<ani_long>(addonPtr.get()))) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", className);
        return nullptr;
    }
    addonPtr.release();
    return nativeResMgr;
}

static std::string AniStrToString(ani_env *env, ani_ref aniStr)
{
    ani_string str = static_cast<ani_string>(aniStr);
    ani_size size = 0;
    env->String_GetUTF8Size(str, &size);

    std::string result;
    result.resize(size);
    ani_size written;
    env->String_GetUTF8(str, result.data(), size + 1, &written);
    return result;
}

static ani_string StringToAniStr(ani_env *env, const std::string &str)
{
    ani_string ret;
    if (ANI_OK != env->String_NewUTF8(str.c_str(), str.size(), &ret)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Create ani string failed");
        return nullptr;
    }
    return ret;
}

static ani_object CreateAniArray(ani_env *env, const std::vector<std::string> strs)
{
    static const char *className = "Lescompat/Array;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret;
    if (ANI_OK != env->Object_New(cls, ctor, &ret, strs.size())) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "iC{std.core.Object}:", &set)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '$_set' failed");
        return ret;
    }

    for (size_t i = 0; i < strs.size(); i++) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, StringToAniStr(env, strs[i]))) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Call method '$_set' failed");
            return ret;
        }
    }
    return ret;
}

static ani_object CreateAniUint8Array(ani_env* env, ResMgrDataContext &context)
{
    size_t length = context.len_;
    static const char *className = "escompat.Uint8Array";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object ret = {};
    if (ANI_OK != env->Object_New(cls, ctor, &ret, length)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", className);
        return nullptr;
    }

    ani_method set;
    if (ANI_OK != env->Class_FindMethod(cls, "$_set", "ii:", &set)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '$_set' failed");
        return nullptr;
    }

    for (size_t i = 0; i < length; i++) {
        if (ANI_OK != env->Object_CallMethod_Void(ret, set, i, context.mediaData[i])) {
            RESMGR_HILOGE(RESMGR_ANI_TAG, "Call method '$_set' failed");
            return nullptr;
        }
    }
    return ret;
}

static std::shared_ptr<ResMgrAddon> UnwrapAddon(ani_env* env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeResMgr", &ptr)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get Long 'nativeResMgr' failed");
        return nullptr;
    }
    return *reinterpret_cast<std::shared_ptr<ResMgrAddon>*>(ptr);
}

static ani_string CreateAniString(ani_env *env, ResMgrDataContext& context)
{
    ani_string result;
    std::string str = context.value_;
    if (ANI_OK != env->String_NewUTF8(str.c_str(), str.size(), &result)) {
        context.SetErrorMsg("Failed to create result");
        return nullptr;
    }
    return result;
}

ani_object ResMgrAddon::GetSystemResourceManager(ani_env* env)
{
    if (sysResMgr == nullptr) {
        std::lock_guard<std::mutex> lock(sysMgrMutex);
        if (sysResMgr == nullptr) {
            std::shared_ptr<Global::Resource::ResourceManager>
                systemResManager(Global::Resource::GetSystemResourceManager());
            if (systemResManager == nullptr) {
                ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED);
                return nullptr;
            }
            sysResMgr = systemResManager;
        }
    }
    std::shared_ptr<ResMgrAddon> addon = std::make_shared<ResMgrAddon>(sysResMgr, true);
    return WrapResourceManager(env, addon);
}

static bool GetHapResourceManager(const ResMgrDataContext* dataContext,
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
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetHapResourceManager context == nullptr");
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
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetHapResourceManager moduleContext == nullptr, bundleName = %{public}s," \
            "moduleName = %{public}s", bundleName.c_str(), resource->moduleName.c_str());
        return false;
    }
    resMgr = moduleContext->GetResourceManager();
    return true;
}

ani_string ResMgrAddon::GetStringSyncById(ani_env* env, ani_object object, ani_long resId)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("getStringSyncById failed state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

static ArrayElement GetArrayElement(ani_env* env, ani_object args, const int index)
{
    ani_ref value;
    if (ANI_OK != env->Array_Get_Ref(static_cast<ani_array_ref>(args), index, &value)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Call get failed");
        return ArrayElement{ArrayElement::ElementType::NUMBER, 0};
    }

    ani_class stringClass;
    if (ANI_OK != env->FindClass("std.core.String", &stringClass)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class 'C{std.core.String}' failed");
        return ArrayElement{ArrayElement::ElementType::NUMBER, 0};
    }

    ani_boolean isString;
    env->Object_InstanceOf(static_cast<ani_object>(value), stringClass, &isString);

    if (isString) {
        return ArrayElement{ArrayElement::ElementType::STRING, AniStrToString(env, value)};
    } else {
        ani_double param;
        env->Object_CallMethodByName_Double(static_cast<ani_object>(value), "unboxed", ":d", &param);
        return ArrayElement{ArrayElement::ElementType::NUMBER, param};
    }
}

static bool InitAniParameters(ani_env *env, ani_object args,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    ani_size size;
    if (ANI_OK != env->Array_GetLength(static_cast<ani_array>(args), &size)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get length failed");
        return false;
    }

    if (size <= 0) {
        return true;
    }

    for (size_t i = 0; i < size; ++i) {
        auto param = GetArrayElement(env, args, i);
        if (param.type == ArrayElement::ElementType::NUMBER) {
            jsParams.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER,
                std::to_string(std::get<double>(param.value))));
        } else if (param.type == ArrayElement::ElementType::STRING) {
            jsParams.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_STRING,
                std::get<std::string>(param.value)));
        } else {
            return false;
        }
    }
    return true;
}

ani_string ResMgrAddon::GetFormatStringSyncById(ani_env *env, ani_object object, ani_long resId, ani_object args)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    if (!InitAniParameters(env, args, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "getFormatStringSyncById formatting error");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }
    RState state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("getFormatStringSyncById failed state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_string ResMgrAddon::GetStringByNameSync(ani_env* env, ani_object object, ani_string resName)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringByNameSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetStringFormatByName(dataContext->resName_.c_str(),
        dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetStringByNameSync failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_string ResMgrAddon::GetFormatStringByNameSync(ani_env *env, ani_object object, ani_string resName, ani_object args)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getFormatStringByNameSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    if (!InitAniParameters(env, args, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "getFormatStringByNameSync formatting error");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NAME_FORMAT_ERROR);
        return nullptr;
    }

    RState state = resMgr->GetStringFormatByName(dataContext->resName_.c_str(),
        dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetStringByNameSync failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_boolean ResMgrAddon::GetBooleanById(ani_env* env, ani_object object, ani_long resId)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getBooleanById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return false;
    }

    RState state = resMgr->GetBooleanById(resId, dataContext->bValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetBoolean state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return false;
    }
    return dataContext->bValue_;
}

ani_boolean ResMgrAddon::GetBooleanByName(ani_env* env, ani_object object, ani_string resName)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getBooleanByName");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return false;
    }

    RState state = resMgr->GetBooleanByName(dataContext->resName_.c_str(), dataContext->bValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetBooleanByName failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return false;
    }
    return dataContext->bValue_;
}

ani_int ResMgrAddon::GetIntById(ani_env* env, ani_object object, ani_long resId)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetIntById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetIntegerById(resId, dataContext->iValue_);
    if (state != RState::SUCCESS) {
            dataContext->SetErrorMsg("Failed to process string in GetIntById", true);
            ResourceManagerAniUtils::AniThrow(env, state);
            return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->iValue_;
}

ani_int ResMgrAddon::GetIntByName(ani_env* env, ani_object object, ani_string resName)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in GetIntByName");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    auto resMgr = dataContext->addon_->GetResMgr();
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in GetIntByName");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetIntegerByName(dataContext->resName_.c_str(), dataContext->iValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process number in GetIntByName", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->iValue_;
}

ani_double ResMgrAddon::GetDoubleById(ani_env* env, ani_object object, ani_long resId)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getNumberById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetFloatById(resId, dataContext->fValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process string in getNumberById", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->fValue_;
}

ani_double ResMgrAddon::GetDoubleByName(ani_env* env, ani_object object, ani_string resName)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getNumberByName");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    auto resMgr = dataContext->addon_->GetResMgr();
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getNumberByName");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetFloatByName(dataContext->resName_.c_str(), dataContext->fValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process number in getNumberByName", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->fValue_;
}

ani_string ResMgrAddon::GetIntPluralStringValueSyncById(ani_env* env, ani_object object,
    ani_long resId, ani_int num, ani_object args)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    double number = num;
    if (number > INT_MAX) {
        number = INT_MAX;
    } else if (number < INT_MIN) {
        number = INT_MIN;
    }
    dataContext->quantity_ = { true, number, 0.0 };

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getIntPluralStringValueSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    if (!InitAniParameters(env, args, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }

    RState state = resMgr->GetFormatPluralStringById(dataContext->value_, resId, dataContext->quantity_,
        dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetFormatPluralStringById state", true, state);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_string ResMgrAddon::GetIntPluralStringByNameSync(ani_env* env, ani_object object,
    ani_string resName, ani_int num, ani_object args)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));
    double number = num;
    if (number > INT_MAX) {
        number = INT_MAX;
    } else if (number < INT_MIN) {
        number = INT_MIN;
    }
    dataContext->quantity_ = { true, number, 0.0 };

    if (!InitAniParameters(env, args, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getIntPluralStringByNameSync");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetFormatPluralStringByName(dataContext->value_,
        dataContext->resName_.c_str(), dataContext->quantity_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetFormatPluralStringByName failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_string ResMgrAddon::GetDoublePluralStringValueSyncById(ani_env* env, ani_object object,
    ani_long resId, ani_double num, ani_object args)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->quantity_ = { false, 0, num };

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getIntPluralStringValueSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    if (!InitAniParameters(env, args, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }

    RState state = resMgr->GetFormatPluralStringById(dataContext->value_, resId, dataContext->quantity_,
        dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetFormatPluralStringById state", true, state);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_string ResMgrAddon::GetDoublePluralStringByNameSync(ani_env* env, ani_object object,
    ani_string resName, ani_double num, ani_object args)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));
    dataContext->quantity_ = { false, 0, num };

    if (!InitAniParameters(env, args, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "InitOptionalParameters formatting error");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getDoublePluralStringByNameSync");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetFormatPluralStringByName(dataContext->value_,
        dataContext->resName_.c_str(), dataContext->quantity_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetFormatPluralStringByName failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_long ResMgrAddon::GetColorSyncById(ani_env* env, ani_object object, ani_long resId)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getColorSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetColorById(resId, dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("getColorSyncById failed state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->colorValue_;
}

ani_long ResMgrAddon::GetColorByNameSync(ani_env* env, ani_object object, ani_string resName)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getColorByNameSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetColorByName(dataContext->resName_.c_str(), dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to process number in getColorByNameSync", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->colorValue_;
}

void ResMgrAddon::AddResource(ani_env* env, ani_object object, ani_string path)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);

    auto resMgr = UnwrapAddon(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "resMgr is null, add overlay path = %{public}s", dataContext->path_.c_str());
        return;
    }

    if (!resMgr->GetResMgr()->AddAppOverlay(dataContext->path_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to add overlay path = %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_OVERLAY_RES_PATH_INVALID);
        return;
    }
}

void ResMgrAddon::RemoveResource(ani_env* env, ani_object object, ani_string path)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);

    auto resMgr = UnwrapAddon(env, object);
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_JS_TAG, "resMgr is null, overlay path = %{public}s", dataContext->path_.c_str());
        return;
    }

    if (!resMgr->GetResMgr()->RemoveAppOverlay(dataContext->path_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to add overlay path = %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_OVERLAY_RES_PATH_INVALID);
        return;
    }
}

ani_object ResMgrAddon::GetRawFdSync(ani_env* env, ani_object object, ani_string path)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);
    dataContext->addon_ = UnwrapAddon(env, object);

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getRawFdSync");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetRawFileDescriptorFromHap(dataContext->path_,
        dataContext->descriptor_);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get rawfd by %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }

    static const char *className = "global.rawFileDescriptorInner.RawFileDescriptorInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object obj;
    if (ANI_OK != env->Object_New(cls, ctor, &obj)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", className);
        return nullptr;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Double(obj, "fd", dataContext->descriptor_.fd)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set property 'fd' failed");
    }

    if (ANI_OK != env->Object_SetPropertyByName_Double(obj, "offset", dataContext->descriptor_.offset)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set property 'offset' failed");
    }

    if (ANI_OK != env->Object_SetPropertyByName_Double(obj, "length", dataContext->descriptor_.length)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set property 'length' failed");
    }
    return obj;
}

void ResMgrAddon::CloseRawFdSync(ani_env* env, ani_object object, ani_string path)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);
    dataContext->addon_ = UnwrapAddon(env, object);

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in closeRawFdSync");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return;
    }

    RState state = dataContext->addon_->GetResMgr()->CloseRawFileDescriptor(dataContext->path_);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to close rawfd by %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, state);
        return;
    }
}

ani_object ResMgrAddon::GetRawFileListSync(ani_env* env, ani_object object, ani_string path)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);
    dataContext->addon_ = UnwrapAddon(env, object);

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getRawFileListSync");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetRawFileList(dataContext->path_.c_str(),
        dataContext->arrayValue_);
    if (state != RState::SUCCESS || dataContext->arrayValue_.empty()) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get rawfile list by %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_PATH_INVALID);
        return nullptr;
    }
    
    return CreateAniArray(env, dataContext->arrayValue_);
}

ani_object ResMgrAddon::GetRawFileContentSync(ani_env* env, ani_object object, ani_string path)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);
    dataContext->addon_ = UnwrapAddon(env, object);

    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in getRawFileContentSync");
        ResourceManagerAniUtils::AniThrow(env, NOT_FOUND);
        return nullptr;
    }

    RState state = dataContext->addon_->GetResMgr()->GetRawFileFromHap(dataContext->path_,
        dataContext->len_, dataContext->mediaData);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get rawfile by %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniUint8Array(env, *dataContext);
}

ani_object ResMgrAddon::GetMediaContentSyncById(ani_env* env, ani_object object,
    ani_long resId, ani_object density)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    ani_boolean isUndefined;
    env->Reference_IsUndefined(density, &isUndefined);
    ani_int densityInner = 0;
    if (!isUndefined) {
        env->Object_CallMethodByName_Int(density, "unboxed", ":i", &densityInner);
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getMediaContentSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetMediaDataById(resId, dataContext->len_, dataContext->mediaData, densityInner);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetMediaContentSync state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniUint8Array(env, *dataContext);
}

ani_string ResMgrAddon::GetMediaContentBase64SyncById(ani_env* env, ani_object object,
    ani_long resId, ani_object density)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    ani_boolean isUndefined;
    env->Reference_IsUndefined(density, &isUndefined);
    ani_int densityInner = 0;
    if (!isUndefined) {
        env->Object_CallMethodByName_Int(density, "unboxed", ":i", &densityInner);
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getMediaContentBase64SyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetMediaBase64DataById(resId, dataContext->value_, densityInner);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetMediaContentBase64Sync state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_object ResMgrAddon::GetStringArrayValueSyncById(ani_env* env, ani_object object, ani_long resId)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringArrayValueSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetStringArrayById(resId, dataContext->arrayValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetStringArrayValueSync state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniArray(env, dataContext->arrayValue_);
}

ani_object ResMgrAddon::GetMediaByNameSync(ani_env* env, ani_object object, ani_string resName, ani_object density)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    ani_boolean isUndefined;
    env->Reference_IsUndefined(density, &isUndefined);
    ani_int densityInner = 0;
    if (!isUndefined) {
        env->Object_CallMethodByName_Int(density, "unboxed", ":i", &densityInner);
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getMediaByNameSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetMediaDataByName(dataContext->resName_.c_str(),
        dataContext->len_, dataContext->mediaData, densityInner);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetMediaByNameSync failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniUint8Array(env, *dataContext);
}

ani_string ResMgrAddon::GetMediaBase64ByNameSync(ani_env* env, ani_object object,
    ani_string resName, ani_object density)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    ani_boolean isUndefined;
    env->Reference_IsUndefined(density, &isUndefined);
    ani_int densityInner = 0;
    if (!isUndefined) {
        env->Object_CallMethodByName_Int(density, "unboxed", ":i", &densityInner);
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getMediaBase64ByNameSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetMediaBase64DataByName(dataContext->resName_.c_str(),
        dataContext->value_, densityInner);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to get media data in GetMediaBase64ByNameSync", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_object CreateDrawableDescriptorbyId(ani_env* env, std::unique_ptr<ResMgrDataContext>& dataContext)
{
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getDrawableDescriptorById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    Ace::Ani::DrawableInfo drawableInfo;
    drawableInfo.density = dataContext->density_;
    drawableInfo.manager = resMgr;
    RState state = SUCCESS;
    if (dataContext->iconType_ == 1) {
        std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
        std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
        state = resMgr->GetThemeIcons(resId, foregroundInfo, backgroundInfo, dataContext->density_);
        if (state == SUCCESS) {
            drawableInfo.firstBuffer.data = std::move(foregroundInfo.first);
            drawableInfo.firstBuffer.len = foregroundInfo.second;
            drawableInfo.secondBuffer.data = std::move(backgroundInfo.first);
            drawableInfo.secondBuffer.len = backgroundInfo.second;
            drawableInfo.type = "layered";
            return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
        }
    }
    state = resMgr->GetDrawableInfoById(resId, drawableInfo.type,
        drawableInfo.firstBuffer.len, drawableInfo.firstBuffer.data, dataContext->density_);
    if (SUCCESS != state) {
        dataContext->SetErrorMsg("Failed to Create drawableDescriptor", true);
            ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    };
    return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
}

ani_object ResMgrAddon::GetDrawableDescriptorById(ani_env* env, ani_object object,
    ani_long resId, ani_object density, ani_object type)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resId_ = static_cast<uint32_t>(resId);

    ani_boolean isUndefined;
    env->Reference_IsUndefined(density, &isUndefined);
    if (!isUndefined) {
        ani_int densityInner;
        env->Object_CallMethodByName_Int(density, "unboxed", ":i", &densityInner);
        dataContext->density_ = static_cast<uint32_t>(densityInner);
    }

    env->Reference_IsUndefined(type, &isUndefined);
    if (!isUndefined) {
        ani_int typeInner;
        env->Object_CallMethodByName_Int(type, "unboxed", ":i", &typeInner);
        dataContext->iconType_ = static_cast<uint32_t>(typeInner);
    }
    return CreateDrawableDescriptorbyId(env, dataContext);
}

ani_object CreateDrawableDescriptorbyName(ani_env* env, std::unique_ptr<ResMgrDataContext>& dataContext)
{
    auto resMgr = dataContext->addon_->GetResMgr();
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getDrawableDescriptorByName");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }
    Ace::Ani::DrawableInfo drawableInfo;
    drawableInfo.density = dataContext->density_;
    drawableInfo.manager = resMgr;
    RState state = SUCCESS;
    if (dataContext->iconType_ == 1) {
        std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
        std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
        state = resMgr->GetThemeIcons(0, foregroundInfo, backgroundInfo, dataContext->density_);
        if (state == SUCCESS) {
            drawableInfo.firstBuffer.data = std::move(foregroundInfo.first);
            drawableInfo.firstBuffer.len = foregroundInfo.second;
            drawableInfo.secondBuffer.data = std::move(backgroundInfo.first);
            drawableInfo.secondBuffer.len = backgroundInfo.second;
            drawableInfo.type = "layered";
            return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
        }
    }

    if (dataContext->iconType_ == 2) { // 2 means get the dynamic icon from theme
        std::pair<std::unique_ptr<uint8_t[]>, size_t> iconInfo;
        if (resMgr->GetDynamicIcon(dataContext->resName_, iconInfo, dataContext->density_) == SUCCESS) {
            drawableInfo.firstBuffer.data = std::move(iconInfo.first);
            drawableInfo.firstBuffer.len = iconInfo.second;
            return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
        }
    }

    state = resMgr->GetDrawableInfoByName(dataContext->resName_.c_str(), drawableInfo.type,
        drawableInfo.firstBuffer.len, drawableInfo.firstBuffer.data, dataContext->density_);
    if (SUCCESS != state) {
        dataContext->SetErrorMsg("Failed to Create drawableDescriptor");
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    };
    return Ace::Ani::DrawableDescriptorAni::CreateDrawableDescriptor(env, drawableInfo);
}

ani_object ResMgrAddon::GetDrawableDescriptorByName(ani_env* env, ani_object object,
    ani_string resName, ani_object density, ani_object type)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    ani_boolean isUndefined;
    env->Reference_IsUndefined(density, &isUndefined);
    if (!isUndefined) {
        ani_int densityInner;
        env->Object_CallMethodByName_Int(density, "unboxed", ":i", &densityInner);
        dataContext->density_ = static_cast<uint32_t>(densityInner);
    }

    env->Reference_IsUndefined(type, &isUndefined);
    if (!isUndefined) {
        ani_int typeInner;
        env->Object_CallMethodByName_Int(type, "unboxed", ":i", &typeInner);
        dataContext->iconType_ = static_cast<uint32_t>(typeInner);
    }

    dataContext->addon_ = UnwrapAddon(env, object);
    if (dataContext->addon_ == nullptr) {
        return nullptr;
    }
    dataContext->resName_ = AniStrToString(env, resName);
    return CreateDrawableDescriptorbyName(env, dataContext);
}

ani_object ResMgrAddon::GetStringArrayByNameSync(ani_env* env, ani_object object, ani_string resName)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringArrayByNameSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetStringArrayByName(dataContext->resName_.c_str(), dataContext->arrayValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetStringArrayByNameSync failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniArray(env, dataContext->arrayValue_);
}

static int GetScreenDensityIndex(ScreenDensity value)
{
    switch (value) {
        case ScreenDensity::SCREEN_DENSITY_SDPI: return static_cast<int>(ScreenDensityIndex::SCREEN_DENSITY_ONE);
        case ScreenDensity::SCREEN_DENSITY_MDPI: return static_cast<int>(ScreenDensityIndex::SCREEN_DENSITY_TWO);
        case ScreenDensity::SCREEN_DENSITY_LDPI: return static_cast<int>(ScreenDensityIndex::SCREEN_DENSITY_THREE);
        case ScreenDensity::SCREEN_DENSITY_XLDPI: return static_cast<int>(ScreenDensityIndex::SCREEN_DENSITY_FOUR);
        case ScreenDensity::SCREEN_DENSITY_XXLDPI: return static_cast<int>(ScreenDensityIndex::SCREEN_DENSITY_FIVE);
        case ScreenDensity::SCREEN_DENSITY_XXXLDPI: return static_cast<int>(ScreenDensityIndex::SCREEN_DENSITY_SIX);
        default: return -1;
    }
}

static void SetEnumMember(ani_env *env, ani_object obj, const char* memberName, const char* enumName, const int index)
{
    if (index < 0) {
        RESMGR_HILOGE(RESMGR_TAG, "SetEnumMember: invalid index %{public}d", index);
        return;
    }

    ani_enum aniEnum;
    if (ANI_OK != env->FindEnum(enumName, &aniEnum)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find enum '%{public}s' failed", enumName);
        return;
    }

    ani_enum_item enumItem;
    if (ANI_OK != env->Enum_GetEnumItemByIndex(aniEnum, index, &enumItem)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get enumItem '%{public}s' failed", enumName);
        return;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, memberName, enumItem)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set property '%{public}s' failed", enumName);
        return;
    }
}

static std::string GetLocale(std::unique_ptr<ResConfig> &cfg)
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

static ani_object CreateConfig(ani_env* env, std::unique_ptr<ResConfig> &cfg)
{
    static const char *className = "@ohos.resourceManager.resourceManager.Configuration";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object obj;
    if (ANI_OK != env->Object_New(cls, ctor, &obj)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", className);
        return nullptr;
    }

    SetEnumMember(env, obj, "direction", "@ohos.resourceManager.resourceManager.Direction",
        static_cast<int>(cfg->GetDirection()));
    SetEnumMember(env, obj, "deviceType", "@ohos.resourceManager.resourceManager.DeviceType",
        static_cast<int>(cfg->GetDeviceType()));
    SetEnumMember(env, obj, "screenDensity", "@ohos.resourceManager.resourceManager.ScreenDensity",
        GetScreenDensityIndex(cfg->GetScreenDensityDpi()));
    SetEnumMember(env, obj, "colorMode", "@ohos.resourceManager.resourceManager.ColorMode",
        static_cast<int>(cfg->GetColorMode()));

    if (ANI_OK != env->Object_SetPropertyByName_Int(obj, "mcc", static_cast<int>(cfg->GetMcc()))) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set property 'mcc' failed");
        return obj;
    }

    if (ANI_OK != env->Object_SetPropertyByName_Int(obj, "mnc", static_cast<int>(cfg->GetMnc()))) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set property 'mnc' failed");
        return obj;
    }

    std::string value = GetLocale(cfg);
    if (ANI_OK != env->Object_SetPropertyByName_Ref(obj, "locale", StringToAniStr(env, value))) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Set property 'locale' failed");
        return obj;
    }
    return obj;
}

ani_object ResMgrAddon::GetConfigurationSync(ani_env* env, ani_object object)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in GetConfigurationSync");
        return nullptr;
    }

    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        dataContext->SetErrorMsg("Failed to create ResConfig object.");
        return nullptr;
    }
    dataContext->addon_->GetResMgr()->GetResConfig(*cfg);
    return CreateConfig(env, cfg);
}

ani_object ResMgrAddon::GetDeviceCapabilitySync(ani_env* env, ani_object object)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in GetConfigurationSync");
        return nullptr;
    }

    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        dataContext->SetErrorMsg("Failed to create ResConfig object.");
        return nullptr;
    }
    dataContext->addon_->GetResMgr()->GetResConfig(*cfg);

    static const char *className = "@ohos.resourceManager.resourceManager.DeviceCapability";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object obj;
    if (ANI_OK != env->Object_New(cls, ctor, &obj)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", className);
        return nullptr;
    }

    SetEnumMember(env, obj, "screenDensity", "@ohos.resourceManager.resourceManager.ScreenDensity",
        GetScreenDensityIndex(cfg->GetScreenDensityDpi()));
    SetEnumMember(env, obj, "deviceType", "@ohos.resourceManager.resourceManager.DeviceType",
        static_cast<int>(cfg->GetDeviceType()));
    return obj;
}

ani_object ResMgrAddon::GetLocales(ani_env* env, ani_object object, ani_object includeSystem)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get addon in GetConfigurationSync");
        return nullptr;
    }
    
    ani_boolean isUndefined;
    env->Reference_IsUndefined(includeSystem, &isUndefined);
    if (!isUndefined) {
        ani_boolean includeSys;
        env->Object_CallMethodByName_Boolean(includeSystem, "unboxed", ":z", &includeSys);
        dataContext->bValue_ = includeSys;
    }

    dataContext->addon_->GetResMgr()->GetLocales(dataContext->arrayValue_, dataContext->bValue_);
    return CreateAniArray(env, dataContext->arrayValue_);
}

ani_long ResMgrAddon::GetSymbolById(ani_env* env, ani_object object, ani_long resId)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getSymbolById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetSymbolById(resId, dataContext->symbolValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetSymbol failed state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->symbolValue_;
}

ani_long ResMgrAddon::GetSymbolByName(ani_env* env, ani_object object, ani_string resName)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resName_ = AniStrToString(env, static_cast<ani_ref>(resName));

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getSymbolByName");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetSymbolByName(dataContext->resName_.c_str(), dataContext->symbolValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("GetSymbolByName failed state", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->symbolValue_;
}

ani_boolean ResMgrAddon::IsRawDir(ani_env* env, ani_object object, ani_string path)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);
    dataContext->addon_ = UnwrapAddon(env, object);

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret || resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in isRawDir");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return false;
    }

    RState state = resMgr->IsRawDirFromHap(dataContext->path_, dataContext->bValue_);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG,
            "Failed to determine the raw file is directory by %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, state);
        return false;
    }
    return dataContext->bValue_;
}

static bool GetEnumMember(ani_env *env, ani_object options, const std::string name, int& member)
{
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(options, name.c_str(), &ref)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get property '%{public}s' failed", name.c_str());
        return false;
    }

    ani_boolean isUndefined;
    env->Reference_IsUndefined(ref, &isUndefined);
    if (isUndefined) {
        return false;
    }

    if (ANI_OK != env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(ref), &member)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Enum '%{public}s' get value int failed", name.c_str());
        return false;
    }
    return true;
}

static bool GetNumberMember(ani_env *env, ani_object options, const std::string name, int& value)
{
    if (ANI_OK != env->Object_GetPropertyByName_Int(options, name.c_str(), &value)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get property '%{public}s' failed", name.c_str());
        return false;
    }
    return true;
}

static bool GetEnumParamOfConfig(ani_env* env, std::shared_ptr<ResConfig> configPtr, ani_object configuration)
{
    int intEnum;
    if (GetEnumMember(env, configuration, "direction", intEnum)) {
        configPtr->SetDirection(static_cast<Direction>(intEnum));
    }
    if (GetEnumMember(env, configuration, "deviceType", intEnum)) {
        configPtr->SetDeviceType(static_cast<DeviceType>(intEnum));
    }
    if (GetEnumMember(env, configuration, "screenDensity", intEnum)) {
        configPtr->SetScreenDensityDpi(static_cast<ScreenDensity>(intEnum));
    }
    if (GetEnumMember(env, configuration, "colorMode", intEnum)) {
        configPtr->SetColorMode(static_cast<ColorMode>(intEnum));
    }

    int value;
    if (GetNumberMember(env, configuration, "mcc", value)) {
        configPtr->SetMcc(value);
    }
    if (GetNumberMember(env, configuration, "mnc", value)) {
        configPtr->SetMnc(value);
    }
    return true;
}

static bool GetLocaleOfConfig(ani_env* env, std::shared_ptr<ResConfig> configPtr, ani_object configuration)
{
#ifdef SUPPORT_GRAPHICS
    ani_ref ret;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(configuration, "locale", &ret)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get property 'locale' failed");
        return false;
    }
    if (ret == nullptr) {
        RESMGR_HILOGD(RESMGR_JS_TAG, "GetLocaleOfConfig property locale not set");
        return true;
    }
    if (configPtr->SetLocaleInfo(AniStrToString(env, static_cast<ani_string>(ret)).c_str()) != SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetLocaleOfConfig failed to SetLocaleInfo");
        return false;
    }
#endif
    return true;
}

static RState GetAddonAndConfig(ani_env* env, ani_object object,
    ani_object configuration, std::unique_ptr<ResMgrDataContext> &dataContext)
{
    dataContext->addon_ = UnwrapAddon(env, object);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetAddonAndConfig failed to get addon");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    ani_boolean isUndefined;
    env->Reference_IsUndefined(configuration, &isUndefined);
    if (isUndefined) {
        RESMGR_HILOGD(RESMGR_ANI_TAG, "GetConfigObject, no config");
        return SUCCESS;
    }

    ResConfig *config = CreateDefaultResConfig();
    if (config == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetConfigObject, new config failed");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    dataContext->overrideResConfig_.reset(config);

    if (!GetEnumParamOfConfig(env, dataContext->overrideResConfig_, configuration)) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!GetLocaleOfConfig(env, dataContext->overrideResConfig_, configuration)) {
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    return SUCCESS;
}

ani_object ResMgrAddon::CreateOverrideAddon(ani_env* env, const std::shared_ptr<ResourceManager>& resMgr)
{
    std::shared_ptr<ResMgrAddon> addon = std::make_shared<ResMgrAddon>(bundleName_, resMgr, context_);
    addon->isOverrideAddon_ = true;
    return WrapResourceManager(env, addon);
}

ani_object ResMgrAddon::GetOverrideResourceManager(ani_env* env, ani_object object, ani_object configuration)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    int32_t state = GetAddonAndConfig(env, object, configuration, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to get param in GetOverrideResourceManager", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> resMgr = dataContext->addon_->GetResMgr();
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getOverrideResourceManager");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> overrideResMgr = resMgr->GetOverrideResourceManager(
        dataContext->overrideResConfig_);
    if (overrideResMgr == nullptr) {
        dataContext->SetErrorMsg("GetOverrideResourceManager, overrideResMgr is null", false);
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }

    return dataContext->addon_->CreateOverrideAddon(env, overrideResMgr);
}

ani_object ResMgrAddon::GetOverrideConfiguration(ani_env* env, ani_object object)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    if (dataContext->addon_ == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetOverrideConfiguration failed to get addon");
        return nullptr;
    }

    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        dataContext->SetErrorMsg("Failed to create ResConfig object.");
        return nullptr;
    }
    dataContext->addon_->GetResMgr()->GetOverrideResConfig(*cfg);
    return CreateConfig(env, cfg);
}

void ResMgrAddon::UpdateOverrideConfiguration(ani_env* env, ani_object object, ani_object configuration)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    int32_t state = GetAddonAndConfig(env, object, configuration, dataContext);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to get param in GetOverrideResourceManager", false);
        ResourceManagerAniUtils::AniThrow(env, state);
        return;
    }

    std::shared_ptr<ResourceManager> resMgr = dataContext->addon_->GetResMgr();
    if (resMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in updateOverrideConfiguration");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return;
    }

    state = resMgr->UpdateOverrideResConfig(*dataContext->overrideResConfig_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("UpdateOverrideConfiguration failed due to invalid config", false);
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return;
    }
}

ani_status ResMgrAddon::BindContext(ani_env* env)
{
    static const char* className = "@ohos.resourceManager.resourceManager.ResourceManagerInner";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return (ani_status)ANI_ERROR;
    }

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Cannot bind native methods to '%{public}s'", className);
        return (ani_status)ANI_ERROR;
    };

    static const char* nameSpaceName = "@ohos.resourceManager.resourceManager";
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(nameSpaceName, &ns)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find namespace '%{public}s' failed", nameSpaceName);
        return (ani_status)ANI_ERROR;
    }

    std::array nsMethods = {
        ani_native_function { "getSystemResourceManager", nullptr, reinterpret_cast<void*>(GetSystemResourceManager) },
    };

    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, nsMethods.data(), nsMethods.size())) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Cannot bind native methods to '%{public}s'", nameSpaceName);
        return (ani_status)ANI_ERROR;
    };
    return ANI_OK;
}
