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

/**
 * @addtogroup resoucemanager
 *
 * @brief Provides the C interface corresponding to the js interface.
 *
 * @since 12
 */

/**
 * @file ohresmgr.h
 *
 * @brief Provides an implementation of the interface.
 *
 * @library libohresmgr.so librawfile.so
 * @since 12
 */
#ifndef GLOBAL_OHRESMGR_H
#define GLOBAL_OHRESMGR_H

#include "resmgr_common.h"
#include "raw_file_manager.h"
#include "interfaces/native/node/native_drawable_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Obtains the Base64 code of the image resource.
 *
 * Obtains the Base64 code of the image resource corresponding to the specified resource ID in callback mode.
 *
 * @param mgr Indicates the pointer to {@link NativeResourceManager}
 * {@link OH_ResourceManager_InitNativeResourceManager}.
 * @param resId Indicates the resource ID.
 * @param density The optional parameter ScreenDensity{@link ScreenDensity}, A value of 0 means
 * to use the density of current system dpi.
 * @param resultValue the result write to resultValue.
 * @param resultLen the media length write to resultLen.
 * @return { BusinessError } 401 - If the input parameter invalid. Possible causes: 1.Incorrect parameter types;
 * 2.Parameter verification failed.
           { BusinessError } 9001001 - Invalid resource ID.
           { BusinessError } 9001002 - No matching resource is found based on the resource ID.
 * @since 12
 */
ResourceManager_ErrorCode OH_ResourceManager_GetMediaBase64(const NativeResourceManager *mgr, uint32_t resId,
    char **resultValue, uint64_t *resultLen, uint32_t density = 0);

/**
 * @brief Obtains the Base64 code of the image resource.
 *
 * Obtains the Base64 code of the image resource corresponding to the specified resource Name in callback mode.
 *
 * @param mgr Indicates the pointer to {@link NativeResourceManager}
 * {@link OH_ResourceManager_InitNativeResourceManager}.
 * @param resName Indicates the resource name.
 * @param density The optional parameter ScreenDensity{@link ScreenDensity}, A value of 0 means
 * to use the density of current system dpi.
 * @param resultValue the result write to resultValue.
 * @param resultLen the media length write to resultLen.
 * @return { BusinessError } 401 - If the input parameter invalid. Possible causes: 1.Incorrect parameter types;
 * 2.Parameter verification failed.
           { BusinessError } 9001003 - Invalid resource name.
           { BusinessError } 9001004 - No matching resource is found based on the resource name.
 * @since 12
 */
ResourceManager_ErrorCode OH_ResourceManager_GetMediaBase64ByName(const NativeResourceManager *mgr,
    const char *resName, char **resultValue, uint64_t *resultLen, uint32_t density = 0);

/**
 * @brief Obtains the content of the image resource.
 *
 * Obtains the content of the specified screen density media file corresponding to a specified resource ID.
 *
 * @param mgr Indicates the pointer to {@link NativeResourceManager}
 * {@link OH_ResourceManager_InitNativeResourceManager}.
 * @param resId Indicates the resource ID.
 * @param density The optional parameter ScreenDensity{@link ScreenDensity}, A value of 0 means
 * to use the density of current system dpi.
 * @param resultValue the result write to resultValue.
 * @param resultLen the media length write to resultLen.
 * @return { BusinessError } 401 - If the input parameter invalid. Possible causes: 1.Incorrect parameter types;
 * 2.Parameter verification failed.
           { BusinessError } 9001001 - Invalid resource ID.
           { BusinessError } 9001002 - No matching resource is found based on the resource ID.
 * @since 12
 */
ResourceManager_ErrorCode OH_ResourceManager_GetMedia(const NativeResourceManager *mgr, uint32_t resId,
    uint8_t **resultValue, uint64_t *resultLen, uint32_t density = 0);

/**
 * @brief Obtains the content of the image resource.
 *
 * Obtains the content of the specified screen density media file corresponding to a specified resource Name.
 *
 * @param mgr Indicates the pointer to {@link NativeResourceManager}
 * {@link OH_ResourceManager_InitNativeResourceManager}.
 * @param resName Indicates the resource name.
 * @param density The optional parameter ScreenDensity{@link ScreenDensity}, A value of 0 means
 * to use the density of current system dpi.
 * @param resultValue the result write to resultValue.
 * @param resultLen the media length write to resultLen.
 * @return { BusinessError } 401 - If the input parameter invalid. Possible causes: 1.Incorrect parameter types;
 * 2.Parameter verification failed.
           { BusinessError } 9001003 - Invalid resource name.
           { BusinessError } 9001004 - No matching resource is found based on the resource name.
 * @since 12
 */
ResourceManager_ErrorCode OH_ResourceManager_GetMediaByName(const NativeResourceManager *mgr, const char *resName,
    uint8_t **resultValue, uint64_t *resultLen, uint32_t density = 0);

/**
 * @brief Obtains the DrawableDescriptor of the media file.
 *
 * Obtains the DrawableDescriptor of the media file corresponding to a specified resource ID.
 *
 * @param mgr Indicates the pointer to {@link NativeResourceManager}
 * {@link OH_ResourceManager_InitNativeResourceManager}.
 * @param resId Indicates the resource ID.
 * @param density The optional parameter ScreenDensity{@link ScreenDensity}, A value of 0 means
 * to use the density of current system dpi.
 * @param type The optional parameter means the media type, 0 means the normal media,
 * 1 means the the theme style media.
 * @param drawableDescriptor the result write to drawableDescriptor.
 * @return { BusinessError } 401 - If the input parameter invalid. Possible causes: 1.Incorrect parameter types;
 * 2.Parameter verification failed.
           { BusinessError } 9001001 - Invalid resource ID.
           { BusinessError } 9001002 - No matching resource is found based on the resource ID.
 * @since 12
 */
ResourceManager_ErrorCode OH_ResourceManager_GetDrawableDescriptor(const NativeResourceManager *mgr,
    uint32_t resId, ArkUI_DrawableDescriptor **drawableDescriptor, uint32_t density = 0, uint32_t type = 0);

/**
 * @brief Obtains the DrawableDescriptor of the media file.
 *
 * Obtains the DrawableDescriptor of the media file corresponding to a specified resource Name.
 * @param mgr Indicates the pointer to {@link NativeResourceManager}
 * {@link OH_ResourceManager_InitNativeResourceManager}.
 * @param resName Indicates the resource name.
 * @param density The optional parameter ScreenDensity{@link ScreenDensity}, A value of 0 means
 * to use the density of current system dpi.
 * @param type The optional parameter means the media type, 0 means the normal media,
 * 1 means the the theme style media, 2 means the theme dynamic media.
 * @param drawableDescriptor the result write to drawableDescriptor.
 * @return { BusinessError } 401 - If the input parameter invalid. Possible causes: 1.Incorrect parameter types;
 * 2.Parameter verification failed.
           { BusinessError } 9001003 - Invalid resource name.
           { BusinessError } 9001004 - No matching resource is found based on the resource name.
 * @since 12
 */
ResourceManager_ErrorCode OH_ResourceManager_GetDrawableDescriptorByName(const NativeResourceManager *mgr,
    char *resName, ArkUI_DrawableDescriptor **drawableDescriptor, uint32_t density = 0, uint32_t type = 0);

#ifdef __cplusplus
};
#endif

/** @} */
#endif // GLOBAL_OHRESMGR_H