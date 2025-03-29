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

#include <algorithm>

#include "resourceManager.h"
#include "resource_manager_data_context.h"
#include "resource_manager_impl.h"

using namespace OHOS;
using namespace Global;
using namespace Resource;

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
    napiContext_ = std::make_shared<ResourceManagerNapiContext>();
}

ResMgrAddon::ResMgrAddon(const std::shared_ptr<ResourceManager>& resMgr, bool isSystem)
    : resMgr_(resMgr), isSystem_(isSystem)
{
    napiContext_ = std::make_shared<ResourceManagerNapiContext>();
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
    ani_object nativeResMgr = {};
    static const char *className = "L@ohos/resourceManager/resourceManager/ResourceManagerInner;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        std::cerr << "Not found '" << className << "'" << std::endl;
        return nativeResMgr;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        std::cerr << "get ctor Failed'" << className << "'" << std::endl;
        return nativeResMgr;
    }

    if (ANI_OK != env->Object_New(cls, ctor, &nativeResMgr, reinterpret_cast<ani_long>(addon->GetResMgr().get()))) {
        std::cerr << "Create Object Failed'" << className << "'" << std::endl;
        return nativeResMgr;
    }
    return nativeResMgr;
}

static std::string AniStrToString(ani_env *env, ani_ref aniStr)
{
    ani_string ani_str = static_cast<ani_string>(aniStr);
    ani_size strSize;
    env->String_GetUTF8Size(ani_str, &strSize);
   
    std::vector<char> buffer(strSize + 1);
    char* utf8Buffer = buffer.data();

    ani_size bytes_written = 0;
    env->String_GetUTF8(ani_str, utf8Buffer, strSize + 1, &bytes_written);

    utf8Buffer[bytes_written] = '\0';
    std::string content = std::string(utf8Buffer);
    return content;
}

static ResourceManagerImpl* unwrapAddon(ani_env* env, ani_object object)
{
    ani_long ptr;
    if (ANI_OK != env->Object_GetFieldByName_Long(object, "nativeResMgr", &ptr)) {
        return nullptr;
    }
    return reinterpret_cast<ResourceManagerImpl*>(ptr);
}

static void InitIdResourceAddon(ani_env* env, ani_object object,
    std::unique_ptr<ResMgrDataContext>& dataContext, const ani_object resource)
{
    auto resourcePtr = std::make_shared<ResourceManager::Resource>();
    if (resource == nullptr) {
        return;
    }
    ani_ref bundleName;
    ani_status status;
    if ((status = env->Object_GetPropertyByName_Ref(resource, "bundleName", &bundleName)) != ANI_OK) {
        return;
    }
    resourcePtr->bundleName = AniStrToString(env, bundleName);
    ani_ref moduleName;
    if ((status = env->Object_GetPropertyByName_Ref(resource, "moduleName", &moduleName)) != ANI_OK) {
        return;
    }
    resourcePtr->moduleName = AniStrToString(env, moduleName);
    ani_double id;
    if ((status = env->Object_GetPropertyByName_Double(resource, "id", &id)) != ANI_OK) {
        return;
    }
    resourcePtr->id = id;
    dataContext->resource_ = resourcePtr;
}

ani_object ResMgrAddon::getSystemResourceManager([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object)
{
    if (sysResMgr == nullptr) {
        std::lock_guard<std::mutex> lock(sysMgrMutex);
        if (sysResMgr == nullptr) {
            std::shared_ptr<Global::Resource::ResourceManager>
                systemResManager(Global::Resource::GetSystemResourceManager());
            if (systemResManager == nullptr) {
                return nullptr;
            }
            sysResMgr = systemResManager;
        }
    }
    std::shared_ptr<ResMgrAddon> addon = std::make_shared<ResMgrAddon>(sysResMgr, true);
    return WrapResourceManager(env, addon);
}

ani_string ResMgrAddon::getStringSync0([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
    ani_double resId)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    auto resMgr = unwrapAddon(env, object);
    dataContext->resId_ = resId;
    ani_string ret;
    std::string str = "";
    if (resMgr == nullptr) {
        env->String_NewUTF8(str.c_str(), str.size(), &ret);
        return ret;
    }
    RState state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetStringSync state", true, state);
        env->String_NewUTF8(str.c_str(), str.size(), &ret);
        return ret;
    }
    str = dataContext->value_;
    env->String_NewUTF8(str.c_str(), str.size(), &ret);
    return ret;
}

ArrayElement getArrayElement(ani_env* env, ani_object args, int index)
{
    ani_ref value;
    if (ANI_OK != env->Object_CallMethodByName_Ref(args, "$_get", "I:Lstd/core/Object;", &value, index)) {
        std::cerr << "Object_CallMethodByName_Ref Failed" << std::endl;
        return ArrayElement{ArrayElement::ElementType::NUMBER, 0};
    }

    ani_class stringClass;
    env->FindClass("Lstd/core/String;", &stringClass);

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

ani_string ResMgrAddon::getStringSync1([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_double resId, ani_object args)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    dataContext->resId_ = resId;

    ani_string ret;
    std::string str = "";
    env->String_NewUTF8(str.c_str(), str.size(), &ret);

    ani_double length;
    if (ANI_OK != env->Object_GetPropertyByName_Double(args, "length", &length)) {
        std::cerr << "Object_GetPropertyByName_Double length Failed" << std::endl;
        return ret;
    }
    if (length == -1) {
        return ret;
    }

    for (int i = 0; i < length; ++i) {
        auto param = getArrayElement(env, args, i);
        if (param.type == ArrayElement::ElementType::NUMBER) {
            dataContext->jsParams_.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER,
                std::to_string(std::get<double>(param.value))));
        } else if (param.type == ArrayElement::ElementType::STRING) {
            dataContext->jsParams_.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_STRING,
                std::get<string>(param.value)));
        } else {
            return ret;
        }
    }

    auto resMgr = unwrapAddon(env, object);
    if (resMgr == nullptr) {
        return ret;
    }

    RState state = resMgr->GetStringFormatById(resId, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetStringSync state", true, state);
        return ret;
    }
    str = dataContext->value_;
    env->String_NewUTF8(str.c_str(), str.size(), &ret);
    return ret;
}

ani_string ResMgrAddon::getStringSync2([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
    ani_object resource)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    InitIdResourceAddon(env, object, dataContext, resource);
    ani_string ret;
    std::string str = "";
    auto resMgr = unwrapAddon(env, object);
    if (resMgr == nullptr) {
        env->String_NewUTF8(str.c_str(), str.size(), &ret);
        return ret;
    }
    RState state = resMgr->GetStringFormatById(dataContext->resource_->id, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetStringSync state", true, state);
        env->String_NewUTF8(str.c_str(), str.size(), &ret);
        return ret;
    }
    str = dataContext->value_;
    env->String_NewUTF8(str.c_str(), str.size(), &ret);
    return ret;
}

ani_string ResMgrAddon::getStringSync3([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object,
    ani_object resource, ani_object args)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    InitIdResourceAddon(env, object, dataContext, resource);

    ani_string ret;
    std::string str = "";
    env->String_NewUTF8(str.c_str(), str.size(), &ret);

    ani_double length;
    if (ANI_OK != env->Object_GetPropertyByName_Double(args, "length", &length)) {
        std::cerr << "Object_GetPropertyByName_Double length Failed" << std::endl;
        return ret;
    }
    if (length == -1) {
        return ret;
    }

    for (int i = 0; i < length; ++i) {
        auto param = getArrayElement(env, args, i);
        if (param.type == ArrayElement::ElementType::NUMBER) {
            dataContext->jsParams_.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER,
                std::to_string(std::get<double>(param.value))));
        } else if (param.type == ArrayElement::ElementType::STRING) {
            dataContext->jsParams_.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_STRING,
                std::get<string>(param.value)));
        } else {
            return ret;
        }
    }

    auto resMgr = unwrapAddon(env, object);
    if (resMgr == nullptr) {
        return ret;
    }

    RState state = resMgr->GetStringFormatById(dataContext->resource_->id, dataContext->value_, dataContext->jsParams_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to GetStringSync state", true, state);
        return ret;
    }
    str = dataContext->value_;
    env->String_NewUTF8(str.c_str(), str.size(), &ret);
    return ret;
}

ani_double ResMgrAddon::getNumber0([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object, ani_double resId)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    auto resMgr = unwrapAddon(env, object);
    dataContext->resId_ = resId;
    RState state = resMgr->GetIntegerById(resId, dataContext->iValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getNumber0 state", true, state);
        return -1;
    }
    return dataContext->iValue_;
}

ani_double ResMgrAddon::getNumber1([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
    ani_object resource)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    InitIdResourceAddon(env, object, dataContext, resource);
    auto resMgr = unwrapAddon(env, object);
    RState state = resMgr->GetIntegerById(dataContext->resource_->id, dataContext->iValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getNumber1 state", true, state);
        return -1;
    }
    return dataContext->iValue_;
}

ani_double ResMgrAddon::getColorSync0([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
    ani_double resId)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    auto resMgr = unwrapAddon(env, object);
    dataContext->resId_ = resId;
    RState state = resMgr->GetColorById(resId, dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getColorSync0 state", true, state);
        return -1;
    }
    return dataContext->colorValue_;
}

ani_double ResMgrAddon::getColorSync1([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
    ani_object resource)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    InitIdResourceAddon(env, object, dataContext, resource);
    auto resMgr = unwrapAddon(env, object);
    RState state = resMgr->GetColorById(dataContext->resource_->id, dataContext->colorValue_);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getColorSync1 state", true, state);
        return -1;
    }
    return dataContext->colorValue_;
}

ani_object createEmptyUint8Array(ani_env* env)
{
    ani_object ret = {};
    static const char *className = "Lescompat/Uint8Array;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        std::cerr << "Not found '" << className << "'" << std::endl;
        return ret;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":V", &ctor)) {
        std::cerr << "get ctor Failed'" << className << "'" << std::endl;
        return ret;
    }

    if (ANI_OK != env->Object_New(cls, ctor, &ret)) {
        std::cerr << "Create Object Failed'" << className << "'" << std::endl;
    }
    return ret;
}

ani_object createUint8Array(ani_env* env, ani_array data)
{
    ani_object ret = {};
    static const char *className = "Lescompat/Uint8Array;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        std::cerr << "Not found '" << className << "'" << std::endl;
        return ret;
    }

    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "[I:V", &ctor)) {
        std::cerr << "get ctor Failed'" << className << "'" << std::endl;
        return ret;
    }

    if (ANI_OK != env->Object_New(cls, ctor, &ret, data)) {
        std::cerr << "Create Object Failed'" << className << "'" << std::endl;
    }
    return ret;
}

ani_object ResMgrAddon::getRawFileContentSync([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object object,
    ani_string path)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    auto resMgr = unwrapAddon(env, object);
    dataContext->path_ = AniStrToString(env, path);

    RState state = resMgr->GetRawFileFromHap(dataContext->path_, dataContext->len_, dataContext->mediaData);
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getRawFileContentSync state", true, state);
        return createEmptyUint8Array(env);
    }

    size_t length = dataContext->len_;
    uint8_t* data = new uint8_t[length];
    uint8_t *tempData = dataContext->mediaData.release();
    std::copy(tempData, tempData + length, data);
    delete[] tempData;

    ani_array_int datas;
    if (env->Array_New_Int(length, &datas) != ANI_OK) {
        std::cerr << "Array_New_Int Fail" << std::endl;
        delete[] data;
        return createEmptyUint8Array(env);
    }

    int* intData = new int[length];
    for (size_t i = 0; i < length; ++i) {
        intData[i] = static_cast<int>(data[i]);
    }

    if (env->Array_SetRegion_Int(datas, 0, length, intData) != ANI_OK) {
        std::cerr << "Array_SetRegion_Int Fail" << std::endl;
        delete[] data;
        return createEmptyUint8Array(env);
    }

    ani_object obj = createUint8Array(env, datas);
    delete[] data;
    return obj;
}

ani_string ResMgrAddon::getPluralStringValueSync0([[maybe_unused]] ani_env* env, [[maybe_unused]]
    ani_object object, ani_double resId, ani_double num)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    auto resMgr = unwrapAddon(env, object);
    dataContext->resId_ = resId;
    dataContext->param_ = num;
    RState state = resMgr->GetFormatPluralStringById(dataContext->value_,
        resId, dataContext->param_, dataContext->jsParams_);
    ani_string ret;
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getPluralStringValueSync0 state", true, state);
        std::string str = "Failed";
        env->String_NewUTF8(str.c_str(), str.size(), &ret);
        return ret;
    }
    auto str = dataContext->value_;
    env->String_NewUTF8(str.c_str(), str.size(), &ret);
    return ret;
}

ani_string ResMgrAddon::getPluralStringValueSync1([[maybe_unused]] ani_env* env, [[maybe_unused]]
    ani_object object, ani_object resource, ani_double num)
{
    std::unique_ptr<ResMgrDataContext> dataContext = std::make_unique<ResMgrDataContext>();
    InitIdResourceAddon(env, object, dataContext, resource);
    dataContext->param_ = num;
    auto resMgr = unwrapAddon(env, object);
    RState state = resMgr->GetFormatPluralStringById(dataContext->value_,
        dataContext->resource_->id, dataContext->param_, dataContext->jsParams_);
    ani_string ret;
    if (state != RState::SUCCESS) {
        dataContext->SetErrorMsg("Failed to getPluralStringValueSync1 state", true, state);
        std::string str = "Failed";
        env->String_NewUTF8(str.c_str(), str.size(), &ret);
        return ret;
    }
    auto str = dataContext->value_;
    env->String_NewUTF8(str.c_str(), str.size(), &ret);
    return ret;
}

ani_object ResMgrAddon::create([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_class clazz)
{
    auto nativeResMgr = new ResourceManagerImpl();
    nativeResMgr->Init();
    static const char* className = "L@ohos/resourceManager/resourceManager/ResourceManagerInner;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        std::cerr << "Not found '" << className << "'" << std::endl;
        ani_object nullobj = nullptr;
        return nullobj;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "J:V", &ctor)) {
        std::cerr << "get ctor Failed'" << className << "'" << std::endl;
        ani_object nullobj = nullptr;
        return nullobj;
    }
    ani_object resmgr_object;
    if (ANI_OK != env->Object_New(cls, ctor, &resmgr_object, reinterpret_cast<ani_long>(nativeResMgr))) {
        std::cerr << "New ResMgr Fail" << std::endl;
    }
    return resmgr_object;
}

ani_status ResMgrAddon::BindContext(ani_env* env)
{
    static const char* className = "L@ohos/resourceManager/resourceManager/ResourceManagerInner;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        std::cerr << "Not found '" << className << "'" << std::endl;
        return (ani_status)ANI_ERROR;
    }
    std::array methods = {
        ani_native_function { "create", ":L@ohos/resourceManager/resourceManager/ResourceManagerInner;",
            reinterpret_cast<void*>(create) },
        ani_native_function { "getStringSync", "D:Lstd/core/String;", reinterpret_cast<void*>(getStringSync0) },
        ani_native_function { "getStringSync", "DLescompat/Array;:Lstd/core/String;",
            reinterpret_cast<void *>(getStringSync1) },
        ani_native_function { "getStringSync", "Lglobal/resource/Resource;:Lstd/core/String;",
            reinterpret_cast<void*>(getStringSync2) },
        ani_native_function { "getStringSync", "Lglobal/resource/Resource;Lescompat/Array;:Lstd/core/String;",
            reinterpret_cast<void *>(getStringSync3) },
        ani_native_function { "getNumber", "D:D", reinterpret_cast<void*>(getNumber0) },
        ani_native_function { "getNumber", "Lglobal/resource/Resource;:D", reinterpret_cast<void*>(getNumber1) },
        ani_native_function { "getColorSync", "D:D", reinterpret_cast<void*>(getColorSync0) },
        ani_native_function { "getColorSync", "Lglobal/resource/Resource;:D", reinterpret_cast<void*>(getColorSync1) },
        ani_native_function { "getRawFileContentSync", "Lstd/core/String;:Lescompat/Uint8Array;",
            reinterpret_cast<void*>(getRawFileContentSync) },
        ani_native_function { "getPluralStringValueSync", "DD:Lstd/core/String;",
            reinterpret_cast<void*>(getPluralStringValueSync0) },
        ani_native_function { "getPluralStringValueSync", "Lglobal/resource/Resource;D:Lstd/core/String;",
            reinterpret_cast<void*>(getPluralStringValueSync1) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        std::cerr << "Cannot bind native methods to '" << className << "'" << std::endl;
        return (ani_status)ANI_ERROR;
    };
    
    static const char* nameSpaceName = "L@ohos/resourceManager/resourceManager;";
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(nameSpaceName, &ns)) {
        std::cerr << "Not found '" << nameSpaceName << "'" << std::endl;
        return (ani_status)ANI_ERROR;
    }
    std::array nsMethods = {
        ani_native_function { "getSystemResourceManager", ":L@ohos/resourceManager/resourceManager/ResourceManager;",
            reinterpret_cast<void*>(getSystemResourceManager) },
    };
    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, nsMethods.data(), nsMethods.size())) {
        std::cerr << "Cannot bind native methods to '" << nameSpaceName << "'" << std::endl;
        return (ani_status)ANI_ERROR;
    };
    return ANI_OK;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        std::cerr << "Unsupported ANI_VERSION_1" << std::endl;
        return (ani_status)ANI_ERROR;
    }

    auto status = ResMgrAddon::BindContext(env);
    if (status != ANI_OK) {
        return status;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}