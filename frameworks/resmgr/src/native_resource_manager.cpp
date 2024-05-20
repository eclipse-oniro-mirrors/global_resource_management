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
#include "drawable_descriptor.h"
#include "hilog/log_cpp.h"
#include "resource_manager_impl.h"
#include "rstate.h"

using namespace OHOS::Global::Resource;
using namespace OHOS::HiviewDFX;
using namespace OHOS::Ace::Napi;

namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, 0xD001E00, "NativeResourceManager"};
}

struct NativeResourceManager {
    std::shared_ptr<ResourceManager> resManager = nullptr;
};

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
        HiLog::Error(LABEL, "failed get media base64 id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    *resultValue = (char*)malloc(tempResultValue.size() + 1);
    if (*resultValue == nullptr) {
        HiLog::Error(LABEL, "GetMediaBase64 malloc error");
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    if (strncpy_s(*resultValue, tempResultValue.size() + 1, tempResultValue.c_str(), tempResultValue.size()) != 0) {
        HiLog::Error(LABEL, "GetMediaBase64 strncpy_s error");
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
        HiLog::Error(LABEL, "failed get media base64 name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    *resultValue = (char*)malloc(tempResultValue.size() + 1);
    if (*resultValue == nullptr) {
        HiLog::Error(LABEL, "GetMediaBase64Byname malloc error");
        return ResourceManager_ErrorCode::ERROR_CODE_OUT_OF_MEMORY;
    }
    if (strncpy_s(*resultValue, tempResultValue.size() + 1, tempResultValue.c_str(), tempResultValue.size()) != 0) {
        HiLog::Error(LABEL, "GetMediaBase64Byname strncpy_s error");
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
        HiLog::Error(LABEL, "failed get media resource id = %{public}d, errorCode = %{public}d", resId, errorCode);
        return errorCode;
    }
    uint8_t *temPtr = tempResultValue.get();
    *resultValue = static_cast<uint8_t*>(malloc(len));
    if (*resultValue == nullptr) {
        HiLog::Error(LABEL, "GetMedia malloc error");
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
        HiLog::Error(LABEL, "failed get media resource name = %{public}s, errorCode = %{public}d", resName, errorCode);
        return errorCode;
    }
    uint8_t *temPtr = tempResultValue.get();
    *resultValue = static_cast<uint8_t*>(malloc(len));
    if (*resultValue == nullptr) {
        HiLog::Error(LABEL, "GetMediaByName malloc error");
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
        HiLog::Error(LABEL, "Failed to Create drawableDescriptor");
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
        HiLog::Error(LABEL, "Failed to Create drawableDescriptor");
        return static_cast<ResourceManager_ErrorCode>(state);
    }
    *drawableDescriptor = OH_ArkUI_CreateFromNapiDrawable(descriptor.get());
    return (*drawableDescriptor != nullptr) ? ResourceManager_ErrorCode::SUCCESS :
        ResourceManager_ErrorCode::ERROR_CODE_RES_NOT_FOUND_BY_ID;
}