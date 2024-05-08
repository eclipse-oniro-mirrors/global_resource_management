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

#include "cj_ffi/cj_common_ffi.h"

extern "C" {
FFI_EXPORT int CJ_GetResourceManagerStageMode = 0;
FFI_EXPORT int CJ_CloseRawFd = 0;
FFI_EXPORT int CJ_GetRawFd = 0;
FFI_EXPORT int CJ_GetRawFileContent = 0;
FFI_EXPORT int CJ_GetRawFileList = 0;
FFI_EXPORT int CJ_GetPluralStringValue = 0;
FFI_EXPORT int CJ_GetPluralStringValueByResource = 0;
FFI_EXPORT int CJ_GetPluralStringByName = 0;
FFI_EXPORT int CJ_GetStringArrayValue = 0;
FFI_EXPORT int CJ_GetStringArrayValueByResource = 0;
FFI_EXPORT int CJ_GetStringArrayByName = 0;
FFI_EXPORT int CJ_GetString = 0;
FFI_EXPORT int CJ_GetStringByResource = 0;
FFI_EXPORT int CJ_GetStringByName = 0;
FFI_EXPORT int CJ_GetColorByName = 0;
FFI_EXPORT int CJ_GetColor = 0;
FFI_EXPORT int CJ_GetColorByResource = 0;
FFI_EXPORT int CJ_GetBoolean = 0;
FFI_EXPORT int CJ_GetBooleanByResource = 0;
FFI_EXPORT int CJ_GetBooleanByName = 0;
FFI_EXPORT int CJ_GetNumber = 0;
FFI_EXPORT int CJ_GetNumberByResource = 0;
FFI_EXPORT int CJ_GetNumberByName = 0;
FFI_EXPORT int CJ_GetConfiguration = 0;
FFI_EXPORT int CJ_GetDeviceCapability = 0;
FFI_EXPORT int CJ_GetMediaByName = 0;
FFI_EXPORT int CJ_GetMediaContent = 0;
FFI_EXPORT int CJ_GetMediaContentByResource = 0;
FFI_EXPORT int CJ_GetMediaContentBase64 = 0;
FFI_EXPORT int CJ_GetMediaContentBase64ByResource = 0;
FFI_EXPORT int CJ_GetMediaContentBase64ByName = 0;
FFI_EXPORT int CJ_GetDrawableDescriptor = 0;
FFI_EXPORT int CJ_GetDrawableDescriptorByResource = 0;
FFI_EXPORT int CJ_GetDrawableDescriptorByName = 0;
FFI_EXPORT int CJ_AddResource = 0;
FFI_EXPORT int CJ_RemoveResource = 0;
FFI_EXPORT int CJ_GetLocales = 0;
}