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

#include "utils/psue_manager.h"

#include "hilog_wrapper.h"
#include "map"
#include "utils/errors.h"

namespace OHOS {
namespace Global {
namespace Resource {
using namespace std;

namespace {
const float DEFAULT_EXTEND_RATIO = 0.3f;
const int DEFAULT_PSUE_LEVEL = 3;

struct ExtendRatioTable {
    int32_t count;
    float ratio;
};
const ExtendRatioTable EXTERN_RATIO_TABLE[] = {
    {10, 2.0f},
    {20, 1.0f},
    {30, 0.8f},
    {50, 0.6f},
    {70, 0.4f},
};

const map<wchar_t, wchar_t> REPLACE_TABLE {
    {L'a', L'a'},
    {L'A', L'a'},
    {L'c', L'a'},
    {L'C', L'a'},
    {L'i', L'a'},
    {L'I', L'a'},
    {L'o', L'a'},
    {L'O', L'a'},
    {L'u', L'a'},
    {L'U', L'a'},
    {L'y', L'a'},
    {L'Y', L'a'},
    {L'z', L'a'},
    {L'Z', L'a'},
    {L's', L'a'},
    {L'S', L'a'},
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

std::string PsueManager::Convert(const std::string &src, std::string &dest)
{
    dest = "[" + dest + "]";
    return src;
}
} // namespace Resource
} // Global
} // OHOS
