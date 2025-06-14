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

#include "hap_parser_v1.h"

#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <unordered_map>

#include "hap_resource_manager.h"
#include "hap_resource_v1.h"
#include "hilog_wrapper.h"
#include "locale_matcher.h"
#include "utils/errors.h"
#include "utils/string_utils.h"
#include "utils/utils.h"

#if defined(__WINNT__)
#include <cstring>
#else
#include "securec.h"
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#include "file_mapper.h"
#include "extractor.h"
#endif

namespace OHOS {
namespace Global {
namespace Resource {
const std::string NOT_DEVICE_TYPE = "not_device_type";
const std::string DEVICE_DEFAULT = "default";
static const std::unordered_map<ResType, uint32_t> TYPE_MAP {
    {INTEGER, SELECT_INTEGER},
    {STRING, SELECT_STRING},
    {STRINGARRAY, SELECT_STRINGARRAY},
    {INTARRAY, SELECT_INTARRAY},
    {BOOLEAN, SELECT_BOOLEAN},
    {COLOR, SELECT_COLOR},
    {THEME, SELECT_THEME},
    {PLURALS, SELECT_PLURALS},
    {FLOAT, SELECT_FLOAT},
    {MEDIA, SELECT_MEDIA},
    {PROF, SELECT_PROF},
    {PATTERN, SELECT_PATTERN},
    {SYMBOL, SELECT_SYMBOL}
};

HapParserV1::HapParserV1() : deviceType_(DEVICE_DEFAULT)
{}

HapParserV1::HapParserV1(std::shared_ptr<ResConfigImpl> &defaultConfig, uint32_t selectedTypes,
    bool loadAll, bool isUpdate) : defaultConfig_(defaultConfig), selectedTypes_(selectedTypes),
    deviceType_(DEVICE_DEFAULT), loadAll_(loadAll), isUpdate_(isUpdate)
{}

HapParserV1::~HapParserV1()
{}

void HapParserV1::SetResDesc(std::shared_ptr<ResDesc> &resDesc)
{
    resDesc_ = resDesc;
}

bool HapParserV1::Init(const char *path)
{
    if (resDesc_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResDesc failed when construct hapParser");
        return false;
    }

    if (!GetIndexData(path, buffer_, bufLen_)) {
        RESMGR_HILOGE(RESMGR_TAG, "GetIndexData failed when construct hapParser, file path: %{public}s", path);
        return false;
    }

    int32_t out = this->ParseResHex();
    if (out != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed! retcode:%d", out);
        return false;
    }
    return true;
}

int32_t HapParserV1::ParseResHex()
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif
    ResHeader resHeader;
    uint32_t offset = 0;
    if (offset + ResHeader::RES_HEADER_LEN > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&resHeader, sizeof(ResHeader), buffer_.get() + offset, ResHeader::RES_HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += ResHeader::RES_HEADER_LEN;
    if (resHeader.keyCount_ == 0 || resHeader.length_ == 0) {
        return UNKNOWN_ERROR;
    }
    deviceType_ = ResConfigImpl::GetCurrentDeviceType();
    if (deviceType_ == std::string(TABLET_STR) || deviceType_ == std::string(TWOINONE_STR)) {
        deviceTypes_ = ResConfigImpl::GetAppSupportDeviceTypes();
    }
    std::vector<bool> keyTypes(KeyType::KEY_TYPE_MAX, false);
    for (uint32_t i = 0; i < resHeader.keyCount_; i++) {
        std::shared_ptr<ResKey> key = std::make_shared<ResKey>();
        if (key == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new ResKey failed when ParseResHex");
            return SYS_ERROR;
        }
        bool match = true;
        int32_t ret = ParseKey(offset, key, match, keyTypes);
        if (ret != OK) {
            return ret;
        }
        if (match) {
            resDesc_->keys_.push_back(key);
        }
    }
    return OK;
}

int32_t HapParserV1::ParseKey(uint32_t &offset, std::shared_ptr<ResKey> key, bool &match, std::vector<bool> &keyTypes)
{
    if (offset + ResKey::RESKEY_HEADER_LEN > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResKeyHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(key.get(), sizeof(ResKey), buffer_.get() + offset, ResKey::RESKEY_HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += ResKey::RESKEY_HEADER_LEN;
    if (key->tag_[ArrayIndex::INDEX_ZERO] != 'K' || key->tag_[ArrayIndex::INDEX_ONE] != 'E' ||
        key->tag_[ArrayIndex::INDEX_TWO] != 'Y' || key->tag_[ArrayIndex::INDEX_THREE] != 'S') {
        return -1;
    }

    std::vector<std::shared_ptr<KeyParam>> keyParams;
    if (!this->getKeyParams(offset, key->keyParamsCount_, match, keyTypes, keyParams)) {
        return SYS_ERROR;
    }
    key->resConfig_ = HapParser::CreateResConfigFromKeyParams(keyParams);
    if (SkipParseItem(key, match)) {
        match = false;
        return OK;
    }

    uint32_t idOffset = key->offset_;
    std::shared_ptr<ResId> id = std::make_shared<ResId>();
    if (id == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResId failed when ParseKey");
        return SYS_ERROR;
    }
    int32_t ret = ParseId(idOffset, id);
    if (ret != OK) {
        return ret;
    }
    key->resId_ = id;
    return OK;
}

bool HapParserV1::getKeyParams(uint32_t &offset, uint32_t paramsCount, bool &match, std::vector<bool> &keyTypes,
    std::vector<std::shared_ptr<KeyParam>> &keyParams)
{
    std::string locale;
    bool isLocale = false;
    for (uint32_t i = 0; i < paramsCount; ++i) {
        std::shared_ptr<KeyParam> kp;
        if (ParseKeyParam(offset, match, kp) != OK) {
            return false;
        }
        if (kp == nullptr) {
            return false;
        }
        GetLimitKeyValue(kp->type_, keyTypes);
        GetKeyParamsLocales(kp, locale, isLocale);
        keyParams.push_back(kp);
    }
    if (isLocale) {
        locales_.emplace(locale);
    }
    return true;
}

int32_t HapParserV1::ParseKeyParam(uint32_t &offset, bool &match, std::shared_ptr<KeyParam> &kp)
{
    kp = std::make_shared<KeyParam>();
    if (kp == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseKeyParam new KeyParam failed");
        return SYS_ERROR;
    }
    if (offset + KeyParam::KEYPARAM_LEN > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseKeyParam failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(kp.get(), sizeof(KeyParam), buffer_.get() + offset, KeyParam::KEYPARAM_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += KeyParam::KEYPARAM_LEN;
    kp->InitStr();
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    auto resDeviceType = kp->GetDeviceTypeStr();
    deviceTypes_.push_back(deviceType_);
    if (deviceType_ != DEVICE_DEFAULT && resDeviceType != NOT_DEVICE_TYPE &&
        find(deviceTypes_.begin(), deviceTypes_.end(), resDeviceType) == deviceTypes_.end()) {
        match = false;
    }
#endif
    return OK;
}

void HapParserV1::GetLimitKeyValue(KeyType type, std::vector<bool> &keyTypes)
{
    const uint32_t limitKeysBase = 0x00000001;
    uint32_t typeValue = static_cast<uint32_t>(type);
    if (type < KeyType::KEY_TYPE_MAX && !keyTypes[typeValue]) {
        keyTypes[typeValue] = true;
        limitKeyValue_ |= limitKeysBase << typeValue;
    }
}

bool HapParserV1::SkipParseItem(const std::shared_ptr<ResKey> &key, bool &match)
{
    if (!match || (selectedTypes_ != SELECT_ALL &&
                    defaultConfig_ != nullptr && !defaultConfig_->Match(key->resConfig_, false))) {
        return true;
    }

    if (loadAll_) {
        return false;
    }

    if (!key->resConfig_->GetLocaleInfo()) {
        return isUpdate_;
    }

    if (defaultConfig_ != nullptr && defaultConfig_->MatchLocal(*key->resConfig_)) {
        return false;
    }
    return true;
}

int32_t HapParserV1::ParseId(uint32_t &offset, std::shared_ptr<ResId> id)
{
    if (offset + ResId::RESID_HEADER_LEN > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResIdHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(id.get(), sizeof(ResId), buffer_.get() + offset, ResId::RESID_HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += ResId::RESID_HEADER_LEN;
    if (id->tag_[ArrayIndex::INDEX_ZERO] != 'I' || id->tag_[ArrayIndex::INDEX_ONE] != 'D'
        || id->tag_[ArrayIndex::INDEX_TWO] != 'S' || id->tag_[ArrayIndex::INDEX_THREE] != 'S') {
        return -1;
    }
    for (uint32_t i = 0; i < id->count_; ++i) {
        std::shared_ptr<IdParam> ip = std::make_shared<IdParam>();
        if (ip == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new IdParam failed when ParseId");
            return SYS_ERROR;
        }
        if (offset + ResId::IDPARAM_HEADER_LEN > bufLen_) {
            RESMGR_HILOGE(RESMGR_TAG, "Parse IdParam failed, the offset will be out of bounds");
            return SYS_ERROR;
        }
        errno_t eret = memcpy_s(ip.get(), sizeof(IdParam), buffer_.get() + offset, ResId::IDPARAM_HEADER_LEN);
        if (eret != OK) {
            return SYS_ERROR;
        }
        offset += ResId::IDPARAM_HEADER_LEN;
        std::shared_ptr<IdItem> idItem = std::make_shared<IdItem>();
        if (idItem == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new IdItem failed when ParseId");
            return SYS_ERROR;
        }
        uint32_t ipOffset = ip->offset_;
        int32_t ret = ParseIdItem(ipOffset, idItem);
        if (ret != OK) {
            return ret;
        }
        ip->idItem_ = idItem;
        id->idParams_.push_back(ip);
    }
    return OK;
}

uint32_t ConvertType(ResType type)
{
    auto it = TYPE_MAP.find(type);
    if (it == TYPE_MAP.end()) {
        return SELECT_ALL;
    }
    return it->second;
}

int32_t HapParserV1::ParseIdItem(uint32_t &offset, std::shared_ptr<IdItem> idItem)
{
    if (offset + IdItem::HEADER_LEN > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse IdItemHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(idItem.get(), sizeof(IdItem), buffer_.get() + offset, IdItem::HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    if (selectedTypes_ != SELECT_ALL && (selectedTypes_ & ConvertType(idItem->resType_)) == 0) {
        return OK;
    }
    offset += IdItem::HEADER_LEN;

    idItem->JudgeArray();
    if (idItem->isArray_) {
        int32_t ret = ParseStringArray(offset, idItem->values_);
        if (ret != OK) {
            return ret;
        }
    } else {
        int32_t ret = ParseString(offset, idItem->value_);
        if (ret != OK) {
            return ret;
        }
        idItem->valueLen_ = idItem->value_.size();
    }
    int32_t ret = ParseString(offset, idItem->name_);
    if (ret != OK) {
        return ret;
    }
    return OK;
}

int32_t HapParserV1::ParseStringArray(uint32_t &offset, std::vector<std::string> &values)
{
    uint16_t arrLen;
    if (offset + IdItem::SIZE_LEN > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseStringArray failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&arrLen, sizeof(arrLen), buffer_.get() + offset, IdItem::SIZE_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += IdItem::SIZE_LEN; // Offset value plus 2
    // next arrLen bytes are several strings. then after, is one '\0'
    uint32_t startOffset = offset;
    std::string value;
    while (true) {
        int32_t ret = ParseString(offset, value, false);
        if (ret != OK) {
            return ret;
        }
        values.push_back(value);

        uint32_t readSize = offset - startOffset;
        if (readSize + 1 == arrLen) {
            offset += 1; // after arrLen, got '\0'
            break;
        }
        if (readSize + 1 > arrLen) {
            // size not match, cannot > arrLen
            return SYS_ERROR;
        }
    }
    return OK;
}

int32_t HapParserV1::ParseString(uint32_t &offset, std::string &id, bool includeTemi)
{
    uint16_t strLen;
    if (offset + IdItem::SIZE_LEN > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseString length failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&strLen, sizeof(strLen), buffer_.get() + offset, IdItem::SIZE_LEN);
    if (eret != OK || (includeTemi && strLen == 0)) {
        return SYS_ERROR;
    }
    offset += IdItem::SIZE_LEN; // Offset value plus 2
    if (offset + (includeTemi ? (strLen - 1) : strLen) > bufLen_) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseString value failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    std::string tmp = std::string(reinterpret_cast<char *>(buffer_.get()) + offset,
        includeTemi ? (strLen - 1) : strLen);
    offset += includeTemi ? strLen : (strLen + 1);
    id = tmp;
    return OK;
}

std::shared_ptr<HapResource> HapParserV1::GetHapResource(const char *path, bool isSystem, bool isOverlay)
{
    std::shared_ptr<HapResourceV1> pResource = std::make_shared<HapResourceV1>(path, 0, resDesc_, isSystem, isOverlay);
    if (pResource == nullptr || !pResource->Init(defaultConfig_)) {
        return nullptr;
    }
    pResource->SetLimitKeysValue(limitKeyValue_);
    pResource->SetLocales(locales_);
    pResource->SetSelectedType(selectedTypes_);
    return pResource;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
