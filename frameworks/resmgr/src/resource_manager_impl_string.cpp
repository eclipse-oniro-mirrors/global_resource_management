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
#include <tuple>
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "parameter.h"
#endif

namespace OHOS {
namespace Global {
namespace Resource {

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
        RESMGR_HILOGD(RESMGR_TAG, "GetStringByName error name = %{public}s", name);
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
    if (ParseArgs(outValue, args, jsParams)) {
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
    if (ParseArgs(outValue, args, jsParams)) {
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
        RESMGR_HILOGD(RESMGR_TAG, "GetStringArrayByName error name = %{public}s", name);
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

RState ResourceManagerImpl::GetPluralStringById(uint32_t id, int quantity, std::string &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    Quantity intQuantity = { true, quantity, 0.0 };
    return GetPluralString(idItem, intQuantity, outValue);
}

RState ResourceManagerImpl::GetPluralStringByName(const char *name, int quantity, std::string &outValue)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::PLURALS, isOverrideResMgr_);
    Quantity intQuantity = { true, quantity, 0.0 };
    return GetPluralString(idItem, intQuantity, outValue);
}

RState ResourceManagerImpl::GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...)
{
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetPluralStringByIdFormat error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    std::string temp;
    Quantity intQuantity = { true, quantity, 0.0 };
    RState rState = GetPluralString(idItem, intQuantity, temp);
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
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::PLURALS, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetPluralStringByNameFormat error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    std::string temp;
    Quantity intQuantity = { true, quantity, 0.0 };
    RState rState = GetPluralString(idItem, intQuantity, temp);
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

RState ResourceManagerImpl::GetPluralString(const std::shared_ptr<IdItem> &idItem,
    Quantity quantity, std::string &outValue)
{
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

void ResourceManagerImpl::ProcessPsuedoTranslate(std::string &outValue)
{
    std::string src = outValue;
    std::string resultMsg = psueManager_->Convert(src, outValue);
    if (resultMsg != "") {
        RESMGR_HILOGE(RESMGR_TAG, "Psuedo translate failed, value:%s", src.c_str());
    }
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
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFormatPluralStringById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState rState = GetPluralString(idItem, quantity, outValue);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        RESMGR_HILOGE(RESMGR_TAG, "find too much ref by plural id = %{public}d", id);
        return rState;
    }
    if (rState != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "plural res not found, id = %{public}d", id);
        return ERROR_CODE_RES_NOT_FOUND_BY_ID;
    }

    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (ParseArgs(outValue, args, params)) {
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
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::PLURALS, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetFormatPluralStringByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState rState = GetPluralString(idItem, quantity, outValue);
    if (rState == ERROR_CODE_RES_REF_TOO_MUCH) {
        RESMGR_HILOGE(RESMGR_TAG, "find too much ref by plural name = %{public}s", name);
        return rState;
    }
    if (rState != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG, "plural res not found, name = %{public}s", name);
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    if (ParseArgs(outValue, args, params)) {
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
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceById(id, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "GetFormatPluralStringById error id = %{public}d", id);
        return ERROR_CODE_RES_ID_NOT_FOUND;
    }
    RState rState = GetPluralString(idItem, quantity, outValue);
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
    const std::shared_ptr<IdItem> idItem = hapManager_->FindResourceByName(name, ResType::PLURALS, isOverrideResMgr_);
    if (idItem == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "GetFormatPluralStringByName error name = %{public}s", name);
        return ERROR_CODE_RES_NAME_NOT_FOUND;
    }
    RState rState = GetPluralString(idItem, quantity, outValue);
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
} // namespace Resource
} // namespace Global
} // namespace OHOS
