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

#ifndef OHOS_RESOURCE_MANAGER_HAPRESOURCEV2_H
#define OHOS_RESOURCE_MANAGER_HAPRESOURCEV2_H

#include <atomic>

#include "hap_resource.h"
#include "mmap_file.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ValueUnderQualifierDirV2 : public ValueUnderQualifierDir {
public:
    static const uint32_t DATA_HEAD_LEN = 2;
    ValueUnderQualifierDirV2(const std::pair<std::string, std::string> &resPath, uint32_t offset,
        std::shared_ptr<ResConfigImpl> resConfig, bool isOverlay = false, bool isSystemResource = false);
    
    virtual ~ValueUnderQualifierDirV2();

    virtual std::shared_ptr<IdItem> GetIdItem() const;

    inline void Init(std::shared_ptr<MmapFile> mMapFile, ResType resType, uint32_t id, std::string name)
    {
        mMapFile_ = mMapFile;
        resType_ = resType;
        id_ = id;
        name_ = name;
    }
private:
    friend class HapResourceV2;

    uint32_t offset_;
    std::shared_ptr<MmapFile> mMapFile_;
    ResType resType_{VALUES};
    uint32_t id_{0};
    std::string name_;
};

class IdValuesV2 : public IdValues {
public:
    IdValuesV2(ResType resType, uint32_t id, uint32_t offset, const std::string &name);

    virtual ~IdValuesV2();

    virtual const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &GetLimitPathsConst() const;

    inline void SetMMap(std::shared_ptr<MmapFile> mMapFile)
    {
        mMapFile_ = mMapFile;
    }

    inline std::shared_ptr<MmapFile> GetMMap()
    {
        return mMapFile_;
    }

    inline uint32_t GetOffset() const
    {
        return offset_;
    }

    inline ResType GetResType() const
    {
        return resType_;
    }

    inline std::string GetName() const
    {
        return name_;
    }

    inline uint32_t GetId() const
    {
        return id_;
    }

    inline void SetId(uint32_t id)
    {
        id_ = id;
    }

    inline void ReserveLimitPaths(uint32_t size)
    {
        limitPaths_.reserve(size);
    }

    inline void AddLimitPath(std::shared_ptr<ValueUnderQualifierDirV2> vuqd)
    {
        limitPaths_.push_back(vuqd);
    }

    inline bool IsParsed()
    {
        return isParsed_.load();
    }

    inline void Parse()
    {
        isParsed_.store(true);
    }
private:
    // resource type
    ResType resType_;

    // resource id
    uint32_t id_;

    // offset from the beginning of the index file, pointing to the resource data
    uint32_t offset_;

    // resource name
    std::string name_;

    // resource.index file inform
    std::shared_ptr<MmapFile> mMapFile_;

    // if the IdValuesV2 has been parsed flag
    std::atomic<bool> isParsed_{false};

    // the folder desc
    std::vector<std::shared_ptr<ValueUnderQualifierDir>> limitPaths_;
};

class HapResourceV2 : public HapResource {
public:
    HapResourceV2(const std::string path, time_t lastModTime, bool hasDarkRes = false);

    virtual ~HapResourceV2();

    virtual bool IsSystemResource() const;

    virtual bool IsOverlayResource() const;

    virtual const std::shared_ptr<IdValues> GetIdValues(const uint32_t id);

    virtual const std::shared_ptr<IdValues> GetIdValuesByName(const std::string name, const ResType resType);

    virtual std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> BuildNameTypeIdMapping();

    virtual void GetLocales(std::set<std::string> &outValue, bool includeSystem);

    bool Init(std::unordered_map<uint32_t, std::shared_ptr<ResConfigImpl>> &keys,
        std::unordered_map<uint32_t, std::shared_ptr<IdValuesV2>> &idMap,
        std::unordered_map<uint32_t, std::unordered_map<std::string, std::shared_ptr<IdValuesV2>>> &typeNameMap,
        std::shared_ptr<MmapFile> mMapFile);

    void InitThemeSystemRes();
protected:
    friend class HapResourceManager;
    friend class HapParser;

    virtual int32_t ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue);
    
    std::mutex idValuesMutex_;

    std::shared_ptr<MmapFile> mMapFile_;

    // <resconfig id, resconfig>
    std::unordered_map<uint32_t, std::shared_ptr<ResConfigImpl>> keys_;

    // <resource id, resource>
    std::unordered_map<uint32_t, std::shared_ptr<IdValuesV2>> idMap_;

    // <type, <resource name, resource>>
    std::unordered_map<uint32_t, std::unordered_map<std::string, std::shared_ptr<IdValuesV2>>> typeNameMap_;
};

class SystemResource : virtual public HapResourceV2 {
public:
    SystemResource();
    SystemResource(const std::string path, time_t lastModTime, bool hasDarkRes = false);
    virtual ~SystemResource();
    virtual bool IsSystemResource() const;
    virtual void GetLocales(std::set<std::string> &outValue, bool includeSystem);
protected:
    virtual int32_t ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue);
};

class OverlayResource : virtual public HapResourceV2 {
public:
    OverlayResource();
    OverlayResource(const std::string path, time_t lastModTime, bool hasDarkRes = false);
    virtual ~OverlayResource();
    virtual bool IsOverlayResource() const;
    virtual void GetLocales(std::set<std::string> &outValue, bool includeSystem);
    virtual void UpdateOverlayInfo(std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> &nameTypeId);
protected:
    virtual int32_t ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue);
};

class SystemOverlayResource : public SystemResource, public OverlayResource {
public:
    SystemOverlayResource(const std::string path, time_t lastModTime, bool hasDarkRes = false);
    virtual ~SystemOverlayResource();
    virtual void GetLocales(std::set<std::string> &outValue, bool includeSystem);
protected:
    virtual int32_t ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue);
};
}
}
}
#endif
