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
#ifndef OHOS_RESOURCE_MANAGER_UTILS_H
#define OHOS_RESOURCE_MANAGER_UTILS_H
#include <cstddef>
#include <cstdint>
#include "res_locale.h"
#include "rstate.h"
#include <dirent.h>
#include <set>
#include <vector>

namespace OHOS {
namespace Global {
namespace Resource {
enum BitOperatorNum {
    BIT_TWO = 2,
    BIT_THREE = 3,
    BIT_FOUR = 4,
    BIT_FIVE = 5,
    BIT_SIX = 6,
    BIT_EIGHT = 8,
    BIT_TWELVE = 12,
    BIT_SIXTEEN = 16,
    BIT_TWENTY = 20,
    BIT_TWENTY_FOUR = 24,
    BIT_TWENTY_EIGHT = 28,
    BIT_FORTY_EIGHT = 48
};

enum ArrayIndex {
    INDEX_ZERO = 0,
    INDEX_ONE = 1,
    INDEX_TWO = 2,
    INDEX_THREE = 3,
    INDEX_FOUR = 4,
    INDEX_FIVE = 5,
    INDEX_SIX = 6,
    INDEX_SEVEN = 7,
    INDEX_EIGHT = 8
};

enum ArrayLen {
    LEN_THREE = 3,
    LEN_FOUR = 4,
    LEN_FIVE = 5,
    LEN_SEVEN = 7,
    LEN_NINE = 9
};
class Utils {
public:
    static bool IsAlphaString(const char *s, int32_t len);

    static bool IsNumericString(const char *s, int32_t len);

    static bool IsStrEmpty(const char *s);

    static size_t StrLen(const char *s);

    static uint16_t EncodeLanguage(const char *language);

    static uint16_t EncodeLanguageByResLocale(const ResLocale *locale);

    static uint32_t EncodeScript(const char *script);

    static uint32_t EncodeScriptByResLocale(const ResLocale *locale);

    static void DecodeScript(uint32_t encodeScript, char *outValue);

    static uint16_t EncodeRegion(const char *region);

    static uint16_t EncodeRegionByResLocale(const ResLocale *locale);

    static uint64_t EncodeLocale(const char *language,
                                 const char *script,
                                 const char *region);

    static RState ConvertColorToUInt32(const char *s, uint32_t &outValue);

    static std::unique_ptr<uint8_t[]> LoadResourceFile(const std::string &path, size_t &len);

    static RState EncodeBase64(std::unique_ptr<uint8_t[]> &data, int srcLen,
                              const std::string &imgType, std::string &outValue);

    static constexpr float DPI_BASE = 160.0f;

    static bool endWithTail(const std::string& path, const std::string& tail);

    static bool IsFileExist(const std::string& filePath);

    static bool ContainsTail(std::string hapPath, std::set<std::string> tailSet);

    static const std::set<std::string> tailSet;

    static RState GetMediaBase64Data(const std::string& iconPath, std::string &base64Data);

    static void CanonicalizePath(const char *path, char *outPath, size_t len);

    static bool IsPrefix(std::string_view prefix, std::string_view full);

    static RState GetFiles(const std::string &strCurrentDir, std::vector<std::string> &vFiles);
private:

    static uint16_t EncodeLanguageOrRegion(const char *str, char base);

    static bool StrCompare(const char *left, const char *right, size_t len, bool isCaseSensitive);
    static constexpr uint64_t ROOT_LOCALE = 0x0;
    static constexpr uint16_t NULL_LANGUAGE = 0x00;
    static constexpr uint16_t NULL_REGION = 0x00;
    static constexpr uint16_t NULL_SCRIPT = 0x0000;
    static constexpr uint64_t NULL_LOCALE = 0x0;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif