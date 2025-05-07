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
#ifndef RESOURCE_MANAGER_HAPRESOURCE_H
#define RESOURCE_MANAGER_HAPRESOURCE_H

#include <map>
#include <string>
#include <time.h>
#include <unordered_map>
#include <set>
#include "res_desc.h"
#include "res_config_impl.h"
#include <shared_mutex>

namespace OHOS {
namespace Global {
namespace Resource {
/**
 * HapResource describe a resource of hap zip file.
 *
 */
class HapResource {
public:
    /**
     * Creates an HapResource.
     *
     * @param path resources.index file path
     * @param defaultConfig  match defaultConfig to keys of index file, only parse the matched keys.
     *                       'null' means parse all keys.
     * @param isSystem If `isSystem` is true, the package is marked as a system package and allows some functions to
     *     filter out this package when computing what configurations/resources are available.
     * @param isOverlay If 'isOverlay' is true, the package is marked as an overlay package and overlay resource can
     *     replace non-overlay resource.
     * @return pResource if create pResource success, else nullptr
     */
    static const std::shared_ptr<HapResource> LoadFromIndex(
        const char *path, std::shared_ptr<ResConfigImpl> &defaultConfig,
        bool isSystem = false, bool isOverlay = false, const uint32_t &selectedTypes = SELECT_ALL);

    /**
     * Creates an HapResource.
     *
     * @param path hap file path
     * @param defaultConfig  match defaultConfig to keys of index file, only parse the matched keys.
     *     'null' means parse all keys.
     * @param isSystem If `isSystem` is true, the package is marked as a system package and allows some functions to
     *     filter out this package when computing what configurations/resources are available.
     * @param isOverlay If 'isOverlay' is true, the package is marked as an overlay package and overlay resource can
     *     replace non-overlay resource.
     * @return pResource if create pResource success, else nullptr
     */
    static const std::shared_ptr<HapResource> LoadFromHap(
        const char *path, std::shared_ptr<ResConfigImpl> defaultConfig,
        bool isSystem = false, bool isOverlay = false, const uint32_t &selectedTypes = SELECT_ALL);

    /**
     * Creates an HapResource.
     *
     * @param path hap file path
     * @param defaultConfig  match defaultConfig to keys of index file, only parse the matched keys.
     *     'null' means parse all keys.
     * @param isSystem If `isSystem` is true, the package is marked as a system package and allows some functions to
     *     filter out this package when computing what configurations/resources are available.
     * @param isOverlay If 'isOverlay' is true, the package is marked as an overlay package and overlay resource can
     *     replace non-overlay resource.
     * @return pResource if create pResource success, else nullptr
     */
    static const std::shared_ptr<HapResource> Load(
        const char* path, std::shared_ptr<ResConfigImpl> defaultConfig,
        bool isSystem = false, bool isOverlay = false, const uint32_t &selectedTypes = SELECT_ALL);

    /**
     * Load overlay resources
     * @param path the resources.index file path
     * @param overlayPath  the resource overlay path
     * @param defaultConfig the resource config
     * @param isSystem judge the overlay is system or not
     * @return the map of overlay resource path and resource info if success, else null
     */
    static const std::unordered_map<std::string, std::shared_ptr<HapResource>> LoadOverlays(
        const std::string &path, const std::vector<std::string> &overlayPath,
        std::shared_ptr<ResConfigImpl> defaultConfig, bool isSystem = false);

    /**
     * The destructor of HapResource
     */
    ~HapResource();

    /**
     * Get the resource.index file path
     */
    inline const std::string GetIndexPath() const
    {
        return indexPath_;
    }

    /**
     * Get the resource path
     */
    inline const std::string GetResourcePath() const
    {
        return resourcePath_;
    }

    /**
     * Get the system flag of HapResource.
     *
     * @return true if isSystem_ is true, false otherwise
     */
    inline bool IsSystemResource() const
    {
        return isSystem_;
    }

    /**
     * Get the overlay flag of HapResource.
     *
     * @return true if isOverlay_ is true, false otherwise
     */
    inline bool IsOverlayResource() const
    {
        return isOverlay_;
    }

    /**
     * Get the hqf resource path.
     */
    inline std::string GetPatchPath() const
    {
        return patchPath_;
    }

    /**
     * Get the hqf flag of HapResource.
     *
     * @return true if isPatch_ is true, false otherwise
     */
    inline bool IsPatch() const
    {
        return isPatch_;
    }

    /**
     * Set the hqf resource path.
     */
    inline void SetPatchPath(const std::string& patchPath)
    {
        patchPath_ = patchPath;
    }

    /**
     * Set the hqf flag of HapResource.
     */
    inline void SetIsPatch(bool isPatch)
    {
        isPatch_ = isPatch;
    }

    /**
     * Get the resource information
     */
    const std::vector<std::string> GetQualifiers();

    /**
     * Get resource last mod time.
     *
     * @return the resource last mod time.
     */
    inline time_t GetLastModTime()
    {
        return lastModTime_;
    }

    /**
     * Set resource last mod time.
     *
     * @param lastModTime resource last mod time
    */
    inline void SetLastModTime(time_t lastModTime)
    {
        lastModTime_ = lastModTime;
    }

    /**
     * Describe limitpath and value under the path
     */
    class ValueUnderQualifierDir {
    public:
        inline const std::shared_ptr<IdItem> GetIdItem() const
        {
            return idItem_;
        }

        inline const std::shared_ptr<ResConfigImpl> GetResConfig() const
        {
            return resConfig_;
        }

        inline bool IsOverlay() const
        {
            return isOverlay_;
        }

        inline bool IsSystemResource() const
        {
            return isSystemResource_;
        }

        inline const std::string GetIndexPath() const
        {
            return indexPath_;
        }

        inline const std::string GetResourcePath() const
        {
            return resourcePath_;
        }

        /**
         * The constructor of ValueUnderQualifierDir.
         *
         * @param resKey resKey, indicate every limit path item.
         * @param idItem idItem value, include type and value of id.
         * @param hapResource hapResource.
         * @param isOverlay the overlay flag, default value is false.
         * @param isSystemResource the system flag, default value is false.
         */
        ValueUnderQualifierDir(const std::shared_ptr<ResKey> resKey, const std::shared_ptr<IdItem> idItem,
            const std::pair<std::string, std::string> &resPath, bool isOverlay = false, bool isSystemResource = false);

        ~ValueUnderQualifierDir();

    private:

        /**
         * resConfig_ is to describe Qualifiers Sub-directory
         * it point to the ResKey resConfig_ and resConfig_ will be unified free in ResKey destruct.
         */
        std::shared_ptr<ResConfigImpl> resConfig_;

        // the value
        std::shared_ptr<IdItem> idItem_;

        friend class HapResource;

        std::string indexPath_;

        std::string resourcePath_;

        bool isOverlay_;

        bool isSystemResource_;
    };

    /**
     * describe value under different Qualifiers Sub-directories
     */
    class IdValues {
    public:
        inline void AddLimitPath(std::shared_ptr<ValueUnderQualifierDir> vuqd)
        {
            limitPaths_.push_back(vuqd);
        }

        inline const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &GetLimitPathsConst() const
        {
            return limitPaths_;
        }

        ~IdValues();

    private:
        // the folder desc
        std::vector<std::shared_ptr<ValueUnderQualifierDir>> limitPaths_;
    };

    /**
     * Get the resource value by resource id
     * @param id the resource id
     * @return the resource value related to id
     */
    const std::shared_ptr<IdValues> GetIdValues(const uint32_t id);

    /**
     * Get the resource value by resource name
     * @param name the resource name
     * @param resType the resource type
     * @return the resource value related to resource name
     */
    const std::shared_ptr<IdValues> GetIdValuesByName(const std::string name, const ResType resType);

    /**
     * Get the resource id by resource name
     * @param name the resource name
     * @param resType the resource type
     * @return the resource id related to resource name
     */
    int GetIdByName(const char *name, const ResType resType);

    RState UpdateResConfig(std::shared_ptr<ResConfigImpl> &defaultConfig);

    size_t IdSize() const
    {
        return idValuesMap_.size();
    }

    /**
     * Get the resource limit keys value which every binary bit corresponds to existing limit key {@link KeyType}
     *
     * @return the resource limit keys
     */
    uint32_t GetResourceLimitKeys();

    std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> BuildNameTypeIdMapping();

    /**
     * Get locale list
     *
     * @param outValue the locales write to, the locale string is divided into three parts: language,
     *     script (optional), and region (optional), concatenated by the connector (-).
     * @param includeSystem the parameter controls whether to include system resources,
     *     it has no effect when only system resources query the locales list.
     */
    void GetLocales(std::set<std::string> &outValue, bool includeSystem);

    HapResource(const std::string path, time_t lastModTime, std::shared_ptr<ResDesc> resDes,
        bool isSystem = false, bool isOverlay = false);

    bool IsThemeSystemResEnable() const;

    /**
     * Determine whether there are dark resource
     *
     * @return true if has dark resource, else false
     */
    bool HasDarkRes();
private:

    void UpdateOverlayInfo(std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> &nameTypeId);

    // must call Init() after constructor
    bool Init(std::shared_ptr<ResConfigImpl> defaultConfig);

    bool InitMap(const std::shared_ptr<ResKey> resKey, const std::pair<std::string, std::string> &resPath,
        std::shared_ptr<ResConfigImpl> defaultConfig);

    // step of Init(), called in Init()
    bool InitIdList(std::shared_ptr<ResConfigImpl> defaultConfig);

    void IsAppDarkRes(const std::shared_ptr<HapResource::ValueUnderQualifierDir> &limitPath,
        std::shared_ptr<ResConfigImpl> &defaultConfig);

    inline void SetLimitKeysValue(uint32_t limitKeyValue)
    {
        limitKeyValue_ = limitKeyValue;
    }

    inline void SetLocales(const std::set<std::string> &locales)
    {
        locales_ = locales;
    }

    inline void SetSelectedType(uint32_t type)
    {
        selectedTypes_ = type;
    }
    
    // resources.index file path
    const std::string indexPath_;

    // resource path , calculated from indexPath_
    std::string resourcePath_;

    // last mod time of hap file
    time_t lastModTime_;

    uint32_t selectedTypes_{SELECT_ALL};
    
    // resource information stored in resDesc_
    std::shared_ptr<ResDesc> resDesc_;

    std::shared_mutex mutex_;

    std::set<std::shared_ptr<ResConfigImpl>> loadedConfig_;

    std::map<uint32_t, std::shared_ptr<IdValues>> idValuesMap_;

    // the key is name, each restype holds one map
    // name may conflict in same restype !
    std::vector<std::shared_ptr<std::map<std::string, std::shared_ptr<IdValues>>>> idValuesNameMap_;

    // judge the hap resource is system or not.
    bool isSystem_;

    // judge the hap resource is overlay or not.
    bool isOverlay_;

    // judge the theme SystemRes is enabled or not.
    bool isThemeSystemResEnable_;

    // hqf resource path.
    std::string patchPath_;

    // judge the hqf is enabled or not.
    bool isPatch_ = false;

    // judge the resource is adapt dark mode or not.
    bool hasDarkRes_ = false;

    uint32_t limitKeyValue_ = 0;

    std::set<std::string> locales_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
