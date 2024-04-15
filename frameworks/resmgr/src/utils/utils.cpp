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
#include "utils/utils.h"

#include <fstream>
#include <vector>
#include <sys/stat.h>
#include "hilog_wrapper.h"
#ifdef __LINUX__
#include <cstring>
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#endif

#ifdef __WINNT__
#include <shlwapi.h>
#include <windows.h>
#undef ERROR
#endif

namespace OHOS {
namespace Global {
namespace Resource {
constexpr int ERROR_RESULT = -1;

const std::set<std::string> Utils::tailSet {
    ".hap",
    ".hsp",
};

std::vector<char> g_codes = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

RState Utils::GetMediaBase64Data(const std::string& iconPath, std::string &base64Data)
{
    size_t len = 0;
    auto tempData = Utils::LoadResourceFile(iconPath, len);
    if (tempData == nullptr) {
        HILOG_ERROR("get the tempData error");
        return NOT_FOUND;
    }
    auto pos = iconPath.find_last_of('.');
    std::string imgType;
    if (pos != std::string::npos) {
        imgType = iconPath.substr(pos + 1);
    }
    Utils::EncodeBase64(tempData, len, imgType, base64Data);
    return SUCCESS;
}

std::unique_ptr<uint8_t[]> Utils::LoadResourceFile(const std::string &path, size_t &len)
{
    std::ifstream mediaStream(path, std::ios::binary);
    if (!mediaStream.is_open()) {
        return nullptr;
    }
    mediaStream.seekg(0, std::ios::end);
    int length = mediaStream.tellg();
    if (length == ERROR_RESULT) {
        HILOG_ERROR("failed to get the file length");
        return nullptr;
    } else {
        len = static_cast<size_t>(length);
    }
    std::unique_ptr<uint8_t[]> tempData = std::make_unique<uint8_t[]>(len);
    if (tempData == nullptr) {
        return nullptr;
    }
    mediaStream.seekg(0, std::ios::beg);
    mediaStream.read(reinterpret_cast<char *>(tempData.get()), len);
    return tempData;
}

RState Utils::EncodeBase64(std::unique_ptr<uint8_t[]> &data, int srcLen,
    const std::string &imgType, std::string &dstData)
{
    const unsigned char *srcData = data.get();
    if (srcData == nullptr) {
        return ERROR;
    }
    std::string base64data;
    base64data += "data:image/" + imgType + ";base64,";
    int i = 0;
    // encode in groups of every 3 bytes
    for (; i < srcLen - 3; i += 3) {
        unsigned char byte1 = static_cast<unsigned char>(srcData[i]);
        unsigned char byte2 = static_cast<unsigned char>(srcData[i + 1]);
        unsigned char byte3 = static_cast<unsigned char>(srcData[i + 2]);
        base64data += g_codes[byte1 >> BitOperatorNum::BIT_TWO];
        base64data += g_codes[((byte1 & 0x3) << BitOperatorNum::BIT_FOUR) | (byte2 >> BitOperatorNum::BIT_FOUR)];
        base64data += g_codes[((byte2 & 0xF) << BitOperatorNum::BIT_TWO) | (byte3 >> BitOperatorNum::BIT_SIX)];
        base64data += g_codes[byte3 & 0x3F];
    }
    // Handle the case where there is one element left
    if (srcLen % ArrayLen::LEN_THREE == 1) {
        unsigned char byte1 = static_cast<unsigned char>(srcData[i]);
        base64data += g_codes[byte1 >> BitOperatorNum::BIT_TWO];
        base64data += g_codes[(byte1 & 0x3) << BitOperatorNum::BIT_FOUR];
        base64data += '=';
        base64data += '=';
    } else {
        unsigned char byte1 = static_cast<unsigned char>(srcData[i]);
        unsigned char byte2 = static_cast<unsigned char>(srcData[i + 1]);
        base64data += g_codes[byte1 >> BitOperatorNum::BIT_TWO];
        base64data += g_codes[((byte1 & 0x3) << BitOperatorNum::BIT_FOUR) | (byte2 >> BitOperatorNum::BIT_FOUR)];
        base64data += g_codes[(byte2 & 0xF) << BitOperatorNum::BIT_TWO];
        base64data += '=';
    }
    dstData = base64data;
    return SUCCESS;
}

bool Utils::IsAlphaString(const char *s, int32_t len)
{
    if (s == nullptr) {
        return false;
    }
    int32_t i;
    for (i = 0; i < len; i++) {
        char c = *(s + i);
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
            return false;
        }
    }
    return true;
}

bool Utils::IsNumericString(const char *s, int32_t len)
{
    if (s == nullptr) {
        return false;
    }
    int32_t i;
    for (i = 0; i < len; i++) {
        char c = *(s + i);
        if (!(c >= '0' && c <= '9')) {
            return false;
        }
    }

    return true;
}

/**
 * @brief decode 32 bits as script array.
 * 31-24 bits is script[0]
 * 23-16 bits is script[1]
 * 15-8 bits is script[2]
 * 0-7 bits is script[3]
 *
 * @param encodeScript
 * @param outValue
 */
void Utils::DecodeScript(uint32_t encodeScript, char *outValue)
{
    if (outValue == nullptr) {
        return;
    }
    outValue[ArrayIndex::INDEX_ZERO] = (encodeScript & 0xFF000000) >> BitOperatorNum::BIT_TWENTY_FOUR;
    outValue[ArrayIndex::INDEX_ONE] = (encodeScript & 0x00FF0000) >> BitOperatorNum::BIT_SIXTEEN;
    outValue[ArrayIndex::INDEX_TWO] = (encodeScript & 0x0000FF00) >> BitOperatorNum::BIT_EIGHT;
    outValue[ArrayIndex::INDEX_THREE] = (encodeScript & 0x000000FF);
}

bool Utils::IsStrEmpty(const char *s)
{
    return (s == nullptr || *s == '\0');
}

size_t Utils::StrLen(const char *s)
{
    if (s == nullptr) {
        return 0;
    }
    return strlen(s);
}

uint16_t Utils::EncodeLanguage(const char *language)
{
    if (Utils::IsStrEmpty(language)) {
        return NULL_LANGUAGE;
    }
    return Utils::EncodeLanguageOrRegion(language, 'a');
}

/**
 * @brief  locale compose of language,script and region,encode as 64bits.
 * 63-48 bits represent language,detail format see EncodeLanguageOrRegion method
 * 47-16 bits represent script,detail format see EncodeScript method
 * 15-0 bits represent region,detail format see EncodeLanguageOrRegion method
 *
 * @param language
 * @param script
 * @param region
 * @return uint64_t
 */
uint64_t Utils::EncodeLocale(const char *language,
                             const char *script,
                             const char *region)
{
    uint16_t languageData = Utils::EncodeLanguage(language);
    uint32_t scriptData = Utils::EncodeScript(script);
    uint16_t regionData = Utils::EncodeRegion(region);

    return (uint64_t)(0xffff000000000000 & (((uint64_t)languageData) << BitOperatorNum::BIT_FORTY_EIGHT)) |
           (0x0000ffffffff0000 & (((uint64_t)scriptData) << BitOperatorNum::BIT_SIXTEEN)) |
           (0x000000000000ffff & (uint64_t)(regionData));
}

uint16_t Utils::EncodeRegionByResLocale(const ResLocale *locale)
{
    if (locale == nullptr) {
        return NULL_REGION;
    }
    return Utils::EncodeRegion(locale->GetRegion());
}

uint16_t Utils::EncodeLanguageByResLocale(const ResLocale *locale)
{
    if (locale == nullptr) {
        return NULL_LANGUAGE;
    }
    return Utils::EncodeLanguage(locale->GetLanguage());
}

uint32_t Utils::EncodeScriptByResLocale(const ResLocale *locale)
{
    if (locale == nullptr) {
        return NULL_SCRIPT;
    }
    return Utils::EncodeScript(locale->GetScript());
}

uint16_t Utils::EncodeRegion(const char *region)
{
    if (Utils::IsStrEmpty(region)) {
        return NULL_REGION;
    }
    if (region[0] >= '0' && region[0] <= '9') {
        return Utils::EncodeLanguageOrRegion(region, '0');
    }
    return Utils::EncodeLanguageOrRegion(region, 'A');
}

/**
 * @brief script is four letter array.encode script array as four bytes.Encode format.
 * 31-24 bits represent script[0]
 * 23-16 bits represent script[1]
 * 15-8 bits represent script[2]
 * 0-7 bits represent script[3]
 *
 * @param script
 * @return uint32_t
 */
uint32_t Utils::EncodeScript(const char *script)
{
    if (Utils::IsStrEmpty(script)) {
        return NULL_SCRIPT;
    }
    return ((uint8_t)script[ArrayIndex::INDEX_ZERO] << BitOperatorNum::BIT_TWENTY_FOUR) |
        ((uint8_t)script[ArrayIndex::INDEX_ONE] << BitOperatorNum::BIT_SIXTEEN) |
        ((uint8_t)script[ArrayIndex::INDEX_TWO] << BitOperatorNum::BIT_EIGHT) |
        (uint8_t)script[ArrayIndex::INDEX_THREE];
}

/**
 * @brief encode language or region str as two byte.
 * language is two or three lowercase.
 * region is two capital  letter or three digit.
 *
 * two char,encode format
 * 15-8 bits is the first char
 * 7-0 bits is the second char
 *
 * three chars,encode format
 * 15 bit is 1
 * 14-10 bits represent the value of  the first char subtract base char,
 * 9-5 bits represent the value of the second char subtract base char  .
 * 4-0 bits represent the value of the third char subtract base char.
 * base char is 'a','A','0'.
 * example when base is 'a',max value('z' - 'a') is 26,so five bits can represent a char.
 *
 * @param str
 * @param base is '0' or 'a' or 'A'
 * @return uint16_t
 */
uint16_t Utils::EncodeLanguageOrRegion(const char *str, char base)
{
    if (str[ArrayIndex::INDEX_TWO] == 0 || str[ArrayIndex::INDEX_TWO] == '-' || str[ArrayIndex::INDEX_TWO] == '_') {
        return ((uint8_t)str[ArrayIndex::INDEX_ZERO] << BitOperatorNum::BIT_EIGHT) |
            ((uint8_t)str[ArrayIndex::INDEX_ONE]);
    }
    uint8_t first = ((uint8_t)(str[ArrayIndex::INDEX_ZERO] - base)) & 0x7f;
    uint8_t second = ((uint8_t)(str[ArrayIndex::INDEX_ONE] - base)) & 0x7f;
    uint8_t third = ((uint8_t)(str[ArrayIndex::INDEX_TWO] - base)) & 0x7f;
    return ((0x80 | (first << BitOperatorNum::BIT_TWO) | (second >> BitOperatorNum::BIT_THREE)) <<
        BitOperatorNum::BIT_EIGHT) | ((second << BitOperatorNum::BIT_FIVE) | third);
};

bool Utils::StrCompare(const char *left, const char *right, size_t len, bool isCaseSensitive)
{
    if (left == nullptr && right == nullptr) {
        return true;
    }

    if (left == nullptr || right == nullptr) {
        return false;
    }

    int rc;
    unsigned char c1;
    unsigned char c2;
    while (len--) {
        c1 = (unsigned char)*left;
        c2 = (unsigned char)*right;
        if (c1 == 0) {
            if (c2 == 0) {
                return true;
            }
            return false;
        } else if (c2 == 0) {
            return false;
        } else {
            if (isCaseSensitive) {
                rc = (int)(c1) - (int)(c2);
            } else {
                rc = tolower(c1) - tolower(c2);
            }
            if (rc != 0) {
                return false;
            }
        }
        ++left;
        ++right;
    }
    return true;
}

/**
 * @brief convert hex char as int value
 *
 * @param c
 * @param state
 * @return uint32_t
 */
static uint32_t ParseHex(char c, RState &state)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 0xa);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 0xa);
    }
    state = INVALID_FORMAT;
    return -1;
}

/**
 * @brief  convert color string to 32 bits value 0xaarrggbb.
 * color string format is
 * #rgb  red (0-f) greed(0-f) blue(0-f)
 * #argb transparency(0-f)  red (0-f) greed(0-f) blue(0-f)
 * #rrggbb red (00-ff) greed(00-ff) blue(00-ff)
 * #aarrggbb transparency(00-ff) red (00-ff) greed(00-ff) blue(00-ff)
 *
 * @param s
 * @param outValue
 * @return RState
 */
RState Utils::ConvertColorToUInt32(const char *s, uint32_t &outValue)
{
    if (s == nullptr) {
        return INVALID_FORMAT;
    }
    uint32_t color = 0;
    RState parseState = SUCCESS;
    size_t len = strlen(s);
    if (*s == '#') {
        if (len == ArrayLen::LEN_FOUR) {
            color |= 0xFF000000;
            color |= ParseHex(s[ArrayIndex::INDEX_ONE], parseState) << BitOperatorNum::BIT_TWENTY;
            color |= ParseHex(s[ArrayIndex::INDEX_ONE], parseState) << BitOperatorNum::BIT_SIXTEEN;
            color |= ParseHex(s[ArrayIndex::INDEX_TWO], parseState) << BitOperatorNum::BIT_TWELVE;
            color |= ParseHex(s[ArrayIndex::INDEX_TWO], parseState) << BitOperatorNum::BIT_EIGHT;
            color |= ParseHex(s[ArrayIndex::INDEX_THREE], parseState) << BitOperatorNum::BIT_FOUR;
            color |= ParseHex(s[ArrayIndex::INDEX_THREE], parseState);
        } else if (len == ArrayLen::LEN_FIVE) {
            color |= ParseHex(s[ArrayIndex::INDEX_ONE], parseState) << BitOperatorNum::BIT_TWENTY_EIGHT;
            color |= ParseHex(s[ArrayIndex::INDEX_ONE], parseState) << BitOperatorNum::BIT_TWENTY_FOUR;
            color |= ParseHex(s[ArrayIndex::INDEX_TWO], parseState) << BitOperatorNum::BIT_TWENTY;
            color |= ParseHex(s[ArrayIndex::INDEX_TWO], parseState) << BitOperatorNum::BIT_SIXTEEN;
            color |= ParseHex(s[ArrayIndex::INDEX_THREE], parseState) << BitOperatorNum::BIT_TWELVE;
            color |= ParseHex(s[ArrayIndex::INDEX_THREE], parseState) << BitOperatorNum::BIT_EIGHT;
            color |= ParseHex(s[ArrayIndex::INDEX_FOUR], parseState) << BitOperatorNum::BIT_FOUR;
            color |= ParseHex(s[ArrayIndex::INDEX_FOUR], parseState);
        } else if (len == ArrayLen::LEN_SEVEN) {
            color |= 0xFF000000;
            color |= ParseHex(s[ArrayIndex::INDEX_ONE], parseState) << BitOperatorNum::BIT_TWENTY;
            color |= ParseHex(s[ArrayIndex::INDEX_TWO], parseState) << BitOperatorNum::BIT_SIXTEEN;
            color |= ParseHex(s[ArrayIndex::INDEX_THREE], parseState) << BitOperatorNum::BIT_TWELVE;
            color |= ParseHex(s[ArrayIndex::INDEX_FOUR], parseState) << BitOperatorNum::BIT_EIGHT;
            color |= ParseHex(s[ArrayIndex::INDEX_FIVE], parseState) << BitOperatorNum::BIT_FOUR;
            color |= ParseHex(s[ArrayIndex::INDEX_SIX], parseState);
        } else if (len == ArrayLen::LEN_NINE) {
            color |= ParseHex(s[ArrayIndex::INDEX_ONE], parseState) << BitOperatorNum::BIT_TWENTY_EIGHT;
            color |= ParseHex(s[ArrayIndex::INDEX_TWO], parseState) << BitOperatorNum::BIT_TWENTY_FOUR;
            color |= ParseHex(s[ArrayIndex::INDEX_THREE], parseState) << BitOperatorNum::BIT_TWENTY;
            color |= ParseHex(s[ArrayIndex::INDEX_FOUR], parseState) << BitOperatorNum::BIT_SIXTEEN;
            color |= ParseHex(s[ArrayIndex::INDEX_FIVE], parseState) << BitOperatorNum::BIT_TWELVE;
            color |= ParseHex(s[ArrayIndex::INDEX_SIX], parseState) << BitOperatorNum::BIT_EIGHT;
            color |= ParseHex(s[ArrayIndex::INDEX_SEVEN], parseState) << BitOperatorNum::BIT_FOUR;
            color |= ParseHex(s[ArrayIndex::INDEX_EIGHT], parseState);
        }
    } else {
        parseState = INVALID_FORMAT;
    }
    outValue = color;
    return parseState;
}

bool Utils::endWithTail(const std::string& path, const std::string& tail)
{
    if (path.size() < tail.size()) {
        HILOG_ERROR("the path is shorter than tail");
        return false;
    }
    return path.compare(path.size() - tail.size(), tail.size(), tail) == 0;
}

bool Utils::IsFileExist(const std::string& filePath)
{
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

bool Utils::ContainsTail(std::string hapPath, std::set<std::string> tailSet)
{
    for (auto tail : tailSet) {
        if (Utils::endWithTail(hapPath, tail)) {
            return true;
        }
    }
    return false;
}

void Utils::CanonicalizePath(const char *path, char *outPath, size_t len)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif
    if (path == nullptr) {
        HILOG_ERROR("path is null");
        return;
    }
    if (strlen(path) >= len) {
        HILOG_ERROR("the length of path longer than len");
        return;
    }
#ifdef __WINNT__
    if (!PathCanonicalizeA(outPath, path)) {
        HILOG_ERROR("failed to canonicalize the path");
        return;
    }
#else
    if (realpath(path, outPath) == nullptr) {
        HILOG_ERROR("failed to realpath the path, %{public}s, errno:%{public}d", path, errno);
        return;
    }
#endif
}

bool Utils::IsPrefix(std::string_view prefix, std::string_view full)
{
    return prefix == full.substr(0, prefix.size());
}

RState Utils::GetFiles(const std::string &strCurrentDir, std::vector<std::string> &vFiles)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__)
    char outPath[PATH_MAX + 1] = {0};
    Utils::CanonicalizePath(strCurrentDir.c_str(), outPath, PATH_MAX);
    if (outPath[0] == '\0') {
        HILOG_ERROR("invalid path, %{public}s", strCurrentDir.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
    DIR *dir;
    struct dirent *pDir;
    if ((dir = opendir(strCurrentDir.c_str())) == nullptr) {
        HILOG_ERROR("opendir failed strCurrentDir = %{public}s", strCurrentDir.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
    while ((pDir = readdir(dir)) != nullptr) {
        if (strcmp(pDir->d_name, ".") == 0 || strcmp(pDir->d_name, "..") == 0) {
            continue;
        }
        if (pDir->d_type != DT_REG && pDir->d_type != DT_DIR) {
            continue;
        }
        vFiles.emplace_back(pDir->d_name);
    }
    closedir(dir);
#endif
    return SUCCESS;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
