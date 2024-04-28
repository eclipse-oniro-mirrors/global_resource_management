/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "utils/psue_manager.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unzip.h>
#include <vector>
#include "hilog_wrapper.h"
#include "map"
#include "utils/errors.h"
#ifdef __WINNT__
#include <windows.h>
#undef ERROR
#endif

namespace OHOS {
namespace Global {
namespace Resource {
using namespace std;

namespace {
const float DEFAULT_EXTEND_RATIO = 0.3f;

struct ExtendRatioTable {
    int32_t count;
    float ratio;
};

const ExtendRatioTable EXTEND_RATIO_RABLE[] = {
    {10, 2.0f},
    {20, 1.0f},
    {30, 0.8f},
    {50, 0.6f},
    {70, 0.4f},
};
const std::wstring PSUE_CONFIG_CHARS = {L"ReÇÉÄßÑ¿ÃóèìжДﺥ"};

const map<wchar_t, wchar_t> REPLACE_TABLE {
    {L'a', L'à'},
    {L'A', L'À'},
    {L'c', L'ć'},
    {L'C', L'Ć'},
    {L'i', L'ì'},
    {L'I', L'Ì'},
    {L'o', L'ó'},
    {L'O', L'Ó'},
    {L'u', L'ü'},
    {L'U', L'Ü'},
    {L'y', L'ÿ'},
    {L'Y', L'Ÿ'},
    {L'z', L'ž'},
    {L'Z', L'Ž'},
};
}

PsueManager::PsueManager()
{
}

PsueManager::~PsueManager()
{
}

/**
  PsuedoTranslation level default value is 3
  1. Enclosed in brackets
  2. Letter replacement
  3. Lengthen string
*/
int g_fakeLocaleLevel = 3;

int g_levelForReplace = 2;
int g_levelForAppend = 3;
int g_levelForAddBracket = 1;

std::string PsueManager::Convert(const std::string &src, std::string &dest)
{
    if (isDigit(src)) {
        return "";
    }
    std::wstring ws;
    std::string wsStr = ToWstring(ws, src);
    if (wsStr != "") {
        return wsStr;
    }
    if (g_fakeLocaleLevel >= g_levelForReplace) {
        // char replace
        ToAccent(ws);
    }
    if (g_fakeLocaleLevel == g_levelForAppend) {
        // enhance length
        unsigned int len = src.length();
        unsigned int extendCount = len * GetExtendRatio(len);
        unsigned int loop = extendCount / PSUE_CONFIG_CHARS.length();
        unsigned int left = extendCount % PSUE_CONFIG_CHARS.length();
        for (unsigned int i = 0; i < loop ; i++) {
            ws += PSUE_CONFIG_CHARS;
        }
        if (left > 0) {
            ws += PSUE_CONFIG_CHARS.substr(0, left);
        }
    }
    std::string tsStr = ToString(dest, ws);
    if (tsStr != "") {
        return tsStr;
    }
    if (g_fakeLocaleLevel >= g_levelForAddBracket) {
        // add brackets
        dest = '[' + dest + ']';
    }
    return "";
}

bool PsueManager::isDigit(const std::string src)
{
    for (unsigned int i = 0 ; i < src.size() ; i++) {
        if (!isdigit(src[i])) {
            return false;
        }
    }
    return true;
}

float PsueManager::GetExtendRatio(int32_t len) const
{
    for (size_t i = 0; i < sizeof(EXTEND_RATIO_RABLE) / sizeof(EXTEND_RATIO_RABLE[0]) ; i++) {
        if (len <= EXTEND_RATIO_RABLE[i].count) {
            return EXTEND_RATIO_RABLE[i].ratio;
        }
    }
    return DEFAULT_EXTEND_RATIO;
}

// letter replace
void PsueManager::ToAccent(wstring &ws) const
{
    for (std::wstring::size_type i = 0 ; i < ws.length(); i++) {
        if (ws[i] == L'%') {
            i++;
        } else if (ws[i] == L'{') {
            while ((i + 1 < ws.length()) && (ws[++i] != L'}')) {}
        } else {
            auto iter = REPLACE_TABLE.find(ws[i]);
            if (iter != REPLACE_TABLE.end()) {
                ws[i] = iter->second;
            }
        }
    }
}

std::string PsueManager::ToWstring(std::wstring &dest, const std::string &src)
{
    std::string result = setlocale(LC_CTYPE, "");
    size_t destSize = mbstowcs(NULL, src.c_str(), 0);
    if (destSize == size_t(-1)) {
        cout << result << endl;
        return "get widechar size fail ";
    }
    vector<wchar_t> buf(destSize + 1);
    if (mbstowcs(&buf[0], src.c_str(), src.size()) == static_cast<size_t>(-1)) {
        return "convert to widechar fail";
    }

    dest.assign(buf.begin(), buf.end() - 1);
    return "";
}

std::string PsueManager::ToString(std::string &dest, const std::wstring &src)
{
    size_t destSize = wcstombs(NULL, src.c_str(), 0);
    if (destSize == size_t(-1)) {
        return "get multibyte size fail";
    }
    vector<char> buf(destSize + 1);
    if (wcstombs(&buf[0], src.c_str(), buf.size()) == static_cast<size_t>(-1)) {
        return "convert to multibyte fail";
    }

    dest.assign(buf.begin(), buf.end() - 1);
    return "";
}

void PsueManager::SetFakeLocaleLevel(const int level)
{
    if (level <= g_levelForAppend && level >= g_levelForAddBracket) {
        g_fakeLocaleLevel = level;
    }
}

bool PsueManager::IsAsciiString(const std::string &src)
{
    bool isAscii = true;
    for (size_t i = 0; i < src.length(); i++) {
        if (src[i] < 0 || src[i] > 127) {  // 127 is the max value of ascii
            isAscii = false;
            break;
        }
    }
    return isAscii;
}

std::string PsueManager::BidirectionConvert(std::string &src)
{
    if (!IsAsciiString(src)) {
        return src;
    }
    std::string result;
    size_t start = 0;
    size_t end = 0;
    while (start < src.length()) {
        while (start < src.length() && isspace(src[start])) {
            start++;
        }
        if (start > end) {
            result += src.substr(end, start - end);
        }
        end = start;
        while (end < src.length() && !isspace(src[end])) {
            end++;
        }
        if (start < src.length()) {
            result += directionHead;
            result += src.substr(start, end - start);
            result += directionTail;
        }
        start = end;
    }
    return result;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS