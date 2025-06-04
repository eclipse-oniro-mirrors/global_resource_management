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
#ifndef OHOS_RESOURCE_MANAGER_HAPRESOURCEV1_H
#define OHOS_RESOURCE_MANAGER_HAPRESOURCEV1_H

#include "hap_resource.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ValueUnderQualifierDirV1 : public ValueUnderQualifierDir {
public:
    ValueUnderQualifierDirV1(const std::shared_ptr<ResKey> &resKey, const std::shared_ptr<IdItem> &idItem,
        const std::pair<std::string, std::string> &resPath, bool isOverlay = false, bool isSystemResource = false);
    
    virtual ~ValueUnderQualifierDirV1();

    virtual std::shared_ptr<IdItem> GetIdItem() const;
private:
    friend class HapResourceV1;

    // the value
    std::shared_ptr<IdItem> idItem_;
};

class IdValuesV1 : public IdValues {
public:
    virtual ~IdValuesV1();

    virtual const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &GetLimitPathsConst() const;

    inline void AddLimitPath(std::shared_ptr<ValueUnderQualifierDirV1> vuqd)
    {
        limitPaths_.push_back(vuqd);
    }
private:
    // the folder desc
    std::vector<std::shared_ptr<ValueUnderQualifierDir>> limitPaths_;
};

/**
 * HapResource describe a resource of hap zip file.
 *
 */
class HapResourceV1 : public HapResource {
public:
    HapResourceV1(const std::string path, time_t lastModTime, std::shared_ptr<ResDesc> resDes,
        bool isSystem = false, bool isOverlay = false);
    virtual ~HapResourceV1();
    virtual bool IsSystemResource() const;
    virtual bool IsOverlayResource() const;
    virtual const std::shared_ptr<IdValues> GetIdValues(const uint32_t id);
    virtual const std::shared_ptr<IdValues> GetIdValuesByName(const std::string name, const ResType resType);
    virtual RState Update(std::shared_ptr<ResConfigImpl> &defaultConfig);
    virtual std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> BuildNameTypeIdMapping();
    virtual void GetLocales(std::set<std::string> &outValue, bool includeSystem);
    virtual void UpdateOverlayInfo(std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> &nameTypeId);

    bool Init(std::shared_ptr<ResConfigImpl> &defaultConfig);
    
    inline void SetSelectedType(uint32_t type)
    {
        selectedTypes_ = type;
    }
private:
    friend class HapResourceManager;
    friend class HapParser;

    void IsAppDarkRes(const std::shared_ptr<ValueUnderQualifierDir> &limitPath,
        std::shared_ptr<ResConfigImpl> &defaultConfig);
    
    RState UpdateResConfig(std::shared_ptr<ResConfigImpl> &defaultConfig);

    bool InitIdList(std::shared_ptr<ResConfigImpl> &defaultConfig);

    bool InitMap(const std::shared_ptr<ResKey> &resKey, const std::pair<std::string, std::string> &resPath,
        std::shared_ptr<ResConfigImpl> &defaultConfig);

    // resource information stored in resDesc_
    std::shared_ptr<ResDesc> resDesc_;

    std::set<std::shared_ptr<ResConfigImpl>> loadedConfig_;

    std::map<uint32_t, std::shared_ptr<IdValuesV1>> idValuesMap_;

    // the key is name, each restype holds one map
    // name may conflict in same restype !
    std::vector<std::shared_ptr<std::map<std::string, std::shared_ptr<IdValuesV1>>>> idValuesNameMap_;
    
    uint32_t selectedTypes_{SELECT_ALL};

    // judge the hap resource is system or not.
    bool isSystem_;

    // judge the hap resource is overlay or not.
    bool isOverlay_;
};
}
}
}
#endif
