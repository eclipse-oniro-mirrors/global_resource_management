/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "utils/string_utils.h"
#include "utils/utils.h"
#include <cctype>
#include <cstdarg>
#include <cstdint>
#include <limits>
#include <vector>
#include <regex>
#include <algorithm>
#include "hilog_wrapper.h"

#ifdef SUPPORT_GRAPHICS
#include "unicode/numberformatter.h"
#endif

#if defined(__WINNT__)
#include <cstring>
#else
#include "securec.h"
#endif

namespace OHOS {
namespace Global {
namespace Resource {
const std::regex PLACEHOLDER_MATCHING_RULES(R"((%%)|%((\d+)\$){0,1}(\.\d)?([dsf]))");
// the whole string match the regex, such as "%1$.2f" in the string "count is %1$.2f"
constexpr uint8_t MATCHE_INDEX_WHOLE_STRING = 0;
// match %%
constexpr uint8_t MATCHE_INDEX_DOUBLE_PERCENT = 1;
// match the placeholder index number, such as 1 in 1$
constexpr uint8_t MATCHE_INDEX_PLACEHOLDER_INDEX = 3;
// match precision, such as .2f
constexpr uint8_t MATCHE_INDEX_PRECISION = 4;
// match type [dsf]
constexpr uint8_t MATCHE_INDEX_PLACEHOLDER_TYPE = 5;
constexpr int32_t INVALID_PRECISION = -1;
const std::string SIZE_T_MAX_STR = std::to_string(std::numeric_limits<size_t>::max());
#ifdef SUPPORT_GRAPHICS
#endif

std::string FormatString(const char *fmt, ...)
{
    std::string strResult;
    if (fmt != nullptr) {
        va_list marker;
        va_start(marker, fmt);
        strResult = FormatString(fmt, marker);
        va_end(marker);
    }
    return strResult;
}

std::string FormatString(const char *fmt, va_list args)
{
    std::string strResult;
    if (fmt != nullptr) {
        va_list tmpArgs;
        va_copy(tmpArgs, args);
        int nLength = vsnprintf(nullptr, 0, fmt, tmpArgs); // compute buffer size
        va_end(tmpArgs);
        std::vector<char> vBuffer(nLength + 1, '\0');
        int nWritten = vsnprintf_s(&vBuffer[0], nLength + 1, nLength, fmt, args);
        if (nWritten > 0) {
            strResult = &vBuffer[0];
        }
    }
    return strResult;
}

bool getJsParams(const std::string &inputOutputValue, va_list args,
    std::vector<std::pair<int, std::string>> paramsWithOutNum,
    std::vector<std::pair<int, std::string>> paramsWithNum,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    std::sort(paramsWithNum.begin(), paramsWithNum.end(),
        [](const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) {
            return a.first < b.first;
        });
    for (size_t i = 0; i < paramsWithOutNum.size(); i++) {
        std::string type = paramsWithOutNum[i].second;
        if (type == "d") {
            int temp = va_arg(args, int);
            jsParams.emplace_back(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(temp));
        } else if (type == "s") {
            char *temp = va_arg(args, char*);
            jsParams.emplace_back(ResourceManager::NapiValueType::NAPI_STRING, temp);
        } else if (type == "f") {
            float temp = va_arg(args, double);
            jsParams.emplace_back(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(temp));
        }
    }
    for (size_t i = 0; i < paramsWithNum.size(); i++) {
        size_t index = static_cast<size_t>(paramsWithNum[i].first);
        std::string type = paramsWithNum[i].second;
        if (index < paramsWithOutNum.size()) {
            if (type != paramsWithOutNum[index].second) {
                return false;
            }
        } else if (index == paramsWithOutNum.size()) {
            paramsWithOutNum.push_back({index, type});
            if (type == "d") {
                int temp = va_arg(args, int);
                jsParams.emplace_back(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(temp));
            } else if (type == "s") {
                char *temp = va_arg(args, char*);
                jsParams.emplace_back(ResourceManager::NapiValueType::NAPI_STRING, temp);
            } else if (type == "f") {
                float temp = va_arg(args, double);
                jsParams.emplace_back(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(temp));
            }
        } else {
            return false;
        }
    }
    return true;
}

bool parseArgs(const std::string &inputOutputValue, va_list args,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    if (inputOutputValue.empty()) {
        return true;
    }
    std::string::const_iterator start = inputOutputValue.begin();
    std::string::const_iterator end = inputOutputValue.end();
    std::smatch matches;
    size_t matchCount = 0;
    int prefixLength = 0;
    int offset = 2;
    std::vector<std::pair<int, std::string>> paramsWithOutNum;
    std::vector<std::pair<int, std::string>> paramsWithNum;
    while (std::regex_search(start, end, matches, PLACEHOLDER_MATCHING_RULES)) {
        prefixLength = matches[MATCHE_INDEX_WHOLE_STRING].first - inputOutputValue.begin();
        if (matches[1].length() != 0) {
            start = inputOutputValue.begin() + prefixLength + offset;
            continue;
        }
        std::string placeholderIndex = matches[MATCHE_INDEX_PLACEHOLDER_INDEX];
        std::string placeholderType = matches[MATCHE_INDEX_PLACEHOLDER_TYPE];
        size_t paramIndex;
        if (placeholderIndex.length() != 0) {
            if (placeholderIndex.size() > SIZE_T_MAX_STR.size() ||
                (placeholderIndex.size() == SIZE_T_MAX_STR.size() && placeholderIndex > SIZE_T_MAX_STR)) {
                RESMGR_HILOGE(RESMGR_TAG, "index of placeholder is too large");
                return false;
            }
            unsigned long index;
            if (!Utils::convertToUnsignedLong(placeholderIndex, index) || index < 1) {
                RESMGR_HILOGE(RESMGR_TAG, "convert value error, placeholderIndex = %{public}s",
                    placeholderIndex.c_str());
                return false;
            }
            paramIndex = index - 1;
            paramsWithNum.push_back({paramIndex, placeholderType});
        } else {
            paramIndex = matchCount++;
            paramsWithOutNum.push_back({paramIndex, placeholderType});
        }
        start = inputOutputValue.begin() + prefixLength + matches[0].length();
    }
    return getJsParams(inputOutputValue, args, paramsWithOutNum, paramsWithNum, jsParams);
}

std::string GetLocalInfo(const ResLocale *resLocale)
{
    std::string localeInfo;
    const char *language = resLocale->GetLanguage();
    if (language != nullptr && strlen(language) > 0) {
        localeInfo.assign(language);
    } else {
        RESMGR_HILOGW(RESMGR_TAG, "GetLocalInfo language is null");
        return localeInfo;
    }
    std::string temp;
    const char *script = resLocale->GetScript();
    if (script != nullptr && strlen(script) > 0) {
        temp.assign(script);
        localeInfo += "-" + temp;
    }
    const char *region = resLocale->GetRegion();
    if (region != nullptr && strlen(region) > 0) {
        temp.assign(region);
        localeInfo += "-" + temp;
    }
    return localeInfo;
}

bool LocalizeNumber(std::string &inputOutputNum, const ResConfigImpl &resConfig,
    const int32_t precision = INVALID_PRECISION)
{
#ifdef SUPPORT_GRAPHICS
    const ResLocale *resLocale = resConfig.GetResLocale();
    if (resLocale == nullptr) {
        RESMGR_HILOGW(RESMGR_TAG, "LocalizeNumber resLocale is null");
        return true;
    }

    std::string localeInfo = GetLocalInfo(resLocale);
    if (localeInfo.empty()) {
        return true;
    }
    icu::Locale locale(localeInfo.c_str());
    if (locale.isBogus()) {
        return true;
    }

    icu::number::LocalizedNumberFormatter numberFormat = icu::number::NumberFormatter::withLocale(locale);
    numberFormat = numberFormat.grouping(UNumberGroupingStrategy::UNUM_GROUPING_OFF);
    numberFormat = numberFormat.roundingMode(UNUM_ROUND_HALFUP);
    if (precision != INVALID_PRECISION) {
        numberFormat = numberFormat.precision(icu::number::Precision::fixedFraction(precision));
    }
    double num;
    if (!Utils::convertToDouble(inputOutputNum, num)) {
        return false;
    }
    inputOutputNum.clear();
    UErrorCode status = U_ZERO_ERROR;
    numberFormat.formatDouble(num, status).toString(status).toUTF8String(inputOutputNum);
    if (status == U_ZERO_ERROR) {
        RESMGR_HILOGE(RESMGR_TAG, "LocalizeNumber failed, status = %{public}d", status);
        return false;
    }
    return true;
#else
    return true;
#endif
}

bool GetReplaceStr(const std::tuple<ResourceManager::NapiValueType, std::string> &jsParam,
    const std::string &placeHolderType, int32_t precision, const ResConfigImpl &config, std::string &replaceStr)
{
    ResourceManager::NapiValueType paramType = std::get<0>(jsParam);
    std::string paramValue = std::get<1>(jsParam);

    // string type
    if (placeHolderType == "s") {
        if (paramType != ResourceManager::NapiValueType::NAPI_STRING) {
            RESMGR_HILOGE(RESMGR_TAG, "the type of placeholder and param does not match");
            return false;
        }
        replaceStr = paramValue;
        return true;
    }

    // number type
    if (paramType != ResourceManager::NapiValueType::NAPI_NUMBER) {
        RESMGR_HILOGE(RESMGR_TAG, "the type of placeholder and param does not match");
        return false;
    }

    // int type
    if (placeHolderType == "d") {
        size_t posOfDecimalPoint = paramValue.find(".");
        replaceStr = paramValue.substr(0, posOfDecimalPoint);
        return LocalizeNumber(replaceStr, config);
    }

    // double type
    replaceStr = paramValue;
    return LocalizeNumber(replaceStr, config, precision);
}

bool MatchPlaceholderIndex(std::string placeholderIndex, size_t &paramIndex, size_t &matchCount)
{
    if (placeholderIndex.length() != 0) {
        if (placeholderIndex.size() > SIZE_T_MAX_STR.size() ||
            (placeholderIndex.size() == SIZE_T_MAX_STR.size() && placeholderIndex > SIZE_T_MAX_STR)) {
            RESMGR_HILOGE(RESMGR_TAG, "index of placeholder is too large");
            return false;
        }
        unsigned long index;
        if (!Utils::convertToUnsignedLong(placeholderIndex, index) || index < 1) {
            return false;
        }
        paramIndex = index - 1;
    } else {
        paramIndex = matchCount++;
    }
    return true;
}

int32_t GetPrecision(const std::string &precisionStr)
{
    size_t posOfDecimalPoint = precisionStr.find(".");
    if (posOfDecimalPoint != std::string::npos) {
        return std::stoi(precisionStr.substr(posOfDecimalPoint + 1));
    }
    return INVALID_PRECISION;
}

bool ReplacePlaceholderWithParams(std::string &inputOutputValue, const ResConfigImpl &resConfig,
    const std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    if (inputOutputValue.empty()) {
        return true;
    }

    std::string::const_iterator start = inputOutputValue.begin();
    std::string::const_iterator end = inputOutputValue.end();
    std::smatch matches;
    size_t matchCount = 0;
    int prefixLength = 0;
    
    while (std::regex_search(start, end, matches, PLACEHOLDER_MATCHING_RULES)) {
        prefixLength = matches[MATCHE_INDEX_WHOLE_STRING].first - inputOutputValue.begin();
        // Matched to %%, replace it with %
        if (matches[MATCHE_INDEX_DOUBLE_PERCENT].length() != 0) {
            inputOutputValue.erase(matches[MATCHE_INDEX_DOUBLE_PERCENT].first);
            start = inputOutputValue.begin() + prefixLength + 1;
            end = inputOutputValue.end();
            continue;
        } else if (jsParams.size() == 0) { // Matched to placeholder but no params, ignore placehold
            start = inputOutputValue.begin() + prefixLength + matches[0].length();
            continue;
        }

        // Matched to placeholder, check and parse param index
        std::string placeholderIndex = matches[MATCHE_INDEX_PLACEHOLDER_INDEX];
        size_t paramIndex;
        if (!MatchPlaceholderIndex(placeholderIndex, paramIndex, matchCount)) {
            return false;
        }
        if (paramIndex >= jsParams.size()) {
            RESMGR_HILOGE(RESMGR_TAG, "index of placeholder out of range");
            return false;
        }
        // Replace placeholder with corresponding param
        std::string replaceStr;
        int32_t precision = GetPrecision(matches[MATCHE_INDEX_PRECISION]);
        std::string placeholderType = matches[MATCHE_INDEX_PLACEHOLDER_TYPE];
        if (!GetReplaceStr(jsParams[paramIndex], placeholderType, precision, resConfig, replaceStr)) {
            return false;
        }
        inputOutputValue.replace(prefixLength, matches[MATCHE_INDEX_WHOLE_STRING].length(), replaceStr);

        // Update iterator
        start = inputOutputValue.begin() + prefixLength + replaceStr.length();
        end = inputOutputValue.end();
    }
    return true;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS