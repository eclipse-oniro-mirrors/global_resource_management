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

#include "resource_manager_ffi.h"
#include <vector>
#include "utils.h"

using namespace OHOS::FFI;
using namespace OHOS::Global::Resource;

namespace OHOS {
namespace Resource {
extern "C" {
void ClearCharPointer(char** ptr, int count)
{
    for (int i = 0; i < count; i++) {
        free(ptr[i]);
    }
}

CArrString VectorToCArrString(std::vector<std::string>& vec, int32_t &code)
{
    CArrString ret = { .head = nullptr, .size = 0};
    if (vec.size() == 0) {
        return ret;
    }
    char** result = reinterpret_cast<char**>(malloc(sizeof(char*) * vec.size()));
    if (result == nullptr) {
        code = RState::NOT_ENOUGH_MEM;
        return ret;
    }
    for (size_t i = 0; i < vec.size(); i++) {
        result[i] = ::Utils::MallocCString(vec[i]);
        if (result[i] == nullptr) {
            ClearCharPointer(result, i);
            free(result);
            code = RState::NOT_ENOUGH_MEM;
            return ret;
        }
    }
    ret.head = result;
    ret.size = static_cast<int64_t>(vec.size());
    return ret;
}

int64_t CJ_GetResourceManagerStageMode(OHOS::AbilityRuntime::Context* context)
{
    if (context == nullptr) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    auto nativeResMgrLibrary = FFIData::Create<ResourceManagerImpl>(context);
    if (!nativeResMgrLibrary) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    return nativeResMgrLibrary->GetID();
}

RetDataI64 CJ_GetSystemResMgr()
{
    RetDataI64 ret = { .code = SUCCESS_CODE, .data = -1 };
    auto resMgr = FFIData::Create<ResourceManagerImpl>();
    if (resMgr->IsEmpty()) {
        ret.code = ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED;
        return ret;
    }
    ret.data = resMgr->GetID();
    return ret;
}

int32_t CJ_CloseRawFd(int64_t id, const char* path)
{
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->CloseRawFd(path);
}

int32_t CJ_GetRawFd(int64_t id, const char* rawFileName, ResourceManager::RawFileDescriptor &descriptor)
{
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->GetRawFd(rawFileName, descriptor);
}

void CopyUI8Arr(RetDataCArrUI8 &ret, std::unique_ptr<uint8_t[]> &data, size_t len)
{
    if (len <= 0) {
        LOGE("Wrong data size!")
        return;
    }
    uint8_t* outValue = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * len));
    if (outValue == nullptr) {
        ret.code = RState::ERROR;
        return;
    }
    for (size_t i = 0; i < len; ++i) {
        outValue[i] = data[i];
    }
    ret.data.size = static_cast<int64_t>(len);
    ret.data.head = outValue;
}

RetDataCArrUI8 CJ_GetRawFileContent(int64_t id, const char* path)
{
    LOGI("CJ_GetRawFileContent start");
    RetDataCArrUI8 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0} };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    int32_t state = instance->GetRawFileContent(path, len, data);
    ret.code = state;
    if (state == SUCCESS_CODE) {
        CopyUI8Arr(ret, data, len);
    }
    return ret;
}

RetDataCArrString CJ_GetRawFileList(int64_t id, const char* path)
{
    RetDataCArrString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0 } };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        return ret;
    }
    std::vector<std::string> value;
    ret.code = instance->GetRawFileList(path, value);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = VectorToCArrString(value, ret.code);
    return ret;
}

int32_t CJ_AddResource(int64_t id, const char *path)
{
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->AddResource(path);
}

int32_t CJ_RemoveResource(int64_t id, const char *path)
{
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->RemoveResource(path);
}

RetDataCString CJ_GetPluralStringValue(int64_t id, uint32_t resId, int64_t num)
{
    LOGI("CJ_GetPluralStringValue start");
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        return ret;
    }
    std::string value;
    ret.code = instance->GetPluralStringValue(resId, num, value);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataCString CJ_GetPluralStringValueByResource(int64_t id, CResource resource, int64_t num)
{
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetPluralStringValueByResource failed at GetHapResourceManager");
        ret.code = RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        return ret;
    }
    std::string value;
    ret.code = resMgr->GetPluralStringByIdFormat(value, resId, num, num);
    if (ret.code != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetPluralStringByIdFormat failed %{public}" PRIu32, ret.code);
        return ret;
    }
    LOGI("ResourceManagerImpl::GetPluralStringByIdFormat success");
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataCString CJ_GetPluralStringByName(int64_t id, const char *name, int64_t quantity)
{
    LOGI("CJ_GetPluralStringValue start");
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    std::string value;
    ret.code = instance->GetPluralStringValue(name, quantity, value);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataCArrString CJ_GetStringArrayValue(int64_t id, uint32_t resId)
{
    RetDataCArrString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0 } };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        return ret;
    }
    std::vector<std::string> value;
    ret.code = instance->GetStringArrayValue(resId, value);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = VectorToCArrString(value, ret.code);
    return ret;
}

RetDataCArrString CJ_GetStringArrayValueByResource(int64_t id, CResource resource)
{
    RetDataCArrString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0 } };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetStringArrayValueByResource failed at GetHapResourceManager");
        ret.code = RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        return ret;
    }
    std::vector<std::string> value;
    ret.code = resMgr->GetStringArrayById(resId, value);
    if (ret.code != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetStringArrayById failed %{public}" PRIu32, ret.code);
        return ret;
    }
    ret.data = VectorToCArrString(value, ret.code);
    return ret;
}

RetDataCArrString CJ_GetStringArrayByName(int64_t id, const char *name)
{
    LOGI("CJ_GetStringArrayByName start");
    RetDataCArrString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0 } };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    std::vector<std::string> value;
    ret.code = instance->GetStringArrayByName(name, value);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = VectorToCArrString(value, ret.code);
    return ret;
}

RetDataCString CJ_GetString(int64_t id, uint32_t resId)
{
    LOGI("CJ_GetString start");
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    std::string value;
    ret.code = instance->GetString(resId, value);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataCString CJ_GetStringByResource(int64_t id, CResource resource)
{
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetStringByResource failed at GetHapResourceManager");
        ret.code = RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        return ret;
    }
    std::string value;
    ret.code = resMgr->GetStringById(resId, value);
    if (ret.code != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetStringById failed %{public}" PRIu32, ret.code);
        return ret;
    }
    LOGI("ResourceManagerImpl::GetStringById success");
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataCString CJ_GetStringByName(int64_t id, const char* name)
{
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        ret.code = ERR_INVALID_INSTANCE_CODE;
        return ret;
    }
    std::string value;
    ret.code = instance->GetStringByName(name, value);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

int32_t CJ_GetColorByName(int64_t id, const char* name, uint32_t &data)
{
    LOGI("CJ_GetColorByName start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->GetColorByName(name, data);
}

int32_t CJ_GetColor(int64_t id, uint32_t resId, uint32_t &data)
{
    LOGI("CJ_GetColor start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->GetColorById(resId, data);
}

int32_t CJ_GetColorByResource(int64_t id, CResource resource, uint32_t &data)
{
    LOGI("CJ_GetColorByResource start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetColorByResource failed at GetHapResourceManager");
        return RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return resMgr->GetColorById(resId, data);
}

int32_t CJ_GetBoolean(int64_t id, uint32_t resId, bool &data)
{
    LOGI("CJ_GetBoolean start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->GetBooleanById(resId, data);
}

int32_t CJ_GetBooleanByResource(int64_t id, CResource resource, bool &data)
{
    LOGI("CJ_GetBoolean start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetBooleanByResource failed at GetHapResourceManager");
        return RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return resMgr->GetBooleanById(resId, data);
}

int32_t CJ_GetBooleanByName(int64_t id, const char* name, bool &data)
{
    LOGI("CJ_GetBooleanByName start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->GetBooleanByName(name, data);
}

int32_t CJ_GetNumber(int64_t id, uint32_t resId, RetGetNumber &data)
{
    LOGI("CJ_GetNumber start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    data.isInt = true;
    auto state = instance->GetIntegerById(resId, data.intValue);
    if (state != SUCCESS_CODE) {
        state = instance->GetFloatById(resId, data.floatValue);
        data.isInt = false;
    }
    return state;
}

int32_t CJ_GetNumberByResource(int64_t id, CResource resource, RetGetNumber &data)
{
    LOGI("CJ_GetNumber start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetNumberByResource failed at GetHapResourceManager");
        return RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    data.isInt = true;
    auto state = instance->GetIntegerById(resId, data.intValue);
    if (state != SUCCESS_CODE) {
        state = instance->GetFloatById(resId, data.floatValue);
        data.isInt = false;
    }
    return state;
}

int32_t CJ_GetNumberByName(int64_t id, const char* name, RetGetNumber &data)
{
    LOGI("CJ_GetNumberByName start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    data.isInt = true;
    auto state = instance->GetIntegerByName(name, data.intValue);
    if (state != SUCCESS_CODE) {
        state = instance->GetFloatByName(name, data.floatValue);
        data.isInt = false;
    }
    return state;
}

int32_t CJ_GetConfiguration(int64_t id, OHOS::Resource::Configuration &cfg)
{
    LOGI("CJ_GetConfiguration start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    instance->GetConfiguration(cfg);
    return SUCCESS_CODE;
}

int32_t CJ_GetDeviceCapability(int64_t id, OHOS::Resource::DeviceCapability &deviceCapability)
{
    LOGI("CJ_GetDeviceCapability start");
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    instance->GetDeviceCapability(deviceCapability);
    return SUCCESS_CODE;
}

RetDataCArrUI8 CJ_GetMediaByName(int64_t id, const char* resName, uint32_t density)
{
    LOGI("CJ_GetMediaByName start");
    RetDataCArrUI8 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0} };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    int32_t state = instance->GetMediaDataByName(resName, len, data, density);
    ret.code = state;
    if (state == SUCCESS_CODE) {
        CopyUI8Arr(ret, data, len);
    }
    return ret;
}

RetDataCArrUI8 CJ_GetMediaContent(int64_t id, uint32_t resId, uint32_t density)
{
    LOGI("CJ_GetMediaContent start");
    RetDataCArrUI8 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0} };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    int32_t state = instance->GetMediaDataById(resId, len, data, density);
    ret.code = state;
    if (state == SUCCESS_CODE) {
        CopyUI8Arr(ret, data, len);
    }
    return ret;
}

RetDataCArrUI8 CJ_GetMediaContentByResource(int64_t id, CResource resource, uint32_t density)
{
    LOGI("CJ_GetMediaContent start");
    RetDataCArrUI8 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0} };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetMediaContentByResource failed at GetHapResourceManager");
        ret.code = RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        return ret;
    }
    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    int32_t state = resMgr->GetMediaDataById(resId, len, data, density);
    ret.code = state;
    if (state == SUCCESS_CODE) {
        CopyUI8Arr(ret, data, len);
    }
    return ret;
}

RetDataCString CJ_GetMediaContentBase64(int64_t id, uint32_t resId, uint32_t density)
{
    LOGI("CJ_GetMediaContentBase64 start");
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    std::string value;
    ret.code = instance->GetMediaContentBase64ById(resId, value, density);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataCString CJ_GetMediaContentBase64ByResource(int64_t id, CResource resource, uint32_t density)
{
    LOGI("CJ_GetMediaContentBase64ByResource start");
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetMediaContentBase64ByResource failed at GetHapResourceManager");
        ret.code = RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        return ret;
    }
    std::string value;
    ret.code = resMgr->GetMediaBase64DataById(resId, value, density);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataCString CJ_GetMediaContentBase64ByName(int64_t id, const char* resName, uint32_t density)
{
    LOGI("CJ_GetMediaContentBase64 start");
    RetDataCString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = nullptr };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    std::string value;
    ret.code = instance->GetMediaContentBase64ByName(resName, value, density);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    ret.data = ::Utils::MallocCString(value);
    return ret;
}

RetDataI64 CJ_GetDrawableDescriptor(int64_t id, uint32_t resId, uint32_t density)
{
    LOGI("CJ_GetDrawableDescriptor start");
    RetDataI64 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = -1 };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    ret.code = instance->GetDrawableDescriptor(resId, ret.data, density);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    return ret;
}

RetDataI64 CJ_GetDrawableDescriptorByResource(int64_t id, CResource resource, uint32_t density)
{
    LOGI("CJ_GetDrawableDescriptor start");
    RetDataI64 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = -1 };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetDrawableDescriptorByResource failed at GetHapResourceManager");
        ret.code = RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        return ret;
    }
    RState state;
    auto drawableDescriptor = OHOS::Resource::GetDrawableDescriptorPtr(resId, resMgr, density, state);
    ret.code = state;
    if (ret.code != SUCCESS) {
        LOGE("Failed to Create drawableDescriptor by %{public}" PRIu32, resId);
        return ret;
    }
    auto ptr = FFIData::Create<DrawableDescriptorImpl>(drawableDescriptor);
    if (!ptr) {
        ret.code = ERR_INVALID_INSTANCE_CODE;
        return ret;
    }
    ret.data = ptr->GetID();
    return ret;
}

RetDataI64 CJ_GetDrawableDescriptorByName(int64_t id, const char* resName, uint32_t density)
{
    LOGI("CJ_GetDrawableDescriptorByName start");
    RetDataI64 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = -1 };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    ret.code = instance->GetDrawableDescriptorByName(resName, ret.data, density);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    return ret;
}

RetDataCArrString CJ_GetLocales(int64_t id, bool includeSystem)
{
    LOGI("CJ_GetLocales start");
    RetDataCArrString ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = { .head = nullptr, .size = 0 } };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    std::vector<std::string> value;
    instance->GetLocales(includeSystem, value);
    ret.code = RState::SUCCESS;
    ret.data = VectorToCArrString(value, ret.code);
    return ret;
}

RetDataU32 CJ_GetSymbol(int64_t id, uint32_t resId)
{
    LOGI("CJ_GetSymbol start");
    RetDataU32 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = -1 };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    ret.code = instance->GetSymbolById(resId, ret.data);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    return ret;
}

RetDataU32 CJ_GetSymbolByResource(int64_t id, CResource resource)
{
    LOGI("CJ_GetSymbol start");
    RetDataU32 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = -1 };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    OHOS::Global::Resource::ResourceManager::Resource res = { .bundleName = std::string(resource.bundleName),
        .moduleName = std::string(resource.moduleName), .id = resource.id };
    std::shared_ptr<ResourceManager> resMgr = nullptr;
    uint32_t resId = 0;
    if (!instance->GetHapResourceManager(res, resMgr, resId)) {
        LOGE("ResourceManager CJ_GetSymbolByResource failed at GetHapResourceManager");
        ret.code = RState::ERROR_CODE_RES_NOT_FOUND_BY_ID;
        return ret;
    }
    ret.code = resMgr->GetSymbolById(resId, ret.data);
    if (ret.code != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetSymbolByResource failed %{public}" PRIu32, ret.code);
        return ret;
    }
    LOGI("ResourceManagerImpl::GetSymbolByResource success")
    return ret;
}

RetDataU32 CJ_GetSymbolByName(int64_t id, const char* name)
{
    LOGI("CJ_GetSymbolByName start");
    RetDataU32 ret = { .code = ERR_INVALID_INSTANCE_CODE, .data = -1 };
    auto instance = FFIData::GetData<ResourceManagerImpl>(id);
    if (!instance) {
        LOGE("ResourceManager instance not exist %{public}" PRId64, id);
        return ret;
    }
    ret.code = instance->GetSymbolByName(name, ret.data);
    if (ret.code != RState::SUCCESS) {
        return ret;
    }
    return ret;
}
}
}
}