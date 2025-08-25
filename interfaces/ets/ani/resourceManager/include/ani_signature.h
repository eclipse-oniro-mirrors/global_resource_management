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

#ifndef RESOURCE_MANAGER_ANI_SIGN_H
#define RESOURCE_MANAGER_ANI_SIGN_H

namespace OHOS {
namespace Global {
namespace Resource {
class AniSignature {
public:

    static constexpr const char *ANI_ARRAY = "escompat.Array";
    static constexpr const char *ANI_STRING = "std.core.String";
    static constexpr const char *ANI_UINT8ARRAY = "escompat.Uint8Array";
    static constexpr const char *BUSINESSERROR = "@ohos.base.BusinessError";

    static constexpr const char *NAMESPACE = "@ohos.resourceManager.resourceManager";
    static constexpr const char *RESOURCE_MANAGER_INNER =
        "@ohos.resourceManager.resourceManager.ResourceManagerInner";
    static constexpr const char *RAWFILE_DESCRIPTOR_INNER = "global.rawFileDescriptorInner.RawFileDescriptorInner";
    static constexpr const char *CONFIGURATION = "@ohos.resourceManager.resourceManager.Configuration";
    static constexpr const char *DIRECTION = "@ohos.resourceManager.resourceManager.Direction";
    static constexpr const char *DEVICE_TYPE = "@ohos.resourceManager.resourceManager.DeviceType";
    static constexpr const char *SCREEN_DENSITY = "@ohos.resourceManager.resourceManager.ScreenDensity";
    static constexpr const char *COLOR_MODE = "@ohos.resourceManager.resourceManager.ColorMode";
    static constexpr const char *DEVICE_CAPABILITY = "@ohos.resourceManager.resourceManager.DeviceCapability";
};
}
}
}
#endif
