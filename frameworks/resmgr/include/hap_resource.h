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
#ifndef OHOS_RESOURCE_MANAGER_HAPRESOURCE_H
#define OHOS_RESOURCE_MANAGER_HAPRESOURCE_H

#include <map>
#include <set>
#include <shared_mutex>
#include <string>
#include <time.h>
#include <unordered_map>
#include "res_config_impl.h"
#include "res_desc.h"

namespace OHOS {
namespace Global {
namespace Resource {
/**
 * Describe limitpath and value under the path
 */
class ValueUnderQualifierDir {
public:
    ValueUnderQualifierDir(const std::pair<std::string, std::string> &resPath,
        std::shared_ptr<ResConfigImpl> resConfig, bool isOverlay = false, bool isSystemResource = false);

    ValueUnderQualifierDir(const std::pair<std::string, std::string> &resPath);

    virtual ~ValueUnderQualifierDir();

    virtual std::shared_ptr<IdItem> GetIdItem() const;

    inline const std::string GetIndexPath() const
    {
        return indexPath_;
    }

    inline const std::string GetResourcePath() const
    {
        return resourcePath_;
    }

    inline std::shared_ptr<ResConfigImpl> GetResConfig() const
    {
        return resConfig_;
    }

    inline bool IsOverlayResource() const
    {
        return isOverlay_;
    }

    inline bool IsSystemResource() const
    {
        return isSystemResource_;
    }
protected:
    friend class HapResource;

    std::string indexPath_;

    std::string resourcePath_;

    std::shared_ptr<ResConfigImpl> resConfig_;

    bool isOverlay_;

    bool isSystemResource_;
};

/**
 * describe value under different Qualifiers Sub-directories
 */
class IdValues {
public:
    virtual ~IdValues();
    virtual const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &GetLimitPathsConst() const = 0;
};

class HapResource {
public:
    HapResource(const std::string path, time_t lastModTime);

    virtual ~HapResource();

    /**
     * Get the system flag of HapResource.
     *
     * @return true if isSystem_ is true, false otherwise
     */
    virtual bool IsSystemResource() const;

    /**
     * Get the overlay flag of HapResource.
     *
     * @return true if isOverlay_ is true, false otherwise
     */
    virtual bool IsOverlayResource() const;

    /**
     * Get the resource value by resource id
     * @param id the resource id
     * @return the resource value related to id
     */
    virtual const std::shared_ptr<IdValues> GetIdValues(const uint32_t id);

    /**
     * Get the resource value by resource name
     * @param name the resource name
     * @param resType the resource type
     * @return the resource value related to resource name
     */
    virtual const std::shared_ptr<IdValues> GetIdValuesByName(const std::string name, const ResType resType);

    virtual RState Update(std::shared_ptr<ResConfigImpl> &defaultConfig);

    /**
     * Get the resource limit keys value which every binary bit corresponds to existing limit key {@link KeyType}
     *
     * @return the resource limit keys
     */
    virtual uint32_t GetLimitKeysValue();

    virtual std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> BuildNameTypeIdMapping();

    /**
     * Get locale list
     *
     * @param outValue the locales write to, the locale string is divided into three parts: language,
     *     script (optional), and region (optional), concatenated by the connector (-).
     * @param includeSystem the parameter controls whether to include system resources,
     *     it has no effect when only system resources query the locales list.
     */
    virtual void GetLocales(std::set<std::string> &outValue, bool includeSystem) {};

    virtual void UpdateOverlayInfo(std::unordered_map<std::string,
        std::unordered_map<ResType, uint32_t>> &nameTypeId) {};

    inline void SetLimitKeysValue(uint32_t limitKeyValue)
    {
        limitKeyValue_ = limitKeyValue;
    }

    /**
     * Get the resource.index file path
     */
    inline std::string GetIndexPath() const
    {
        return indexPath_;
    }

    /**
     * Get the resource path
     */
    inline std::string GetResourcePath() const
    {
        return resourcePath_;
    }

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

    inline bool IsThemeSystemResEnable() const
    {
        return isThemeSystemResEnable_;
    }

    /**
     * Determine whether there are dark resource
     *
     * @return true if has dark resource, else false
     */
    inline bool HasDarkRes() const
    {
        return hasDarkRes_;
    }

    /**
     * Set the hqf flag of HapResource.
     */
    inline void SetHasPatch(bool hasPatch)
    {
        hasPatch_ = hasPatch;
    }

    /**
     * Get the hqf flag of HapResource.
     *
     * @return true if hasPatch_ is true, false otherwise
     */
    inline bool HasPatch() const
    {
        return hasPatch_;
    }

    /**
     * Set the hqf resource path.
     */
    inline void SetPatchPath(const std::string& patchPath)
    {
        patchPath_ = patchPath;
    }

    /**
     * Get the hqf resource path.
     */
    inline std::string GetPatchPath() const
    {
        return patchPath_;
    }

    inline void SetLocales(const std::set<std::string> &locales)
    {
        locales_ = locales;
    }
protected:
    friend class HapResourceManager;
    friend class HapParser;
    
    std::shared_mutex mutex_;

    // resources.index file path
    const std::string indexPath_;

    // resource path , calculated from indexPath_
    std::string resourcePath_;

    // last mod time of hap file
    time_t lastModTime_;

    uint32_t limitKeyValue_{0};

    // hqf resource path.
    std::string patchPath_;

    std::set<std::string> locales_;

    // judge the theme SystemRes is enabled or not.
    bool isThemeSystemResEnable_{false};

    // judge the resource is adapt dark mode or not.
    bool hasDarkRes_{false};

    // judge the hqf is enabled or not.
    bool hasPatch_{false};
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
