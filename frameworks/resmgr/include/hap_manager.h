/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#ifndef OHOS_RESOURCE_MANAGER_HAPMANAGER_H
#define OHOS_RESOURCE_MANAGER_HAPMANAGER_H

#include "res_config_impl.h"
#include "hap_resource.h"
#include "hap_resource_manager.h"
#include "res_desc.h"
#include "resource_manager.h"
#include <shared_mutex>
#include <set>

#ifdef SUPPORT_GRAPHICS
#include <unicode/plurrule.h>
#endif

namespace OHOS {
namespace Global {
namespace Resource {
class HapManager {
public:
    /**
     * The constructor of HapManager
     * @param resConfig resource config
     * @param isSystem system resource flag, default value is false
     */
    HapManager(std::shared_ptr<ResConfigImpl> resConfig, bool isSystem = false);

    /**
     * The constructor of HapManager
     * @param resConfig resource config
     * @param hapResources hap resources
     * @param loadedHapPaths loaded hap paths
     * @param isSystem system resource flag, default value is false
    */
    HapManager(std::shared_ptr<ResConfigImpl> resConfig, std::vector<std::shared_ptr<HapResource>> hapResources,
        std::unordered_map<std::string, std::vector<std::string>> loadedHapPaths, bool isSystem = false);

    /**
     * The destructor of HapManager
     */
    ~HapManager();

    /**
     * Update the resConfig
     * @param resConfig the input new resource config
     * @return SUCCESS if the resConfig updated success, else HAP_INIT_FAILED
     */
    RState UpdateResConfig(ResConfig &resConfig);

    /**
     * Update the override resConfig
     * @param resConfig the input new override resource config
     * @return SUCCESS if the override resConfig updated success, else ERROR
     */
    RState UpdateOverrideResConfig(ResConfig &resConfig);

    /**
     * Get the resConfig
     * @param resConfig the output resource config
     */
    void GetResConfig(ResConfig &resConfig);

    /**
     * Get resConfig by id
     * @param resId the resource Id
     * @param resConfig the output resource config
     * @param isGetOverrideResource override resource flag, default value is false
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity,
     *      default value is SCREEN_DENSITY_NOT_SET
     * @return SUCCESS if resource exist, else ERROR_CODE_RES_NOT_FOUND_BY_ID
     */
    RState GetResConfigById(uint32_t resId, ResConfig &resConfig,
        bool isGetOverrideResource = false, uint32_t density = ScreenDensity::SCREEN_DENSITY_NOT_SET);

    /**
     * Get resConfig by name
     * @param name the resource name
     * @param type the resource type
     * @param resConfig the output resource config
     * @param isGetOverrideResource override resource flag, default value is false
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity,
     *      default value is SCREEN_DENSITY_NOT_SET
     * @return SUCCESS if resource exist, else ERROR_CODE_RES_NOT_FOUND_BY_NAME
     */
    RState GetResConfigByName(const std::string &name, const ResType type, ResConfig &resConfig,
        bool isGetOverrideResource = false, uint32_t density = ScreenDensity::SCREEN_DENSITY_NOT_SET);

    /**
     * Get the override resConfig
     * @param resConfig the output override resource config
     * @return SUCCESS if the override resConfig updated success, else ERROR
     */
    void GetOverrideResConfig(ResConfig &resConfig);

    /**
     * Add resource path to hap paths
     * @param path the resource path
     * @param selectedTypes the selected resource types to add, defined in res_common.h,
     *      multi types can be connected with "|" operator
     * @param forceReload force reload flag, default value is false
     * @return true if add resource path success, else false
     */
    bool AddResource(const char *path, const uint32_t &selectedTypes, bool forceReload = false);

    /**
     * Add patch resource to hap resource
     * @param path the hap resource path
     * @param patchPath the patch resource path
     * @return true if add patch resource path success, else false
     */
    bool AddPatchResource(const char *path, const char *patchPath);

    /**
     * Add resource path to overlay paths
     * @param path the resource path
     * @param overlayPaths the exist overlay resource path
     * @return true if add resource path success, else false
     */
    bool AddResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /**
     * Remove resource
     * @param path the resource path
     * @param overlayPaths the remove overlay resource path
     * @return true if remove resource success, else false
     */
    bool RemoveResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /**
     * Add the overlay resource for current application
     * @param path the overlay resource path
     * @return true if add resource path success, else false
     */
    bool AddAppOverlay(const std::string &path);

    /**
     * Remove the overlay resource for current application
     * @param path the overlay resource path
     * @return true if add resource path success, else false
     */
    bool RemoveAppOverlay(const std::string &path);

    /**
     * Obtain the hap path of the current application
     * @return the current application hap path
     */
    std::string GetValidAppPath();

    /**
     * Find resource by resource id
     * @param id the resource id
     * @param isGetOverrideResource get override resource flag, default value is false
     * @return the resources related to resource id
     */
    const std::shared_ptr<IdItem> FindResourceById(uint32_t id, bool isGetOverrideResource = false);

    /**
     * Find resource by resource name
     * @param name the resource name
     * @param resType the resource type, within the area of OHOS::Global::Resource::ResType
     * @param isGetOverrideResource get override resource flag, default value is false
     * @return the resources related to resource name
     */
    const std::shared_ptr<IdItem> FindResourceByName(
        const char *name, const ResType resType, bool isGetOverrideResource = false);

    /**
     * Find best resource path by resource id
     * @param id the resource id
     * @param isGetOverrideResource get override resource flag, default value is false
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity,
     *      default value is SCREEN_DENSITY_NOT_SET
     * @return the best resource path
     */
    const std::shared_ptr<ValueUnderQualifierDir> FindQualifierValueById(uint32_t id,
        bool isGetOverrideResource = false, uint32_t density = ScreenDensity::SCREEN_DENSITY_NOT_SET);
    
    /**
     * Find best resource path by resource name
     * @param name the resource name
     * @param resType the resource type, within the area of OHOS::Global::Resource::ResType
     * @param isGetOverrideResource get override resource flag, default value is false
     * @param density the screen density, within the area of OHOS::Global::Resource::ScreenDensity,
     *      default value is SCREEN_DENSITY_NOT_SET
     * @return the best resource path
     */
    const std::shared_ptr<ValueUnderQualifierDir> FindQualifierValueByName(const char *name,
        const ResType resType, bool isGetOverrideResource = false, uint32_t density = 0);

    /**
     * Find the raw file path
     * @param name the raw file name
     * @param outValue output raw file path
     * @return SUCCESS if find the raw file path success, else NOT_FOUND
     */
    RState FindRawFile(const std::string &name, std::string &outValue);

    /**
     * Get the language pluralRule related to quantity
     * @param quantity the language quantity
     * @param isGetOverrideResource get override resource flag, default value is false
     * @return the language pluralRule related to quantity
     */
    std::string GetPluralRulesAndSelect(ResourceManager::Quantity quantity, bool isGetOverrideResource = false);

    /**
     * Get resource paths vector
     * @return the resource paths
     */
    std::vector<std::string> GetResourcePaths();

    /**
     * Get the media data
     * @param qualifierDir the QualifierDir
     * @param len the output data len
     * @param outValue the output media data
     * @return SUCCESS if get the media data success, else NOT_FOUND
     */
    RState GetMediaData(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir, size_t& len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the mediabase64 data
     * @param qualifierDir the QualifierDir
     * @param outValue the output mediabase64 data
     * @return SUCCESS if get the mediabase64 data success, else NOT_FOUND
     */
    RState GetMediaBase64Data(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir, std::string &outValue);

    /**
     * Get the Profile data
     * @param qualifierDir the QualifierDir
     * @param len the output profile data len
     * @param outValue the output profile data
     * @return SUCCESS if get the profile data success, else NOT_FOUND
     */
    RState GetProfileData(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Find raw file from hap
     * @param rawFileName the raw file name
     * @param len the output rawfile data len
     * @param outValue the output rawfile data
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    RState FindRawFileFromHap(const std::string &rawFileName, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Find raw file descriptor from hap
     * @param rawFileName the raw file name
     * @param descriptor the output rawfile descriptor
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    RState FindRawFileDescriptorFromHap(const std::string &rawFileName,
        ResourceManager::RawFileDescriptor &descriptor);

    /**
     * Is load hap
     * @param hapPath the output hap path
     * @return true if has loaded hap, else false
     */
    bool IsLoadHap(std::string &hapPath);

    /**
     * Get the valid hapPath
     * @param hapPath the output hap path
     * @return OK if has loaded hap, else NOT_FOUND
     */
    int32_t GetValidHapPath(std::string &hapPath);

    /**
     * Get the valid resource.index path
     * @param indexPath the output resource.index path
     * @return OK if the resource.index path exist, else NOT_FOUND
     */
    int32_t GetValidResourceIndexPath(std::string &indexPath);

    /**
     * Get the raw file list
     * @param rawDirPath the rawfile directory path
     * @param rawfileList the output rawfile list
     * @return SUCCESS if resource exist, else not found
     */
    RState GetRawFileList(const std::string &rawDirPath, std::vector<std::string> &rawfileList);

    /**
     * Get the resource file path
     * @param qualifierDir the QualifierDir
     * @param resType the resource type
     * @param outValue the resource path write to
     * @return SUCCESS if resource exist, else not found
     */
    RState GetFilePath(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir, const ResType resType,
        std::string &outValue);

    /**
     * Get the media data from hap
     * @param qualifierDir the QualifierDir
     * @param len the output media data len
     * @param outValue the output media data
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaDataFromHap(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the media data from resource.index
     * @param qualifierDir the QualifierDir
     * @param len the output media data len
     * @param outValue the output media data
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaDataFromIndex(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the media base64 data from hap
     * @param qualifierDir the QualifierDir
     * @param outValue the output media base64 data
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaBase64DataFromHap(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir,
        std::string &outValue);

    /**
     * Get the media base64 data from resource.index
     * @param qualifierDir the QualifierDir
     * @param outValue the output media base64 data
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaBase64DataFromIndex(const std::shared_ptr<ValueUnderQualifierDir> qualifierDir,
        std::string &outValue);

    /**
     * Get the raw file descriptor
     * @param name the rawfile name
     * @param descriptor the output file descriptor
     * @return SUCCESS if resource exist, else not found
     */
    RState FindRawFileDescriptor(const std::string &name, ResourceManager::RawFileDescriptor &descriptor);

    /**
     * Close rawFile descriptor by resource name
     * @param name the rawfile name
     * @return SUCCESS if close the rawFile descriptor, else ERROR
     */
    RState CloseRawFileDescriptor(const std::string &name);

    /**
     * Get hap resources in hap manager
     * @return HapResource vector of manager
     */
    std::vector<std::shared_ptr<HapResource>> GetHapResource();

    /**
     * Get loaded hap path in hap manager
     * @return loaded path map in hap manager
     */
    std::unordered_map<std::string, std::vector<std::string>> GetLoadedHapPaths();

    /**
     * Add system resource
     * @param systemHapManager system resource hap manager
     */
    void AddSystemResource(const std::shared_ptr<HapManager> &systemHapManager);

    /**
     * Get the resource limit keys value which every binary bit corresponds to existing limit key
     * @return the resource limit keys
     */
    uint32_t GetResourceLimitKeys();

    /**
     * Get the raw file descriptor
     * @param rawFile the raw file name or relative path
     * @param descriptor the output raw file descriptor
     * @return SUCCESS if raw file exist, else ERROR
     */
    RState GetRawFd(const std::string &rawFile, ResourceManager::RawFileDescriptor &descriptor);

    /**
     * Get the resource id by resType and resName
     * @param resTypeName the resType and resName
     * @param resId the output resId
     * @return SUCCESS if resource exist, else ERROR
     */
    RState GetResId(const std::string &resTypeName, uint32_t &resId);

    /**
     * Get locale list
     * @param outValue the output locales, the locale string is divided into three parts: language,
     *     script (optional), and region (optional), concatenated by the connector (-).
     * @param includeSystem the parameter controls whether to include system resources,
     *     the default value is false, it has no effect when only system resources query the locales list.
     */
    void GetLocales(std::vector<std::string> &outValue, bool includeSystem = false);

    /**
     * Whether the raw resource is a directory
     * @param path the raw resource path
     * @param outValue the output resoult
     * @return SUCCESS if raw resource exist, else ERROR
     */
    RState IsRawDirFromHap(const std::string &pathName, bool &outValue);

    /**
     * Whether the theme system resource is enabled
     * @return true if theme system resource is enabled, else not.
     */
    bool IsThemeSystemResEnableHap();

    /**
     * Whether this is a system hap manager
     * @return true if is system hap manager, else not.
     */
    bool IsSystem();

    /**
     * Set override flag
     * @param isOverride override flag
     */
    inline void SetOverride(bool isOverride)
    {
        isOverride_ = isOverride;
    }

    /**
     * Update the app config for system resource manager
     *
     * @param isAppDarkRes true if app set drak res, else not.
     * @param isThemeSystemResEnable true if theme system resource is enabled, else not.
     */
    void UpdateAppConfigForSysResManager(bool isAppDarkRes, bool isThemeSystemResEnable);
private:
    void UpdateResConfigImpl(ResConfigImpl &resConfig);

    void GetResConfigImpl(ResConfigImpl &resConfig);

    std::vector<std::shared_ptr<IdValues>> GetResourceList(uint32_t ident) const;

    std::vector<std::shared_ptr<IdValues>> GetResourceListByName(const char *name,
        const ResType resType) const;

    void MatchBestResource(std::shared_ptr<ResConfigImpl> &bestResConfig,
        std::shared_ptr<ValueUnderQualifierDir> &result,
        const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &paths,
        uint32_t density, std::shared_ptr<ResConfigImpl> currentResConfig);

    const std::shared_ptr<ValueUnderQualifierDir> GetBestMatchResource(
        const std::vector<std::shared_ptr<IdValues>> &candidates,
        uint32_t density, bool isGetOverrideResource);
#if defined(__ARKUI_CROSS__)
    void RemoveHapResource(const std::string &path);
#endif
    static bool Init();

    std::shared_ptr<ResConfigImpl> getCompleteOverrideConfig(bool isGetOverrideResource);

    static bool icuInitialized;

    std::shared_ptr<ResConfigImpl> resConfig_;

    std::shared_ptr<ResConfigImpl> overrideResConfig_ = std::make_shared<ResConfigImpl>();

    std::vector<std::shared_ptr<HapResource>> hapResources_;

    std::unordered_map<std::string, std::vector<std::string>> loadedHapPaths_;

    std::unordered_map<std::string, ResourceManager::RawFileDescriptor> rawFileDescriptor_;
#ifdef SUPPORT_GRAPHICS
    // key is language
    std::vector<std::pair<std::string, icu::PluralRules *>> plurRulesCache_;
#endif
    std::shared_mutex mutex_;

    // indicate manager is system hap manager
    bool isSystem_;

    bool isOverride_{ false };

    bool isThemeSystemResEnable_{ false };

    bool isUpdateAppConfig_{ true };
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif