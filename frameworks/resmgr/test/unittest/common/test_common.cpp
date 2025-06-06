/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "test_common.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "utils/errors.h"

#if defined(__linux__)
#include <malloc.h>
#endif

namespace OHOS {
namespace Global {
namespace Resource {
std::string FormatFullPath(const char *fileRelativePath)
{
    const char *value = "/data/test";
    std::string result(value);
    result.append("/");
    result.append(fileRelativePath);
    return result;
}

void PrintIdValues(const std::shared_ptr<IdValues> &idValues)
{
    if (idValues == nullptr) {
        return;
    }
    for (size_t i = 0; i < idValues->GetLimitPathsConst().size(); ++i) {
        auto limitPath = idValues->GetLimitPathsConst()[i];
        RESMGR_HILOGD(RESMGR_TAG, "%zu: folder is: %s, value: %s", i,
            limitPath->GetResConfig()->ToString().c_str(),
            limitPath->GetIdItem()->ToString().c_str());
    }
}

void PrintMapString(const std::map<std::string, std::string> &value)
{
    auto iter = value.begin();
    for (; iter != value.end(); ++iter) {
        std::string key = iter->first;
        std::string val = iter->second;
        RESMGR_HILOGD(RESMGR_TAG, "%s : %s", key.c_str(), val.c_str());
    }
}

void PrintVectorString(const std::vector<std::string> &value)
{
    for (size_t i = 0; i < value.size(); ++i) {
        std::string val = value[i];
        RESMGR_HILOGD(RESMGR_TAG, "%zu : %s", i, val.c_str());
    }
}

ResConfig *CreateResConfig(const char *language, const char *script, const char *region)
{
    ResConfig *resConfig = CreateResConfig();
    if (resConfig == nullptr) {
        return nullptr;
    }
    resConfig->SetLocaleInfo(language, script, region);
    return resConfig;
}

Locale GetLocale(const char *language, const char *script, const char *region)
{
    UErrorCode errCode = U_ZERO_ERROR;
    Locale locale  = icu::LocaleBuilder().setLanguage(language)
        .setRegion(region).setScript(script).build(errCode);
    return locale;
}

std::shared_ptr<ResConfigImpl> InitDefaultResConfig()
{
    std::shared_ptr<ResConfigImpl> resConfig = std::make_shared<ResConfigImpl>();
    if (resConfig == nullptr) {
        return nullptr;
    }
    resConfig->SetAppDarkRes(true);
    return resConfig;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS