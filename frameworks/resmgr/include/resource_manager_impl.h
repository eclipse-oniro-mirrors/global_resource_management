/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_RESOURCE_MANAGER_RESOURCEMANAGERIMPL_H
#define OHOS_RESOURCE_MANAGER_RESOURCEMANAGERIMPL_H

#include <map>
#include <string>
#include <vector>
#include "hap_manager.h"
#include "resource_manager.h"
#include "utils/psue_manager.h"
#include "theme_pack_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResourceManagerImpl : public ResourceManager {
public:
    ResourceManagerImpl(bool isOverrideResMgr = false);

    ~ResourceManagerImpl();

    /**
     * Init resource manager.
     *
     * @param isSystem indicate the resource manager is system or not.
     * @return true if init success, else false
     */
    bool Init(bool isSystem = false);

    /**
     * Init resource manager.
     *
     * @param hapManager hapManager.
     * @return true if init success, else false
     */
    bool Init(std::shared_ptr<HapManager> hapManager);

    /**
     * Add system resource to hap resource vector.
     *
     * @param systemResourceManager the system resource manager.
     */
    void AddSystemResource(ResourceManagerImpl *systemResourceManager);

    /**
     * Add resource path to hap paths
     * @param path the resource path
     * @return true if add resource path success, else false
     */
    virtual bool AddResource(const char *path, const uint32_t &selectedTypes = SELECT_ALL);

    /**
     * Add resource path to overlay paths
     * @param path the resource path
     * @param overlayPaths the exist overlay resource path
     * @return true if add resource path success, else false
     */
    virtual bool AddResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /**
     * Remove resource path to overlay paths
     * @param path the resource path
     * @param overlayPaths the exist overlay resource path
     * @return true if add resource path success, else false
     */
    virtual bool RemoveResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /**
     * Add the overlay resource for current application
     *
     * @param path the overlay resource path
     * @return true if add resource path success, else false
     */
    virtual bool AddAppOverlay(const std::string &path);

    /**
     * Remove the overlay resource for current application
     *
     * @param path the overlay resource path
     * @return true if add resource path success, else false
     */
    virtual bool RemoveAppOverlay(const std::string &path);

    /**
     * Update the resConfig
     * @param resConfig the resource config
     * @return SUCCESS if the resConfig updated success, else HAP_INIT_FAILED
     */
    virtual RState UpdateResConfig(ResConfig &resConfig, bool isUpdateTheme = false);

    /**
     * Get the resConfig
     * @param resConfig the resource config
     */
    virtual void GetResConfig(ResConfig &resConfig);

    /**
     * Get string resource by Id
     * @param id the resource Id
     * @param outValue the string resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringById(uint32_t id, std::string &outValue);

    /**
     * Get string by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringByName(const char *name, std::string &outValue);

    /**
     * Get string format by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringFormatById(std::string &outValue, uint32_t id, ...);

    /**
     * Get string format by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringFormatByName(std::string &outValue, const char *name, ...);

    /**
     * Get the STRINGARRAY resource by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringArrayById(uint32_t id, std::vector<std::string> &outValue);

    /**
     * Get the STRINGARRAY resource by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringArrayByName(const char *name, std::vector<std::string> &outValue);

    /**
     * Get the PATTERN resource by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPatternById(uint32_t id, std::map<std::string, std::string> &outValue);

    /**
     * Get the PATTERN resource by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPatternByName(const char *name, std::map<std::string, std::string> &outValue);

    /**
     * Get the plural string by resource id
     * @param id the resource id
     * @param quantity the language quantity
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringById(uint32_t id, int quantity, std::string &outValue);

    /**
     * Get the plural string by resource name
     * @param name the resource name
     * @param quantity the language quantity
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringByName(const char *name, int quantity, std::string &outValue);

    /**
     * Get the plural format string by resource id
     * @param outValue the resource write to
     * @param id the resource id
     * @param quantity the language quantity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...);

    /**
     * Get the plural format string by resource name
     * @param outValue the resource write to
     * @param id the resource id
     * @param quantity the language quantity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringByNameFormat(std::string &outValue, const char *name, int quantity, ...);

    /**
     * Get the THEME resource by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetThemeById(uint32_t id, std::map<std::string, std::string> &outValue);

    /**
     * Get the THEME resource by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetThemeByName(const char *name, std::map<std::string, std::string> &outValue);

    /**
     * Get the BOOLEAN resource by resource id
     * @param id the resource id
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetBooleanById(uint32_t id, bool &outValue);

    /**
     * Get the BOOLEAN resource by resource name
     * @param name the resource name
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetBooleanByName(const char *name, bool &outValue);

    /**
     * Get the INTEGER resource by resource id
     * @param id the resource id
     * @param outValue the obtain Integer value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetIntegerById(uint32_t id, int &outValue);

    /**
     * Get the INTEGER resource by resource name
     * @param name the resource name
     * @param outValue the obtain Integer value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetIntegerByName(const char *name, int &outValue);

    /**
     * Get the FLOAT resource by resource id
     * @param id the resource id
     * @param outValue the obtain float value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetFloatById(uint32_t id, float &outValue);

    /**
     * Get the FLOAT resource by resource id
     * @param id the resource id
     * @param outValue the obtain float value write to
     * @param unit the unit do not in parsing
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetFloatById(uint32_t id, float &outValue, std::string &unit);

    /**
     * Get the FLOAT resource by resource name
     * @param name the resource name
     * @param outValue the obtain float value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetFloatByName(const char *name, float &outValue);

    /**
     * Get the FLOAT resource by resource id
     * @param id the resource id
     * @param outValue the obtain float value write to
     * @param unit the string do not in parsing
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetFloatByName(const char *name, float &outValue, std::string &unit);

    /**
     * Get the INTARRAY resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource value convert to vector<int> write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetIntArrayById(uint32_t id, std::vector<int> &outValue);

    /**
     * Get the INTARRAY resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource value convert to vector<int> write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetIntArrayByName(const char *name, std::vector<int> &outValue);

    /**
     * Get the COLOR resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource value convert to uint32_t write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetColorById(uint32_t id, uint32_t &outValue);

    /**
     * Get the COLOR resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource value convert to uint32_t write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetColorByName(const char *name, uint32_t &outValue);

    /**
     * Get the SYMBOL resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource value convert to uint32_t write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetSymbolById(uint32_t id, uint32_t &outValue);

    /**
     * Get the SYMBOL resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource value convert to uint32_t write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetSymbolByName(const char *name, uint32_t &outValue);

    /**
     * Get the PROF resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetProfileById(uint32_t id, std::string &outValue);

    /**
     * Get the PROF resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetProfileByName(const char *name, std::string &outValue);

    /**
     * Get the MEDIA resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource path write to
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaById(uint32_t id, std::string &outValue, uint32_t density = 0);

    /**
     * Get the MEDIA resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource path write to
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaByName(const char *name, std::string &outValue, uint32_t density = 0);

    /**
     * Get the raw file path by resource name
     * @param name the resource name
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetRawFilePathByName(const std::string &name, std::string &outValue);

    /**
     * Get the rawFile descriptor by resource name
     * @param name the resource name
     * @param descriptor the obtain raw file member fd, length, offet write to
     * @return SUCCESS if resource exist, else ERROR
     */
    virtual RState GetRawFileDescriptor(const std::string &name, RawFileDescriptor &descriptor);

    /**
     * Get the rawFile descriptor by resource name
     * @param name the resource name
     * @param descriptor the obtain raw file member fd, length, offet write to
     * @return SUCCESS if resource exist, else ERROR
     */
    virtual RState GetRawFdNdkFromHap(const std::string &name, RawFileDescriptor &descriptor);

    /**
     * Close rawFile descriptor by resource name
     * @param name the resource name
     * @return SUCCESS if close the rawFile descriptor, else ERROR
     */
    virtual RState CloseRawFileDescriptor(const std::string &name);

    /**
     * Get all resource paths
     * @return The vector of resource paths
     */
    std::vector<std::string> GetResourcePaths();

    /**
     * Get the MEDIA data by resource id
     * @param id the resource id
     * @param len the data len write to
     * @param outValue the obtain resource path write to
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t density = 0);

    /**
     * Get the MEDIA data by resource name
     * @param name the resource name
     * @param len the data len write to
     * @param outValue the obtain resource path write to
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t density = 0);

    /**
     * Get the MEDIA base64 data resource by resource id
     * @param id the resource id
     * @param outValue the media base64 data
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaBase64DataById(uint32_t id, std::string &outValue, uint32_t density = 0);

    /**
     * Get the MEDIA base64 data resource by resource id
     * @param name the resource name
     * @param outValue the media base64 data
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaBase64DataByName(const char *name, std::string &outValue, uint32_t density = 0);

    /**
     * Get the PROF resource by resource id
     * @param name the resource id
     * @param len the data len write to
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetProfileDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the PROF resource by resource name
     * @param name the resource name
     * @param len the data len write to
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetProfileDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the rawFile base64 from hap by rawFile name
     * @param rawFileName the rawFile name
     * @param len the data len write to
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetRawFileFromHap(const std::string &rawFileName, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the rawFile Descriptor from hap by rawFile name
     * @param rawFileName the rawFile name
     * @param descriptor the raw file member fd, length, offet write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetRawFileDescriptorFromHap(const std::string &rawFileName, RawFileDescriptor &descriptor);

    /**
     * Is load hap
     * @param hapPath the hap path
     */
    virtual RState IsLoadHap(std::string &hapPath);

    /**
     * Get the raw file list
     * @param rawDirPath the rawfile directory path
     * @param rawfileList the rawfile list write to
     * @return SUCCESS if resource exist, else not found
     */
    virtual RState GetRawFileList(const std::string &rawDirPath, std::vector<std::string>& rawfileList);

    /**
     * Get the drawable information for given resId, mainly about type, len, buffer
     * @param id the resource id
     * @param type the drawable type
     * @param len the drawable buffer length
     * @param outValue the drawable buffer write to
     * @param density the drawable density
     * @return SUCCESS if resource exist, else not found
     */
    virtual RState GetDrawableInfoById(uint32_t id, std::string &type, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue, uint32_t density = 0);

    /**
     * Get the drawable information for given resName, mainly about type, len, buffer
     * @param name the resource Name
     * @param type the drawable type
     * @param len the drawable buffer length
     * @param outValue the drawable buffer write to
     * @param density the drawable density
     * @return SUCCESS if resource exist, else not found
     */
    virtual RState GetDrawableInfoByName(const char *name, std::string &type, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue, uint32_t density = 0);

    /**
     * Get string format by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @param jsParams the formatting string resource js parameters, the tuple first parameter represents the type,
     *     napi_number is denoted by NAPI_NUMBER, napi_string is denoted by NAPI_STRING,
     *     the tuple second parameter represents the value
     * @return SUCCESS if resource exists and was formatted successfully, else ERROR
     */
    virtual RState GetStringFormatById(uint32_t id, std::string &outValue,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);

    /**
     * Get string format by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @param jsParams the formatting string resource js parameters, the tuple first parameter represents the type,
     *     napi_number is denoted by NAPI_NUMBER, napi_string is denoted by NAPI_STRING,
     *     the tuple second parameter represents the value
     * @return SUCCESS if resource exists and was formatted successfully, else ERROR
     */
    virtual RState GetStringFormatByName(const char *name, std::string &outValue,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);

    /**
     * Get the resource limit keys value which every binary bit corresponds to existing limit key {@link KeyType},
     *     enum KeyType {
     *         LANGUAGES       = 0,
     *         REGION          = 1,
     *         SCREEN_DENSITY  = 2,
     *         DIRECTION       = 3,
     *         DEVICETYPE      = 4,
     *         SCRIPT          = 5,
     *         COLORMODE       = 6
     *         MCC             = 7,
     *         MNC             = 8,
     *         // RESERVER 9
     *         INPUTDEVICE     = 10,
     *         KEY_TYPE_MAX,
     *     }
     *
     * @return the resource limit keys, like if resource has LANGUAGES/REGION/DEVICETYPE, then the return value to
     *     binary bits is 0000010011
     */
    virtual uint32_t GetResourceLimitKeys();

    /**
     * Get the resource id by resType and resName
     *
     * @param resTypeName the resType and resName
     * @param resId the resId write to
     * @return SUCCESS if resource exist, else ERROR
     */
    virtual RState GetResId(const std::string &resTypeName, uint32_t &resId);

    /**
     * Get locale list
     *
     * @param outValue the locales write to, the locale string is divided into three parts: language,
     *     script (optional), and region (optional), concatenated by the connector (-).
     * @param includeSystem the parameter controls whether to include system resources,
     *     the default value is false, it has no effect when only system resources query the locales list.
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
    
    /**
     * Get the drawable information for given resName, mainly about type, len, buffer
     * @param name the resource Name
     * @param drawableInfo the drawable info
     * @param outValue the drawable buffer write to
     * @param iconType the drawable type
     * @param density the drawable density
     * @return SUCCESS if resource exist, else not found
     */
    virtual RState GetDrawableInfoByName(const char *name,
        std::tuple<std::string, size_t, std::string> &drawableInfo,
        std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density = 0);
    
    /**
     * Get the theme icon from theme pack
     *
     * @param resId the resource id
     * @param foregroundInfo the foreground info
     * @param backgroundInfo the background info
     * @param density the drawable density
     * @return SUCCESS if resource exist, else not found
     */
    virtual RState GetThemeIcons(uint32_t resId, std::pair<std::unique_ptr<uint8_t[]>, size_t> &foregroundInfo,
        std::pair<std::unique_ptr<uint8_t[]>, size_t> &backgroundInfo, uint32_t density = 0);

    /**
     * Get the theme mask
     *
     * @return the theme mask path, else empty
     */
    virtual std::string GetThemeMask();

    /**
     * Whether this raw resource is a directory
     *
     * @param pathName the raw resource path
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if raw resource is a directory, else not found
     */
    virtual RState IsRawDirFromHap(const std::string &pathName, bool &outValue);

    /**
     * Get override ResourceManager
     *
     * @param overrideResConfig the override resource config
     * @return ResourceManager if success, else nullptr
     */
    virtual std::shared_ptr<ResourceManager> GetOverrideResourceManager(std::shared_ptr<ResConfig> overrideResConfig);

    /**
     * Update the override resConfig
     * @param resConfig the override resource config
     * @return SUCCESS if the override resConfig updated success, else ERROR
     */
    virtual RState UpdateOverrideResConfig(ResConfig &resConfig);

    /**
     * Get the override resConfig
     * @param resConfig the override resource config
     */
    virtual void GetOverrideResConfig(ResConfig &resConfig);

    virtual RState GetDynamicIcon(const std::string &resName, std::pair<std::unique_ptr<uint8_t[]>, size_t> &iconInfo,
        uint32_t density = 0);

private:
    RState GetString(const std::shared_ptr<IdItem> idItem, std::string &outValue);

    RState GetStringArray(const std::shared_ptr<IdItem> idItem, std::vector<std::string> &outValue);

    RState GetPattern(const std::shared_ptr<IdItem> idItem, std::map<std::string, std::string> &outValue);

    RState GetTheme(const std::shared_ptr<IdItem> idItem, std::map<std::string, std::string> &outValue);

    RState GetPluralString(const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd, int quantity,
        std::string &outValue);

    RState ResolveReference(const std::string value, std::string &outValue);

    RState GetBoolean(const std::shared_ptr<IdItem> idItem, bool &outValue);

    RState ParseFloat(const std::string &strValue, float &result, std::string &unit);

    RState RecalculateFloat(const std::string &unit, float &result);

    RState GetFloat(const std::shared_ptr<IdItem> idItem, float &outValue, std::string &unit);

    RState GetInteger(const std::shared_ptr<IdItem> idItem, int &outValue);

    RState GetColor(const std::shared_ptr<IdItem> idItem, uint32_t &outValue);

    RState GetIntArray(const std::shared_ptr<IdItem> idItem, std::vector<int> &outValue);

    RState GetSymbol(const std::shared_ptr<IdItem> idItem, uint32_t &outValue);

    void ProcessPsuedoTranslate(std::string &outValue);

    RState ResolveParentReference(const std::shared_ptr<IdItem> idItem, std::map<std::string, std::string> &outValue);

    bool IsDensityValid(uint32_t density);

    bool IsFileExist(const std::string& path);

    RState GetThemeColor(const std::shared_ptr<IdItem> idItem, uint32_t &outValue);

    RState GetThemeFloat(const std::shared_ptr<IdItem> idItem, float &outValue);

    RState GetThemeFloat(const std::shared_ptr<IdItem> idItem, float &outValue, std::string &unit);

    RState GetThemeMedia(const std::shared_ptr<IdItem> idItem, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue, uint32_t density);

    RState GetThemeMediaBase64(const std::shared_ptr<IdItem> idItem, std::string &outValue);

    RState GetThemeDrawable(const std::shared_ptr<IdItem> idItem, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t iconType, uint32_t density);

    RState GetThemeIcon(const std::shared_ptr<IdItem> idItem, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
        uint32_t density);

    RState ProcessReference(const std::string value, std::vector<std::shared_ptr<IdItem>> &idItems);

    RState GetThemeIconInfo(const std::string &iconName, size_t &len, std::unique_ptr<uint8_t[]> &outValue);

    RState GetThemeValues(const std::string &value, std::string &outValue);

    RState UpdateFakeLocaleFlag(ResConfig &resConfig);

    std::shared_ptr<HapManager> GetHapManager();

    std::shared_ptr<HapManager> hapManager_;

    float fontRatio_ = 0.0f;

    bool isFakeLocale = false;

    bool isBidirectionFakeLocale = false;

    std::shared_ptr<PsueManager> psueManager_;

    const std::string VIRTUAL_PIXEL = "vp";

    const std::string FONT_SIZE_PIXEL = "fp";

    bool isOverrideResMgr_ = false;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif