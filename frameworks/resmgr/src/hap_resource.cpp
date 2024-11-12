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

#include "hap_resource.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <fstream>
#include "utils/utils.h"
#include <set>
#ifdef __WINNT__
#include <shlwapi.h>
#include <windows.h>
#endif

#ifdef __LINUX__
#include <cstring>
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#include "file_mapper.h"
#include "extractor.h"
#endif
#include "hap_parser.h"
#include "hap_resource_manager.h"
#include "hilog_wrapper.h"
#include "utils/errors.h"

namespace OHOS {
namespace Global {
namespace Resource {
HapResource::ValueUnderQualifierDir::ValueUnderQualifierDir(const std::shared_ptr<ResKey> &resKey,
    const std::shared_ptr<IdItem> &idItem, const std::pair<std::string, std::string> &resPath, bool isOverlay,
    bool systemResource)
{
    keyParams_ = resKey->keyParams_;
    folder_ = HapParser::ToFolderPath(keyParams_);
    idItem_ = idItem;
    isOverlay_ = isOverlay;
    isSystemResource_ = systemResource;
    resConfig_ = resKey->resConfig_;
    indexPath_ = resPath.first;
    resourcePath_ = resPath.second;
}

HapResource::ValueUnderQualifierDir::~ValueUnderQualifierDir()
{}

// IdValues
HapResource::IdValues::~IdValues()
{}

// HapResource
HapResource::HapResource(const std::string path, time_t lastModTime, std::shared_ptr<ResDesc> resDes,
    bool isSystem, bool isOverlay) : indexPath_(path), lastModTime_(lastModTime), resDesc_(resDes),
    isSystem_(isSystem), isOverlay_(isOverlay), isThemeSystemResEnable_(false)
{}

HapResource::~HapResource()
{
    lastModTime_ = 0;
}

const std::shared_ptr<HapResource> HapResource::Load(const char *path,
    std::shared_ptr<ResConfigImpl> &defaultConfig, bool isSystem, bool isOverlay, const uint32_t &selectedTypes)
{
    std::shared_ptr<HapResource> pResource;
    if (selectedTypes == SELECT_ALL) {
        pResource = HapResourceManager::GetInstance()->GetHapResource(path);
        if (pResource) {
            return pResource;
        }
    }
    if (Utils::ContainsTail(path, Utils::tailSet)) {
        pResource = LoadFromHap(path, defaultConfig, isSystem, isOverlay, selectedTypes);
    } else {
        pResource = LoadFromIndex(path, defaultConfig, isSystem, isOverlay, selectedTypes);
    }
    if (pResource != nullptr && selectedTypes == SELECT_ALL) {
        pResource = HapResourceManager::GetInstance()->PutAndGetResource(path, pResource);
    }
    return pResource;
}

const std::shared_ptr<HapResource> HapResource::LoadFromIndex(const char *path,
    std::shared_ptr<ResConfigImpl> &defaultConfig, bool isSystem, bool isOverlay, const uint32_t &selectedTypes)
{
    char outPath[PATH_MAX + 1] = {0};
    Utils::CanonicalizePath(path, outPath, PATH_MAX);
    std::ifstream inFile(outPath, std::ios::binary | std::ios::in);
    if (!inFile.good()) {
        return nullptr;
    }
    inFile.seekg(0, std::ios::end);
    int bufLen = inFile.tellg();
    if (bufLen <= 0) {
        RESMGR_HILOGE(RESMGR_TAG, "file size is zero");
        inFile.close();
        return nullptr;
    }
    void *buf = malloc(bufLen);
    if (buf == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "Error allocating memory");
        inFile.close();
        return nullptr;
    }
    inFile.seekg(0, std::ios::beg);
    inFile.read(static_cast<char *>(buf), bufLen);
    inFile.close();

    RESMGR_HILOGD(RESMGR_TAG, "extract success, bufLen:%d", bufLen);

    std::shared_ptr<ResDesc> resDesc = std::make_shared<ResDesc>();
    if (resDesc == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResDesc failed when LoadFromIndex");
        free(buf);
        return nullptr;
    }
    int32_t out = HapParser::ParseResHex(static_cast<char *>(buf), bufLen, *resDesc, defaultConfig, selectedTypes);
    if (out != OK) {
        free(buf);
        RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed! retcode:%d", out);
        return nullptr;
    }
    free(buf);

    std::shared_ptr<HapResource> pResource = std::make_shared<HapResource>(std::string(path),
        0, resDesc, isSystem, isOverlay);
    if (pResource == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new HapResource failed when LoadFromIndex");
        return nullptr;
    }
    if (!pResource->Init(defaultConfig)) {
        return nullptr;
    }
    return pResource;
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string GetIndexFilePath(std::shared_ptr<AbilityBase::Extractor> &extractor)
{
    std::string mName = HapParser::ParseModuleName(extractor);
    std::string indexFilePath = std::string("assets/");
    indexFilePath.append(mName);
    indexFilePath.append("/resources.index");
    return indexFilePath;
}
#endif

bool GetIndexData(const char *path, std::unique_ptr<uint8_t[]> &tmpBuf, size_t &len)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool isNewExtractor = false;
    std::shared_ptr<AbilityBase::Extractor> extractor = AbilityBase::ExtractorUtil::GetExtractor(path, isNewExtractor);
    if (extractor == nullptr) {
        return false;
    }
    std::string indexFilePath;
    if (extractor->IsStageModel()) {
        indexFilePath = "resources.index";
    } else {
        indexFilePath = GetIndexFilePath(extractor);
    }
    bool ret = extractor->ExtractToBufByName(indexFilePath, tmpBuf, len);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get buf data indexFilePath, %{public}s", indexFilePath.c_str());
        return false;
    }
#endif
    return true;
}

const std::shared_ptr<HapResource> HapResource::LoadFromHap(const char *path,
    std::shared_ptr<ResConfigImpl> &defaultConfig, bool isSystem, bool isOverlay, const uint32_t &selectedTypes)
{
    std::unique_ptr<uint8_t[]> tmpBuf;
    size_t tmpLen = 0;
    bool ret = GetIndexData(path, tmpBuf, tmpLen);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "read Index from file failed path, %{public}s", path);
        return nullptr;
    }
    std::shared_ptr<ResDesc> resDesc = std::make_shared<ResDesc>();
    if (resDesc == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResDesc failed when LoadFromHap");
        return nullptr;
    }
    int32_t out = HapParser::ParseResHex(
        reinterpret_cast<char *>(tmpBuf.get()), tmpLen, *resDesc, defaultConfig, selectedTypes);
    if (out != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed! retcode:%{public}d", out);
        return nullptr;
    }

    auto pResource = std::make_shared<HapResource>(path, 0, resDesc, isSystem, isOverlay);
    if (pResource == nullptr) {
        return nullptr;
    }

    if (!pResource->Init(defaultConfig)) {
        return nullptr;
    }
    return pResource;
}

const std::unordered_map<std::string, std::shared_ptr<HapResource>> HapResource::LoadOverlays(const std::string &path,
    const std::vector<std::string> &overlayPaths, std::shared_ptr<ResConfigImpl> &defaultConfig, bool isSystem)
{
    std::unordered_map<std::string, std::shared_ptr<HapResource>> result;
    do {
        const std::shared_ptr<HapResource> targetResource = Load(path.c_str(), defaultConfig, isSystem);
        if (targetResource == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "load target failed");
            break;
        }
        result[path] = targetResource;
        bool success = true;
        std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> mapping =
            targetResource->BuildNameTypeIdMapping();
        for (auto iter = overlayPaths.begin(); iter != overlayPaths.end(); iter++) {
            // load overlay hap, the isOverlay flag set true.
            const std::shared_ptr<HapResource> overlayResource = Load(iter->c_str(), defaultConfig, isSystem, true);
            if (overlayResource == nullptr) {
                success = false;
                break;
            }
            result[*iter] = overlayResource;
        }

        if (!success) {
            RESMGR_HILOGE(RESMGR_TAG, "load overlay failed");
            break;
        }

        for (auto iter = result.begin(); iter != result.end(); iter++) {
            auto index = iter->first.find(path);
            if (index == std::string::npos) {
                iter->second->UpdateOverlayInfo(mapping);
            }
        }
        return result;
    } while (false);

    result.clear();
    return std::unordered_map<std::string, std::shared_ptr<HapResource>>();
}

std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> HapResource::BuildNameTypeIdMapping() const
{
    std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> result;
    for (auto iter = idValuesMap_.begin(); iter != idValuesMap_.end(); iter++) {
        const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &limitPaths = iter->second->GetLimitPathsConst();
        if (limitPaths.size() > 0) {
            std::shared_ptr<ValueUnderQualifierDir> value = limitPaths[0];
            result[value->idItem_->name_][value->idItem_->resType_] = value->idItem_->id_;
        }
    }
    return result;
}

void HapResource::UpdateOverlayInfo(std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> &nameTypeId)
{
    std::map<uint32_t, std::shared_ptr<IdValues>> newIdValuesMap;
    for (auto iter = idValuesMap_.begin(); iter != idValuesMap_.end(); iter++) {
        const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &limitPaths = iter->second->GetLimitPathsConst();
        if (limitPaths.size() > 0) {
            std::shared_ptr<ValueUnderQualifierDir> value = limitPaths[0];
            std::string name = value->idItem_->name_;
            ResType type = value->idItem_->resType_;
            if (nameTypeId.find(name) == nameTypeId.end()) {
                continue;
            }
            auto &typeId = nameTypeId[name];
            if (typeId.find(type) == typeId.end()) {
                continue;
            }
            uint32_t newId = typeId[type];
            for_each(limitPaths.begin(), limitPaths.end(), [&](auto &item) {
                item->idItem_->id_ = newId;
            });
            newIdValuesMap[newId] = iter->second;
        }
    }
    idValuesMap_.swap(newIdValuesMap);
}

bool HapResource::Init(std::shared_ptr<ResConfigImpl> &defaultConfig)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif
#ifdef __WINNT__
    char separator = '\\';
#else
    char separator = '/';
#endif
    auto index = indexPath_.rfind(separator);
    if (index == std::string::npos) {
        RESMGR_HILOGE(RESMGR_TAG, "index path format error, %s", indexPath_.c_str());
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
        RESMGR_HILOGE(RESMGR_TAG, "index path format error, %s", indexPath_.c_str());
        return false;
    }
    resourcePath_ = indexPath_.substr(0, index + 1);
#endif
    for (int i = 0; i < ResType::MAX_RES_TYPE; ++i) {
        auto mptr = std::make_shared<std::map<std::string, std::shared_ptr<IdValues>>>();
        idValuesNameMap_.push_back(mptr);
    }
    return InitIdList(defaultConfig);
}

bool HapResource::InitMap(const std::shared_ptr<ResKey> &resKey, const std::pair<std::string, std::string> &resPath,
    std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    for (size_t j = 0; j < resKey->resId_->idParams_.size(); ++j) {
        std::shared_ptr<IdParam> idParam = resKey->resId_->idParams_[j];
        uint32_t id = idParam->id_;
        std::map<uint32_t, std::shared_ptr<IdValues>>::iterator iter = idValuesMap_.find(id);
        if (iter == idValuesMap_.end()) {
            auto idValues = std::make_shared<HapResource::IdValues>();
            if (idValues == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "new IdValues failed in HapResource::InitIdList");
                return false;
            }
            auto limitPath = std::make_shared<HapResource::ValueUnderQualifierDir>(resKey,
                idParam->idItem_, resPath, isOverlay_, isSystem_);
            if (limitPath == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "new ValueUnderQualifierDir failed in HapResource::InitIdList");
                return false;
            }
            idValues->AddLimitPath(limitPath);
            HapResource::IsAppDarkRes(limitPath, defaultConfig);
            idValuesMap_.insert(std::make_pair(id, idValues));
            std::string name = std::string(idParam->idItem_->name_);
            idValuesNameMap_[idParam->idItem_->resType_]->insert(std::make_pair(name, idValues));
            if (name == "system_color_change" && idParam->idItem_->value_ == "true") {
                isThemeSystemResEnable_ = true;
            }
        } else {
            std::shared_ptr<HapResource::IdValues> idValues = iter->second;
            auto limitPath = std::make_shared<HapResource::ValueUnderQualifierDir>(resKey,
                idParam->idItem_, resPath, isOverlay_, isSystem_);
            if (limitPath == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "new ValueUnderQualifierDir failed in HapResource::InitIdList");
                return false;
            }
            idValues->AddLimitPath(limitPath);
            HapResource::IsAppDarkRes(limitPath, defaultConfig);
        }
    }
    return true;
}

bool HapResource::InitIdList(std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    if (resDesc_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "resDesc_ is null ! InitIdList failed");
        return false;
    }
    const auto resPath = std::make_pair(indexPath_, resourcePath_);
    for (size_t i = 0; i < resDesc_->keys_.size(); i++) {
        const auto resKey = resDesc_->keys_[i];
        if (!HapResource::InitMap(resKey, resPath, defaultConfig)) {
            return false;
        }
    }
    return true;
};

const std::shared_ptr<HapResource::IdValues> HapResource::GetIdValues(const uint32_t id) const
{
    if (idValuesMap_.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "idValuesMap_ is empty");
        return nullptr;
    }
    uint32_t uid = id;
    std::map<uint32_t, std::shared_ptr<IdValues>>::const_iterator iter = idValuesMap_.find(uid);
    if (iter == idValuesMap_.end()) {
        return nullptr;
    }

    return iter->second;
}

const std::shared_ptr<HapResource::IdValues> HapResource::GetIdValuesByName(
    const std::string name, const ResType resType) const
{
    const auto map = idValuesNameMap_[resType];
    std::map<std::string, std::shared_ptr<IdValues>>::const_iterator iter = map->find(name);
    if (iter == map->end()) {
        return nullptr;
    }

    return iter->second;
}

int HapResource::GetIdByName(const char *name, const ResType resType) const
{
    if (name == nullptr) {
        return -1;
    }
    const auto map = idValuesNameMap_[resType];
    std::map<std::string, std::shared_ptr<IdValues>>::const_iterator iter = map->find(name);
    if (iter == map->end()) {
        return OBJ_NOT_FOUND;
    }
    const std::shared_ptr<IdValues> ids = iter->second;

    if (ids->GetLimitPathsConst().size() == 0) {
        RESMGR_HILOGE(RESMGR_TAG, "limitPaths empty");
        return UNKNOWN_ERROR;
    }

    if (ids->GetLimitPathsConst()[0]->GetIdItem()->resType_ != resType) {
        RESMGR_HILOGE(RESMGR_TAG, "ResType mismatch");
        return UNKNOWN_ERROR;
    }
    return ids->GetLimitPathsConst()[0]->GetIdItem()->id_;
}

const std::vector<std::string> HapResource::GetQualifiers() const
{
    std::vector<std::string> result;
    for (size_t i = 0; i < resDesc_->keys_.size(); i++) {
        result.push_back(resDesc_->keys_[i]->ToString());
    }
    return result;
}

uint32_t HapResource::GetResourceLimitKeys() const
{
    uint32_t limitKeyValue = 0;
    std::vector<bool> keyTypes(KeyType::KEY_TYPE_MAX, false);
    for (auto iter = idValuesMap_.begin(); iter != idValuesMap_.end(); iter++) {
        if (iter->second == nullptr) {
            continue;
        }
        const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &limitPaths = iter->second->GetLimitPathsConst();
        if (limitPaths.size() <= 0) {
            continue;
        }
        limitKeyValue |= GetLimitPathsKeys(limitPaths, keyTypes);
    }
    return limitKeyValue;
}

uint32_t HapResource::GetLimitPathsKeys(const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &limitPaths,
    std::vector<bool> &keyTypes) const
{
    uint32_t limitKeyValue = 0;
    const uint32_t limitKeysBase = 0x00000001;
    for_each(limitPaths.begin(), limitPaths.end(), [&](auto &item) {
        const std::vector<std::shared_ptr<KeyParam>> &keyParams = item->keyParams_;
        for_each(keyParams.begin(), keyParams.end(), [&](auto &keyParam) {
            uint32_t typeValue = static_cast<uint32_t>(keyParam->type_);
            if (keyParam->type_ < KeyType::KEY_TYPE_MAX && !keyTypes[typeValue]) {
                keyTypes[typeValue] = true;
                limitKeyValue |= limitKeysBase << typeValue;
            }
        });
    });
    return limitKeyValue;
}

void HapResource::GetLocales(std::set<std::string> &outValue, bool includeSystem)
{
    if ((!includeSystem && isSystem_) || (!isSystem_ && isOverlay_)) {
        return;
    }
    for (size_t i = 0; i < resDesc_->keys_.size(); i++) {
        GetKeyParamsLocales(resDesc_->keys_[i]->keyParams_, outValue);
    }
}

void HapResource::GetKeyParamsLocales(const std::vector<std::shared_ptr<KeyParam>> keyParams,
    std::set<std::string> &outValue)
{
    std::string locale;
    bool isLocale = false;
    for (size_t i = 0; i < keyParams.size(); i++) {
        KeyType keyType = keyParams[i]->type_;
        if (keyType == KeyType::MCC || keyType == KeyType::MNC) {
            continue;
        }
        if (keyType == KeyType::LANGUAGES) {
            locale = keyParams[i]->GetStr();
            isLocale = true;
            continue;
        }
        if (keyType == KeyType::SCRIPT) {
            locale.append("-");
            locale.append(keyParams[i]->GetStr());
            continue;
        }
        if (keyType == KeyType::REGION) {
            locale.append("-");
            locale.append(keyParams[i]->GetStr());
            break;
        }
        break;
    }
    if (isLocale) {
        outValue.emplace(locale);
    }
}
bool HapResource::IsThemeSystemResEnable() const
{
    return this->isThemeSystemResEnable_;
}

void HapResource::IsAppDarkRes(const std::shared_ptr<HapResource::ValueUnderQualifierDir> &limitPath,
    std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    if (!defaultConfig) {
        return;
    }
    if (isSystem_ || isOverlay_ || defaultConfig->GetAppDarkRes()) {
        return;
    }
    std::string folder = limitPath->GetFolder();
    if (folder.find("dark") != std::string::npos) {
        defaultConfig->SetAppDarkRes(true);
        hasDarkRes_ = true;
    }
}

bool HapResource::HasDarkRes()
{
    return hasDarkRes_;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
