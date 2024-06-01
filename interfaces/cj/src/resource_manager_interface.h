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

#ifndef RESOURCE_MANAGER_INTERFACE_H
#define RESOURCE_MANAGER_INTERFACE_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace Resource {
struct Configuration {
    int32_t direction;
    char* locale;
};
struct DeviceCapability {
    int32_t screenDensity;
    int32_t deviceType;
};
class IResourceManager {
public:
    virtual int32_t CloseRawFd(const std::string &name) = 0;
    virtual int32_t GetRawFd(const std::string &rawFileName,
        Global::Resource::ResourceManager::RawFileDescriptor &descriptor) = 0;
    virtual int32_t GetRawFileContent(const std::string &name, size_t &len, std::unique_ptr<uint8_t[]> &outValue) = 0;
    virtual int32_t GetRawFileList(const std::string &rawDirPath, std::vector<std::string>& rawfileList) = 0;
    virtual int32_t GetColorByName(const char *name, uint32_t &outValue) = 0;
    virtual int32_t GetColorById(uint32_t id, uint32_t &outValue) = 0;
    virtual int32_t GetBooleanById(uint32_t id, bool &outValue) = 0;
    virtual int32_t GetBooleanByName(const char *name, bool &outValue) = 0;
    virtual int32_t GetIntegerById(uint32_t id, int &outValue) = 0;
    virtual int32_t GetIntegerByName(const char *name, int &outValue) = 0;
    virtual int32_t GetFloatById(uint32_t id, float &outValue) = 0;
    virtual int32_t GetFloatByName(const char *name, float &outValue) = 0;
    virtual void GetConfiguration(Configuration &cfg) = 0;
    virtual void GetDeviceCapability(DeviceCapability &deviceCapability) = 0;
    virtual int32_t GetMediaDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t density) = 0;
    virtual int32_t GetMediaDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t density) = 0;
    virtual int32_t GetPluralStringValue(uint32_t resId, int64_t num, std::string &outValue) = 0;
    virtual int32_t GetPluralStringValue(const char *name, int64_t num, std::string &outValue) = 0;
    virtual int32_t GetStringArrayValue(uint32_t resId, std::vector<std::string> &outValue) = 0;
    virtual int32_t GetStringArrayByName(const char *name, std::vector<std::string> &outValue) = 0;
    virtual int32_t GetString(uint32_t resId, std::string &outValue) = 0;
    virtual int32_t GetStringByName(const char *name, std::string &outValue) = 0;
    virtual int32_t AddResource(const char *path) = 0;
    virtual int32_t RemoveResource(const char *path) = 0;
    virtual int32_t GetMediaContentBase64ById(uint32_t id, std::string &outValue, uint32_t density) = 0;
    virtual int32_t GetMediaContentBase64ByName(const char *name, std::string &outValue, uint32_t density) = 0;
    virtual int32_t GetDrawableDescriptor(uint32_t id, int64_t &outValue, uint32_t density) = 0;
    virtual int32_t GetDrawableDescriptorByName(const char *name, int64_t &outValue, uint32_t density) = 0;
    virtual void GetLocales(bool includeSystem, std::vector<std::string> &outValue) = 0;
};
} // namespace Resource
} // namespace OHOS

#endif