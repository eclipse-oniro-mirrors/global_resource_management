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

#include "hilog_wrapper.h"
#include "resourceManager.h"
#include "resource_manager_ani_utils.h"
#include "resource_manager_data_context.h"
#include "resource_manager_impl.h"

using namespace OHOS;
using namespace Global;
using namespace Resource;

constexpr ani_double ABNORMAL_NUMBER_RETURN_VALUE = -1;

struct ArrayElement {
    enum class ElementType { NUMBER, STRING } type;
    std::variant<double, std::string> value;

    ArrayElement(ElementType type, double number) : type(type), value(number) {}
    ArrayElement(ElementType type, const std::string& str) : type(type), value(str) {}
};

static std::shared_ptr<ResourceManager> sysResMgr = nullptr;
static std::mutex sysMgrMutex;

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
    static const char *className = "L@ohos/resourceManager/resourceManager/ResourceManagerInner;";
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

static ani_object createUint8Array(ani_env* env, ani_array data)
{
    ani_object ret = {};
    static const char *className = "Lescompat/Uint8Array;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return nullptr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "[D:V", &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find method '<ctor>' failed");
        return nullptr;
    }

    if (ANI_OK != env->Object_New(cls, ctor, &ret, data)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "New object '%{public}s' failed", className);
        return nullptr;
    }
    return ret;
}

static ani_object CreateAniUint8Array(ani_env* env, ResMgrDataContext &context)
{
    size_t length = context.len_;
    uint8_t* data = new uint8_t[length];
    uint8_t *tempData = context.mediaData.release();
    std::copy(tempData, tempData + length, data);
    delete[] tempData;

    ani_array_int datas;
    if (ANI_OK != env->Array_New_Int(length, &datas)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Array_New_Int Fail");
        delete[] data;
        return nullptr;
    }

    std::vector<int> intData(length);
    for (size_t i = 0; i < length; ++i) {
        intData[i] = static_cast<int>(data[i]);
    }

    if (ANI_OK != env->Array_SetRegion_Int(datas, 0, length, intData.data())) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Array_SetRegion_Int Fail");
        delete[] data;
        return nullptr;
    }

    ani_object obj = createUint8Array(env, datas);
    delete[] data;
    return obj;
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

static int32_t InitIdResourceAddon(ani_env* env, ani_object object,
    std::unique_ptr<ResMgrDataContext>& dataContext, const ani_object resource)
{
    dataContext->addon_ = UnwrapAddon(env, object);
    auto resourcePtr = std::make_shared<ResourceManager::Resource>();
    if (resource == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Make shared ResourceManager::Resource failed");
        return ERROR;
    }

    ani_ref bundleName;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(resource, "bundleName", &bundleName)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get property 'bundleName' failed");
        return ERROR;
    }

    resourcePtr->bundleName = AniStrToString(env, bundleName);
    ani_ref moduleName;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(resource, "moduleName", &moduleName)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get property 'moduleName' failed");
        return ERROR;
    }

    resourcePtr->moduleName = AniStrToString(env, moduleName);
    ani_double id;
    if (ANI_OK != env->Object_GetPropertyByName_Double(resource, "id", &id)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Get property 'id' failed");
        return ERROR;
    }
    resourcePtr->id = id;
    dataContext->resource_ = resourcePtr;
    return SUCCESS;
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

ani_object ResMgrAddon::getSystemResourceManager(ani_env* env)
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
    std::shared_ptr<ResourceManager> &resMgr, int32_t &resId)
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

ani_string ResMgrAddon::getStringSyncById(ani_env* env, ani_object object, ani_double resId)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resId_ = resId;

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret) {
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

static ArrayElement getArrayElement(ani_env* env, ani_object args, const int index)
{
    ani_ref value;
    if (ANI_OK != env->Array_Get_Ref(static_cast<ani_array_ref>(args), index, &value)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Call get failed");
        return ArrayElement{ArrayElement::ElementType::NUMBER, 0};
    }

    ani_class stringClass;
    if (ANI_OK != env->FindClass("Lstd/core/String;", &stringClass)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class 'Lstd/core/String;' failed");
        return ArrayElement{ArrayElement::ElementType::NUMBER, 0};
    }

    ani_boolean isString;
    env->Object_InstanceOf(static_cast<ani_object>(value), stringClass, &isString);

    if (isString) {
        return ArrayElement{ArrayElement::ElementType::STRING, AniStrToString(env, value)};
    } else {
        ani_double param;
        env->Object_CallMethodByName_Double(static_cast<ani_object>(value), "unboxed", ":D", &param);
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
        auto param = getArrayElement(env, args, i);
        if (param.type == ArrayElement::ElementType::NUMBER) {
            jsParams.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER,
                std::to_string(std::get<double>(param.value))));
        } else if (param.type == ArrayElement::ElementType::STRING) {
            jsParams.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_STRING,
                std::get<string>(param.value)));
        } else {
            return false;
        }
    }
    return true;
}

ani_string ResMgrAddon::getFormatStringSyncById(ani_env *env, ani_object object, ani_double resId, ani_object args)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resId_ = resId;
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret) {
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

ani_string ResMgrAddon::getStringSync(ani_env* env, ani_object object, ani_object resource)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    int32_t state = InitIdResourceAddon(env, object, dataContext, resource);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in getStringSync", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getStringSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("getStringSync failed state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_string ResMgrAddon::getFormatStringSync(ani_env *env, ani_object object, ani_object resource, ani_object args)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    int32_t state = InitIdResourceAddon(env, object, dataContext, resource);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in getFormatStringSync", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getFormatStringSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    if (!InitAniParameters(env, args, dataContext->jsParams_)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "getFormatStringSync formatting error");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_ID_FORMAT_ERROR);
        return nullptr;
    }

    state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("getFormatStringSync failed state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_double ResMgrAddon::getNumberById(ani_env* env, ani_object object, ani_double resId)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resId_ = resId;

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getNumberById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    RState state = resMgr->GetIntegerById(resId, dataContext->iValue_);
    if (state != RState::SUCCESS) {
        state = resMgr->GetFloatById(resId, dataContext->fValue_);
        dataContext->SetErrorMsg("Failed to process string in getNumberById", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    ani_double aniValue;
    if (dataContext->iValue_) {
        aniValue = dataContext->iValue_;
    } else {
        aniValue = dataContext->fValue_;
    }
    return aniValue;
}

ani_double ResMgrAddon::getNumber(ani_env* env, ani_object object, ani_object resource)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    int32_t state = InitIdResourceAddon(env, object, dataContext, resource);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in getNumber", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getNumber");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    state = resMgr->GetIntegerById(resId, dataContext->iValue_);
    if (state != RState::SUCCESS) {
        state = resMgr->GetFloatById(resId, dataContext->fValue_);
        dataContext->SetErrorMsg("Failed to process string in getNumber", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    ani_double aniValue;
    if (dataContext->iValue_) {
        aniValue = dataContext->iValue_;
    } else {
        aniValue = dataContext->fValue_;
    }
    return aniValue;
}

ani_double ResMgrAddon::getColorSyncById(ani_env* env, ani_object object, ani_double resId)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resId_ = resId;

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret) {
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

ani_double ResMgrAddon::getColorSync(ani_env* env, ani_object object, ani_object resource)
{
    auto dataContext = std::make_unique<ResMgrDataContext>();
    int32_t state = InitIdResourceAddon(env, object, dataContext, resource);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in getColorSync", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getColorSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }

    state = resMgr->GetColorById(resId, dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("getColorSync failed state", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return ABNORMAL_NUMBER_RETURN_VALUE;
    }
    return dataContext->colorValue_;
}

ani_object ResMgrAddon::getRawFileContentSync(ani_env* env, ani_object object, ani_string path)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->path_ = AniStrToString(env, path);
    dataContext->addon_ = UnwrapAddon(env, object);

    RState state = dataContext->addon_->GetResMgr()->GetRawFileFromHap(dataContext->path_,
        dataContext->len_, dataContext->mediaData);
    if (state != RState::SUCCESS) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get rawfile by %{public}s", dataContext->path_.c_str());
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniUint8Array(env, *dataContext);
}

ani_string ResMgrAddon::getPluralStringValueSyncById(ani_env* env, ani_object object, ani_double resId, ani_double num)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->addon_ = UnwrapAddon(env, object);
    dataContext->resId_ = resId;
    dataContext->param_ = num;

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t notUse = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, notUse);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getPluralStringValueSyncById");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    RState state = resMgr->GetFormatPluralStringById(dataContext->value_,
        resId, dataContext->param_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getPluralStringValueSyncById state", true, state);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_string ResMgrAddon::getPluralStringValueSync(ani_env* env, ani_object object, ani_object resource, ani_double num)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    int32_t state = InitIdResourceAddon(env, object, dataContext, resource);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to init para in getPluralStringValueSync", true);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    dataContext->param_ = num;

    std::shared_ptr<ResourceManager> resMgr = nullptr;
    int32_t resId = 0;
    bool ret = GetHapResourceManager(dataContext.get(), resMgr, resId);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Failed to get resMgr in getPluralStringValueSync");
        ResourceManagerAniUtils::AniThrow(env, ERROR_CODE_RES_NOT_FOUND_BY_ID);
        return nullptr;
    }

    state = resMgr->GetFormatPluralStringById(dataContext->value_, resId, dataContext->param_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getPluralStringValueSync state", true, state);
        ResourceManagerAniUtils::AniThrow(env, state);
        return nullptr;
    }
    return CreateAniString(env, *dataContext);
}

ani_status ResMgrAddon::BindContext(ani_env* env)
{
    static const char* className = "L@ohos/resourceManager/resourceManager/ResourceManagerInner;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find class '%{public}s' failed", className);
        return (ani_status)ANI_ERROR;
    }
    std::array methods = {
        ani_native_function { "getStringSync", "D:Lstd/core/String;", reinterpret_cast<void*>(getStringSyncById) },
        ani_native_function { "getStringSync", "D[Lstd/core/Object;:Lstd/core/String;",
            reinterpret_cast<void *>(getFormatStringSyncById) },
        ani_native_function { "getStringSync", "Lglobal/resource/Resource;:Lstd/core/String;",
            reinterpret_cast<void*>(getStringSync) },
        ani_native_function { "getStringSync", "Lglobal/resource/Resource;[Lstd/core/Object;:Lstd/core/String;",
            reinterpret_cast<void *>(getFormatStringSync) },
        ani_native_function { "getNumber", "D:D", reinterpret_cast<void*>(getNumberById) },
        ani_native_function { "getNumber", "Lglobal/resource/Resource;:D", reinterpret_cast<void*>(getNumber) },
        ani_native_function { "getColorSync", "D:D", reinterpret_cast<void*>(getColorSyncById) },
        ani_native_function { "getColorSync", "Lglobal/resource/Resource;:D", reinterpret_cast<void*>(getColorSync) },
        ani_native_function { "getRawFileContentSync", nullptr, reinterpret_cast<void*>(getRawFileContentSync) },
        ani_native_function { "getPluralStringValueSync", "DD:Lstd/core/String;",
            reinterpret_cast<void*>(getPluralStringValueSyncById) },
        ani_native_function { "getPluralStringValueSync", "Lglobal/resource/Resource;D:Lstd/core/String;",
            reinterpret_cast<void*>(getPluralStringValueSync) },
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Cannot bind native methods to '%{public}s'", className);
        return (ani_status)ANI_ERROR;
    };

    static const char* nameSpaceName = "L@ohos/resourceManager/resourceManager;";
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(nameSpaceName, &ns)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Find namespace '%{public}s' failed", nameSpaceName);
        return (ani_status)ANI_ERROR;
    }

    std::array nsMethods = {
        ani_native_function { "getSystemResourceManager", nullptr, reinterpret_cast<void*>(getSystemResourceManager) },
    };

    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, nsMethods.data(), nsMethods.size())) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Cannot bind native methods to '%{public}s'", nameSpaceName);
        return (ani_status)ANI_ERROR;
    };
    return ANI_OK;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Unsupported ANI_VERSION_1");
        return (ani_status)ANI_ERROR;
    }

    auto status = ResMgrAddon::BindContext(env);
    if (status != ANI_OK) {
        return status;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}