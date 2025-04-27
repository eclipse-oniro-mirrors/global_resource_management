/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include <fstream>

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
    return true;
}

RState ResourceManagerImpl::GetStringById(uint32_t id, std::string &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetStringById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetString(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetStringByName(const char *name, std::string &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::STRING, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetStringByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetString(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetStringFormatById(std::string &outValue, uint32_t id, ...)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    std::string temp;
    RState rState = GetString(idItem, temp);
    if (rState != SUCCESS) {
        return rState;
    }
    va_list args;
    va_start(args, id);
    outValue = FormatString(temp.c_str(), args);
    va_end(args);
    return SUCCESS;
}

RState ResourceManagerImpl::GetStringFormatByName(std::string &outValue, const char *name, ...)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::STRING, isOverrideResMgr_);
    std::string temp;
    RState rState = GetString(idItem, temp);
    if (rState != SUCCESS) {
        return rState;
    }
    va_list args;
    va_start(args, name);
    outValue = FormatString(temp.c_str(), args);
    va_end(args);
    return SUCCESS;
}

RState ResourceManagerImpl::GetStringFormatById(std::string &outValue, uint32_t id, va_list args)
{
    RState state = GetStringById(id, outValue);
    if (state != SUCCESS) {
        return state;
    }
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    if (parseArgs(outValue, args, jsParams)) {
        ResConfigImpl resConfig;
        GetResConfig(resConfig);
        if (!ReplacePlaceholderWithParams(outValue, resConfig, jsParams)) {
            return ERROR_CODE_RES_ID_FORMAT_ERROR;
        }
        return SUCCESS;
    }
    return ERROR_CODE_INVALID_INPUT_PARAMETER;
}

RState ResourceManagerImpl::GetStringFormatByName(std::string &outValue, const char *name, va_list args)
{
    RState state = GetStringByName(name, outValue);
    if (state != SUCCESS) {
        return state;
    }
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    if (parseArgs(outValue, args, jsParams)) {
        ResConfigImpl resConfig;
        GetResConfig(resConfig);
        if (!ReplacePlaceholderWithParams(outValue, resConfig, jsParams)) {
            return ERROR_CODE_RES_NAME_FORMAT_ERROR;
        }
        return SUCCESS;
    }
    return ERROR_CODE_INVALID_INPUT_PARAMETER;
}

RState ResourceManagerImpl::GetString(const std::shared_ptr<IdItem> idItem, std::string &outValue)
{
    // not found or type invalid
    if (idItem == nullptr || idItem->resType_ != ResType::STRING) {
        return NOT_FOUND;
    }
    RState ret = ResolveReference(idItem->value_, outValue);
    if (isFakeLocale) {
        ProcessPsuedoTranslate(outValue);
    }
    if (isBidirectionFakeLocale) {
        outValue = psueManager_->BidirectionConvert(outValue);
    }
    if (ret != SUCCESS) {
        return ret;
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetStringArrayById(uint32_t id, std::vector<std::string> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetStringArrayById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = GetStringArray(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return state;
}

RState ResourceManagerImpl::GetStringArrayByName(const char *name, std::vector<std::string> &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(
        name, ResType::STRINGARRAY, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetStringArrayByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetStringArray(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetStringArray(const std::shared_ptr<IdItem> idItem, std::vector<std::string> &outValue)
{
    // not found or type invalid
    if (idItem == nullptr || idItem->resType_ != ResType::STRINGARRAY) {
        return NOT_FOUND;
    }
    outValue.clear();

    for (size_t i = 0; i < idItem->values_.size(); ++i) {
        std::string resolvedValue;
        RState rrRet = ResolveReference(idItem->values_[i], resolvedValue);
        if (rrRet != SUCCESS) {
            RESMGR_HILOGD(RESMGR_TAG,
                "GetStringArray ResolveReference failed, value:%{public}s", idItem->values_[i].c_str());
            return rrRet;
        }
        outValue.push_back(resolvedValue);
    }
    if (isFakeLocale) {
        for (auto &iter : outValue) {
            ProcessPsuedoTranslate(iter);
        }
    }
    if (isBidirectionFakeLocale) {
        for (auto &iter : outValue) {
            iter = psueManager_->BidirectionConvert(iter);
        }
    }
    return SUCCESS;
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

RState ResourceManagerImpl::GetPluralStringById(uint32_t id, int quantity, std::string &outValue)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd = hapManager_->FindQualifierValueById(id,
        isOverrideResMgr_);
    Quantity intQuantity = { true, quantity, 0.0 };
    return GetPluralString(vuqd, intQuantity, outValue);
}

RState ResourceManagerImpl::GetPluralStringByName(const char *name, int quantity, std::string &outValue)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd =
        hapManager_->FindQualifierValueByName(name, ResType::PLURALS, isOverrideResMgr_);
        Quantity intQuantity = { true, quantity, 0.0 };
    return GetPluralString(vuqd, intQuantity, outValue);
}

RState ResourceManagerImpl::GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd = hapManager_->FindQualifierValueById(id,
        isOverrideResMgr_);
    if (vuqd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetPluralStringByIdFormat error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    std::string temp;
    Quantity intQuantity = { true, quantity, 0.0 };
    RState rState = GetPluralString(vuqd, intQuantity, temp);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        return rState;
    }
    if (rState != SUCCESS) {
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }

    va_list args;
    va_start(args, quantity);
    outValue = FormatString(temp.c_str(), args);
    va_end(args);

    return SUCCESS;
}

RState ResourceManagerImpl::GetPluralStringByNameFormat(std::string &outValue, const char *name, int quantity, ...)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd =
        hapManager_->FindQualifierValueByName(name, ResType::PLURALS, isOverrideResMgr_);
    if (vuqd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetPluralStringByNameFormat error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    std::string temp;
    Quantity intQuantity = { true, quantity, 0.0 };
    RState rState = GetPluralString(vuqd, intQuantity, temp);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        return rState;
    }
    if (rState != SUCCESS) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }

    va_list args;
    va_start(args, quantity);
    outValue = FormatString(temp.c_str(), args);
    va_end(args);

    return SUCCESS;
}

RState ResourceManagerImpl::GetPluralString(const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd,
    Quantity quantity, std::string &outValue)
{
    // not found or type invalid
    if (vuqd == nullptr) {
        return NOT_FOUND;
    }
    auto idItem = vuqd->GetIdItem();
    if (idItem == nullptr || idItem->resType_ != ResType::PLURALS) {
        return NOT_FOUND;
    }
    std::map<std::string, std::string> map;

    size_t startIdx = 0;
    size_t loop = idItem->values_.size() / 2;
    for (size_t i = 0; i < loop; ++i) {
        std::string key(idItem->values_[startIdx + i * 2]); // 2 means keyappear in pairs
        std::string value(idItem->values_[startIdx + i * 2 + 1]); // 2 means value appear in pairs
        auto iter = map.find(key);
        if (iter == map.end()) {
            std::string resolvedValue;
            RState rrRet = ResolveReference(value, resolvedValue);
            if (rrRet != SUCCESS) {
                RESMGR_HILOGD(RESMGR_TAG, "ResolveReference failed, value:%{public}s", value.c_str());
                return rrRet;
            }
            map[key] = resolvedValue;
        }
    }

    std::string converted = hapManager_->GetPluralRulesAndSelect(quantity, isOverrideResMgr_);
    auto mapIter = map.find(converted);
    if (mapIter == map.end()) {
        mapIter = map.find("other");
        if (mapIter == map.end()) {
            return NOT_FOUND;
        }
    }
    outValue = mapIter->second;
    if (isFakeLocale) {
        ProcessPsuedoTranslate(outValue);
    }
    if (isBidirectionFakeLocale) {
        outValue = psueManager_->BidirectionConvert(outValue);
    }
    return SUCCESS;
}

RState ResourceManagerImpl::ResolveReference(const std::string value, std::string &outValue)
{
    uint32_t id;
    ResType resType;
    bool isRef = true;
    int count = 0;
    std::string refStr(value);
    while (isRef) {
        isRef = IdItem::IsRef(refStr, resType, id);
        if (!isRef || resType == ResType::PLURALS) {
            outValue = refStr;
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

RState ResourceManagerImpl::ResolveDataReference(const std::string key, const std::string value,
    std::map<std::string, ResData> &outValue)
{
    uint32_t id;
    ResType resType = ResType::MAX_RES_TYPE;
    bool isRef = true;
    int count = 0;
    std::string refStr(value);
    while (isRef) {
        ResType realType = resType;
        isRef = IdItem::IsRef(refStr, resType, id);
        if (!isRef) {
            if (realType == ResType::MAX_RES_TYPE) {
                realType = ResType::STRING;
            }
            outValue[key] = { .resType = realType, .value = refStr };
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

RState ResourceManagerImpl::GetThemeValues(const std::string &value, std::string &outValue,
    const ResConfigImpl &resConfig)
{
    std::vector<std::shared_ptr<IdItem>> idItems;
    if (ProcessReference(value, idItems) != SUCCESS) {
        return NOT_FOUND;
    }
    outValue = ThemePackManager::GetThemePackManager()->FindThemeResource(bundleInfo, idItems, resConfig, userId,
        hapManager_->IsThemeSystemResEnableHap());
    return outValue.empty() ? NOT_FOUND : SUCCESS;
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
        RESMGR_HILOGE(RESMGR_TAG, "GetBooleanByName error name = %{public}s", name);
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
        RESMGR_HILOGE(RESMGR_TAG, "GetFloatByName error name = %{public}s", name);
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
        RESMGR_HILOGE(RESMGR_TAG, "GetIntegerByName error name = %{public}s", name);
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
        idItems.emplace_back(idItem);
        if (idItem == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "ref %s id not found", refStr.c_str());
            return ERROR;
        }
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
    return state == SUCCESS ? Utils::ConvertColorToUInt32(result.c_str(), outValue) : state;
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
    return state;
}

RState ResourceManagerImpl::GetColorByName(const char *name, uint32_t &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::COLOR, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetColorByName error name = %{public}s", name);
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
        RESMGR_HILOGE(RESMGR_TAG, "GetSymbolByName error name = %{public}s", name);
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
        RESMGR_HILOGE(RESMGR_TAG, "GetThemeByName error name = %{public}s", name);
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
        RESMGR_HILOGE(RESMGR_TAG, "GetThemeDataByName error name = %{public}s", name);
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
    auto qd = hapManager_->FindQualifierValueById(id, isOverrideResMgr_);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetProfileById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qd, ResType::PROF, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetProfileByName(const char *name, std::string &outValue)
{
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::PROF, isOverrideResMgr_);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG,
            "GetProfileByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qd, ResType::PROF, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImpl::GetMediaById(uint32_t id, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qd, ResType::MEDIA, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetMediaByName(const char *name, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qd, ResType::MEDIA, outValue);
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

void ResourceManagerImpl::ProcessPsuedoTranslate(std::string &outValue)
{
    auto len = outValue.length() + 1;
    char src[len];
    if (strcpy_s(src, len, outValue.c_str()) == EOK) {
        std::string resultMsg = psueManager_->Convert(src, outValue);
        if (resultMsg != "") {
            RESMGR_HILOGE(RESMGR_TAG, "Psuedo translate failed, value:%s", src);
        }
    }
}

ResourceManagerImpl::~ResourceManagerImpl()
{}

bool ResourceManagerImpl::AddResource(const char *path, const uint32_t &selectedTypes, bool forceReload)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif

#if defined(__ARKUI_CROSS__)
    if (!isSystemResMgr_ && std::string(path).find("/systemres/resources.index") != std::string::npos) {
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

RState ResourceManagerImpl::UpdateFakeLocaleFlag(ResConfig &resConfig)
{
#ifdef SUPPORT_GRAPHICS
    if (resConfig.GetLocaleInfo() == nullptr) {
        return LOCALEINFO_IS_NULL;
    }
    if (resConfig.GetLocaleInfo()->getLanguage() == nullptr) {
        return LOCALEINFO_IS_NULL;
    }
    const char* language = resConfig.GetLocaleInfo()->getLanguage();
    const char* region = resConfig.GetLocaleInfo()->getCountry();
    if (language != nullptr && region != nullptr) {
        std::string languageStr = language;
        std::string regionStr = region;
        if (languageStr == "en" && regionStr == "XA") {
            isFakeLocale = true;
        } else {
            isFakeLocale = false;
        }
        if (languageStr == "ar" && regionStr == "XB") {
            isBidirectionFakeLocale = true;
        } else {
            isBidirectionFakeLocale = false;
        }
    }
#endif
    return SUCCESS;
}

RState ResourceManagerImpl::UpdateResConfig(ResConfig &resConfig, bool isUpdateTheme)
{
    auto themePackManager = ThemePackManager::GetThemePackManager();
    if (themePackManager->UpdateThemeId(resConfig.GetThemeId())) {
        if (resConfig.GetThemeIcon()) {
            RESMGR_HILOGD(RESMGR_TAG, "The themeIcon enabled");
            themePackManager->LoadThemeIconRes(bundleInfo.first, bundleInfo.second, userId);
        } else {
            RESMGR_HILOGD(RESMGR_TAG, "The theme enabled");
            themePackManager->LoadThemeRes(bundleInfo.first, bundleInfo.second, userId);
        }
    }
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif
    RState state = UpdateFakeLocaleFlag(resConfig);
    if (state != SUCCESS) {
        return state;
    }
    return this->hapManager_->UpdateResConfig(resConfig);
}

RState ResourceManagerImpl::UpdateOverrideResConfig(ResConfig &resConfig)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
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
    std::string result = ThemePackManager::GetThemePackManager()->FindThemeResource(
        bundleInfo, idItems, resConfig, userId);
    outValue = Utils::LoadResourceFile(result, len);
    return result.empty() ? ERROR_CODE_RES_ID_NOT_FOUND : SUCCESS;
}

RState ResourceManagerImpl::GetMediaDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaDataById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }

    // find in theme
    const std::shared_ptr<IdItem> idItem = qd->GetIdItem();
    if (GetThemeMedia(idItem, len, outValue, density) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qd, len, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetMediaDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }

    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG,
            "GetMediaDataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }

    const std::shared_ptr<IdItem> idItem = qd->GetIdItem();
    if (GetThemeMedia(idItem, len, outValue, density) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qd, len, outValue);
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
    
    auto qd = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetMediaBase64DataById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }

    const std::shared_ptr<IdItem> idItem = qd->GetIdItem();
    if (GetThemeMediaBase64(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaBase64Data(qd, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetMediaBase64DataByName(const char *name, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG,
            "GetMediaBase64DataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }

    const std::shared_ptr<IdItem> idItem = qd->GetIdItem();
    if (GetThemeMediaBase64(idItem, outValue) == SUCCESS) {
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaBase64Data(qd, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImpl::GetProfileDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    auto qd = hapManager_->FindQualifierValueById(id, isOverrideResMgr_);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetProfileDataById error id = %{public}d", id);
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return hapManager_->GetProfileData(qd, len, outValue);
}

RState ResourceManagerImpl::GetProfileDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::PROF, isOverrideResMgr_);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG,
            "GetProfileDataByName error name = %{public}s", name);
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return hapManager_->GetProfileData(qd, len, outValue);
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

std::string GetSuffix(const std::shared_ptr<HapResource::ValueUnderQualifierDir> qd)
{
    const std::shared_ptr<IdItem> idItem = qd->GetIdItem();
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
    std::string iconName = idItem->GetItemResName();
    std::string result = ThemePackManager::GetThemePackManager()->FindThemeIconResource(
        bundleInfo, iconName, userId);
    if (result.empty()) {
        RESMGR_HILOGD(RESMGR_TAG,
            "GetThemeIcon FAILED bundlename = %{public}s, modulename = %{public}s, iconName = %{public}s",
            bundleInfo.first.c_str(), bundleInfo.second.c_str(), iconName.c_str());
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    outValue = Utils::LoadResourceFile(result, len);
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
    auto qd = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoById id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    type = GetSuffix(qd);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return hapManager_->GetMediaData(qd, len, outValue);
}

RState ResourceManagerImpl::GetDrawableInfoByName(const char *name, std::string &type, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    type = GetSuffix(qd);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return hapManager_->GetMediaData(qd, len, outValue);
}

RState ResourceManagerImpl::GetDrawableInfoById(uint32_t id,
    std::tuple<std::string, size_t, std::string> &drawableInfo,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueById(id, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    std::string type = GetSuffix(qd);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    size_t len = 0;
    // find in theme
    const std::shared_ptr<IdItem> idItem = qd->GetIdItem();
    std::string themeMask = ThemePackManager::GetThemePackManager()->GetMask();
    if (GetThemeDrawable(idItem, len, outValue, iconType, density) == SUCCESS) {
        drawableInfo = std::make_tuple(type, len, themeMask);
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qd, len, outValue);
    drawableInfo = std::make_tuple(type, len, themeMask);
    return state;
}

RState ResourceManagerImpl::GetDrawableInfoByName(const char *name,
    std::tuple<std::string, size_t, std::string> &drawableInfo,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density)
{
    if (!IsDensityValid(density)) {
        RESMGR_HILOGE(RESMGR_TAG, "density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, isOverrideResMgr_, density);
    if (qd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetDrawableInfoByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    std::string type = GetSuffix(qd);
    if (type.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    size_t len = 0;
    // find in theme
    std::string themeMask = ThemePackManager::GetThemePackManager()->GetMask();
    const std::shared_ptr<IdItem> idItem = qd->GetIdItem();
    if (GetThemeDrawable(idItem, len, outValue, iconType, density) == SUCCESS) {
        drawableInfo = std::make_tuple(type, len, themeMask);
        return SUCCESS;
    }

    RState state = hapManager_->GetMediaData(qd, len, outValue);
    drawableInfo = std::make_tuple(type, len, themeMask);
    return state;
}

RState ResourceManagerImpl::GetStringFormatById(uint32_t id, std::string &outValue,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    RState state = GetStringById(id, outValue);
    if (state != SUCCESS) {
        return state;
    }
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    if (!ReplacePlaceholderWithParams(outValue, resConfig, jsParams)) {
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetStringFormatByName(const char *name, std::string &outValue,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    RState state = GetStringByName(name, outValue);
    if (state != SUCCESS) {
        return state;
    }
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    if (!ReplacePlaceholderWithParams(outValue, resConfig, jsParams)) {
        return ERROR_CODE_RES_NAME_FORMAT_ERROR;
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetFormatPluralStringById(std::string &outValue, uint32_t id, int quantity,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    Quantity intqQantity = { true, quantity, 0.0 };
    return GetFormatPluralStringById(outValue, id, intqQantity, jsParams);
}

RState ResourceManagerImpl::GetFormatPluralStringByName(std::string &outValue, const char *name, int quantity,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    Quantity intQuantity = { true, quantity, 0.0 };
    return GetFormatPluralStringByName(outValue, name, intQuantity, jsParams);
}

RState ResourceManagerImpl::GetFormatPluralStringById(std::string &outValue, uint32_t id, Quantity quantity,
    va_list args)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd = hapManager_->FindQualifierValueById(id,
        isOverrideResMgr_);
    if (vuqd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFormatPluralStringById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState rState = GetPluralString(vuqd, quantity, outValue);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        RESMGR_HILOGE(RESMGR_TAG, "find too much ref by plural id = %{public}d", id);
        return rState;
    }
    if (rState != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "plural res not found, id = %{public}d", id);
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }

    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (parseArgs(outValue, args, params)) {
        ResConfigImpl resConfig;
        GetResConfig(resConfig);
        if (!ReplacePlaceholderWithParams(outValue, resConfig, params)) {
            RESMGR_HILOGE(RESMGR_TAG, "format plural string error, id = %{public}d", id);
            return ERROR_CODE_RES_ID_FORMAT_ERROR;
        }
        return SUCCESS;
    }
    return ERROR_CODE_INVALID_INPUT_PARAMETER;
}

RState ResourceManagerImpl::GetFormatPluralStringByName(std::string &outValue, const char *name, Quantity quantity,
    va_list args)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd =
        hapManager_->FindQualifierValueByName(name, ResType::PLURALS, isOverrideResMgr_);
    if (vuqd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFormatPluralStringByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState rState = GetPluralString(vuqd, quantity, outValue);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        RESMGR_HILOGE(RESMGR_TAG, "find too much ref by plural name = %{public}s", name);
        return rState;
    }
    if (rState != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "plural res not found, name = %{public}s", name);
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (parseArgs(outValue, args, params)) {
        ResConfigImpl resConfig;
        GetResConfig(resConfig);
        if (!ReplacePlaceholderWithParams(outValue, resConfig, params)) {
            RESMGR_HILOGE(RESMGR_TAG, "format plural string error, name = %{public}s", name);
            return ERROR_CODE_RES_NAME_FORMAT_ERROR;
        }
        return SUCCESS;
    }
    return ERROR_CODE_INVALID_INPUT_PARAMETER;
}

RState ResourceManagerImpl::GetFormatPluralStringById(std::string &outValue, uint32_t id, Quantity quantity,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd = hapManager_->FindQualifierValueById(id,
        isOverrideResMgr_);
    if (vuqd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFormatPluralStringById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState rState = GetPluralString(vuqd, quantity, outValue);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        RESMGR_HILOGE(RESMGR_TAG, "find too much ref by plural id = %{public}d", id);
        return rState;
    }
    if (rState != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "plural res not found, id = %{public}d", id);
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    if (!ReplacePlaceholderWithParams(outValue, resConfig, jsParams)) {
        RESMGR_HILOGE(RESMGR_TAG, "format plural string error, id = %{public}d", id);
        return ERROR_CODE_RES_ID_FORMAT_ERROR;
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetFormatPluralStringByName(std::string &outValue, const char *name, Quantity quantity,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    const std::shared_ptr<HapResource::ValueUnderQualifierDir> vuqd =
        hapManager_->FindQualifierValueByName(name, ResType::PLURALS, isOverrideResMgr_);
    if (vuqd == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFormatPluralStringByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState rState = GetPluralString(vuqd, quantity, outValue);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        RESMGR_HILOGE(RESMGR_TAG, "find too much ref by plural name = %{public}s", name);
        return rState;
    }
    if (rState != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "plural res not found, name = %{public}s", name);
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    if (!ReplacePlaceholderWithParams(outValue, resConfig, jsParams)) {
        RESMGR_HILOGE(RESMGR_TAG, "format plural string error, name = %{public}s", name);
        return ERROR_CODE_RES_NAME_FORMAT_ERROR;
    }
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
    std::unique_ptr<uint8_t[]> &outValue, const std::string &abilityName)
{
    std::string result = ThemePackManager::GetThemePackManager()->FindThemeIconResource(
        bundleInfo, iconName, userId, abilityName);
    if (result.empty()) {
        RESMGR_HILOGD(RESMGR_TAG, "GetThemeIconInfo FAILED bundlename = %{public}s,", result.c_str());
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    outValue = Utils::LoadResourceFile(result, len);
    if (outValue == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "LoadResourceFile FAILED");
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
    return ERROR_CODE_RES_ID_NOT_FOUND;
}

RState ResourceManagerImpl::GetDynamicIcon(const std::string &resName,
    std::pair<std::unique_ptr<uint8_t[]>, size_t> &iconInfo, uint32_t density)
{
    return GetThemeIconInfo(resName, iconInfo.second, iconInfo.first);
}

std::string ResourceManagerImpl::GetThemeMask()
{
    return ThemePackManager::GetThemePackManager()->GetMask();
}

bool ResourceManagerImpl::HasIconInTheme(const std::string &bundleName)
{
    return ThemePackManager::GetThemePackManager()->HasIconInTheme(bundleName, userId);
}

RState ResourceManagerImpl::GetOtherIconsInfo(const std::string &iconName,
    std::unique_ptr<uint8_t[]> &outValue, size_t &len, bool isGlobalMask)
{
    std::string iconTag;
    if (iconName.find("icon_mask") != std::string::npos && isGlobalMask) {
        iconTag = "global_" + iconName;
    } else {
        iconTag = "other_icons_" + iconName;
    }
    RState result = ThemePackManager::GetThemePackManager()->GetThemeIconFromCache(iconTag, outValue, len);
    if (result == SUCCESS) {
        return SUCCESS;
    }
    return ThemePackManager::GetThemePackManager()->GetOtherIconsInfo(iconName, outValue, len, isGlobalMask, userId);
}

RState ResourceManagerImpl::IsRawDirFromHap(const std::string &pathName, bool &outValue)
{
    return hapManager_->IsRawDirFromHap(pathName, outValue);
}

std::shared_ptr<HapManager> ResourceManagerImpl::GetHapManager()
{
    return hapManager_;
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
