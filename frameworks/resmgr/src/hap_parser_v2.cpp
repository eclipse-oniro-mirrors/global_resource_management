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

#include "hap_parser_v2.h"

#include <fcntl.h>
#include <fstream>
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include <sys/mman.h>
#include <sys/stat.h>
#endif
#include <unistd.h>

#if defined(__WINNT__)
#include <cstring>
#else
#include "securec.h"
#endif

#include "hilog_wrapper.h"
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#endif

#include "utils/errors.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
HapParserV2::HapParserV2()
{}

HapParserV2::~HapParserV2()
{
    if (mmap_ != nullptr) {
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
        munmap(mmap_, mmapLen_);
#else
        delete[] mmap_;
#endif
    }
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    if (fd_ > 0) {
        close(fd_);
    }
#endif
}

bool HapParserV2::Init(const char *path)
{
    if (!GetIndexMmap(path)) {
        RESMGR_HILOGE(RESMGR_TAG, "GetIndexMmap failed when construct hapParser, file path: %{public}s", path);
        return false;
    }

    int32_t out = this->ParseResHex();
    if (out != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed! retcode:%d", out);
        return false;
    }
    return true;
}

int32_t HapParserV2::ParseResHex()
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
#endif
    uint32_t offset = 0;
    uint8_t *buf = nullptr;
    size_t bufLen = 0;
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    if (mapper_ != nullptr) {
        buf = mapper_->GetDataPtr();
        bufLen = mapper_->GetDataLen();
    } else if (mmap_ != nullptr && mmapLen_ != 0) {
        buf = mmap_;
        bufLen = mmapLen_;
    } else {
        RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed, Mmap is empty.");
        return SYS_ERROR;
    }
#else
    if (mmap_ != nullptr && mmapLen_ != 0) {
        buf = mmap_;
        bufLen = mmapLen_;
    } else {
        RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed, Mmap is empty.");
        return SYS_ERROR;
    }
#endif

    int32_t ret = ParseHeader(offset, bufLen, buf);
    if (ret != OK) {
        return ret;
    }

    ret = ParseKeys(offset, bufLen, buf);
    if (ret != OK) {
        return ret;
    }

    ret = ParseIds(offset, bufLen, buf);
    if (ret != OK) {
        return ret;
    }

    if (offset != resHeader_.dataBlockOffset_) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResHeader failed, resource data error.");
        return UNKNOWN_ERROR;
    }
    return OK;
}

int32_t HapParserV2::ParseHeader(uint32_t &offset, const size_t bufLen, const uint8_t *buf)
{
    if (offset + ResIndexHeader::RES_HEADER_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResHeader failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&resHeader_, sizeof(ResIndexHeader), buf + offset, ResIndexHeader::RES_HEADER_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResHeader failed, memory copy failed.");
        return SYS_ERROR;
    }
    if (resHeader_.keyCount_ == 0 || resHeader_.length_ == 0 || resHeader_.dataBlockOffset_ > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResHeader failed, ResHeader data error.");
        return UNKNOWN_ERROR;
    }
    offset += ResIndexHeader::RES_HEADER_LEN;
    keys_.reserve(resHeader_.keyCount_);
    return OK;
}

int32_t HapParserV2::ParseKeys(uint32_t &offset, const size_t bufLen, const uint8_t *buf)
{
    for (uint32_t i = 0; i < resHeader_.keyCount_; i++) {
        std::shared_ptr<KeyInfo> key = std::make_shared<KeyInfo>();
        if (key == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new ResKey failed when ParseResHex");
            return SYS_ERROR;
        }
        int32_t ret = ParseKey(offset, key, bufLen, buf);
        if (ret != OK) {
            return ret;
        }
        keys_[key->resConfigId_] = HapParser::CreateResConfigFromKeyParams(key->params_);
        if (keys_[key->resConfigId_]->GetColorMode() == ColorMode::DARK) {
            hasDarkRes_ = true;
        }
    }
    return OK;
}

int32_t HapParserV2::ParseIds(uint32_t &offset, const size_t bufLen, const uint8_t *buf)
{
    if (offset + IdsHeader::IDS_HEADER_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse IdsHeader failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&idsHeader_, sizeof(IdsHeader), buf + offset, IdsHeader::IDS_HEADER_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse IdsHeader failed, memory copy failed.");
        return SYS_ERROR;
    }
    if (idsHeader_.tag_[ArrayIndex::INDEX_ZERO] != 'I' || idsHeader_.tag_[ArrayIndex::INDEX_ONE] != 'D' ||
        idsHeader_.tag_[ArrayIndex::INDEX_TWO] != 'S' || idsHeader_.tag_[ArrayIndex::INDEX_THREE] != 'S' ||
        offset + idsHeader_.length_ > resHeader_.dataBlockOffset_ ||
        idsHeader_.typeCount_ == 0 || idsHeader_.idCount_ == 0) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse IdsHeader failed, IdsHeader data error.");
        return UNKNOWN_ERROR;
    }
    offset += IdsHeader::IDS_HEADER_LEN;
    idMap_.reserve(idsHeader_.idCount_);
    typeNameMap_.reserve(idsHeader_.typeCount_);

    for (uint32_t i = 0; i < idsHeader_.typeCount_; i++) {
        int32_t ret = ParseType(offset, bufLen, buf);
        if (ret != OK) {
            return ret;
        }
    }
    return OK;
}

int32_t HapParserV2::ParseType(uint32_t &offset, const size_t bufLen, const uint8_t *buf)
{
    if (offset + TypeInfo::TYPE_INFO_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse TypeInfo failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    TypeInfo typeInfo;
    errno_t eret = memcpy_s(&typeInfo, sizeof(TypeInfo), buf + offset, TypeInfo::TYPE_INFO_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse TypeInfo failed, memory copy failed.");
        return SYS_ERROR;
    }
    if (typeInfo.type_ >= ResType::MAX_RES_TYPE || offset + typeInfo.length_ > resHeader_.dataBlockOffset_ ||
        typeInfo.count_ > idsHeader_.idCount_) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse TypeInfo failed, TypeInfo data error.");
        return UNKNOWN_ERROR;
    }
    offset += TypeInfo::TYPE_INFO_LEN;
    typeNameMap_[typeInfo.type_].reserve(typeInfo.count_);

    for (uint32_t i = 0; i < typeInfo.count_; i++) {
        int32_t ret = ParseItem(offset, bufLen, buf, typeInfo);
        if (ret != OK) {
            return ret;
        }
    }
    return OK;
}

int32_t HapParserV2::ParseItem(uint32_t &offset, const size_t bufLen, const uint8_t *buf, const TypeInfo &typeInfo)
{
    if (offset + ResItem::RES_ITEM_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResItem failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    ResItem resItem;
    errno_t eret = memcpy_s(&resItem, sizeof(ResItem), buf + offset, ResItem::RES_ITEM_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResItem failed, memory copy failed.");
        return SYS_ERROR;
    }
    offset += ResItem::RES_ITEM_LEN;
    resItem.name_ = std::string(reinterpret_cast<const char *>(buf) + offset, resItem.length_);
    offset += resItem.length_;

    std::shared_ptr<IdValuesV2> idValues =
        std::make_shared<IdValuesV2>((ResType)typeInfo.type_, resItem.resId_, resItem.offset_, resItem.name_);
    idValues->SetBuf(bufLen, buf);
    typeNameMap_[typeInfo.type_][resItem.name_] = idValues;
    idMap_[resItem.resId_] = idValues;
    return OK;
}

int32_t HapParserV2::ParseStringArray(uint32_t &offset, std::vector<std::string> &values, size_t bufLen, uint8_t *buf)
{
    uint16_t arrLen;
    if (offset + ValueUnderQualifierDirV2::DATA_HEAD_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseStringArray failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&arrLen, sizeof(uint16_t), buf + offset, ValueUnderQualifierDirV2::DATA_HEAD_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseStringArray failed, memory copy failed.");
        return SYS_ERROR;
    }
    offset += ValueUnderQualifierDirV2::DATA_HEAD_LEN;

    uint32_t startOffset = offset;
    std::string value;
    while (true) {
        int32_t ret = ParseString(offset, value, bufLen, buf);
        if (ret != OK) {
            return ret;
        }
        values.push_back(value);
        offset++;

        uint32_t readSize = offset - startOffset;
        if (readSize == arrLen) {
            break;
        }
        if (readSize > arrLen) {
            RESMGR_HILOGE(RESMGR_TAG, "ParseStringArray failed, the read size out of bounds.");
            return SYS_ERROR;
        }
    }
    return OK;
}

int32_t HapParserV2::ParseString(uint32_t &offset, std::string &id, size_t bufLen, uint8_t *buf)
{
    uint16_t strLen;
    if (offset + ValueUnderQualifierDirV2::DATA_HEAD_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseString failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&strLen, sizeof(uint16_t), buf + offset, ValueUnderQualifierDirV2::DATA_HEAD_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseString failed, memory copy failed.");
        return SYS_ERROR;
    }
    offset += ValueUnderQualifierDirV2::DATA_HEAD_LEN;

    if (offset + strLen > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseString failed, the string offset will be out of bounds");
        return SYS_ERROR;
    }
    std::string tmp = std::string(reinterpret_cast<char *>(buf) + offset, strLen);
    offset += strLen;
    id = tmp;
    return OK;
}

int32_t HapParserV2::ParseResInfo(uint32_t &offset, ResInfo &resInfo, const size_t bufLen, const uint8_t *buf)
{
    if (offset + ResInfo::RES_INFO_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResInfo failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&resInfo, sizeof(ResInfo), buf + offset, ResInfo::RES_INFO_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResInfo failed, memory copy failed.");
        return SYS_ERROR;
    }
    offset += ResInfo::RES_INFO_LEN;
    return OK;
}

int32_t HapParserV2::ParseConfigItem(uint32_t &offset, ConfigItem &configItem, const size_t bufLen, const uint8_t *buf)
{
    if (offset + ConfigItem::CONFIG_ITEM_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ConfigItem failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&configItem, sizeof(ConfigItem), buf + offset, ConfigItem::CONFIG_ITEM_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ConfigItem failed, memory copy failed.");
        return SYS_ERROR;
    }
    offset += ConfigItem::CONFIG_ITEM_LEN;
    return OK;
}

int32_t HapParserV2::ParseKey(uint32_t &offset, std::shared_ptr<KeyInfo> key, const size_t bufLen, const uint8_t *buf)
{
    if (offset + KeyInfo::RESKEY_HEADER_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResKey failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(key.get(), sizeof(KeyInfo), buf + offset, KeyInfo::RESKEY_HEADER_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResKey failed, memory copy failed.");
        return SYS_ERROR;
    }
    if (key->tag_[ArrayIndex::INDEX_ZERO] != 'K' || key->tag_[ArrayIndex::INDEX_ONE] != 'E' ||
        key->tag_[ArrayIndex::INDEX_TWO] != 'Y' || key->tag_[ArrayIndex::INDEX_THREE] != 'S') {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResKey failed, ResKey data error.");
        return UNKNOWN_ERROR;
    }
    offset += KeyInfo::RESKEY_HEADER_LEN;

    std::string locale;
    bool isLocale = false;
    for (uint32_t i = 0; i < key->keyParamsCount_; i++) {
        std::shared_ptr<KeyParam> keyParam = std::make_shared<KeyParam>();
        if (keyParam == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new KeyParam failed when ParseResHex");
            return SYS_ERROR;
        }
        int32_t ret = ParseKeyParam(offset, keyParam, bufLen, buf);
        GetKeyParamsLocales(keyParam, locale, isLocale);
        if (ret != OK) {
            return ret;
        }
        key->params_.push_back(keyParam);
    }
    if (isLocale) {
        locales_.emplace(locale);
    }
    return OK;
}

int32_t HapParserV2::ParseKeyParam(uint32_t &offset, std::shared_ptr<KeyParam> keyParam,
    const size_t bufLen, const uint8_t *buf)
{
    if (offset + KeyParam::KEYPARAM_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse KeyParam failed, the offset will be out of bounds.");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(keyParam.get(), sizeof(KeyParam), buf + offset, KeyParam::KEYPARAM_LEN);
    if (eret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse KeyParam failed, memory copy failed.");
        return SYS_ERROR;
    }
    offset += KeyParam::KEYPARAM_LEN;
    keyParam->InitStr();

    GetLimitKeyValue(keyParam->type_);
    return OK;
}

void HapParserV2::GetLimitKeyValue(KeyType type)
{
    const uint32_t limitKeysBase = 0x00000001;
    if (type < KeyType::KEY_TYPE_MAX) {
        uint32_t typeValue = static_cast<uint32_t>(type);
        limitKeyValue_ |= limitKeysBase << typeValue;
    }
}

std::shared_ptr<HapResource> HapParserV2::GetHapResource(const char *path, bool isSystem, bool isOverlay)
{
    if (keys_.size() == 0 || idMap_.size() == 0 || typeNameMap_.size() == 0) {
        RESMGR_HILOGE(RESMGR_TAG, "Get Hap resource failed, parse data is empty.");
        return nullptr;
    }

    std::shared_ptr<HapResourceV2> pResource = nullptr;
    if (isSystem && isOverlay) {
        pResource = std::make_shared<SystemOverlayResource>(path, 0);
    } else if (isSystem) {
        pResource = std::make_shared<SystemResource>(path, 0);
    } else if (isOverlay) {
        pResource = std::make_shared<OverlayResource>(path, 0);
    } else {
        pResource = std::make_shared<HapResourceV2>(path, 0, hasDarkRes_);
    }

    if (pResource == nullptr || !pResource->Init(keys_, idMap_, typeNameMap_)) {
        RESMGR_HILOGE(RESMGR_TAG, "Get Hap resource failed, HapResource init failed.");
        return nullptr;
    }

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    if (mapper_ != nullptr) {
        pResource->InitMmap(extractor_, mapper_);
    } else if (mmap_ != nullptr && mmapLen_ != 0) {
        pResource->InitMmap(mmapLen_, mmap_);
        mmap_ = nullptr;
    } else {
        RESMGR_HILOGE(RESMGR_TAG, "Get Hap resource failed, Mmap is empty.");
        return nullptr;
    }
#else
    if (mmap_ != nullptr && mmapLen_ != 0) {
        pResource->InitMmap(mmapLen_, mmap_);
        mmap_ = nullptr;
    } else {
        RESMGR_HILOGE(RESMGR_TAG, "Get Hap resource failed, Mmap is empty.");
        return nullptr;
    }
#endif

    pResource->SetLimitKeysValue(limitKeyValue_);
    pResource->SetLocales(locales_);
    pResource->InitThemeSystemRes();
    return pResource;
}

bool HapParserV2::GetIndexMmap(const char *path)
{
    if (Utils::ContainsTail(path, Utils::tailSet)) {
        return GetIndexMmapFromHap(path);
    } else {
        return GetIndexMmapFromIndex(path);
    }
}

bool HapParserV2::GetIndexMmapFromHap(const char *path)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
    bool isNewExtractor = false;
    extractor_ = AbilityBase::ExtractorUtil::GetExtractor(path, isNewExtractor);
    if (extractor_ == nullptr) {
        return false;
    }
    std::string indexFilePath;
    if (extractor_->IsStageModel()) {
        indexFilePath = "resources.index";
    } else {
        indexFilePath = HapParser::GetIndexFilePath(extractor_);
    }
    mapper_ = extractor_->GetMmapData(indexFilePath);
    if (mapper_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get mmap data indexFilePath, %{public}s", indexFilePath.c_str());
        return false;
    }
#endif
    return true;
}

bool HapParserV2::GetIndexMmapFromIndex(const char *path)
{
    char indexPath[PATH_MAX + 1] = {0};
    Utils::CanonicalizePath(path, indexPath, PATH_MAX);
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    fd_ = open(indexPath, O_RDONLY);
    if (fd_ <= 0) {
        return false;
    }
    struct stat fileStat;
    fstat(fd_, &fileStat);
    mmapLen_ = static_cast<size_t>(fileStat.st_size);
    mmap_ = (uint8_t*)mmap(nullptr, mmapLen_, PROT_READ, MAP_PRIVATE, fd_, 0);
    if (mmap_ == MAP_FAILED) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get mmap data indexFilePath, %{public}s", indexPath);
        return false;
    }
#else
    std::ifstream inFile(indexPath, std::ios::binary | std::ios::in);
    if (!inFile.good()) {
        return false;
    }
    inFile.seekg(0, std::ios::end);
    int fileLen = inFile.tellg();
    if (fileLen <= 0) {
        RESMGR_HILOGE(RESMGR_TAG, "file size is zero");
        inFile.close();
        return false;
    }
    mmapLen_ = static_cast<size_t>(fileLen);
    mmap_ = new uint8_t[fileLen + 1];
    inFile.seekg(0, std::ios::beg);
    inFile.read(reinterpret_cast<char*>(mmap_), fileLen);
    inFile.close();
    RESMGR_HILOGD(RESMGR_TAG, "extract success, bufLen:%d", fileLen);
#endif
    return true;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
