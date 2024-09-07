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
#ifndef OHOS_RESOURCE_MANAGER_HAPMANAGER_H
#define OHOS_RESOURCE_MANAGER_HAPMANAGER_H

#include "res_config_impl.h"
#include "hap_resource.h"
#include "res_desc.h"
#include "resource_manager.h"
#include "lock.h"
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
     *
     * @param resConfig resource config
     * @param isSystem system flag, default value is false
     */
    HapManager(std::shared_ptr<ResConfigImpl> resConfig, bool isSystem = false);

    /**
     * The destructor of HapManager
     */
    ~HapManager();

    /**
     * Update the resConfig
     * @param resConfig the resource config
     * @return SUCCESS if the resConfig updated success, else HAP_INIT_FAILED
     */
    RState UpdateResConfig(ResConfig &resConfig);

    /**
     * Update the override resConfig
     * @param resConfig the override resource config
     * @return SUCCESS if the override resConfig updated success, else ERROR
     */
    RState UpdateOverrideResConfig(ResConfig &resConfig);

    /**
     * Get the resConfig
     * @param resConfig the resource config
     */
    void GetResConfig(ResConfig &resConfig);

    /**
     * Get the override resConfig
     * @param resConfig the override resource config
     * @return SUCCESS if the override resConfig updated success, else ERROR
     */
    void GetOverrideResConfig(ResConfig &resConfig);

    /**
     * Add resource path to hap paths
     * @param path the resource path
     * @return true if add resource path success, else false
     */
    bool AddResource(const char *path, const uint32_t &selectedTypes);

    /**
     * Add resource path to overlay paths
     * @param path the resource path
     * @param overlayPaths the exist overlay resource path
     * @return true if add resource path success, else false
     */
    bool AddResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /**
     * Remove resource path to overlay paths
     * @param path the resource path
     * @param overlayPaths the exist overlay resource path
     * @return true if add resource path success, else false
     */
    bool RemoveResource(const std::string &path, const std::vector<std::string> &overlayPaths);

    /**
     * Add the overlay resource for current application
     *
     * @param path the overlay resource path
     * @return true if add resource path success, else false
     */
    bool AddAppOverlay(const std::string &path);

    /**
     * Remove the overlay resource for current application
     *
     * @param path the overlay resource path
     * @return true if add resource path success, else false
     */
    bool RemoveAppOverlay(const std::string &path);

    /**
     * Obtain the hap path of the current application
     *
     * @return the current application hap path
     */
    std::string GetValidAppPath();

    /**
     * Find resource by resource id
     * @param id the resource id
     * @return the resources related to resource id
     */
    const std::shared_ptr<IdItem> FindResourceById(uint32_t id, bool isGetOverrideResource = false);

    /**
     * Find resource by resource name
     * @param name the resource name
     * @param resType the resource type
     * @return the resources related to resource name
     */
    const std::shared_ptr<IdItem> FindResourceByName(
        const char *name, const ResType resType, bool isGetOverrideResource = false);

    /**
     * Find best resource path by resource id
     * @param id the resource id
     * @param density the input screen density
     * @return the best resource path
     */
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> FindQualifierValueById(uint32_t id,
        bool isGetOverrideResource = false, uint32_t density = 0);
    
    /**
     * Find best resource path by resource name
     * @param name the resource name
     * @param resType the resource type
     * @param density the input screen density
     * @return the best resource path
     */
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> FindQualifierValueByName(const char *name,
        const ResType resType, bool isGetOverrideResource = false, uint32_t density = 0);

    /**
     * Find the raw file path
     * @param name the resource name
     * @param outValue raw file path
     * @return SUCCESS if find the raw file path success, else NOT_FOUND
     */
    RState FindRawFile(const std::string &name, std::string &outValue);

    /**
     * Get the language pluralRule related to quantity
     * @param quantity the language quantity
     * @return the language pluralRule related to quantity
     */
    std::string GetPluralRulesAndSelect(int quantity, bool isGetOverrideResource = false);

    /**
     * Get resource paths vector
     */
    std::vector<std::string> GetResourcePaths();

    /**
     * Get the media data
     * @param qd the QualifierDir
     * @param len the data len
     * @param outValue the media data
     * @return SUCCESS if get the media data success, else NOT_FOUND
     */
    RState GetMediaData(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd, size_t& len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the mediabase64 data
     * @param qd the QualifierDir
     * @param outValue the mediabase64 data
     * @return SUCCESS if get the mediabase64 data success, else NOT_FOUND
     */
    RState GetMediaBase64Data(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd, std::string &outValue);

    /**
     * Get the Profile data
     * @param qd the QualifierDir
     * @param len the data len write to
     * @param outValue the profile data
     * @return SUCCESS if get the profile data success, else NOT_FOUND
     */
    RState GetProfileData(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Find raw file from hap
     * @param rawFileName the rawFileName
     * @param len the data len write to
     * @param outValue the rawfile data
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    RState FindRawFileFromHap(const std::string &rawFileName, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Find raw file descriptor from hap
     * @param rawFileName the rawFileName
     * @param descriptor the rawfile descriptor
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    RState FindRawFileDescriptorFromHap(const std::string &rawFileName,
        ResourceManager::RawFileDescriptor &descriptor);

    /**
     * Is load hap
     * @param hapPath the hap path
     */
    bool IsLoadHap(std::string &hapPath);

    /**
     * Get the valid hapPath
     * @param hapPath the hap path
     * @return OK if the hapPath exist, else NOT_FOUND
     */
    int32_t GetValidHapPath(std::string &hapPath);

    /**
     * Get the valid resource.index path
     * @param hapPath the hap path
     * @return OK if the hapPath exist, else NOT_FOUND
     */
    int32_t GetValidIndexPath(std::string &indexPath);

    /**
     * Get the raw file list
     * @param rawDirPath the rawfile directory path
     * @param rawfileList the rawfile list write to
     * @return SUCCESS if resource exist, else not found
     */
    RState GetRawFileList(const std::string &rawDirPath, std::vector<std::string> &rawfileList);

    /**
     * Get the raw file list
     * @param qd the QualifierDir
     * @param resType the resource type
     * @param outValue the resource path write to
     * @return SUCCESS if resource exist, else not found
     */
    RState GetFilePath(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd, const ResType resType,
        std::string &outValue);

    /**
     * Get the raw file list
     * @param qd the QualifierDir
     * @param len the resource type
     * @param outValue the media data write to
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaDataFromHap(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the raw file list
     * @param qd the QualifierDir
     * @param len the resource type
     * @param outValue the media data write to
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaDataFromIndex(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd, size_t &len,
        std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the raw file list
     * @param qd the QualifierDir
     * @param outValue the mediabase64 data write to
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaBase64DataFromHap(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd,
        std::string &outValue);

    /**
     * Get the raw file list
     * @param qd the QualifierDir
     * @param outValue the mediabase64 data write to
     * @return SUCCESS if resource exist, else not found
     */
    RState GetMediaBase64DataFromIndex(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd,
        std::string &outValue);

    /**
     * Get the raw file list
     * @param name the rawfile name
     * @param descriptor the file descriptor write to
     * @return SUCCESS if resource exist, else not found
     */
    RState FindRawFileDescriptor(const std::string &name, ResourceManager::RawFileDescriptor &descriptor);

    /**
     * Close rawFile descriptor by resource name
     * @param name the resource name
     * @return SUCCESS if close the rawFile descriptor, else ERROR
     */
    RState CloseRawFileDescriptor(const std::string &name);

    /**
     * Get hap resource of manager
     *
     * @return HapResource vector of manager
     */
    std::vector<std::shared_ptr<HapResource>> GetHapResource();

    /**
     * Add system resourc to app resource vector
     *
     * @param systemHapManager system manager
     */
    void AddSystemResource(const std::shared_ptr<HapManager> &systemHapManager);

    /**
     * Get the resource limit keys value which every binary bit corresponds to existing limit key {@link KeyType}
     *
     * @return the resource limit keys
     */
    uint32_t GetResourceLimitKeys();

    /**
     * Get the rawFile descriptor by resource name
     *
     * @param rawFile the rawfile name or relative path
     * @param descriptor the obtain raw file member fd, length, offet write to
     * @return SUCCESS if resource exist, else ERROR
     */
    RState GetRawFd(const std::string &rawFile, ResourceManager::RawFileDescriptor &descriptor);

    /**
     * Get the resource id by resType and resName
     *
     * @param resTypeName the resType and resName
     * @param resId the resId write to
     * @return SUCCESS if resource exist, else ERROR
     */
    RState GetResId(const std::string &resTypeName, uint32_t &resId);

    /**
     * Get locale list
     *
     * @param outValue the locales write to, the locale string is divided into three parts: language,
     *     script (optional), and region (optional), concatenated by the connector (-).
     * @param includeSystem the parameter controls whether to include system resources,
     *     the default value is false, it has no effect when only system resources query the locales list.
     */
    void GetLocales(std::vector<std::string> &outValue, bool includeSystem = false);

    /**
     * Whether this raw resource is a directory
     *
     * @param path the raw resource path
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if raw resource is a directory, else ERROR
     */
    RState IsRawDirFromHap(const std::string &pathName, bool &outValue);

    /**
     * Whether this theme system resource is enabled
     *
     * @return true is enabled, else not.
     */
    bool IsThemeSystemResEnableHap() const;

private:
    void UpdateResConfigImpl(ResConfigImpl &resConfig);

    void GetResConfigImpl(ResConfigImpl &resConfig);

    std::vector<std::shared_ptr<HapResource::IdValues>> GetResourceList(uint32_t ident) const;

    std::vector<std::shared_ptr<HapResource::IdValues>> GetResourceListByName(const char *name,
        const ResType resType) const;

    void MatchBestResource(std::shared_ptr<ResConfigImpl> &bestResConfig,
        std::shared_ptr<HapResource::ValueUnderQualifierDir> &result,
        const std::vector<std::shared_ptr<HapResource::ValueUnderQualifierDir>> &paths,
        uint32_t density, std::shared_ptr<ResConfigImpl> currentResConfig);

    const std::shared_ptr<HapResource::ValueUnderQualifierDir> GetBestMatchResource(
        std::vector<std::shared_ptr<HapResource::IdValues>> candidates, uint32_t density, bool isGetOverrideResource);

    bool AddResourcePath(const char *path, const uint32_t &selectedTypes = SELECT_ALL);

    // when resConfig_ updated we must call ReloadAll()
    RState ReloadAll();

    static bool Init();

    std::shared_ptr<ResConfigImpl> getCompleteOverrideConfig(bool isGetOverrideResource);

    static bool icuInitialized;

    // app res config
    std::shared_ptr<ResConfigImpl> resConfig_;

    // app override res config
    std::shared_ptr<ResConfigImpl> overrideResConfig_ = std::make_shared<ResConfigImpl>();

    // set of hap Resources
    std::vector<std::shared_ptr<HapResource>> hapResources_;

    // set of loaded hap path
    std::unordered_map<std::string, std::vector<std::string>> loadedHapPaths_;

    std::unordered_map<std::string, ResourceManager::RawFileDescriptor> rawFileDescriptor_;

#ifdef SUPPORT_GRAPHICS
    // key is language
    std::vector<std::pair<std::string, icu::PluralRules *>> plurRulesCache_;
#endif

    Lock lock_;

    // indicate manager is system hap manager
    bool isSystem_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif