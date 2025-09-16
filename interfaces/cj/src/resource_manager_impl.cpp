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
#include "cj_common_ffi.h"

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
    bundleName_ = context->GetBundleName();
    context_ = context->shared_from_this();
    isSystem_ = false;
    LOGI("ResourceManagerImpl::ResourceManagerImpl success");
}

ResourceManagerImpl::ResourceManagerImpl()
{
    std::shared_ptr<Global::Resource::ResourceManager> resMgr(Global::Resource::GetSystemResourceManager());
    resMgr_ = resMgr;
    context_ = nullptr;
    bundleName_ = "";
    isSystem_ = true;
}

ResourceManagerImpl::ResourceManagerImpl(std::string bundleName,
    std::shared_ptr<Global::Resource::ResourceManager> resMgr, std::shared_ptr<AbilityRuntime::Context> context)
    : resMgr_(resMgr), bundleName_(bundleName), context_(context)
{
    isOverride_ = true;
}

bool ResourceManagerImpl::IsEmpty()
{
    return resMgr_ == nullptr;
}

int32_t ResourceManagerImpl::CloseRawFd(const std::string &name)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->CloseRawFileDescriptor(name);
}

int32_t ResourceManagerImpl::GetRawFd(
    const std::string &rawFileName, Global::Resource::ResourceManager::RawFileDescriptor &descriptor)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetRawFileDescriptorFromHap(rawFileName, descriptor);
}

int32_t ResourceManagerImpl::GetRawFileContent(
    const std::string &name, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetRawFileFromHap(name, len, outValue);
}

int32_t ResourceManagerImpl::GetRawFileList(const std::string &rawDirPath, std::vector<std::string> &rawfileList)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetRawFileList(rawDirPath, rawfileList);
}

int32_t ResourceManagerImpl::GetColorByName(const char *name, uint32_t &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetColorByName(name, outValue);
}

int32_t ResourceManagerImpl::GetColorById(uint32_t id, uint32_t &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetColorById(id, outValue);
}

int32_t ResourceManagerImpl::GetPluralStringValue(uint32_t resId, int64_t num, std::string &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetPluralStringByIdFormat(outValue, resId, num, num);
}

int32_t ResourceManagerImpl::GetStringArrayValue(uint32_t resId, std::vector<std::string> &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetStringArrayById(resId, outValue);
}

int32_t ResourceManagerImpl::GetStringArrayByName(const char *name, std::vector<std::string> &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetStringArrayByName(name, outValue);
}

int32_t ResourceManagerImpl::GetString(uint32_t resId, std::string &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetStringById(resId, outValue);
}

int32_t ResourceManagerImpl::GetStringByName(const char *name, std::string &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetStringByName(name, outValue);
}

int32_t ResourceManagerImpl::GetPluralStringValue(const char *name, int64_t num, std::string &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetPluralStringByNameFormat(outValue, name, num, num);
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
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetBooleanById(id, outValue);
}

int32_t ResourceManagerImpl::GetBooleanByName(const char *name, bool &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetBooleanByName(name, outValue);
}

int32_t ResourceManagerImpl::GetIntegerById(uint32_t id, int &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetIntegerById(id, outValue);
}

int32_t ResourceManagerImpl::GetIntegerByName(const char *name, int &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetIntegerByName(name, outValue);
}

int32_t ResourceManagerImpl::GetFloatById(uint32_t id, float &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetFloatById(id, outValue);
}

int32_t ResourceManagerImpl::GetFloatByName(const char *name, float &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetFloatByName(name, outValue);
}

std::string GetLocale(std::unique_ptr<Global::Resource::ResConfig> &cfg)
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

void ResourceManagerImpl::GetConfiguration(Configuration &configuration)
{
    LOGI("ResourceManagerImpl::GetConfiguration start");
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        LOGE("Failed to create ResConfig object.");
        return;
    }
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

void ResConfigToConfigurationEx(std::unique_ptr<ResConfig> &config, ConfigurationEx *configuration)
{
    configuration->direction = static_cast<int32_t>(config->GetDirection());
    configuration->deviceType = static_cast<int32_t>(config->GetDeviceType());
    configuration->screenDensity = static_cast<int32_t>(config->GetScreenDensityDpi());
    configuration->colorMode = static_cast<int32_t>(config->GetColorMode());
    configuration->mcc = config->GetMcc();
    configuration->mnc = config->GetMnc();

    std::string locale = GetLocale(config);
    auto temp = ::Utils::MallocCString(locale);
    if (temp == nullptr) {
        return;
    }
    configuration->locale = temp;
    return;
}

void ResourceManagerImpl::GetConfiguration(ConfigurationEx *configuration)
{
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        LOGE("Failed to create ResConfig object.");
        return;
    }
    resMgr_->GetResConfig(*cfg);
    return ResConfigToConfigurationEx(cfg, configuration);
}

void ResourceManagerImpl::GetOverrideConfiguration(ConfigurationEx *configuration)
{
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        LOGE("Failed to create ResConfig object.");
        return;
    }
    resMgr_->GetOverrideResConfig(*cfg);
    return ResConfigToConfigurationEx(cfg, configuration);
}

void ResourceManagerImpl::GetDeviceCapability(DeviceCapability &deviceCapability)
{
    std::unique_ptr<ResConfig> cfg(CreateResConfig());
    if (!cfg) {
        LOGE("Failed to create ResConfig object.");
        return;
    }
    resMgr_->GetResConfig(*cfg);
    deviceCapability.screenDensity = static_cast<int32_t>(cfg->ConvertDensity(cfg->GetScreenDensity()));
    deviceCapability.deviceType = static_cast<int32_t>(cfg->GetDeviceType());
    LOGI("ResourceManagerImpl::GetDeviceCapability ok screenDensity %{public}" PRId32, deviceCapability.screenDensity);
}

int32_t ResourceManagerImpl::GetMediaDataByName(
    const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetMediaDataByName(name, len, outValue, density);
}

int32_t ResourceManagerImpl::GetMediaDataById(
    uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetMediaDataById(id, len, outValue, density);
}

int32_t ResourceManagerImpl::GetMediaContentBase64ById(uint32_t id, std::string &outValue, uint32_t density)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetMediaBase64DataById(id, outValue, density);
}

int32_t ResourceManagerImpl::GetMediaContentBase64ByName(const char *name, std::string &outValue, uint32_t density)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetMediaBase64DataByName(name, outValue, density);
}

int32_t ResourceManagerImpl::GetDrawableDescriptor(uint32_t id, int64_t &outValue, uint32_t density)
{
    RState state = SUCCESS;
    OHOS::Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor =
        OHOS::Ace::Napi::DrawableDescriptorFactory::Create(id, resMgr_, state, drawableType, density);
    if (state != SUCCESS) {
        LOGE("Failed to Create drawableDescriptor by %{public}" PRIu32, id);
        return state;
    }
    auto ptr = FFIData::Create<DrawableDescriptorImpl>(drawableDescriptor.release());
    if (!ptr) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    outValue = ptr->GetID();
    return state;
}

int32_t ResourceManagerImpl::GetDrawableDescriptorByName(const char *name, int64_t &outValue, uint32_t density)
{
    RState state = SUCCESS;
    OHOS::Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor =
        OHOS::Ace::Napi::DrawableDescriptorFactory::Create(name, resMgr_, state, drawableType, density);
    if (state != SUCCESS) {
        return state;
    }
    auto ptr = FFIData::Create<DrawableDescriptorImpl>(drawableDescriptor.release());
    if (!ptr) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    outValue = ptr->GetID();
    return state;
}

bool ResourceManagerImpl::GetHapResourceManager(Global::Resource::ResourceManager::Resource resource,
    std::shared_ptr<Global::Resource::ResourceManager> &resMgr, uint32_t &resId)
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

OHOS::Ace::Napi::DrawableDescriptor* GetDrawableDescriptorPtr(
    uint32_t id, std::shared_ptr<Global::Resource::ResourceManager> resMgr, uint32_t density, RState &state)
{
    OHOS::Ace::Napi::DrawableDescriptor::DrawableType drawableType;
    auto drawableDescriptor =
        OHOS::Ace::Napi::DrawableDescriptorFactory::Create(id, resMgr, state, drawableType, density);
    if (state != SUCCESS) {
        LOGE("Failed to Create drawableDescriptor by %{public}" PRIu32, id);
        return nullptr;
    }

    return drawableDescriptor.release();
}

void ResourceManagerImpl::GetLocales(bool includeSystem, std::vector<std::string> &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return;
    }
    return resMgr_->GetLocales(outValue, includeSystem);
}

int32_t ResourceManagerImpl::GetSymbolById(uint32_t id, uint32_t &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetSymbolById(id, outValue);
}

int32_t ResourceManagerImpl::GetSymbolByName(const char *name, uint32_t &outValue)
{
    if (IsEmpty()) {
        LOGE("Empty resource manager.");
        return ERR_INVALID_INSTANCE_CODE;
    }
    return resMgr_->GetSymbolByName(name, outValue);
}

std::shared_ptr<ResourceManager> ResourceManagerImpl::GetOverrideResMgr(ConfigurationEx &cfg, int32_t &errCode)
{
    std::shared_ptr<ResConfig> config(CreateDefaultResConfig());
    if (config == nullptr) {
        LOGE("GetOverrideResMgr, new config failed");
        errCode = ERROR_CODE_INVALID_INPUT_PARAMETER;
        return nullptr;
    }
    config->SetDirection(static_cast<Direction>(cfg.direction));
    config->SetDeviceType(static_cast<DeviceType>(cfg.deviceType));
    config->SetScreenDensityDpi(static_cast<ScreenDensity>(cfg.screenDensity));
    config->SetColorMode(static_cast<ColorMode>(cfg.colorMode));
    config->SetMcc(cfg.mcc);
    config->SetMnc(cfg.mnc);
#ifdef SUPPORT_GRAPHICS
    config->SetLocaleInfo(cfg.locale);
#endif
    std::shared_ptr<ResourceManager> overrideResMgr = resMgr_->GetOverrideResourceManager(config);
    if (overrideResMgr == nullptr) {
        errCode = ERROR_CODE_INVALID_INPUT_PARAMETER;
        return nullptr;
    }
    return overrideResMgr;
}

std::string ResourceManagerImpl::GetBundleName()
{
    return bundleName_;
}

std::shared_ptr<AbilityRuntime::Context> ResourceManagerImpl::GetContext()
{
    return context_;
}
}
