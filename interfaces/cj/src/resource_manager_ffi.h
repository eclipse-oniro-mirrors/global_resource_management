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

#ifndef RESOURCE_MANAGER_FFI_H
#define RESOURCE_MANAGER_FFI_H

#include "ffi_remote_data.h"
#include "cj_common_ffi.h"
#include "resource_manager_log.h"
#include "resource_manager_impl.h"

#include <cstdint>

extern "C" {
    struct RetGetNumber {
        bool isInt;
        int intValue;
        float floatValue;
    };

    struct CResource {
        /** the hap bundle name */
        char* bundleName;

        /** the hap module name */
        char* moduleName;

        /** the resource id in hap */
        int32_t id;
    };

    FFI_EXPORT int64_t CJ_GetResourceManagerStageMode(OHOS::AbilityRuntime::Context* context);
    FFI_EXPORT int32_t CJ_CloseRawFd(int64_t id, const char* path);
    FFI_EXPORT int32_t CJ_GetRawFd(int64_t id, const char* rawFileName,
        OHOS::Global::Resource::ResourceManager::RawFileDescriptor &descriptor);
    FFI_EXPORT RetDataCArrUI8 CJ_GetRawFileContent(int64_t id, const char* path);
    FFI_EXPORT RetDataCArrString CJ_GetRawFileList(int64_t id, const char* path);
    FFI_EXPORT RetDataCString CJ_GetPluralStringValue(int64_t id, uint32_t resId, int64_t num);
    FFI_EXPORT RetDataCString CJ_GetPluralStringValueByResource(int64_t id, CResource resource, int64_t num);
    FFI_EXPORT RetDataCString CJ_GetPluralStringByName(int64_t id, const char* name, int64_t quantity);
    FFI_EXPORT RetDataCArrString CJ_GetStringArrayValue(int64_t id, uint32_t resId);
    FFI_EXPORT RetDataCArrString CJ_GetStringArrayValueByResource(int64_t id, CResource resource);
    FFI_EXPORT RetDataCArrString CJ_GetStringArrayByName(int64_t id, const char *name);
    FFI_EXPORT RetDataCString CJ_GetString(int64_t id, uint32_t resId);
    FFI_EXPORT RetDataCString CJ_GetStringByResource(int64_t id, CResource resource);
    FFI_EXPORT RetDataCString CJ_GetStringByName(int64_t id, const char* name);
    FFI_EXPORT int32_t CJ_GetColorByName(int64_t id, const char* name, uint32_t &data);
    FFI_EXPORT int32_t CJ_GetColor(int64_t id, uint32_t resId, uint32_t &data);
    FFI_EXPORT int32_t CJ_GetColorByResource(int64_t id, CResource resource, uint32_t &data);
    FFI_EXPORT int32_t CJ_GetBoolean(int64_t id, uint32_t resId, bool &data);
    FFI_EXPORT int32_t CJ_GetBooleanByResource(int64_t id, CResource resource, bool &data);
    FFI_EXPORT int32_t CJ_GetBooleanByName(int64_t id, const char* name, bool &data);
    FFI_EXPORT int32_t CJ_GetNumber(int64_t id, uint32_t resId, RetGetNumber &data);
    FFI_EXPORT int32_t CJ_GetNumberByResource(int64_t id, CResource resource, RetGetNumber &data);
    FFI_EXPORT int32_t CJ_GetNumberByName(int64_t id, const char* name, RetGetNumber &data);
    FFI_EXPORT int32_t CJ_GetConfiguration(int64_t id, OHOS::Resource::Configuration &cfg);
    FFI_EXPORT int32_t CJ_GetDeviceCapability(int64_t id, OHOS::Resource::DeviceCapability &deviceCapability);
    FFI_EXPORT RetDataCArrUI8 CJ_GetMediaByName(int64_t id, const char* resName, uint32_t density);
    FFI_EXPORT RetDataCArrUI8 CJ_GetMediaContent(int64_t id, uint32_t resId, uint32_t density);
    FFI_EXPORT RetDataCArrUI8 CJ_GetMediaContentByResource(int64_t id, CResource resource, uint32_t density);
    FFI_EXPORT RetDataCString CJ_GetMediaContentBase64(int64_t id, uint32_t resId, uint32_t density);
    FFI_EXPORT RetDataCString CJ_GetMediaContentBase64ByResource(int64_t id, CResource resource, uint32_t density);
    FFI_EXPORT RetDataCString CJ_GetMediaContentBase64ByName(int64_t id, const char* resName, uint32_t density);
    FFI_EXPORT RetDataI64 CJ_GetDrawableDescriptor(int64_t id, uint32_t resId, uint32_t density);
    FFI_EXPORT RetDataI64 CJ_GetDrawableDescriptorByResource(int64_t id, CResource resource, uint32_t density);
    FFI_EXPORT RetDataI64 CJ_GetDrawableDescriptorByName(int64_t id, const char* resName, uint32_t density);
    FFI_EXPORT int32_t CJ_AddResource(int64_t id, const char *path);
    FFI_EXPORT int32_t CJ_RemoveResource(int64_t id, const char *path);
    FFI_EXPORT RetDataCArrString CJ_GetLocales(int64_t id, bool includeSystem);
}

#endif