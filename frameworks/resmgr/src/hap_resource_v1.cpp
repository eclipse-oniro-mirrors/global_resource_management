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

#include "hap_resource_v1.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <fstream>
#include "utils/utils.h"
#include <set>
#ifdef __WINNT__
#include <shlwapi.h>
#include <windows.h>
#undef GetLocaleInfo
#endif

#ifdef __LINUX__
#include <cstring>
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#include "file_mapper.h"
#include "extractor.h"
#endif

#include "hap_parser_v1.h"
#include "hap_resource_manager.h"
#include "hilog_wrapper.h"
#include "utils/errors.h"

using ReadLock = std::shared_lock<std::shared_mutex>;
using WriteLock = std::unique_lock<std::shared_mutex>;

namespace OHOS {
namespace Global {
namespace Resource {
ValueUnderQualifierDirV1::ValueUnderQualifierDirV1(const std::shared_ptr<ResKey> &resKey,
    const std::shared_ptr<IdItem> &idItem, const std::pair<std::string, std::string> &resPath, bool isOverlay,
    bool systemResource) : ValueUnderQualifierDir(resPath, resKey->resConfig_, isOverlay, systemResource),
    idItem_(idItem)
{}

ValueUnderQualifierDirV1::~ValueUnderQualifierDirV1()
{}

std::shared_ptr<IdItem> ValueUnderQualifierDirV1::GetIdItem() const
{
    return idItem_;
}

IdValuesV1::~IdValuesV1()
{}

const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &IdValuesV1::GetLimitPathsConst() const
{
    return limitPaths_;
}

HapResourceV1::HapResourceV1(const std::string path, time_t lastModTime, std::shared_ptr<ResDesc> resDes,
    bool isSystem, bool isOverlay) : HapResource(path, lastModTime), resDesc_(resDes),
    isSystem_(isSystem), isOverlay_(isOverlay)
{}

HapResourceV1::~HapResourceV1()
{}

bool HapResourceV1::Init(std::shared_ptr<ResConfigImpl> &defaultConfig)
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
        auto mptr = std::make_shared<std::map<std::string, std::shared_ptr<IdValuesV1>>>();
        idValuesNameMap_.push_back(mptr);
    }
    WriteLock lock(mutex_);
    return InitIdList(defaultConfig);
}

bool HapResourceV1::IsSystemResource() const
{
    return isSystem_;
}

bool HapResourceV1::IsOverlayResource() const
{
    return isOverlay_;
}

const std::shared_ptr<IdValues> HapResourceV1::GetIdValues(const uint32_t id)
{
    ReadLock lock(mutex_);
    if (idValuesMap_.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "idValuesMap_ is empty");
        return nullptr;
    }
    uint32_t uid = id;
    std::map<uint32_t, std::shared_ptr<IdValuesV1>>::const_iterator iter = idValuesMap_.find(uid);
    if (iter == idValuesMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

const std::shared_ptr<IdValues> HapResourceV1::GetIdValuesByName(
    const std::string name, const ResType resType)
{
    ReadLock lock(mutex_);
    const auto map = idValuesNameMap_[resType];
    std::map<std::string, std::shared_ptr<IdValuesV1>>::const_iterator iter = map->find(name);
    if (iter == map->end()) {
        return nullptr;
    }
    return iter->second;
}

RState HapResourceV1::Update(std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    return this->UpdateResConfig(defaultConfig);
}

std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> HapResourceV1::BuildNameTypeIdMapping()
{
    ReadLock lock(mutex_);
    std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> result;
    for (auto iter = idValuesMap_.begin(); iter != idValuesMap_.end(); iter++) {
        const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &limitPaths = iter->second->GetLimitPathsConst();
        if (limitPaths.size() > 0) {
            std::shared_ptr<ValueUnderQualifierDirV1> value =
                std::static_pointer_cast<ValueUnderQualifierDirV1>(limitPaths[0]);
            result[value->idItem_->name_][value->idItem_->resType_] = value->idItem_->id_;
        }
    }
    return result;
}

void HapResourceV1::GetLocales(std::set<std::string> &outValue, bool includeSystem)
{
    ReadLock lock(mutex_);
    if (resDesc_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "resDesc_ is null! GetLocales failed");
        return;
    }
    if ((!includeSystem && isSystem_) || (!isSystem_ && isOverlay_)) {
        return;
    }
    outValue.insert(locales_.begin(), locales_.end());
}

void HapResourceV1::UpdateOverlayInfo(
    std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> &nameTypeId)
{
    WriteLock lock(mutex_);
    std::map<uint32_t, std::shared_ptr<IdValuesV1>> newIdValuesMap;
    for (auto iter = idValuesMap_.begin(); iter != idValuesMap_.end(); iter++) {
        const std::vector<std::shared_ptr<ValueUnderQualifierDir>> &limitPaths = iter->second->GetLimitPathsConst();
        if (limitPaths.size() > 0) {
            std::shared_ptr<ValueUnderQualifierDirV1> value =
                std::static_pointer_cast<ValueUnderQualifierDirV1>(limitPaths[0]);
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
                item->GetIdItem()->id_ = newId;
            });
            newIdValuesMap[newId] = iter->second;
        }
    }
    idValuesMap_.swap(newIdValuesMap);
}

bool HapResourceV1::InitIdList(std::shared_ptr<ResConfigImpl> &defaultConfig)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
#endif
    if (resDesc_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "resDesc_ is null ! InitIdList failed");
        return false;
    }
    const auto resPath = std::make_pair(indexPath_, resourcePath_);
    for (size_t i = 0; i < resDesc_->keys_.size(); i++) {
        const auto resKey = resDesc_->keys_[i];
        if (!InitMap(resKey, resPath, defaultConfig)) {
            return false;
        }
    }
#ifdef SUPPORT_GRAPHICS
    if (defaultConfig && (defaultConfig->GetPreferredLocaleInfo() || defaultConfig->GetLocaleInfo())) {
        std::shared_ptr<ResConfigImpl> currentConfig = std::make_shared<ResConfigImpl>();
        currentConfig->Copy(*defaultConfig);
        loadedConfig_.insert(currentConfig);
    }
#endif
    return true;
};

bool HapResourceV1::InitMap(const std::shared_ptr<ResKey> &resKey, const std::pair<std::string, std::string> &resPath,
    std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    for (size_t j = 0; j < resKey->resId_->idParams_.size(); ++j) {
        std::shared_ptr<IdParam> idParam = resKey->resId_->idParams_[j];
        uint32_t id = idParam->id_;
        std::map<uint32_t, std::shared_ptr<IdValuesV1>>::iterator iter = idValuesMap_.find(id);
        if (iter == idValuesMap_.end()) {
            auto idValues = std::make_shared<IdValuesV1>();
            if (idValues == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "new IdValues failed in HapResource::InitIdList");
                return false;
            }
            auto limitPath = std::make_shared<ValueUnderQualifierDirV1>(resKey,
                idParam->idItem_, resPath, isOverlay_, isSystem_);
            if (limitPath == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "new ValueUnderQualifierDir failed in HapResource::InitIdList");
                return false;
            }
            idValues->AddLimitPath(limitPath);
            IsAppDarkRes(limitPath, defaultConfig);
            idValuesMap_.insert(std::make_pair(id, idValues));
            std::string name = std::string(idParam->idItem_->name_);
            idValuesNameMap_[idParam->idItem_->resType_]->insert(std::make_pair(name, idValues));
            if (name == "system_color_change" && idParam->idItem_->value_ == "true") {
                isThemeSystemResEnable_ = true;
            }
        } else {
            std::shared_ptr<IdValuesV1> idValues = iter->second;
            auto limitPath = std::make_shared<ValueUnderQualifierDirV1>(resKey,
                idParam->idItem_, resPath, isOverlay_, isSystem_);
            if (limitPath == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "new ValueUnderQualifierDir failed in HapResource::InitIdList");
                return false;
            }
            idValues->AddLimitPath(limitPath);
            IsAppDarkRes(limitPath, defaultConfig);
        }
    }
    return true;
}

void HapResourceV1::IsAppDarkRes(const std::shared_ptr<ValueUnderQualifierDir> &limitPath,
    std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    if (!defaultConfig) {
        return;
    }
    if (isSystem_ || isOverlay_ || defaultConfig->GetAppDarkRes()) {
        return;
    }

    if (limitPath->GetResConfig()->GetColorMode() == ColorMode::DARK) {
        defaultConfig->SetAppDarkRes(true);
        hasDarkRes_ = true;
    }
}

RState HapResourceV1::UpdateResConfig(std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    if (isSystem_ || isOverlay_ || !defaultConfig) {
        return SUCCESS;
    }
#ifdef SUPPORT_GRAPHICS
    if (!defaultConfig->GetPreferredLocaleInfo() && !defaultConfig->GetLocaleInfo()) {
        return SUCCESS;
    }
#endif
    WriteLock lock(mutex_);
    for (auto &config : loadedConfig_) {
        if (defaultConfig->MatchLocal(*config)) {
            return SUCCESS;
        }
    }

    HapParserV1 hapParser(defaultConfig, selectedTypes_, false, true);
    resDesc_ = std::make_shared<ResDesc>();
    hapParser.SetResDesc(resDesc_);
    if (!hapParser.Init(indexPath_.c_str())) {
        return HAP_INIT_FAILED;
    }
    if (!InitIdList(defaultConfig)) {
        return HAP_INIT_FAILED;
    }
    return SUCCESS;
}
}
}
}