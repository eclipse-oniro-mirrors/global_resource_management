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

#include "resource_manager_impl.h"
#include "hitrace_meter.h"
#include "resource_manager.h"
#include "resource_manager_log.h"
#include "securec.h"
#include "utils.h"

using namespace OHOS::Global::Resource;
using namespace OHOS::Ace;
using namespace OHOS::FFI;

namespace OHOS::Resource {
std::map<std::string, std::shared_ptr<Global::Resource::ResourceManager>> g_resourceMgr;
std::mutex g_resMapLock;

ResourceManagerImpl::ResourceManagerImpl(OHOS::AbilityRuntime::Context* context)
{
    LOGI("ResourceManagerImpl::ResourceManagerImpl start");
    if (context == nullptr) {
        LOGE("Failed to get native context instance");
        return;
    }
    resMgr_ = context->GetResourceManager();
    context_ = std::shared_ptr<OHOS::AbilityRuntime::Context>(context);
    bundleName_ = context->GetBundleName();
    isSystem_ = false;
    LOGI("ResourceManagerImpl::ResourceManagerImpl success");
}

int32_t ResourceManagerImpl::CloseRawFd(const std::string &name)
{
    RState state = resMgr_->CloseRawFileDescriptor(name);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::CloseRawFd failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::CloseRawFd success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetRawFd(const std::string &rawFileName,
    Global::Resource::ResourceManager::RawFileDescriptor &descriptor)
{
    RState state = resMgr_->GetRawFileDescriptorFromHap(rawFileName, descriptor);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetRawFd failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetRawFd success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetRawFileContent(const std::string &name, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue)
{
    RState state = resMgr_->GetRawFileFromHap(name, len, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetRawFileContent failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetRawFileContent success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetRawFileList(const std::string &rawDirPath, std::vector<std::string>& rawfileList)
{
    RState state = resMgr_->GetRawFileList(rawDirPath, rawfileList);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetRawFileList failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetRawFileList success, list size %{public}" PRIu64,
            static_cast<uint64_t>(rawfileList.size()));
    }
    return state;
}

int32_t ResourceManagerImpl::GetColorByName(const char *name, uint32_t &outValue)
{
    RState state = resMgr_->GetColorByName(name, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetColorByName failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetColorByName success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetColorById(uint32_t id, uint32_t &outValue)
{
    RState state = resMgr_->GetColorById(id, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetColorById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetColorById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetPluralStringValue(uint32_t resId, int64_t num, std::string &outValue)
{
    RState state = resMgr_->GetPluralStringByIdFormat(outValue, resId, num, num);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetPluralStringByIdFormat failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetPluralStringByIdFormat success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetStringArrayValue(uint32_t resId, std::vector<std::string> &outValue)
{
    RState state = resMgr_->GetStringArrayById(resId, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetStringArrayById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetStringArrayById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetStringArrayByName(const char *name, std::vector<std::string> &outValue)
{
    RState state = resMgr_->GetStringArrayByName(name, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetStringArrayById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetStringArrayById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetString(uint32_t resId, std::string &outValue)
{
    RState state = resMgr_->GetStringById(resId, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetStringById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetStringById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetStringByName(const char *name, std::string &outValue)
{
    RState state = resMgr_->GetStringByName(name, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetStringById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetStringById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetPluralStringValue(const char *name, int64_t num, std::string &outValue)
{
    RState state = resMgr_->GetPluralStringByNameFormat(outValue, name, num, num);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetPluralStringByName failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetPluralStringByName success");
    }
    return state;
}

int32_t ResourceManagerImpl::AddResource(const char *path)
{
    bool state = resMgr_->AddAppOverlay(path);
    if (!state) {
        return RState::ERROR_CODE_OVERLAY_RES_PATH_INVALID;
    }
    return RState::SUCCESS;
}

int32_t ResourceManagerImpl::RemoveResource(const char *path)
{
    bool state = resMgr_->RemoveAppOverlay(path);
    if (!state) {
        return RState::ERROR_CODE_OVERLAY_RES_PATH_INVALID;
    }
    return RState::SUCCESS;
}

int32_t ResourceManagerImpl::GetBooleanById(uint32_t id, bool &outValue)
{
    RState state = resMgr_->GetBooleanById(id, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetBooleanById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetBooleanById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetBooleanByName(const char *name, bool &outValue)
{
    RState state = resMgr_->GetBooleanByName(name, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetBooleanByName failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetBooleanByName success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetIntegerById(uint32_t id, int &outValue)
{
    RState state = resMgr_->GetIntegerById(id, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetIntegerById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetIntegerById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetIntegerByName(const char *name, int &outValue)
{
    RState state = resMgr_->GetIntegerByName(name, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetIntegerByName failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetIntegerByName success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetFloatById(uint32_t id, float &outValue)
{
    RState state = resMgr_->GetFloatById(id, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetFloatById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetFloatById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetFloatByName(const char *name, float &outValue)
{
    RState state = resMgr_->GetFloatByName(name, outValue);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetFloatByName failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetFloatByName success");
    }
    return state;
}

void ResourceManagerImpl::GetConfiguration(Configuration &configuration)
{
    LOGI("ResourceManagerImpl::GetConfiguration start");
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    resMgr_->GetResConfig(*cfg);
    configuration.direction = static_cast<int32_t>(cfg->GetDirection());
    LOGI("ResourceManagerImpl::GetConfiguration ok %{public}" PRId32, configuration.direction);
    std::string locale = GetLocale(cfg);

    auto temp = ::Utils::MallocCString(locale);
    if (temp == nullptr) {
        return;
    }
    configuration.locale = temp;
}

void ResourceManagerImpl::GetDeviceCapability(DeviceCapability &deviceCapability)
{
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    resMgr_->GetResConfig(*cfg);
    deviceCapability.screenDensity = static_cast<int32_t>(cfg->ConvertDensity(cfg->GetScreenDensity()));
    deviceCapability.deviceType = static_cast<int32_t>(cfg->GetDeviceType());
    LOGI("ResourceManagerImpl::GetDeviceCapability ok screenDensity %{public}" PRId32,
        deviceCapability.screenDensity);
    LOGI("ResourceManagerImpl::GetDeviceCapability ok deviceType %{public}" PRId32, deviceCapability.deviceType);
}

int32_t ResourceManagerImpl::GetMediaDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    RState state = resMgr_->GetMediaDataByName(name, len, outValue, density);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetMediaDataByName failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetMediaDataByName success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetMediaDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    RState state = resMgr_->GetMediaDataById(id, len, outValue, density);
    if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetMediaDataById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetMediaDataById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetMediaContentBase64ById(uint32_t id, std::string &outValue, uint32_t density)
{
    RState state = resMgr_->GetMediaBase64DataById(id, outValue, density);
        if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetMediaDataById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetMediaDataById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetMediaContentBase64ByName(const char *name, std::string &outValue, uint32_t density)
{
    RState state = resMgr_->GetMediaBase64DataByName(name, outValue, density);
        if (state != RState::SUCCESS) {
        LOGE("ResourceManagerImpl::GetMediaDataById failed %{public}" PRIu32, state);
    } else {
        LOGI("ResourceManagerImpl::GetMediaDataById success");
    }
    return state;
}

int32_t ResourceManagerImpl::GetDrawableDescriptor(uint32_t id, int64_t &outValue, uint32_t density)
{
    RState state = SUCCESS;
    OHOS::Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor = OHOS::Ace::Napi::DrawableDescriptorFactory::Create(id, resMgr_,
        state, drawableType, density);
    if (state != SUCCESS) {
        LOGE("Failed to Create drawableDescriptor by %{public}" PRIu32, id);
        return state;
    }
    auto ptr = FFIData::Create<DrawableDescriptorImpl>(drawableDescriptor.release());
    outValue = ptr->GetID();
    return state;
}

int32_t ResourceManagerImpl::GetDrawableDescriptorByName(const char *name, int64_t &outValue, uint32_t density)
{
    RState state = SUCCESS;
    OHOS::Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor = OHOS::Ace::Napi::DrawableDescriptorFactory::Create(name, resMgr_,
        state, drawableType, density);
    if (state != SUCCESS) {
        return state;
    }
    auto ptr = FFIData::Create<DrawableDescriptorImpl>(drawableDescriptor.release());
    outValue = ptr->GetID();
    return state;
}

bool ResourceManagerImpl::GetHapResourceManager(Global::Resource::ResourceManager::Resource resource,
    std::shared_ptr<Global::Resource::ResourceManager> &resMgr, int32_t &resId)
{
    resId = resource.id;
    if (isSystem_) {
        resMgr = resMgr_;
        return true;
    }

    std::string key(resource.bundleName + "/" + resource.moduleName);
    std::lock_guard<std::mutex> lock(g_resMapLock);
    auto iter = g_resourceMgr.find(key);
    if (iter != g_resourceMgr.end()) {
        resMgr = g_resourceMgr[key];
        return true;
    }

    auto moduleContext = context_->CreateModuleContext(resource.bundleName, resource.moduleName);
    if (moduleContext == nullptr) {
        return false;
    }
    resMgr = moduleContext->GetResourceManager();
    g_resourceMgr[key] = resMgr;
    return true;
}

std::string ResourceManagerImpl::GetLocale(std::unique_ptr<Global::Resource::ResConfig> &cfg)
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

OHOS::Ace::Napi::DrawableDescriptor* GetDrawableDescriptorPtr(uint32_t id,
    std::shared_ptr<Global::Resource::ResourceManager> resMgr, uint32_t density, RState &state)
{
    OHOS::Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor = OHOS::Ace::Napi::DrawableDescriptorFactory::Create(id, resMgr, state, drawableType,
    density);
    if (state != SUCCESS) {
        LOGE("Failed to Create drawableDescriptor by %{public}" PRIu32, id);
        return nullptr;
    }

    return drawableDescriptor.release();
}

void ResourceManagerImpl::GetLocales(bool includeSystem, std::vector<std::string> &outValue)
{
    return resMgr_->GetLocales(outValue, includeSystem);
}
}
