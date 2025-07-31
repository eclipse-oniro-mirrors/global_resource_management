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

#include "hap_resource_v2.h"

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include <sys/mman.h>
#endif
#include <unistd.h>

#include "hap_parser_v2.h"

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#endif

#include "hilog_wrapper.h"
#include "utils/errors.h"

using ReadLock = std::shared_lock<std::shared_mutex>;
using WriteLock = std::unique_lock<std::shared_mutex>;

namespace OHOS {
namespace Global {
namespace Resource {
ValueUnderQualifierDirV2::ValueUnderQualifierDirV2(const std::pair<std::string, std::string> &resPath,
    uint32_t offset, std::shared_ptr<ResConfigImpl> resConfig, bool isOverlay, bool isSystemResource)
    : ValueUnderQualifierDir(resPath, resConfig, isOverlay, isSystemResource), offset_(offset)
{}

ValueUnderQualifierDirV2::~ValueUnderQualifierDirV2()
{}

std::shared_ptr<IdItem> ValueUnderQualifierDirV2::GetIdItem() const
{
    std::shared_ptr<IdItem> idItem = std::make_shared<IdItem>();
    idItem->resType_ = resType_;
    idItem->id_ = id_;
    idItem->name_ = name_;

    uint32_t offset = offset_;
    idItem->JudgeArray();
    if (idItem->isArray_) {
        if (HapParserV2::ParseStringArray(offset, idItem->values_, bufLen_, buf_) != OK) {
            return nullptr;
        }
    } else {
        if (HapParserV2::ParseString(offset, idItem->value_, bufLen_, buf_) != OK) {
            return nullptr;
        }
        idItem->valueLen_ = idItem->value_.size();
    }
    return idItem;
}

IdValuesV2::IdValuesV2(ResType resType, uint32_t id, uint32_t offset, const std::string &name)
    : resType_(resType), id_(id), offset_(offset), name_(name)
{}

IdValuesV2::~IdValuesV2()
{}

const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &IdValuesV2::GetLimitPathsConst() const
{
    return limitPaths_;
}

HapResourceV2::HapResourceV2(const std::string path, time_t lastModTime, bool hasDarkRes)
    : HapResource(path, lastModTime)
{
    hasDarkRes_ = hasDarkRes;
}

HapResourceV2::~HapResourceV2()
{
    if (buf_ != nullptr) {
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
        munmap(buf_, bufLen_);
#else
        delete[] buf_;
#endif
    }
}

bool HapResourceV2::IsSystemResource() const
{
    return false;
}

bool HapResourceV2::IsOverlayResource() const
{
    return false;
}

const std::shared_ptr<IdValues> HapResourceV2::GetIdValues(const uint32_t id)
{
    ReadLock lock(mutex_);
    if (idMap_.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "idMap_ is empty");
        return nullptr;
    }
    std::unordered_map<uint32_t, std::shared_ptr<IdValuesV2>>::const_iterator iter = idMap_.find(id);
    if (iter == idMap_.end()) {
        return nullptr;
    }

    if (iter->second->GetLimitPathsConst().empty() && ParseLimitPaths(iter->second) != OK) {
        return nullptr;
    }
    return iter->second;
}

const std::shared_ptr<IdValues> HapResourceV2::GetIdValuesByName(
    const std::string name, const ResType resType)
{
    ReadLock lock(mutex_);
    if (typeNameMap_.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "typeNameMap_ is empty");
        return nullptr;
    }
    std::unordered_map<uint32_t, std::unordered_map<std::string,
        std::shared_ptr<IdValuesV2>>>::const_iterator mapIterator = typeNameMap_.find(resType);
    if (mapIterator == typeNameMap_.end()) {
        return nullptr;
    }
    std::unordered_map<std::string, std::shared_ptr<IdValuesV2>>::const_iterator iter = mapIterator->second.find(name);
    if (iter == mapIterator->second.end()) {
        return nullptr;
    }

    if (iter->second->GetLimitPathsConst().empty() && ParseLimitPaths(iter->second) != OK) {
        return nullptr;
    }
    return iter->second;
}

std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> HapResourceV2::BuildNameTypeIdMapping()
{
    ReadLock lock(mutex_);
    std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> result;
    result.reserve(idMap_.size());
    for (const auto &item : idMap_) {
        result[item.second->GetName()][item.second->GetResType()] = item.first;
    }
    return result;
}

void HapResourceV2::GetLocales(std::set<std::string> &outValue, bool includeSystem)
{
    ReadLock lock(mutex_);
    if (keys_.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "keys_ is null! GetLocales failed");
        return;
    }
    outValue.insert(locales_.begin(), locales_.end());
}

bool HapResourceV2::Init(std::unordered_map<uint32_t, std::shared_ptr<ResConfigImpl>> &keys,
    std::unordered_map<uint32_t, std::shared_ptr<IdValuesV2>> &idMap,
    std::unordered_map<uint32_t, std::unordered_map<std::string, std::shared_ptr<IdValuesV2>>> &typeNameMap)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
#endif
#ifdef __WINNT__
    char separator = '\\';
#else
    char separator = '/';
#endif
    auto index = indexPath_.rfind(separator);
    if (index == std::string::npos) {
        RESMGR_HILOGE(RESMGR_TAG, "index path format error");
        return false;
    }
#if defined(__IDE_PREVIEW__) || defined(__ARKUI_CROSS__)
    resourcePath_ = indexPath_.substr(0, index + 1);
#else
    if (index < 1) {
        return false;
    }
    index = indexPath_.rfind(separator, index - 1);
    if (index == std::string::npos) {
        RESMGR_HILOGE(RESMGR_TAG, "index path format error");
        return false;
    }
    resourcePath_ = indexPath_.substr(0, index + 1);
#endif

    keys_ = std::move(keys);
    idMap_ = std::move(idMap);
    typeNameMap_ = std::move(typeNameMap);
    return true;
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
void HapResourceV2::InitMmap(std::shared_ptr<AbilityBase::Extractor> extractor,
    std::unique_ptr<AbilityBase::FileMapper> &mapper)
{
    extractor_ = extractor;
    mapper_ = std::move(mapper);
    buf_ = mapper_->GetDataPtr();
    bufLen_ = mapper_->GetDataLen();
}
#endif

bool HapResourceV2::InitMmap(size_t mmapLen, uint8_t *mmap)
{
    bufLen_ = mmapLen;
    buf_ = mmap;
    return true;
}

void HapResourceV2::InitThemeSystemRes()
{
    std::shared_ptr<IdValues> themeSystemRes = nullptr;
    std::vector<std::shared_ptr<ValueUnderQualifierDir>> configList;
    themeSystemRes = GetIdValuesByName("system_color_change", ResType::STRING);
    if (themeSystemRes != nullptr) {
        std::vector<std::shared_ptr<ValueUnderQualifierDir>> configs = themeSystemRes->GetLimitPathsConst();
        configList.insert(configList.end(), configs.begin(), configs.end());
    }
    themeSystemRes = GetIdValuesByName("system_color_change", ResType::BOOLEAN);
    if (themeSystemRes != nullptr) {
        std::vector<std::shared_ptr<ValueUnderQualifierDir>> configs = themeSystemRes->GetLimitPathsConst();
        configList.insert(configList.end(), configs.begin(), configs.end());
    }

    for (const auto &cfg : configList) {
        if (cfg->GetIdItem()->value_ == "true") {
            isThemeSystemResEnable_ = true;
            return;
        }
    }
}

int32_t HapResourceV2::ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue)
{
    std::unique_lock<std::mutex> lock(idValuesMutex_);
    if (!idValue->GetLimitPathsConst().empty()) {
        return OK;
    }
    ResInfo resInfo;
    uint32_t offset = idValue->GetOffset();
    int32_t ret = HapParserV2::ParseResInfo(offset, resInfo, bufLen_, buf_);
    if (ret != OK) {
        return ret;
    }
    if (resInfo.resId_ != idValue->GetId()) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResInfo failed, ResInfo data error.");
        return UNKNOWN_ERROR;
    }

    idValue->ReserveLimitPaths(resInfo.valueCount_);
    std::pair<std::string, std::string> resPath = std::make_pair(indexPath_, resourcePath_);
    for (uint32_t i = 0; i < resInfo.valueCount_; i++) {
        ConfigItem configItem;
        ret = HapParserV2::ParseConfigItem(offset, configItem, bufLen_, buf_);
        if (ret != OK) {
            return ret;
        }

        std::shared_ptr<ValueUnderQualifierDirV2> vuqd =
            std::make_shared<ValueUnderQualifierDirV2>(resPath, configItem.offset_, keys_[configItem.resCfgId_]);
        vuqd->Init(bufLen_, buf_, idValue->GetResType(), idValue->GetId(), idValue->GetName());
        idValue->AddLimitPath(vuqd);
    }
    return OK;
}

SystemResource::SystemResource() : HapResourceV2("", 0)
{}

SystemResource::SystemResource(const std::string path, time_t lastModTime, bool hasDarkRes)
    : HapResourceV2(path, lastModTime, hasDarkRes)
{}

SystemResource::~SystemResource()
{}

bool SystemResource::IsSystemResource() const
{
    return true;
}

void SystemResource::GetLocales(std::set<std::string> &outValue, bool includeSystem)
{
    ReadLock lock(mutex_);
    if (keys_.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "keys_ is null! GetLocales failed");
        return;
    }
    if (!includeSystem) {
        return;
    }
    outValue.insert(locales_.begin(), locales_.end());
}

int32_t SystemResource::ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue)
{
    ResInfo resInfo;
    uint32_t offset = idValue->GetOffset();
    int32_t ret = HapParserV2::ParseResInfo(offset, resInfo, bufLen_, buf_);
    if (ret != OK) {
        return ret;
    }
    if (resInfo.resId_ != idValue->GetId()) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResInfo failed, ResInfo data error.");
        return UNKNOWN_ERROR;
    }

    idValue->ReserveLimitPaths(resInfo.valueCount_);
    for (uint32_t i = 0; i < resInfo.valueCount_; i++) {
        ConfigItem configItem;
        ret = HapParserV2::ParseConfigItem(offset, configItem, bufLen_, buf_);
        if (ret != OK) {
            return ret;
        }

        std::pair<std::string, std::string> resPath = std::make_pair(indexPath_, resourcePath_);
        std::shared_ptr<ValueUnderQualifierDirV2> vuqd = std::make_shared<ValueUnderQualifierDirV2>(
            resPath, configItem.offset_, keys_[configItem.resCfgId_], false, true);
        vuqd->Init(bufLen_, buf_, idValue->GetResType(), idValue->GetId(), idValue->GetName());
        idValue->AddLimitPath(vuqd);
    }
    return OK;
}

OverlayResource::OverlayResource() : HapResourceV2("", 0)
{}

OverlayResource::OverlayResource(const std::string path, time_t lastModTime, bool hasDarkRes)
    : HapResourceV2(path, lastModTime, hasDarkRes)
{}

OverlayResource::~OverlayResource()
{}

bool OverlayResource::IsOverlayResource() const
{
    return true;
}

void OverlayResource::UpdateOverlayInfo(
    std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> &nameTypeId)
{
    WriteLock lock(mutex_);
    std::unordered_map<uint32_t, std::shared_ptr<IdValuesV2>> newIdMap;
    newIdMap.reserve(idMap_.size());
    for (auto &item : idMap_) {
        std::string name = item.second->GetName();
        ResType type = item.second->GetResType();
        if (nameTypeId.find(name) == nameTypeId.end() || nameTypeId[name].find(type) == nameTypeId[name].end()) {
            continue;
        }
        uint32_t newId = nameTypeId[name][type];
        item.second->SetId(newId);
        newIdMap[newId] = item.second;
    }
    idMap_.swap(newIdMap);
}

void OverlayResource::GetLocales(std::set<std::string> &outValue, bool includeSystem)
{
    return;
}

int32_t OverlayResource::ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue)
{
    ResInfo resInfo;
    uint32_t offset = idValue->GetOffset();
    int32_t ret = HapParserV2::ParseResInfo(offset, resInfo, bufLen_, buf_);
    if (ret != OK) {
        return ret;
    }

    idValue->ReserveLimitPaths(resInfo.valueCount_);
    for (uint32_t i = 0; i < resInfo.valueCount_; i++) {
        ConfigItem configItem;
        ret = HapParserV2::ParseConfigItem(offset, configItem, bufLen_, buf_);
        if (ret != OK) {
            return ret;
        }

        std::pair<std::string, std::string> resPath = std::make_pair(indexPath_, resourcePath_);
        std::shared_ptr<ValueUnderQualifierDirV2> vuqd = std::make_shared<ValueUnderQualifierDirV2>(
            resPath, configItem.offset_, keys_[configItem.resCfgId_], true, false);
        vuqd->Init(bufLen_, buf_, idValue->GetResType(), idValue->GetId(), idValue->GetName());
        idValue->AddLimitPath(vuqd);
    }
    return OK;
}

SystemOverlayResource::SystemOverlayResource(const std::string path, time_t lastModTime, bool hasDarkRes)
    : HapResourceV2(path, lastModTime, hasDarkRes)
{}

SystemOverlayResource::~SystemOverlayResource()
{}

void SystemOverlayResource::GetLocales(std::set<std::string> &outValue, bool includeSystem)
{
    SystemResource::GetLocales(outValue, includeSystem);
}

int32_t SystemOverlayResource::ParseLimitPaths(std::shared_ptr<IdValuesV2> idValue)
{
    ResInfo resInfo;
    uint32_t offset = idValue->GetOffset();
    int32_t ret = HapParserV2::ParseResInfo(offset, resInfo, bufLen_, buf_);
    if (ret != OK) {
        return ret;
    }

    idValue->ReserveLimitPaths(resInfo.valueCount_);
    for (uint32_t i = 0; i < resInfo.valueCount_; i++) {
        ConfigItem configItem;
        ret = HapParserV2::ParseConfigItem(offset, configItem, bufLen_, buf_);
        if (ret != OK) {
            return ret;
        }

        std::pair<std::string, std::string> resPath = std::make_pair(indexPath_, resourcePath_);
        std::shared_ptr<ValueUnderQualifierDirV2> vuqd = std::make_shared<ValueUnderQualifierDirV2>(
            resPath, configItem.offset_, keys_[configItem.resCfgId_], true, true);
        vuqd->Init(bufLen_, buf_, idValue->GetResType(), idValue->GetId(), idValue->GetName());
        idValue->AddLimitPath(vuqd);
    }
    return OK;
}
}
}
}