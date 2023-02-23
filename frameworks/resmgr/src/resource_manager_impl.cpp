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
#include "utils/common.h"
#include "utils/string_utils.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
// default logLevel
#ifdef CONFIG_HILOG
LogLevel g_logLevel = LOG_INFO;
#endif

ResourceManager *CreateResourceManager()
{
    ResourceManagerImpl *impl = new (std::nothrow) ResourceManagerImpl;
    if (impl == nullptr) {
        HILOG_ERROR("new ResourceManagerImpl failed when CreateResourceManager");
        return nullptr;
    }
    if (impl->Init()) {
        return impl;
    } else {
        delete (impl);
        return nullptr;
    }
}

ResourceManager::~ResourceManager()
{}

ResourceManagerImpl::ResourceManagerImpl() : hapManager_(nullptr)
{
    psueManager_ = new (std::nothrow) PsueManager();
}

bool ResourceManagerImpl::Init()
{
    ResConfigImpl *resConfig = new (std::nothrow) ResConfigImpl;
    if (resConfig == nullptr) {
        HILOG_ERROR("new ResConfigImpl failed when ResourceManagerImpl::Init");
        return false;
    }
    hapManager_ = new (std::nothrow) HapManager(resConfig);
    if (hapManager_ == nullptr) {
        delete (resConfig);
        HILOG_ERROR("new HapManager failed when ResourceManagerImpl::Init");
        return false;
    }
    if (Utils::IsFileExist(SYSTEM_RESOURCE_OVERLAY_PATH) && Utils::IsFileExist(SYSTEM_RESOURCE_PATH)) {
        vector<string> overlayPaths;
        overlayPaths.push_back(SYSTEM_RESOURCE_OVERLAY_PATH);
        AddResource(SYSTEM_RESOURCE_PATH.c_str(), overlayPaths);
        return true;
    }
    
    if (Utils::IsFileExist(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED) &&
        Utils::IsFileExist(SYSTEM_RESOURCE_PATH_COMPRESSED)) {
        vector<string> overlayPaths;
        overlayPaths.push_back(SYSTEM_RESOURCE_OVERLAY_PATH_COMPRESSED);
        AddResource(SYSTEM_RESOURCE_PATH_COMPRESSED.c_str(), overlayPaths);
        return true;
    }

    if (Utils::IsFileExist(SYSTEM_RESOURCE_PATH)) {
        AddResource(SYSTEM_RESOURCE_PATH.c_str());
    } else {
        AddResource(SYSTEM_RESOURCE_PATH_COMPRESSED.c_str());
    }
    return true;
}

RState ResourceManagerImpl::GetStringById(uint32_t id, std::string &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by string id error");
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
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::STRING);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by string name error");
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
    const IdItem *idItem = hapManager_->FindResourceById(id);
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
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::STRING);
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

RState ResourceManagerImpl::GetString(const IdItem *idItem, std::string &outValue)
{
    // not found or type invalid
    if (idItem == nullptr || idItem->resType_ != ResType::STRING) {
        return NOT_FOUND;
    }
    RState ret = ResolveReference(idItem->value_, outValue);
    if (isFakeLocale) {
        ProcessPsuedoTranslate(outValue);
    }
    if (ret != SUCCESS) {
        return ret;
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetStringArrayById(uint32_t id, std::vector<std::string> &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by string array id error");
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
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::STRINGARRAY);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by string array name error");
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetStringArray(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetStringArray(const IdItem *idItem, std::vector<std::string> &outValue)
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
            HILOG_ERROR("ResolveReference failed, value:%s", idItem->values_[i].c_str());
            return rrRet;
        }
        outValue.push_back(resolvedValue);
    }
    if (isFakeLocale) {
        for (auto &iter : outValue) {
            ProcessPsuedoTranslate(iter);
        }
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetPatternById(uint32_t id, std::map<std::string, std::string> &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    return GetPattern(idItem, outValue);
}

RState ResourceManagerImpl::GetPatternByName(const char *name, std::map<std::string, std::string> &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::PATTERN);
    return GetPattern(idItem, outValue);
}

RState ResourceManagerImpl::GetPattern(const IdItem *idItem, std::map<std::string, std::string> &outValue)
{
    // not found or type invalid
    if (idItem == nullptr || idItem->resType_ != ResType::PATTERN) {
        return NOT_FOUND;
    }
    return ResolveParentReference(idItem, outValue);
}

RState ResourceManagerImpl::GetPluralStringById(uint32_t id, int quantity, std::string &outValue)
{
    const HapResource::ValueUnderQualifierDir *vuqd = hapManager_->FindQualifierValueById(id);
    return GetPluralString(vuqd, quantity, outValue);
}

RState ResourceManagerImpl::GetPluralStringByName(const char *name, int quantity, std::string &outValue)
{
    const HapResource::ValueUnderQualifierDir *vuqd =
        hapManager_->FindQualifierValueByName(name, ResType::PLURALS);
    return GetPluralString(vuqd, quantity, outValue);
}

RState ResourceManagerImpl::GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...)
{
    const HapResource::ValueUnderQualifierDir *vuqd = hapManager_->FindQualifierValueById(id);
    if (vuqd == nullptr) {
        HILOG_ERROR("find qualifier value by plural id error");
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    std::string temp;
    RState rState = GetPluralString(vuqd, quantity, temp);
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
    const HapResource::ValueUnderQualifierDir *vuqd =
        hapManager_->FindQualifierValueByName(name, ResType::PLURALS);
    if (vuqd == nullptr) {
        HILOG_ERROR("find qualifier value by plural name error");
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    std::string temp;
    RState rState = GetPluralString(vuqd, quantity, temp);
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

RState ResourceManagerImpl::GetPluralString(const HapResource::ValueUnderQualifierDir *vuqd,
    int quantity, std::string &outValue)
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
        // 2 means key and value appear in pairs
        std::string key(idItem->values_[startIdx + i * 2]);
        std::string value(idItem->values_[startIdx + i * 2 + 1]);
        auto iter = map.find(key);
        if (iter == map.end()) {
            std::string resolvedValue;
            RState rrRet = ResolveReference(value, resolvedValue);
            if (rrRet != SUCCESS) {
                HILOG_ERROR("ResolveReference failed, value:%s", value.c_str());
                return rrRet;
            }
            map[key] = resolvedValue;
        }
    }

    std::string converted = hapManager_->GetPluralRulesAndSelect(quantity);
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
    return SUCCESS;
}

RState ResourceManagerImpl::ResolveReference(const std::string value, std::string &outValue)
{
    int id;
    ResType resType;
    bool isRef = true;
    int count = 0;
    std::string refStr(value);
    while (isRef) {
        isRef = IdItem::IsRef(refStr, resType, id);
        if (!isRef) {
            outValue = refStr;
            return SUCCESS;
        }

        if (IdItem::IsArrayOfType(resType)) {
            // can't be array
            HILOG_ERROR("ref %s can't be array", refStr.c_str());
            return ERROR;
        }
        const IdItem *idItem = hapManager_->FindResourceById(id);
        if (idItem == nullptr) {
            HILOG_ERROR("ref %s id not found", refStr.c_str());
            return ERROR;
        }
        // unless compile bug
        if (resType != idItem->resType_) {
            HILOG_ERROR("impossible. ref %s type mismatch, found type: %d", refStr.c_str(), idItem->resType_);
            return ERROR;
        }

        refStr = idItem->value_;

        if (++count > MAX_DEPTH_REF_SEARCH) {
            HILOG_ERROR("ref %s has re-ref too much", value.c_str());
            return ERROR_CODE_RES_REF_TOO_MUCH;
        }
    }
    return SUCCESS;
}

RState ResourceManagerImpl::ResolveParentReference(const IdItem *idItem, std::map<std::string, std::string> &outValue)
{
    // only pattern and theme
    // ref always at idx 0
    // child will cover parent
    if (idItem == nullptr) {
        return ERROR;
    }
    if (!(idItem->resType_ == THEME || idItem->resType_ == PATTERN)) {
        HILOG_ERROR("only pattern and theme have parent: %d", idItem->resType_);
        return ERROR;
    }
    outValue.clear();

    bool haveParent = false;
    int count = 0;
    const IdItem *currItem = idItem;
    do {
        haveParent = currItem->HaveParent();
        size_t startIdx = haveParent ? 1 : 0;
        // add currItem values into map when key is absent
        // this make sure child covers parent
        size_t loop = currItem->values_.size() / 2;
        for (size_t i = 0; i < loop; ++i) {
            // 2 means key and value appear in pairs
            std::string key(currItem->values_[startIdx + i * 2]);
            std::string value(currItem->values_[startIdx + i * 2 + 1]);
            auto iter = outValue.find(key);
            if (iter == outValue.end()) {
                std::string resolvedValue;
                RState rrRet = ResolveReference(value, resolvedValue);
                if (rrRet != SUCCESS) {
                    HILOG_ERROR("ResolveReference failed, value:%s", value.c_str());
                    return ERROR;
                }
                outValue[key] = resolvedValue;
            }
        }
        if (haveParent) {
            // get parent
            int id;
            ResType resType;
            bool isRef = IdItem::IsRef(currItem->values_[0], resType, id);
            if (!isRef) {
                HILOG_ERROR("something wrong, pls check HaveParent(). idItem: %s", idItem->ToString().c_str());
                return ERROR;
            }
            currItem = hapManager_->FindResourceById(id);
            if (currItem == nullptr) {
                HILOG_ERROR("ref %s id not found", idItem->values_[0].c_str());
                return ERROR;
            }
        }

        if (++count > MAX_DEPTH_REF_SEARCH) {
            HILOG_ERROR(" %u has too many parents", idItem->id_);
            return ERROR;
        }
    } while (haveParent);

    return SUCCESS;
}

RState ResourceManagerImpl::GetBooleanById(uint32_t id, bool &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by Boolean id error");
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
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::BOOLEAN);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by Boolean name error");
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetBoolean(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetBoolean(const IdItem *idItem, bool &outValue)
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

RState ResourceManagerImpl::GetFloatById(uint32_t id, float &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by Float id error");
        return ERROR_CODE_RES_ID_NOT_FOUND;
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
    const IdItem *idItem = hapManager_->FindResourceById(id);
    return GetFloat(idItem, outValue, unit);
}

RState ResourceManagerImpl::GetFloatByName(const char *name, float &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::FLOAT);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by Float name error");
        return ERROR_CODE_RES_NAME_NOT_FOUND;
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
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::FLOAT);
    return GetFloat(idItem, outValue, unit);
}

RState ResourceManagerImpl::RecalculateFloat(const std::string &unit, float &result)
{
    ResConfigImpl rc;
    GetResConfig(rc);
    float density = rc.GetScreenDensity();
    if (density == SCREEN_DENSITY_NOT_SET) {
        HILOG_INFO("RecalculateFloat srcDensity SCREEN_DENSITY_NOT_SET ");
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
    std::regex reg("(\\+|-)?\\d+(\\.\\d+)? *(px|vp|fp)?");
    std::smatch floatMatch;
    if (!regex_search(strValue, floatMatch, reg)) {
        HILOG_ERROR("not valid float value %{public}s", strValue.c_str());
        return ERROR;
    }
    std::string matchString(floatMatch.str());
    unit = floatMatch[floatMatch.size() - 1];
    std::istringstream stream(matchString.substr(0, matchString.length() - unit.length()));
    stream >> result;
    return SUCCESS;
}

RState ResourceManagerImpl::GetFloat(const IdItem *idItem, float &outValue, std::string &unit)
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
    const IdItem *idItem = hapManager_->FindResourceById(id);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by Integer id error");
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
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::INTEGER);
    if (idItem == nullptr) {
        HILOG_ERROR("find resource by Integer name error");
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState state = GetInteger(idItem, outValue);
    if (state != SUCCESS && state != ERROR_CODE_RES_REF_TOO_MUCH) {
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    return state;
}

RState ResourceManagerImpl::GetInteger(const IdItem *idItem, int &outValue)
{
    if (idItem == nullptr || idItem->resType_ != ResType::INTEGER) {
        return NOT_FOUND;
    }
    std::string temp;
    RState state = ResolveReference(idItem->value_, temp);
    if (state == SUCCESS) {
        outValue = stoi(temp);
        return SUCCESS;
    }
    return state;
}

RState ResourceManagerImpl::GetColorById(uint32_t id, uint32_t &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    return GetColor(idItem, outValue);
}

RState ResourceManagerImpl::GetColorByName(const char *name, uint32_t &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::COLOR);
    return GetColor(idItem, outValue);
}

RState ResourceManagerImpl::GetColor(const IdItem *idItem, uint32_t &outValue)
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

RState ResourceManagerImpl::GetIntArrayById(uint32_t id, std::vector<int> &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    return GetIntArray(idItem, outValue);
}

RState ResourceManagerImpl::GetIntArrayByName(const char *name, std::vector<int> &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::INTARRAY);
    return GetIntArray(idItem, outValue);
}

RState ResourceManagerImpl::GetIntArray(const IdItem *idItem, std::vector<int> &outValue)
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
            HILOG_ERROR("ResolveReference failed, value:%s", idItem->values_[i].c_str());
            return ERROR;
        }
        outValue.push_back(stoi(resolvedValue));
    }
    return SUCCESS;
}

RState ResourceManagerImpl::GetThemeById(uint32_t id, std::map<std::string, std::string> &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceById(id);
    return GetTheme(idItem, outValue);
}

RState ResourceManagerImpl::GetThemeByName(const char *name, std::map<std::string, std::string> &outValue)
{
    const IdItem *idItem = hapManager_->FindResourceByName(name, ResType::THEME);
    return GetTheme(idItem, outValue);
}

RState ResourceManagerImpl::GetTheme(const IdItem *idItem, std::map<std::string, std::string> &outValue)
{
// not found or type invalid
    if (idItem == nullptr || idItem->resType_ != ResType::THEME) {
        return NOT_FOUND;
    }
    return ResolveParentReference(idItem, outValue);
}

RState ResourceManagerImpl::GetProfileById(uint32_t id, std::string &outValue)
{
    auto qd = hapManager_->FindQualifierValueById(id);
    if (qd == nullptr) {
        return NOT_FOUND;
    }
    return hapManager_->GetFilePath(qd, ResType::PROF, outValue);
}

RState ResourceManagerImpl::GetProfileByName(const char *name, std::string &outValue)
{
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::PROF);
    if (qd == nullptr) {
        return NOT_FOUND;
    }
    return hapManager_->GetFilePath(qd, ResType::PROF, outValue);
}

RState ResourceManagerImpl::GetMediaById(uint32_t id, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueById(id, density);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by Media id error");
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState state = hapManager_->GetFilePath(qd, ResType::MEDIA, outValue);
    return state == SUCCESS ? state : ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImpl::GetMediaByName(const char *name, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, density);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by Media name error");
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
    int len = outValue.length() + 1;
    char src[len];
    if (strcpy_s(src, len, outValue.c_str()) == EOK) {
        std::string resultMsg = psueManager_->Convert(src, outValue);
        if (resultMsg != "") {
            HILOG_ERROR("Psuedo translate failed, value:%s", src);
        }
    }
}

ResourceManagerImpl::~ResourceManagerImpl()
{
    if (hapManager_ != nullptr) {
        delete hapManager_;
        hapManager_ = nullptr;
    }
    if (psueManager_ != nullptr) {
        delete (psueManager_);
        psueManager_ = nullptr;
    }
}

bool ResourceManagerImpl::AddResource(const char *path)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif
    return this->hapManager_->AddResource(path);
}

bool ResourceManagerImpl::AddResource(const std::string &path, const std::vector<std::string> &overlayPaths)
{
    return this->hapManager_->AddResource(path, overlayPaths);
}

RState ResourceManagerImpl::UpdateResConfig(ResConfig &resConfig)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif
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
    }
#endif
    return this->hapManager_->UpdateResConfig(resConfig);
}

void ResourceManagerImpl::GetResConfig(ResConfig &resConfig)
{
    this->hapManager_->GetResConfig(resConfig);
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

RState ResourceManagerImpl::GetMediaDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return NOT_SUPPORT_SEP;
    }
    auto qd = hapManager_->FindQualifierValueById(id, density);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by media id error");
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    return hapManager_->GetMediaData(qd, len, outValue);
}

RState ResourceManagerImpl::GetMediaDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue,
    uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return NOT_SUPPORT_SEP;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, density);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by media name error");
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    return hapManager_->GetMediaData(qd, len, outValue);
}

RState ResourceManagerImpl::GetMediaBase64DataById(uint32_t id, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return NOT_SUPPORT_SEP;
    }
    auto qd = hapManager_->FindQualifierValueById(id, density);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by media id error");
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    return hapManager_->GetMediaBase64Data(qd, outValue);
}

RState ResourceManagerImpl::GetMediaBase64DataByName(const char *name, std::string &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return NOT_SUPPORT_SEP;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, density);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by media name error");
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    return hapManager_->GetMediaBase64Data(qd, outValue);
}

RState ResourceManagerImpl::GetProfileDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    auto qd = hapManager_->FindQualifierValueById(id);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by profile id error");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return hapManager_->GetProfileData(qd, len, outValue);
}

RState ResourceManagerImpl::GetProfileDataByName(const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::PROF);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by profile name error");
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

bool ResourceManagerImpl::IsFileExist(const std::string& path)
{
    std::fstream inputFile;
    inputFile.open(path, std::ios::in);
    if (inputFile) {
        return true;
    }
    return false;
}

RState ResourceManagerImpl::GetRawFileList(const std::string rawDirPath, std::vector<std::string>& rawfileList)
{
    return hapManager_->GetRawFileList(rawDirPath, rawfileList);
}

std::string GetSuffix(const HapResource::ValueUnderQualifierDir *qd)
{
    const IdItem *idItem = qd->GetIdItem();
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

RState ResourceManagerImpl::GetDrawableInfoById(uint32_t id, std::string &type, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueById(id, density);
    if (qd == nullptr) {
        HILOG_ERROR("find qualifier value by %{public}zu error", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    type = GetSuffix(qd);
    if (type.empty()) {
        HILOG_ERROR("failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return hapManager_->GetMediaData(qd, len, outValue);
}

RState ResourceManagerImpl::GetDrawableInfoByName(const char *name, std::string &type, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (!IsDensityValid(density)) {
        HILOG_ERROR("density invalid");
        return ERROR_CODE_INVALID_INPUT_PARAMETER;
    }
    auto qd = hapManager_->FindQualifierValueByName(name, ResType::MEDIA, density);
    if (qd == nullptr) {
        HILOG_ERROR("failed to find qualifier value by %{public}s", name);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    type = GetSuffix(qd);
    if (type.empty()) {
        HILOG_ERROR("failed to get resourceType");
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }
    return hapManager_->GetMediaData(qd, len, outValue);
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
