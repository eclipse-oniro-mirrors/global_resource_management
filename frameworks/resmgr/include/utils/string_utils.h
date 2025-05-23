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

#ifndef OHOS_RESOURCE_MANAGER_STRINGUTILS_H
#define OHOS_RESOURCE_MANAGER_STRINGUTILS_H

#include "utils/common.h"
#include "resource_manager.h"
#include "res_config_impl.h"

#include <string>
#include <vector>

namespace OHOS {
namespace Global {
namespace Resource {
std::string FormatString(const char *fmt, ...);

EXPORT_FUNC std::string FormatString(const char *fmt, va_list args);

bool parseArgs(const std::string &inputOutputValue, va_list args,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);

bool ReplacePlaceholderWithParams(std::string &inputOutputValue, const ResConfigImpl &resConfig,
    const std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams);
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
