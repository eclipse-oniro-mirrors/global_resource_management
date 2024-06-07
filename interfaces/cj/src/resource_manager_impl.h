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

#ifndef RESOURCE_MANAGER_IMPL_H
#define RESOURCE_MANAGER_IMPL_H

#include "ability_context_impl.h"
#include "resource_manager.h"
#include "ffi_remote_data.h"
#include "resource_manager_interface.h"
#include "drawable_descriptor/drawable_descriptor.h"
#include "uv.h"

#include <cstdint>
#include <string>

namespace OHOS::Resource {

class ResourceManagerImpl : public virtual IResourceManager, public OHOS::FFI::FFIData {
public:
    OHOS::FFI::RuntimeType *GetRuntimeType() override
    {
        return GetClassType();
    }
    explicit ResourceManagerImpl(OHOS::AbilityRuntime::Context* context);

    int32_t CloseRawFd(const std::string &name) override;

    int32_t GetRawFd(const std::string &rawFileName,
        Global::Resource::ResourceManager::RawFileDescriptor &descriptor) override;

    int32_t GetRawFileContent(const std::string &name, size_t &len, std::unique_ptr<uint8_t[]> &outValue) override;

    int32_t GetRawFileList(const std::string &rawDirPath, std::vector<std::string>& rawfileList) override;

    int32_t GetPluralStringValue(uint32_t resId, int64_t num, std::string &outValue) override;

    int32_t GetPluralStringValue(const char *name, int64_t num, std::string &outValue) override;

    int32_t GetStringArrayValue(uint32_t resId, std::vector<std::string> &outValue) override;

    int32_t GetStringArrayByName(const char *name, std::vector<std::string> &outValue) override;

    int32_t GetString(uint32_t resId, std::string &outValue) override;

    int32_t GetStringByName(const char *name, std::string &outValue) override;

    int32_t AddResource(const char *path) override;

    int32_t RemoveResource(const char *path) override;

    int32_t GetColorByName(const char *name, uint32_t &outValue) override;

    int32_t GetColorById(uint32_t id, uint32_t &outValue) override;

    int32_t GetBooleanById(uint32_t id, bool &outValue) override;

    int32_t GetBooleanByName(const char *name, bool &outValue) override;

    int32_t GetIntegerById(uint32_t id, int &outValue) override;

    int32_t GetIntegerByName(const char *name, int &outValue) override;

    int32_t GetFloatById(uint32_t id, float &outValue) override;

    int32_t GetFloatByName(const char *name, float &outValue) override;

    void GetConfiguration(Configuration &configuration) override;

    void GetDeviceCapability(DeviceCapability &deviceCapability) override;

    int32_t GetMediaDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t density) override;

    int32_t GetMediaDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t density) override;

    int32_t GetMediaContentBase64ById(uint32_t id, std::string &outValue, uint32_t density) override;

    int32_t GetMediaContentBase64ByName(const char *name, std::string &outValue, uint32_t density) override;

    int32_t GetDrawableDescriptor(uint32_t id, int64_t &outValue, uint32_t density) override;

    int32_t GetDrawableDescriptorByName(const char *name, int64_t &outValue, uint32_t density) override;

    void GetLocales(bool includeSystem, std::vector<std::string> &outValue) override;

    bool GetHapResourceManager(Global::Resource::ResourceManager::Resource resource,
        std::shared_ptr<Global::Resource::ResourceManager> &resMgr, int32_t &resId);
private:
    friend class OHOS::FFI::RuntimeType;
    friend class OHOS::FFI::TypeBase;
    static OHOS::FFI::RuntimeType *GetClassType()
    {
        static OHOS::FFI::RuntimeType runtimeType =
            OHOS::FFI::RuntimeType::Create<OHOS::FFI::FFIData>("ResourceManagerImpl");
        return &runtimeType;
    }
    std::shared_ptr<Global::Resource::ResourceManager> resMgr_;
    bool isSystem_ = false;
    std::string bundleName_;
    std::shared_ptr<AbilityRuntime::Context> context_;
    std::string GetLocale(std::unique_ptr<Global::Resource::ResConfig> &cfg);
};

class DrawableDescriptorImpl : public OHOS::FFI::FFIData {
public:
    OHOS::FFI::RuntimeType *GetRuntimeType() override
    {
        return GetClassType();
    }
    explicit DrawableDescriptorImpl(OHOS::Ace::Napi::DrawableDescriptor* drawableDescriptor)
        :drawableDescriptor_(std::shared_ptr<OHOS::Ace::Napi::DrawableDescriptor>(drawableDescriptor)){};
private:
    friend class OHOS::FFI::RuntimeType;
    friend class OHOS::FFI::TypeBase;
    static OHOS::FFI::RuntimeType *GetClassType()
    {
        static OHOS::FFI::RuntimeType runtimeType =
            OHOS::FFI::RuntimeType::Create<OHOS::FFI::FFIData>("DrawableDescriptorImpl");
        return &runtimeType;
    }
    std::shared_ptr<OHOS::Ace::Napi::DrawableDescriptor> drawableDescriptor_;
};

OHOS::Ace::Napi::DrawableDescriptor* GetDrawableDescriptorPtr(uint32_t id,
    std::shared_ptr<Global::Resource::ResourceManager> resMgr, uint32_t density, Global::Resource::RState &state);
char** g_vectorToCharPointer(std::vector<std::string>& vec);
} // namespace OHOS::Resource

#endif // RESOURCE_MANAGER_IMPL_H
