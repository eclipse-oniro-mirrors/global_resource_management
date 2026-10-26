/*
 * Copyright (c) 2021-2026 Huawei Device Co., Ltd.
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

#include "resource_manager_impl.h"

#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <regex>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>

#include "brand_color_manager.h"
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#endif
#include "hilog_wrapper.h"
#include "res_config.h"
#include "securec.h"
#include "system_resource_manager.h"
#include "utils/common.h"
#include "utils/string_utils.h"
#include "utils/utils.h"
#include "tuple"
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "parameter.h"
#endif
#include "theme_pack_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
// default logLevel
#ifdef CONFIG_HILOG
LogLevel g_logLevel = LOG_INFO;
#endif

constexpr int HEX_ADECIMAL = 16;
const std::string FOREGROUND = "foreground";
const std::string BACKGROUND = "background";
const std::regex FLOAT_REGEX = std::regex("(\\+|-)?\\d+(\\.\\d+)? *(px|vp|fp)?");
const char* ResourceManagerImpl::LANGUAGE_KEY = "persist.global.language";

void ResourceManagerImpl::AddSystemResource(ResourceManagerImpl *systemResourceManager)
{
    if (systemResourceManager != nullptr) {
        this->hapManager_->AddSystemResource(systemResourceManager->hapManager_);
    }
}

bool ResourceManagerImpl::AddSystemResource(const std::shared_ptr<ResourceManagerImpl> &systemResourceManager)
{
    if (systemResourceManager != nullptr) {
        this->systemResourceManager_ = systemResourceManager;
        this->hapManager_->AddSystemResource(systemResourceManager->hapManager_);
        return true;
    }
    return false;
}

ResourceManagerImpl::ResourceManagerImpl(bool isOverrideResMgr) : hapManager_(nullptr),
    isOverrideResMgr_(isOverrideResMgr)
{
    psueManager_ = std::make_shared<PsueManager>();
}

bool ResourceManagerImpl::Init(bool isSystem)
{
    auto resConfig = std::make_shared<ResConfigImpl>();
    if (resConfig == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResConfigImpl failed when ResourceManagerImpl::Init");
        return false;
    }
    if (isSystem) {
        resConfig->SetDeviceType(ResConfigImpl::ParseDeviceTypeStr(ResConfigImpl::GetCurrentDeviceType()));
    }
    hapManager_ = std::make_shared<HapManager>(resConfig, isSystem);
    if (hapManager_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new HapManager failed when ResourceManagerImpl::Init");
        return false;
    }
    hapManager_->SetOverride(isOverrideResMgr_);
    isSystemResMgr_ = isSystem;
    BrandColorManager::GetInstance().LoadConfig();
    return true;
}

bool ResourceManagerImpl::Init(std::shared_ptr<HapManager> hapManager)
{
    if (hapManager == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "ResourceManagerImpl::Init, hapManager is nullptr");
        return false;
    }
    auto resConfig = std::make_shared<ResConfigImpl>();
    if (resConfig == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "new ResConfigImpl failed when ResourceManagerImpl::Init");
        return false;
    }
    hapManager->GetResConfig(*resConfig);
    hapManager_ = std::make_shared<HapManager>(resConfig, hapManager->GetHapResource(),
        hapManager->GetLoadedHapPaths(), hapManager->IsSystem());
    if (hapManager_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new HapManager failed from hapManager");
        return false;
    }
    hapManager_->SetOverride(isOverrideResMgr_);
    hapManager_->UpdateAppConfigForSysResManager(
        resConfig->GetAppDarkRes(), hapManager->IsThemeSystemResEnableHap());
    BrandColorManager::GetInstance().LoadConfig();
    return true;
}

RState ResourceManagerImpl::GetPatternById(uint32_t id, std::map<std::string, std::string> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetPatternById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetPattern(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetPatternDataById(uint32_t id, std::map<std::string, ResData> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetPatternDataById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetPatternData(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetPatternByName(const char *name, std::map<std::string, std::string> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::PATTERN, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetPatternByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetPattern(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetPatternDataByName(const char *name, std::map<std::string, ResData> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::PATTERN, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetPatternDataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetPatternData(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetPattern(const std::shared_ptr<IdItem> idItem, std::map<std::string,
    std::string> &outValue)
{
    //type invalid
    if (idItem->resType_ != ResType::PATTERN) {
        RESMGR_HILOGE(RESMGR_TAG,
            "actual resType = %{public}d, expect resType = %{public}d", idItem->resType_, ResType::PATTERN);
        return NOT_FOUND;
    }
    return ResolveParentReference(idItem, outValue);
}

RState ResourceManagerImpl::GetPatternData(const std::shared_ptr<IdItem> idItem,
    std::map<std::string, ResData> &outValue)
{
    //type invalid
    if (idItem->resType_ != ResType::PATTERN) {
        RESMGR_HILOGE(RESMGR_TAG,
            "actual resType = %{public}d, expect resType = %{public}d", idItem->resType_, ResType::PATTERN);
        return NOT_FOUND;
    }
    return ResolveResData(idItem, outValue);
}

RState ResourceManagerImpl::ResolveReference(const std::string value, std::string &outValue)
{
    uint32_t id;
    ResType resType;
    bool isRef = true;
    int count = 0;
    std::string refStr(value);
    std::vector<std::shared_ptr<IdItem>> idItems;
    while (isRef) {
        isRef = IdItem::IsRef(refStr, resType, id);
        if (!isRef || resType == ResType::PLURALS) {
            outValue = refStr;
            ApplyBrandColorByIdItems(idItems, outValue);
            return SUCCESS;
        }

        if (IdItem::IsArrayOfType(resType)) {
            RESMGR_HILOGD(RESMGR_TAG, "ref %{public}s can't be array", refStr.c_str());
            return ERROR;
        }
        const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
        if (idItem == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "ref %s id not found", refStr.c_str());
            return ERROR;
        }
        if (resType != idItem->resType_) {
            RESMGR_HILOGE(RESMGR_TAG,
                "impossible. ref %s type mismatch, found type: %d", refStr.c_str(), idItem->resType_);
            return ERROR;
        }

        idItems.emplace_back(idItem);
        refStr = idItem->value_;

        if (++count > MAX_DEPTH_REF_SEARCH) {
            RESMGR_HILOGE(RESMGR_TAG, "ref %s has re-ref too much", value.c_str());
            return ERROR_CODE_RES_REF_TOO_MUCH;
        }
    }
    return SUCCESS;
}

RState ResourceManagerImpl::ResolveDataReference(const std::string key, const std::string value,
    std::map<std::string, ResData> &outValue)
{
    uint32_t id;
    ResType resType = ResType::MAX_RES_TYPE;
    bool isRef = true;
    int count = 0;
    std::string refStr(value);
    std::vector<std::shared_ptr<IdItem>> idItems;
    while (isRef) {
        ResType realType = resType;
        isRef = IdItem::IsRef(refStr, resType, id);
        if (!isRef) {
            if (realType == ResType::MAX_RES_TYPE) {
                realType = ResType::STRING;
            }
            std::string resolvedValue = refStr;
            ApplyBrandColorByIdItems(idItems, resolvedValue);
            outValue[key] = { .resType = realType, .value = resolvedValue };
            return SUCCESS;
        }

        if (IdItem::IsArrayOfType(resType)) {
            // can't be array
            outValue[key] = { .resType = resType, .value = std::to_string(id) };
            return SUCCESS;
        }
        const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
        if (idItem == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "ref %s id not found", refStr.c_str());
            return ERROR;
        }
        if (resType != idItem->resType_) {
            RESMGR_HILOGE(RESMGR_TAG,
                "impossible. ref %s type mismatch, found type: %d", refStr.c_str(), idItem->resType_);
            return ERROR;
        }

        idItems.emplace_back(idItem);
        refStr = idItem->value_;

        if (++count > MAX_DEPTH_REF_SEARCH) {
            RESMGR_HILOGE(RESMGR_TAG, "ref %s has re-ref too much", value.c_str());
            return ERROR_CODE_RES_REF_TOO_MUCH;
        }
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetThemeValues(const std::string &value, std::string &outValue,
    const ResConfigImpl &resConfig)
{
    std::vector<std::shared_ptr<IdItem>> idItems;
    if (ProcessReference(value, idItems) != SUCCESS) {
        return NOT_FOUND;
    }
    outValue = ThemePackManager::GetThemePackManager()->FindThemeResource(bundleInfo, idItems, resConfig, userId,
        hapManager_->IsThemeSystemResEnableHap());
    if (outValue.empty()) {
        return NOT_FOUND;
    }
    // If the reference chain contains a COLOR resource, apply brand color
    // override using the first name in the brand color list found in the
    // chain (similar to FindThemeResource which returns the first match).
    ApplyBrandColorByIdItems(idItems, outValue);
    return SUCCESS;
}

RState ResourceManagerImpl::ResolveParentReference(const std::shared_ptr<IdItem> idItem, std::map<std::string,
    std::string> &outValue)
{
    // only pattern and theme
    // ref always at idx 0
    // child will cover parent
    outValue.clear();

    bool haveParent = false;
    int count = 0;
    std::shared_ptr<IdItem> currItem = idItem;
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    do {
        haveParent = currItem->HaveParent();
        size_t startIdx = haveParent ? 1 : 0;
        // add currItem values into map when key is absent
        // this make sure child covers parent
        size_t loop = currItem->values_.size() / 2;
        for (size_t i = 0; i < loop; ++i) {
            std::string key(currItem->values_[startIdx + i * 2]); // 2 means key appear in pairs
            std::string value(currItem->values_[startIdx + i * 2 + 1]); // 2 means value appear in pairs
            if (outValue.find(key) != outValue.end()) {
                continue;
            }
            std::string resolvedValue;
            if (GetThemeValues(value, resolvedValue, resConfig) == SUCCESS) {
                outValue[key] = resolvedValue;
                continue;
            }
            if (ResolveReference(value, resolvedValue) != SUCCESS) {
                RESMGR_HILOGD(RESMGR_TAG, "ResolveReference failed, value:%{public}s", value.c_str());
                return ERROR;
            }
            outValue[key] = resolvedValue;
        }
        if (haveParent) {
            // get parent
            uint32_t id;
            ResType resType;
            if (!IdItem::IsRef(currItem->values_[0], resType, id)) {
                RESMGR_HILOGE(RESMGR_TAG,
                    "something wrong, pls check HaveParent(). idItem: %{public}s", idItem->ToString().c_str());
                return ERROR;
            }
            currItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
            if (currItem == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "ref %s id not found", idItem->values_[0].c_str());
                return ERROR;
            }
        }

        if (++count > MAX_DEPTH_REF_SEARCH) {
            RESMGR_HILOGE(RESMGR_TAG, " %u has too many parents", idItem->id_);
            return ERROR;
        }
    } while (haveParent);
    return SUCCESS;
}

RState ResourceManagerImpl::ProcessItem(std::shared_ptr<IdItem> idItem,
    std::map<std::string, ResData> &outValue)
{
    size_t startIdx = idItem->HaveParent() ? 1 : 0;
    // add currItem values into map when key is absent
    // this make sure child covers parent
    size_t loop = idItem->values_.size() / 2;
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    for (size_t i = 0; i < loop; ++i) {
        std::string key(idItem->values_[startIdx + i * 2]); // 2 means key appear in pairs
        std::string value(idItem->values_[startIdx + i * 2 + 1]); // 2 means value appear in pairs
        auto iter = outValue.find(key);
        if (iter != outValue.end()) {
            continue;
        }
        std::string resolvedValue;
        ResType resType = ResType::STRING;
        uint32_t id;
        bool isRef = IdItem::IsRef(value, resType, id);
        if (isRef && GetThemeValues(value, resolvedValue, resConfig) == SUCCESS) {
            outValue[key] = { .resType = resType, .value = resolvedValue };
            continue;
        }
        RState rrRet = ResolveDataReference(key, value, outValue);
        if (rrRet != SUCCESS) {
            RESMGR_HILOGD(RESMGR_TAG, "ResolveReference failed, value:%{public}s", value.c_str());
            return ERROR;
        }
    }
    return SUCCESS;
}

RState ResourceManagerImpl::ResolveResData(const std::shared_ptr<IdItem> idItem,
    std::map<std::string, ResData> &outValue)
{
    // only pattern and theme
    // ref always at idx 0
    // child will cover parent
    outValue.clear();

    bool haveParent = false;
    int count = 0;
    std::shared_ptr<IdItem> currItem = idItem;
    do {
        RState ret = ProcessItem(currItem, outValue);
        if (ret != SUCCESS) {
            outValue.clear();
            return ret;
        }
        haveParent = currItem->HaveParent();
        if (haveParent) {
            // get parent
            uint32_t id;
            ResType resType;
            bool isRef = IdItem::IsRef(currItem->values_[0], resType, id);
            if (!isRef) {
                RESMGR_HILOGE(RESMGR_TAG,
                    "something wrong, pls check HaveParent(). idItem: %{public}s", idItem->ToString().c_str());
                outValue.clear();
                return ERROR;
            }
            currItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
            if (currItem == nullptr) {
                RESMGR_HILOGE(RESMGR_TAG, "ref %s id not found", idItem->values_[0].c_str());
                outValue.clear();
                return ERROR;
            }
        }

        if (++count > MAX_DEPTH_REF_SEARCH) {
            RESMGR_HILOGE(RESMGR_TAG, " %u has too many parents", idItem->id_);
            outValue.clear();
            return ERROR;
        }
    } while (haveParent);

    return SUCCESS;
}

RState ResourceManagerImpl::GetBooleanById(uint32_t id, bool &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetBooleanById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetBoolean(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetBooleanByName(const char *name, bool &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::BOOLEAN, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetBooleanByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetBoolean(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetBoolean(const std::shared_ptr<IdItem> idItem, bool &outValue)
{
    if (idItem == nullptr || idItem->resType_ != ResType::BOOLEAN) {
        return NOT_FOUND;
    }
    std::string temp;
    RState state = ResolveReference(idItem->value_, temp);
    if (state == SUCCESS) {
        if (strcmp(temp.c_str(), "true") == 0) {
            outValue = true;
            return SUCCESS;
        }
        if (strcmp(temp.c_str(), "false") == 0) {
            outValue = false;
            return SUCCESS;
        }
        return ERROR;
    }
    return state;
}

RState ResourceManagerImpl::GetThemeFloat(const std::shared_ptr<IdItem> idItem, float &outValue)
{
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    std::vector<std::shared_ptr<IdItem>> idItems;
    idItems.emplace_back(idItem);
    ProcessReference(idItem->value_, idItems);
    std::string result = ThemePackManager::GetThemePackManager()->FindThemeResource(
        bundleInfo, idItems, resConfig, userId);
    if (result.empty()) {
        return NOT_FOUND;
    }
    std::string unit;
    RState state = ParseFloat(result.c_str(), outValue, unit);
    return state == SUCCESS ? RecalculateFloat(unit, outValue) : state;
}

RState ResourceManagerImpl::GetThemeFloat(const std::shared_ptr<IdItem> idItem, float &outValue, std::string &unit)
{
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    std::vector<std::shared_ptr<IdItem>> idItems;
    idItems.emplace_back(idItem);
    ProcessReference(idItem->value_, idItems);
    std::string result = ThemePackManager::GetThemePackManager()->FindThemeResource(
        bundleInfo, idItems, resConfig, userId);
    if (result.empty()) {
        return NOT_FOUND;
    }
    RState state = ParseFloat(result.c_str(), outValue, unit);
    return state == SUCCESS ? RecalculateFloat(unit, outValue) : state;
}

RState ResourceManagerImpl::GetFloatById(uint32_t id, float &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFloatById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }

    // find in theme pack
    if (GetThemeFloat(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    std::string unit;
    RState state = GetFloat(idItem, outValue, unit);
    if (state == SUCCESS) {
        return RecalculateFloat(unit, outValue);
    }
    if (state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetFloatById(uint32_t id, float &outValue, std::string &unit)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFloatById error with unit id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }

    // find in theme pack
    if (GetThemeFloat(idItem, outValue, unit) == SUCCESS) {
        return SUCCESS;
    }

    RState state = GetFloat(idItem, outValue, unit);
    if (state == SUCCESS) {
        return state;
    }
    if (state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetFloatByName(const char *name, float &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::FLOAT, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetFloatByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }

    // find in theme pack
    if (GetThemeFloat(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    std::string unit;
    RState state = GetFloat(idItem, outValue, unit);
    if (state == SUCCESS) {
        return RecalculateFloat(unit, outValue);
    }
    if (state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetFloatByName(const char *name, float &outValue, std::string &unit)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::FLOAT, isOverrideResMgr_);
    return GetFloat(idItem, outValue, unit);
}

RState ResourceManagerImpl::RecalculateFloat(const std::string &unit, float &result)
{
    ResConfigImpl rc;
    GetResConfig(rc);
    float density = rc.GetScreenDensity();
    if (density == SCREEN_DENSITY_NOT_SET) {
        RESMGR_HILOGD(RESMGR_TAG, "RecalculateFloat srcDensity SCREEN_DENSITY_NOT_SET ");
        return SUCCESS;
    }
    if (unit == VIRTUAL_PIXEL) {
        result = result * density;
    } else if (unit == FONT_SIZE_PIXEL) {
        float fontSizeDensity = density * ((fabs(fontRatio_) <= 1E-6) ? 1.0f : fontRatio_);
        result = result * fontSizeDensity;
    } else {
        // no unit
    }
    return SUCCESS;
}

RState ResourceManagerImpl::ParseFloat(const std::string &strValue, float &result, std::string &unit)
{
    std::smatch floatMatch;
    if (!regex_search(strValue, floatMatch, FLOAT_REGEX)) {
        RESMGR_HILOGD(RESMGR_TAG, "not valid float value %{public}s", strValue.c_str());
        return ERROR;
    }
    std::string matchString(floatMatch.str());
    if (floatMatch.size() < 1) {
        return ERROR;
    }
    unit = floatMatch[floatMatch.size() - 1];
    std::istringstream stream(matchString.substr(0, matchString.length() - unit.length()));
    stream >> result;
    return SUCCESS;
}

RState ResourceManagerImpl::GetFloat(const std::shared_ptr<IdItem> idItem, float &outValue, std::string &unit)
{
    if (idItem == nullptr || idItem->resType_ != ResType::FLOAT) {
        return NOT_FOUND;
    }
    std::string temp;
    RState state = ResolveReference(idItem->value_, temp);
    if (state == SUCCESS) {
        return ParseFloat(temp.c_str(), outValue, unit);
    }
    return state;
}

RState ResourceManagerImpl::GetIntegerById(uint32_t id, int &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetIntegerById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetInteger(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetIntegerByName(const char *name, int &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::INTEGER, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetIntegerByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetInteger(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetInteger(const std::shared_ptr<IdItem> idItem, int &outValue)
{
    if (idItem == nullptr || idItem->resType_ != ResType::INTEGER) {
        return NOT_FOUND;
    }
    std::string temp;
    RState state = ResolveReference(idItem->value_, temp);
    if (state != SUCCESS) {
        return state;
    }
    int intValue;
    if (Utils::convertToInteger(temp, intValue)) {
        outValue = intValue;
        return SUCCESS;
    }
    return ERROR;
}

RState ResourceManagerImpl::ProcessReference(const std::string value,
    std::vector<std::shared_ptr<IdItem>> &idItems)
{
    uint32_t id;
    ResType resType;
    bool isRef = true;
    int count = 0;
    std::string refStr(value);
    while (isRef) {
        isRef = IdItem::IsRef(refStr, resType, id);
        if (!isRef || resType == ResType::PLURALS) {
            return SUCCESS;
        }

        if (IdItem::IsArrayOfType(resType)) {
            // can't be array
            RESMGR_HILOGD(RESMGR_TAG, "ref %{public}s can't be array", refStr.c_str());
            return ERROR;
        }
        const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
        if (idItem == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "ref %s id not found", refStr.c_str());
            return ERROR;
        }
        idItems.emplace_back(idItem);
        // unless compile bug
        if (resType != idItem->resType_) {
            RESMGR_HILOGE(RESMGR_TAG,
                "impossible. ref %s type mismatch, found type: %d", refStr.c_str(), idItem->resType_);
            return ERROR;
        }

        refStr = idItem->value_;

        if (++count > MAX_DEPTH_REF_SEARCH) {
            RESMGR_HILOGE(RESMGR_TAG, "ref %s has re-ref too much", value.c_str());
            return ERROR_CODE_RES_REF_TOO_MUCH;
        }
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetThemeColor(const std::shared_ptr<IdItem> idItem, uint32_t &outValue)
{
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    std::vector<std::shared_ptr<IdItem> > idItems;
    idItems.emplace_back(idItem);
    RState state = ProcessReference(idItem->value_, idItems);
    std::string result = ThemePackManager::GetThemePackManager()->FindThemeResource(bundleInfo, idItems, resConfig,
        userId, hapManager_->IsThemeSystemResEnableHap());
    if (result.empty()) {
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    // Apply brand color using the first name in the brand color list found
    // in the reference chain (similar to FindThemeResource which returns the
    // first match).
    ApplyBrandColorByIdItems(idItems, result);
    return state == SUCCESS ? Utils::ConvertColorToUInt32(result.c_str(), outValue) : state;
}

bool ResourceManagerImpl::ApplyBrandColor(const std::string &name, uint32_t &outValue)
{
    if (!hasBrandColor_.load(std::memory_order_relaxed)) {
        return false;
    }
    uint32_t brandColor = brandColor_.load(std::memory_order_relaxed);
    return BrandColorManager::GetInstance().ApplyBrandColor(name, brandColor, outValue);
}

void ResourceManagerImpl::ApplyBrandColorByIdItems(const std::vector<std::shared_ptr<IdItem>> &idItems,
    std::string &colorStr)
{
    if (idItems.empty() || colorStr.empty()) {
        return;
    }
    if (idItems.back()->resType_ != ResType::COLOR) {
        return;
    }
    // Find the first name in the reference chain that is in the brand color
    // list (similar to FindThemeResource which returns the first match).
    // Falls back to the last name if none is in the list.
    std::string applyName;
    for (const auto &item : idItems) {
        if (item == nullptr || item->resType_ != ResType::COLOR) {
            continue;
        }
        if (BrandColorManager::GetInstance().IsBrandColorName(item->name_)) {
            applyName = item->name_;
            break;
        }
    }
    if (applyName.empty()) {
        applyName = idItems.back()->name_;
    }
    // Convert color string to uint32, apply brand color override, convert back.
    // Only normalize the format when brand color was actually applied; keep
    // the original format otherwise (e.g. "#F5F5F5" stays "#F5F5F5").
    uint32_t color = 0;
    if (Utils::ConvertColorToUInt32(colorStr.c_str(), color) != SUCCESS) {
        return;
    }
    if (!ApplyBrandColor(applyName, color)) {
        return;
    }
    char buf[16] = {0};
    if (snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "#%08X", color) < 0) {
        return;
    }
    colorStr = buf;
}

RState ResourceManagerImpl::SetBrandColor(const std::string &brandColor)
{
    if (brandColor.empty()) {
        hasBrandColor_.store(false, std::memory_order_relaxed);
        brandColor_.store(0, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(brandColorMutex_);
        brandColorStr_.clear();
        return SUCCESS;
    }
    uint32_t color = 0;
    RState state = Utils::ConvertColorToUInt32(brandColor.c_str(), color);
    if (state != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "SetBrandColor invalid format: %{public}s", brandColor.c_str());
        return state;
    }
    brandColor_.store(color, std::memory_order_relaxed);
    hasBrandColor_.store(true, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lock(brandColorMutex_);
    brandColorStr_ = brandColor;
    RESMGR_HILOGI(RESMGR_TAG, "brand color set to 0x%{public}08x", color);
    return SUCCESS;
}

std::string ResourceManagerImpl::GetBrandColor() const
{
    if (!hasBrandColor_.load(std::memory_order_relaxed)) {
        return "";
    }
    std::lock_guard<std::mutex> lock(brandColorMutex_);
    return brandColorStr_;
}

RState ResourceManagerImpl::GetColorById(uint32_t id, uint32_t &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetColorById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }

    // find in theme pack
    if (GetThemeColor(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    RState state = GetColor(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    // Only apply brand color override when outValue is actually populated.
    if (state == SUCCESS) {
        ApplyBrandColor(idItem->name_, outValue);
    }
    return state;
}

RState ResourceManagerImpl::GetColorByName(const char *name, uint32_t &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::COLOR, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetColorByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }

    // find in theme pack
    if (GetThemeColor(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    RState state = GetColor(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    // Only apply brand color override when outValue is actually populated.
    if (state == SUCCESS) {
        ApplyBrandColor(idItem->name_, outValue);
    }
    return state;
}

RState ResourceManagerImpl::GetColor(const std::shared_ptr<IdItem> idItem, uint32_t &outValue)
{
    if (idItem == nullptr || idItem->resType_ != ResType::COLOR) {
        return NOT_FOUND;
    }
    std::string temp;
    RState state = ResolveReference(idItem->value_, temp);
    if (state == SUCCESS) {
        return Utils::ConvertColorToUInt32(temp.c_str(), outValue);
    }
    return state;
}

RState ResourceManagerImpl::GetSymbolById(uint32_t id, uint32_t &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetSymbolById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetSymbol(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetSymbolByName(const char *name, uint32_t &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::SYMBOL, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetSymbolByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetSymbol(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetSymbol(const std::shared_ptr<IdItem> idItem, uint32_t &outValue)
{
    if (idItem == nullptr || idItem->resType_ != ResType::SYMBOL) {
        return NOT_FOUND;
    }
    std::string temp;
    RState state = ResolveReference(idItem->value_, temp);
    if (state == SUCCESS) {
        outValue = static_cast<uint32_t>(strtol(temp.c_str(), nullptr, HEX_ADECIMAL));
    }
    return state;
}

RState ResourceManagerImpl::GetIntArrayById(uint32_t id, std::vector<int> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    return GetIntArray(idItem, outValue);
}

RState ResourceManagerImpl::GetIntArrayByName(const char *name, std::vector<int> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::INTARRAY, isOverrideResMgr_);
    return GetIntArray(idItem, outValue);
}

RState ResourceManagerImpl::GetIntArray(const std::shared_ptr<IdItem> idItem, std::vector<int> &outValue)
{
    // not found or type invalid
    if (idItem == nullptr || idItem->resType_ != ResType::INTARRAY) {
        return NOT_FOUND;
    }
    outValue.clear();

    for (size_t i = 0; i < idItem->values_.size(); ++i) {
        std::string resolvedValue;
        RState rrRet = ResolveReference(idItem->values_[i], resolvedValue);
        if (rrRet != SUCCESS) {
            RESMGR_HILOGD(RESMGR_TAG, "ResolveReference failed, value:%{public}s", idItem->values_[i].c_str());
            return ERROR;
        }
        int intValue;
        if (!Utils::convertToInteger(resolvedValue, intValue)) {
            return ERROR;
        }
        outValue.push_back(intValue);
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetThemeById(uint32_t id, std::map<std::string, std::string> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetThemeById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetTheme(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetThemeDataById(uint32_t id, std::map<std::string, ResData> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetThemeDataById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetThemeData(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetThemeByName(const char *name, std::map<std::string, std::string> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::THEME, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetThemeByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetTheme(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetThemeDataByName(const char *name, std::map<std::string, ResData> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::THEME, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetThemeDataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetThemeData(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetTheme(const std::shared_ptr<IdItem> idItem, std::map<std::string, std::string> &outValue)
{
    //type invalid
    if (idItem->resType_ != ResType::THEME) {
        RESMGR_HILOGE(RESMGR_TAG,
            "actual resType = %{public}d, expect resType = %{public}d", idItem->resType_, ResType::THEME);
        return NOT_FOUND;
    }
    return ResolveParentReference(idItem, outValue);
}

RState ResourceManagerImpl::GetThemeData(const std::shared_ptr<IdItem> idItem, std::map<std::string, ResData> &outValue)
{
    //type invalid
    if (idItem->resType_ != ResType::THEME) {
        RESMGR_HILOGE(RESMGR_TAG,
            "actual resType = %{public}d, expect resType = %{public}d", idItem->resType_, ResType::THEME);
        return NOT_FOUND;
    }
    return ResolveResData(idItem, outValue);
}

RState ResourceManagerImpl::GetProfileById(uint32_t id, std::string &outValue)
{
    auto qualifierDir = hapManager_->FindQualifierValueById(id, isOverrideResMgr_);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetProfileById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qualifierDir, ResType::PROF, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetProfileByName(const char *name, std::string &outValue)
{
    auto qualifierDir = hapManager_->FindQualifierValueByName(name, ResType::PROF, isOverrideResMgr_);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG,
            "GetProfileByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qualifierDir, ResType::PROF, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImpl::GetMediaById(uint32_t id, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qualifierDir, ResType::MEDIA, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetMediaByName(const char *name, std::string &outValue, uint32_t density)
{
    if (name == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaByName name is null");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetMediaByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qualifierDir, ResType::MEDIA, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImpl::GetRawFilePathByName(const std::string &name, std::string &outValue)
{
    return hapManager_->FindRawFile(name, outValue);
}

RState ResourceManagerImpl::GetRawFileDescriptor(const std::string &name, RawFileDescriptor &descriptor)
{
    return hapManager_->FindRawFileDescriptorFromHap(name, descriptor);
}

RState ResourceManagerImpl::CloseRawFileDescriptor(const std::string &name)
{
    return hapManager_->CloseRawFileDescriptor(name);
}

ResourceManagerImpl::~ResourceManagerImpl()
{}

bool ResourceManagerImpl::AddResource(const char *path, const uint32_t &selectedTypes, bool forceReload)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
#endif
    if (Utils::IsStrEmpty(path)) {
        RESMGR_HILOGE(RESMGR_TAG, "AddResource path is empty.");
        return false;
    }
#if defined(__ARKUI_CROSS__) || defined(__IDE_PREVIEW__)
    if (!isSystemResMgr_ && Utils::IsSystemPath(std::string(path))) {
        ResourceManagerImpl* systemResourceManager = SystemResourceManager::GetSystemResourceManager();
        if (systemResourceManager != nullptr) {
            systemResourceManager->AddResource(path);
            AddSystemResource(systemResourceManager);
            return true;
        }
    }
#endif
    return this->hapManager_->AddResource(path, selectedTypes, forceReload);
}

bool ResourceManagerImpl::AddPatchResource(const char *path, const char *patchPath)
{
    return this->hapManager_->AddPatchResource(path, patchPath);
}

bool ResourceManagerImpl::AddResource(const std::string &path, const std::vector<std::string> &overlayPaths)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
#endif
    return this->hapManager_->AddResource(path, overlayPaths);
}

bool ResourceManagerImpl::RemoveResource(const std::string &path, const std::vector<std::string> &overlayPaths)
{
    return this->hapManager_->RemoveResource(path, overlayPaths);
}

bool ResourceManagerImpl::AddAppOverlay(const std::string &path)
{
    return this->hapManager_->AddAppOverlay(path);
}

bool ResourceManagerImpl::RemoveAppOverlay(const std::string &path)
{
    return this->hapManager_->RemoveAppOverlay(path);
}

std::string ResourceManagerImpl::ReadParameter(const char *paramKey, const int paramLength)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    char param[paramLength];
    int status = GetParameter(paramKey, "", param, paramLength);
    if (status > 0) {
        return param;
    }
#endif
    return "";
}

RState ResourceManagerImpl::UpdateFakeLocaleFlag(ResConfig &resConfig)
{
#ifdef SUPPORT_GRAPHICS
    if (resConfig.GetLocaleInfo() == nullptr) {
        return LOCALEINFO_IS_NULL;
    }
    if (resConfig.GetLocaleInfo()->getLanguage() == nullptr) {
        return LOCALEINFO_IS_NULL;
    }
    std::string sysLanguage = ReadParameter(LANGUAGE_KEY, CONFIG_LEN);
    if (!sysLanguage.empty()) {
        if (sysLanguage == "en-XA") {
            isFakeLocale = true;
        } else {
            isFakeLocale = false;
        }
        if (sysLanguage == "ar-XB") {
            isBidirectionFakeLocale = true;
        } else {
            isBidirectionFakeLocale = false;
        }
    }
#endif
    return SUCCESS;
}

void ResourceManagerImpl::UpdateTheme(ResConfig &resConfig)
{
    auto themePackManager = ThemePackManager::GetThemePackManager();
    if (!themePackManager->UpdateThemeId(resConfig.GetThemeId(), userId)) {
        return;
    }
    if (resConfig.GetThemeIcon()) {
        RESMGR_HILOGD(RESMGR_TAG, "The themeIcon enabled");
        themePackManager->LoadThemeIconRes(bundleInfo.first, bundleInfo.second, userId);
    }
    if (resConfig.GetThemeSkin()) {
        RESMGR_HILOGD(RESMGR_TAG, "The themeSkin enabled");
        themePackManager->LoadThemeSkinRes(bundleInfo.first, bundleInfo.second, userId);
    }
}

RState ResourceManagerImpl::UpdateResConfig(ResConfig &resConfig, bool isUpdateTheme)
{
    UpdateTheme(resConfig);
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
#endif
    RState state = UpdateFakeLocaleFlag(resConfig);
    if (state != SUCCESS) {
        return state;
    }
    state = this->hapManager_->UpdateResConfig(resConfig);
    UpdateSystemResourceResConfig();
    return state;
}

void ResourceManagerImpl::UpdateSystemResourceResConfig()
{
    if (isSystemResMgr_) {
        return;
    }

    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    SystemResourceManager::UpdateSysResConfig(resConfig, this->hapManager_->IsThemeSystemResEnableHap());
#if defined(__IDE_PREVIEW__)
    ResourceManagerImpl* systemResourceManager = SystemResourceManager::GetSystemResourceManager();
    if (systemResourceManager != nullptr) {
        ResConfigImpl sysResConfig;
        systemResourceManager->GetResConfig(sysResConfig);
        sysResConfig.SetDeviceType(resConfig.GetDeviceType());
        systemResourceManager->GetHapManager()->UpdateResConfig(sysResConfig);
    }
#endif
}

RState ResourceManagerImpl::UpdateOverrideResConfig(ResConfig &resConfig)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME_EX(HITRACE_LEVEL_INFO, HITRACE_TAG_APP, __PRETTY_FUNCTION__, nullptr);
#endif
    UpdateFakeLocaleFlag(resConfig);
    return this->hapManager_->UpdateOverrideResConfig(resConfig);
}

void ResourceManagerImpl::GetResConfig(ResConfig &resConfig)
{
    this->hapManager_->GetResConfig(resConfig);
}

RState ResourceManagerImpl::GetResConfigById(uint32_t resId, ResConfig &resConfig, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGD(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    return this->hapManager_->GetResConfigById(resId, resConfig, isOverrideResMgr_, density);
}

RState ResourceManagerImpl::GetResConfigByName(const std::string &name, const ResType type,
    ResConfig &resConfig, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGD(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    
    return this->hapManager_->GetResConfigByName(name, type, resConfig, isOverrideResMgr_, density);
}

void ResourceManagerImpl::GetOverrideResConfig(ResConfig &resConfig)
{
    this->hapManager_->GetOverrideResConfig(resConfig);
}

std::vector<std::string> ResourceManagerImpl::GetResourcePaths()
{
    return this->hapManager_->GetResourcePaths();
}

bool ResourceManagerImpl::IsDensityValid(uint32_t density)
{
    switch (density) {
        case SCREEN_DENSITY_NOT_SET:
        case SCREEN_DENSITY_SDPI:
        case SCREEN_DENSITY_MDPI:
        case SCREEN_DENSITY_LDPI:
        case SCREEN_DENSITY_XLDPI:
        case SCREEN_DENSITY_XXLDPI:
        case SCREEN_DENSITY_XXXLDPI:
            return true;
        default:
            return false;
    }
}

RState ResourceManagerImpl::GetThemeMedia(const std::shared_ptr<IdItem> idItem, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    std::vector<std::shared_ptr<IdItem>> idItems;
    idItems.emplace_back(idItem);
    auto themeManager = ThemePackManager::GetThemePackManager();
    std::string result = themeManager->FindThemeResource(bundleInfo, idItems, resConfig, userId);
    if (result.empty()) {
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    outValue = Utils::LoadResourceFile(result, len);
    if (outValue == nullptr) {
        RESMGR_HILOGW(RESMGR_TAG, "GetThemeMedia null, path=%{public}s",
            themeManager->GetLogPath(result, userId).c_str());
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetMediaDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaDataById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }

    // find in theme
    const std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaDataById idItem null, id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    if (GetThemeMedia(idItem, len, outValue, density) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qualifierDir, len, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetMediaDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    if (name == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaDataByName name is null");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    auto qualifierDir = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG,
            "GetMediaDataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }

    const std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaDataByName idItem null, name = %{public}s", name);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    if (GetThemeMedia(idItem, len, outValue, density) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qualifierDir, len, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImpl::GetThemeMediaBase64(const std::shared_ptr<IdItem> idItem, std::string &outValue)
{
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    std::vector<std::shared_ptr<IdItem>> idItems;
    idItems.emplace_back(idItem);
    std::string result = ThemePackManager::GetThemePackManager()->FindThemeResource(
        bundleInfo, idItems, resConfig, userId);
    if (result.empty()) {
        return NOT_FOUND;
    }
    return Utils::GetMediaBase64Data(result, outValue);
}

RState ResourceManagerImpl::GetMediaBase64DataById(uint32_t id, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    
    auto qualifierDir = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaBase64DataById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }

    const std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaBase64DataById idItem null, id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    if (GetThemeMediaBase64(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaBase64Data(qualifierDir, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetMediaBase64DataByName(const char *name, std::string &outValue, uint32_t density)
{
    if (name == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaBase64DataByName name is null");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG,
            "GetMediaBase64DataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }

    const std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaBase64DataByName idItem null, name = %{public}s", name);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    if (GetThemeMediaBase64(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaBase64Data(qualifierDir, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImpl::GetProfileDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    auto qualifierDir = hapManager_->FindQualifierValueById(id, isOverrideResMgr_);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetProfileDataById error id = %{public}d", id);
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return hapManager_->GetProfileData(qualifierDir, len, outValue);
}

RState ResourceManagerImpl::GetProfileDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    if (name == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetProfileDataByName name is null");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueByName(name, ResType::PROF, isOverrideResMgr_);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG,
            "GetProfileDataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return hapManager_->GetProfileData(qualifierDir, len, outValue);
}

RState ResourceManagerImpl::GetRawFileFromHap(const std::string &rawFileName, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue)
{
    return hapManager_->FindRawFileFromHap(rawFileName, len, outValue);
}

RState ResourceManagerImpl::GetRawFileDescriptorFromHap(const std::string &rawFileName, RawFileDescriptor &descriptor)
{
    return hapManager_->FindRawFileDescriptorFromHap(rawFileName, descriptor);
}

RState ResourceManagerImpl::IsLoadHap(std::string &hapPath)
{
    if (hapManager_->IsLoadHap(hapPath)) {
        return SUCCESS;
    }
    return NOT_FOUND;
}

RState ResourceManagerImpl::GetRawFileList(const std::string &rawDirPath, std::vector<std::string>& rawfileList)
{
    return hapManager_->GetRawFileList(rawDirPath, rawfileList);
}

std::string GetSuffix(const std::shared_ptr<IdItem> &idItem)
{
    if (idItem == nullptr || idItem->resType_ != ResType::MEDIA) {
        return std::string();
    }
    std::string mediaPath = idItem->value_;
    auto pos = mediaPath.find_last_of('.');
    if (pos == std::string::npos) {
        return std::string();
    }
    return mediaPath.substr(pos + 1);
}

RState ResourceManagerImpl::GetThemeIcon(const std::shared_ptr<IdItem> idItem, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    auto themeManager = ThemePackManager::GetThemePackManager();
    std::string iconName = idItem->name_;
    std::string result = themeManager->FindThemeIconResource(bundleInfo, iconName, userId);
    if (result.empty()) {
        RESMGR_HILOGD(RESMGR_TAG,
            "GetThemeIcon FAILED bundlename = %{public}s, modulename = %{public}s, iconName = %{public}s",
            bundleInfo.first.c_str(), bundleInfo.second.c_str(), iconName.c_str());
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    outValue = Utils::LoadResourceFile(result, len);
    if (outValue == nullptr) {
        RESMGR_HILOGW(RESMGR_TAG, "GetThemeIcon null, path=%{public}s",
            themeManager->GetLogPath(result, userId).c_str());
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetThemeDrawable(const std::shared_ptr<IdItem> idItem, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density)
{
    if (iconType == 0 && GetThemeMedia(idItem, len, outValue, density) == SUCCESS) {
        return SUCCESS;
    } else if (iconType == 1 && GetThemeIcon(idItem, len, outValue, density) == SUCCESS) {
        return SUCCESS;
    } else {
        // other type
    }
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetDrawableInfoById(uint32_t id, std::string &type, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoById id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoById idItem null, id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    type = GetSuffix(idItem);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return hapManager_->GetMediaData(qualifierDir, len, outValue);
}

RState ResourceManagerImpl::GetDrawableInfoByName(const char *name, std::string &type, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (name == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoByName name is null");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetDrawableInfoByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoByName idItem null, name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    type = GetSuffix(idItem);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return hapManager_->GetMediaData(qualifierDir, len, outValue);
}

RState ResourceManagerImpl::GetDrawableInfoById(uint32_t id,
    std::tuple<std::string, size_t, std::string> &drawableInfo,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoById idItem null, id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    std::string type = GetSuffix(idItem);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    size_t len = 0;
    // find in theme
    std::string themeMask = ThemePackManager::GetThemePackManager()->GetMask(userId);
    if (GetThemeDrawable(idItem, len, outValue, iconType, density) == SUCCESS) {
        drawableInfo = std::make_tuple(type, len, themeMask);
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qualifierDir, len, outValue);
    drawableInfo = std::make_tuple(type, len, themeMask);
    return state;
}

RState ResourceManagerImpl::GetDrawableInfoByName(const char *name,
    std::tuple<std::string, size_t, std::string> &drawableInfo,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density)
{
    if (name == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoByName name is null");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qualifierDir = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qualifierDir == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetDrawableInfoByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    std::shared_ptr<IdItem> idItem = qualifierDir->GetIdItem();
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoByName idItem null, name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    std::string type = GetSuffix(idItem);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    size_t len = 0;
    // find in theme
    std::string themeMask = ThemePackManager::GetThemePackManager()->GetMask(userId);
    if (GetThemeDrawable(idItem, len, outValue, iconType, density) == SUCCESS) {
        drawableInfo = std::make_tuple(type, len, themeMask);
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qualifierDir, len, outValue);
    drawableInfo = std::make_tuple(type, len, themeMask);
    return state;
}

RState ResourceManagerImpl::GetResName(uint32_t id, std::string &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetResName error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    outValue = idItem->name_;
    return SUCCESS;
}

uint32_t ResourceManagerImpl::GetResourceLimitKeys()
{
    if (hapManager_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "resource manager get limit keys failed, hapManager_ is nullptr");
        return 0;
    }
    return hapManager_->GetResourceLimitKeys();
}

RState ResourceManagerImpl::GetRawFdNdkFromHap(const std::string &name, RawFileDescriptor &descriptor)
{
    return hapManager_->GetRawFd(name, descriptor);
}

RState ResourceManagerImpl::GetResId(const std::string &resTypeName, uint32_t &resId)
{
    return hapManager_->GetResId(resTypeName, resId);
}

void ResourceManagerImpl::GetLocales(std::vector<std::string> &outValue, bool includeSystem)
{
    hapManager_->GetLocales(outValue, includeSystem);
}

RState ResourceManagerImpl::GetThemeIconInfo(const std::string &iconName, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, const std::string &abilityName, bool printLog)
{
    auto themeManager = ThemePackManager::GetThemePackManager();
    std::string result = themeManager->FindThemeIconResource(bundleInfo, iconName, userId, abilityName);
    if (result.empty()) {
        if (printLog) {
            RESMGR_HILOGE(RESMGR_TAG, "ThemeIcon err. name:%{public}s bundle:%{public}s ability:%{public}s.",
                iconName.c_str(), bundleInfo.first.c_str(), abilityName.c_str());
        }
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    outValue = Utils::LoadResourceFile(result, len);
    if (outValue == nullptr) {
        if (printLog) {
            RESMGR_HILOGE(RESMGR_TAG, "ThemeIcon LoadFile err. name:%{public}s, path:%{public}s.", iconName.c_str(),
                themeManager->GetLogPath(result, userId).c_str());
        }
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetThemeIcons(uint32_t resId, std::pair<std::unique_ptr<uint8_t[]>, size_t>
    &foregroundInfo, std::pair<std::unique_ptr<uint8_t[]>, size_t> &backgroundInfo, uint32_t density,
    const std::string &abilityName)
{
    RState foreState = GetThemeIconInfo(FOREGROUND, foregroundInfo.second, foregroundInfo.first, abilityName);
    RState backState = GetThemeIconInfo(BACKGROUND, backgroundInfo.second, backgroundInfo.first, abilityName);
    if (foreState == SUCCESS && backState == SUCCESS) {
        RESMGR_HILOGW(RESMGR_TAG,
            "GetThemeIcons bundleName = %{public}s, abilityName = %{public}s, fLen = %{public}zu, bLen = %{public}zu",
            bundleInfo.first.c_str(), abilityName.c_str(), foregroundInfo.second, backgroundInfo.second);
        return SUCCESS;
    }

    if (foreState == SUCCESS && Utils::SupportSingleLayerThemeIcon()) {
        RESMGR_HILOGW(RESMGR_TAG,
            "GetThemeIcons bundlename = %{public}s, abilityName = %{public}s, fLen = %{public}zu",
            bundleInfo.first.c_str(), abilityName.c_str(), foregroundInfo.second);
        return SUCCESS;
    }
    return ERROR_CODE_RES_ID_NOT_FOUND;
}

RState ResourceManagerImpl::GetDynamicIcon(const std::string &resName,
    std::pair<std::unique_ptr<uint8_t[]>, size_t> &iconInfo, uint32_t density)
{
    return GetThemeIconInfo(resName, iconInfo.second, iconInfo.first, "", true);
}

std::string ResourceManagerImpl::GetThemeMask()
{
    return ThemePackManager::GetThemePackManager()->GetMask(userId);
}

bool ResourceManagerImpl::HasIconInTheme(const std::string &bundleName)
{
    return ThemePackManager::GetThemePackManager()->HasIconInTheme(bundleName, userId);
}

RState ResourceManagerImpl::GetOtherIconsInfo(const std::string &iconName,
    std::unique_ptr<uint8_t[]> &outValue, size_t &len, bool isGlobalMask)
{
    auto themeManager = ThemePackManager::GetThemePackManager();
    if (iconName.find("icon_highlightstroke") != std::string::npos && isGlobalMask) {
        return themeManager->GetHighlightIconInfo(iconName, outValue, len, isGlobalMask, userId);
    }
    std::string iconTag;
    if (iconName.find("icon_mask") != std::string::npos && isGlobalMask) {
        iconTag = "global_" + iconName;
    } else {
        iconTag = "other_icons_" + iconName;
    }
    RState result = themeManager->GetThemeIconFromCache(iconTag, outValue, len, userId);
    if (result == SUCCESS) {
        return SUCCESS;
    }
    return themeManager->GetOtherIconsInfo(iconName, outValue, len, isGlobalMask, userId);
}

RState ResourceManagerImpl::IsRawDirFromHap(const std::string &pathName, bool &outValue)
{
    return hapManager_->IsRawDirFromHap(pathName, outValue);
}

std::shared_ptr<HapManager> ResourceManagerImpl::GetHapManager()
{
    return hapManager_;
}

bool ResourceManagerImpl::IsSystem()
{
    return isSystemResMgr_;
}

bool ResourceManagerImpl::IsOverride()
{
    return isOverrideResMgr_;
}

std::shared_ptr<ResourceManager> ResourceManagerImpl::GetOverrideResourceManager(
    std::shared_ptr<ResConfig> overrideResConfig)
{
    ResourceManagerImpl *impl = new (std::nothrow) ResourceManagerImpl(true);
    if (impl == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResourceManagerImpl failed when GetOverrideResourceManager");
        return nullptr;
    }

    if (!impl->Init(this->GetHapManager())) {
        delete (impl);
        return nullptr;
    }

    std::shared_ptr<ResourceManager> overrideResMgr(impl);
    if (overrideResMgr == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetOverrideResourceManager failed bundleName = %{public}s, moduleName = %{public}s",
            this->bundleInfo.first.c_str(), this->bundleInfo.second.c_str());
        return nullptr;
    }

    overrideResMgr->bundleInfo.first = this->bundleInfo.first;
    overrideResMgr->bundleInfo.second = this->bundleInfo.second;
    overrideResMgr->SetBrandColor(this->GetBrandColor());
    if (overrideResConfig && overrideResMgr->UpdateOverrideResConfig(*overrideResConfig) != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "GetOverrideResourceManager UpdateOverrideResConfig failed bundleName = %{public}s, \
            moduleName = %{public}s", this->bundleInfo.first.c_str(), this->bundleInfo.second.c_str());
        return nullptr;
    }

    return overrideResMgr;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
