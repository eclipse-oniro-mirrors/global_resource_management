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

#ifndef OHOS_RESOURCE_MANAGER_RESDESC_H
#define OHOS_RESOURCE_MANAGER_RESDESC_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "res_common.h"
#include "res_config_impl.h"

namespace OHOS {
namespace Global {
namespace Resource {
static constexpr uint32_t RES_VERSION_LEN = 128;

/**
 * old module resource.index file header
 */
class ResHeader {
public:
    static const uint32_t RES_HEADER_LEN = 136;

    // Type identifier for this chunk.  The meaning of this value depends
    // on the containing chunk.
    char version_[RES_VERSION_LEN];

    // Size of the resource.index file (in bytes).  Including header
    uint32_t length_;

    // determiner key count
    uint32_t keyCount_;
};

class IdItem {
public:
    static const uint32_t HEADER_LEN = 12;
    static const uint32_t SIZE_LEN = 2;
    static std::map<ResType, std::string> resTypeStrList;

    /**
     * Whether the resType is array or not
     * @param type the resType
     * @return true if the resType is array, else false
     */
    static bool IsArrayOfType(const ResType &type);

    /**
     * Judge the IdItem is array resource or not
     */
    void JudgeArray();

    /**
     * only theme and pattern may have parent
     * @return true when have parent
     */
    bool HaveParent() const;

    /**
     * Whether the std::string value is ref or not
     * ref start with '$' end with id
     * sample: "$string:16777225"
     * @param value
     * @param resType when return true, set resType. as sample : ResType:STRING
     * @param id      when return true, set id. as sample : 16777225
     * @return        true: value is ref
     */
    static bool IsRef(const std::string &value, ResType &resType, uint32_t &id);

    std::string ToString() const;

    uint32_t size_;
    ResType resType_;
    uint32_t id_;
    uint16_t valueLen_;
    bool isArray_ = false;
    std::string value_;
    std::vector<std::string> values_;
    std::string name_;
};

class IdParam {
public:
    ~IdParam();
    std::string ToString() const;

    uint32_t id_;
    uint32_t offset_;
    std::shared_ptr<IdItem> idItem_;
};

class ResId {
public:
    static const uint32_t RESID_HEADER_LEN = 8;
    static const uint32_t IDPARAM_HEADER_LEN = 8;

    std::string ToString() const;

    char tag_[4];
    uint32_t count_; // ID count
    std::vector<std::shared_ptr<IdParam>> idParams_;
};

/**
 * describe the qualifier
 */
class KeyParam {
public:
    static const uint32_t KEYPARAM_LEN = 8;

    // type of qualifier
    KeyType type_;

    // value of qualifiers
    uint32_t value_;

    // convert from value_
    std::string str_;

    void InitStr()
    {
        str_ = ConvertToStr();
    }

    const std::string &GetStr() const
    {
        return str_;
    }

    std::string ToString() const;

    std::string GetDeviceTypeStr() const;

private:
    const std::string ConvertToStr() const;
    std::string GetScreenDensityStr() const;
    std::string GetColorModeStr() const;
    std::string GetMccStr() const;
    std::string GetMncStr() const;
    std::string GetInputDeviceStr() const;
};

/**
 * a ResKey means a Qualifiers Sub-directories
 */
class ResKey {
public:
    static const uint32_t RESKEY_HEADER_LEN = 12;

    ~ResKey();

    std::string ToString() const;

    // always 'KEYS'
    char tag_[4];

    // offset from the beginning of the index file, pointing to the resource ID data block
    uint32_t offset_;

    // count of qualifiers
    uint32_t keyParamsCount_;

    // the resource ID data
    std::shared_ptr<ResId> resId_;

    // the resConfig of each ResKey and all resConfig_ in ValueUnderQualifierDir will point to this resConfig_
    std::shared_ptr<ResConfigImpl> resConfig_;
};

/**
 * a ResDesc means a index file in hap zip
 */
class ResDesc {
public:
    ResDesc();

    ~ResDesc();

    std::string ToString() const;

    std::vector<std::shared_ptr<ResKey>> keys_;
};

/**
 * new module resource.index file header
 */
class ResIndexHeader {
public:
    static const uint32_t RES_HEADER_LEN = 140;

    // Type identifier for this chunk.  The meaning of this value depends
    // on the containing chunk.
    char version_[RES_VERSION_LEN];

    // Size of the resource.index file (in bytes).  Including header
    uint32_t length_;

    // determiner key count
    uint32_t keyCount_;

    // offset from the beginning of the index file, pointing to the data block
    uint32_t dataBlockOffset_;
};

/**
 * a ResKey means a Qualifiers Sub-directories
 */
class KeyInfo {
public:
    static const uint32_t RESKEY_HEADER_LEN = 12;

    // always 'KEYS'
    char tag_[4];

    // unique resconfig id
    uint32_t resConfigId_;

    // count of qualifiers
    uint32_t keyParamsCount_;

    // key param list
    std::vector<std::shared_ptr<KeyParam>> params_;
};

class IdsHeader {
public:
    static const uint32_t IDS_HEADER_LEN = 16;

    // always 'IDSS'
    char tag_[4];

    // size of the idss block
    uint32_t length_;

    // resource type count
    uint32_t typeCount_;

    // resource id count
    uint32_t idCount_;
};

class TypeInfo {
public:
    static const uint32_t TYPE_INFO_LEN = 12;

    // resource type
    uint32_t type_;

    // size of current resource type
    uint32_t length_;

    // resource id count
    uint32_t count_;
};

class ResItem {
public:
    static const uint32_t RES_ITEM_LEN = 12;

    // resource id
    uint32_t resId_;

    // offset from the beginning of the index file, pointing to the resource data
    uint32_t offset_;

    // size of resource name
    uint32_t length_;

    // resource name
    std::string name_;
};

class DataHander {
public:
    static const uint32_t DATA_HANDER_LEN = 12;

    // always 'DATA'
    char tag_[4];

    // size of the data block
    uint32_t length_;

    // resource id count
    uint32_t idCount_;
};

class ResInfo {
public:
    static const uint32_t RES_INFO_LEN = 12;

    // resource id
    uint32_t resId_;

    // size of the resource information
    uint32_t length_;

    // resource key count
    uint32_t valueCount_;
};

class ConfigItem {
public:
    static const uint32_t CONFIG_ITEM_LEN = 8;

    // resource config id
    uint32_t resCfgId_;

    // offset from the beginning of the index file, pointing to the resource data
    uint32_t offset_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
