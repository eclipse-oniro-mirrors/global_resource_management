/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef RESOURCE_MANAGER_IMPL_EXT_H
#define RESOURCE_MANAGER_IMPL_EXT_H

#include "resource_manager.h"
#if defined(RESMGR_BROKER_ENABLE)
#include "resource_manager_broker_client.h"
#endif
namespace OHOS {
namespace Global {
namespace Resource {
class ResourceManagerImplExt : public ResourceManager {
public:
    ResourceManagerImplExt();

    ~ResourceManagerImplExt();

    bool Init(int32_t appType, std::string bundleName);

    /* *
     * Null function
     */
    virtual bool AddResource(const char *path, const uint32_t &selectedTypes = SELECT_ALL);

    /* *
     * Null function
     */
    virtual bool AddResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /* *
     * Null function
     */
    virtual bool RemoveResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /* *
     * Null function
     */
    virtual bool AddAppOverlay(const std::string &path);

    /* *
     * Null function
     */
    virtual bool RemoveAppOverlay(const std::string &path);

    /* *
     * Null function
     */
    virtual RState UpdateResConfig(ResConfig &resConfig, bool isUpdateTheme = false);

    /* *
     * Null function
     */
    virtual void GetResConfig(ResConfig &resConfig);

    /* *
     * Get string resource by Id
     * @param id the resource Id
     * @param outValue the string resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringById(uint32_t id, std::string &outValue);

    /* *
     * Null function
     */
    virtual RState GetStringByName(const char *name, std::string &outValue);

    /* *
     * Null function
     */
    virtual RState GetStringFormatById(std::string &outValue, uint32_t id, ...);

    /* *
     * Null function
     */
    virtual RState GetStringFormatByName(std::string &outValue, const char *name, ...);

    /* *
     * Null function
     */
    virtual RState GetStringArrayById(uint32_t id, std::vector<std::string> &outValue);

    /* *
     * Null function
     */
    virtual RState GetStringArrayByName(const char *name, std::vector<std::string> &outValue);

    /* *
     * Null function
     */
    virtual RState GetPatternById(uint32_t id, std::map<std::string, std::string> &outValue);

    /* *
     * Null function
     */
    virtual RState GetPatternByName(const char *name, std::map<std::string, std::string> &outValue);

    /* *
     * Null function
     */
    virtual RState GetPluralStringById(uint32_t id, int quantity, std::string &outValue);

    /* *
     * Null function
     */
    virtual RState GetPluralStringByName(const char *name, int quantity, std::string &outValue);

    /* *
     * Null function
     */
    virtual RState GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...);

    /* *
     * Null function
     */
    virtual RState GetPluralStringByNameFormat(std::string &outValue, const char *name, int quantity, ...);

    /* *
     * Null function
     */
    virtual RState GetThemeById(uint32_t id, std::map<std::string, std::string> &outValue);

    /* *
     * Null function
     */
    virtual RState GetThemeByName(const char *name, std::map<std::string, std::string> &outValue);

    /* *
     * Null function
     */
    virtual RState GetBooleanById(uint32_t id, bool &outValue);

    /* *
     * Null function
     *
     */
    virtual RState GetBooleanByName(const char *name, bool &outValue);

    /* *
     * Null function
     */
    virtual RState GetIntegerById(uint32_t id, int &outValue);

    /* *
     * Null function
     */
    virtual RState GetIntegerByName(const char *name, int &outValue);

    /* *
     * Null function
     */
    virtual RState GetFloatById(uint32_t id, float &outValue);

    /* *
     * Null function
     */
    virtual RState GetFloatById(uint32_t id, float &outValue, std::string &unit);

    /* *
     * Null function
     */
    virtual RState GetFloatByName(const char *name, float &outValue);

    /* *
     * Null function
     */
    virtual RState GetFloatByName(const char *name, float &outValue, std::string &unit);

    /* *
     * Null function
     */
    virtual RState GetIntArrayById(uint32_t id, std::vector<int> &outValue);

    /* *
     * Null function
     */
    virtual RState GetIntArrayByName(const char *name, std::vector<int> &outValue);

    /* *
     * Null function
     */
    virtual RState GetColorById(uint32_t id, uint32_t &outValue);

    /* *
     * Null function
     */
    virtual RState GetColorByName(const char *name, uint32_t &outValue);

    /* *
     * Null function
     */
    virtual RState GetProfileById(uint32_t id, std::string &outValue);

    /* *
     * Null function
     */
    virtual RState GetProfileByName(const char *name, std::string &outValue);

    /* *
     * Null function
     */
    virtual RState GetMediaById(uint32_t id, std::string &outValue, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetMediaByName(const char *name, std::string &outValue, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetRawFilePathByName(const std::string &name, std::string &outValue);

    /* *
     * Null function
     */
    virtual RState GetRawFileDescriptor(const std::string &name, RawFileDescriptor &descriptor);

    /* *
     * Null function
     */
    virtual RState CloseRawFileDescriptor(const std::string &name);

    /* *
     * Get the MEDIA data by resource id
     * @param id the resource id
     * @param len the data len write to
     * @param outValue the obtain resource path write to
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaDataById(
        uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetMediaDataByName(
        const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density = 0);

    /* *
     * Get the MEDIA base64 data resource by resource id
     * @param id the resource id
     * @param outValue the media base64 data
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaBase64DataById(uint32_t id, std::string &outValue, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetMediaBase64DataByName(const char *name, std::string &outValue, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetProfileDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue);

    /* *
     * Null function
     */
    virtual RState GetProfileDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue);

    /* *
     * Null function
     */
    virtual RState GetRawFileFromHap(const std::string &rawFileName, size_t &len, std::unique_ptr<uint8_t[]> &outValue);

    /* *
     * Null function
     */
    virtual RState GetRawFileDescriptorFromHap(const std::string &rawFileName, RawFileDescriptor &descriptor);

    /* *
     * Null function
     */
    virtual RState IsLoadHap(std::string &hapPath);

    /* *
     * Null function
     */
    virtual RState GetRawFileList(const std::string &rawDirPath, std::vector<std::string> &rawfileList);

    /* *
     * Get the drawable information for given resId, mainly about type, len, buffer
     * @param id the resource id
     * @param type the drawable type
     * @param len the drawable buffer length
     * @param outValue the drawable buffer write to
     * @param density the drawable density
     * @return SUCCESS if resource exist, else not found
     */
    virtual RState GetDrawableInfoById(
        uint32_t id, std::string &type, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetDrawableInfoByName(
        const char *name, std::string &type, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density = 0);

    /* *
     * Get string format by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @param jsParams the formatting string resource js parameters, the tuple first parameter represents the type,
     * napi_number is denoted by NAPI_NUMBER, napi_string is denoted by NAPI_STRING,
     * the tuple second parameter represents the value
     * @return SUCCESS if resource exists and was formatted successfully, else ERROR
     */
    virtual RState GetStringFormatById(uint32_t id, std::string &outValue,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);

    /* *
     * Null function
     */
    virtual RState GetStringFormatByName(const char *name, std::string &outValue,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);

    /* *
     * Null function
     */
    virtual uint32_t GetResourceLimitKeys();

    /* *
     * Null function
     */
    virtual RState GetRawFdNdkFromHap(const std::string &name, RawFileDescriptor &descriptor);

    /* *
     * Null function
     */
    virtual RState GetResId(const std::string &resTypeName, uint32_t &resId);

    /* *
     * Null function
     */
    virtual void GetLocales(std::vector<std::string> &outValue, bool includeSystem = false);

    /**
     * Get the drawable information for given resId, mainly about type, len, buffer
     * @param id the resource id
     * @param drawableInfo the drawable info
     * @param outValue the drawable buffer write to
     * @param iconType the drawable type
     * @param density the drawable density
     * @return SUCCESS if resource exist, else not found
     */
    virtual RState GetDrawableInfoById(uint32_t id,
        std::tuple<std::string, size_t, std::string> &drawableInfo,
        std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetDrawableInfoByName(const char *name,
        std::tuple<std::string, size_t, std::string> &drawableInfo,
        std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetSymbolById(uint32_t id, uint32_t &outValue);

    /* *
     * Null function
     */
    virtual RState GetSymbolByName(const char *name, uint32_t &outValue);

    /* *
     * Null function
     */
    virtual RState GetThemeIcons(uint32_t resId, std::pair<std::unique_ptr<uint8_t[]>, size_t> &foregroundInfo,
        std::pair<std::unique_ptr<uint8_t[]>, size_t> &backgroundInfo, uint32_t density = 0);

    /* *
     * Null function
     */
    virtual std::string GetThemeMask();

    /* *
     * Null function
     */
    virtual RState IsRawDirFromHap(const std::string &pathName, bool &outValue);

     /* *
     * Null function
     */
    virtual std::shared_ptr<ResourceManager> GetOverrideResourceManager(std::shared_ptr<ResConfig> overrideResConfig);

    /* *
     * Null function
     */
    virtual RState UpdateOverrideResConfig(ResConfig &resConfig);

    /* *
     * Null function
     */
    virtual void GetOverrideResConfig(ResConfig &resConfig);

    virtual RState GetDynamicIcon(const std::string &resName, std::pair<std::unique_ptr<uint8_t[]>, size_t> &iconInfo,
        uint32_t density = 0);

    /* *
     * Null function
     */
    virtual RState GetStringFormatById(std::string &outValue, uint32_t id, va_list args);

    /* *
     * Null function
     */
    virtual RState GetStringFormatByName(std::string &outValue, const char *name, va_list args);

    /* *
     * Null function
     */
    virtual RState GetFormatPluralStringById(std::string &outValue, uint32_t id, int quantity,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);
 
    /* *
     * Null function
     */
    virtual RState GetFormatPluralStringByName(std::string &outValue, const char *name, int quantity,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);

    /* *
     * Null function
     */
    virtual bool AddPatchResource(const char *path, const char *patchPath);

private:
#if defined(RESMGR_BROKER_ENABLE)
    std::shared_ptr<ANCO::ResourceManagerBrokerClient> client_;
#endif
};
} // namespace Resource
}  // namespace Global
}  // namespace OHOS
#endif