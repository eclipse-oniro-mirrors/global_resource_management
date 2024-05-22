/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RESOURCE_MANAGER_RESOURCE_MANANGER_TEST_COMMON_H
#define RESOURCE_MANAGER_RESOURCE_MANANGER_TEST_COMMON_H

#include <climits>
#include <cstring>
#include <gtest/gtest.h>
#define private public

#include "res_config.h"
#include "resource_manager.h"
#include "resource_manager_impl.h"
#include "test_common.h"
#include "utils/errors.h"
#include "utils/string_utils.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResourceManagerTestCommon {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    explicit ResourceManagerTestCommon(ResourceManager *rm);

    explicit ResourceManagerTestCommon(std::shared_ptr<ResourceManager> rm);

    ~ResourceManagerTestCommon();
public:
    ResourceManager *rm;

    std::shared_ptr<ResConfigImpl> defaultResConfig = InitDefaultResConfig();

    int GetResId(const std::string &name, ResType resType);

    void TestStringByName(const char *name, const char *cmp);

    void TestStringById(const char *name, const char *cmp);

    void TestPluralStringById(int quantity, const char *cmp, bool format = false);

    void TestPluralStringByName(int quantity, const char *cmp, bool format = false);

    void TestGetRawFilePathByName(const std::string &name, const std::string &cmp);

    void AddResource(const char *language, const char *script, const char *region);

    void AddColorModeResource(DeviceType deviceType, ColorMode colorMode, float screenDensity);

    void AddHapResource(const char *language, const char *script, const char *region);

    void TestGetStringArrayById(const char *name);

    void TestGetStringArrayByName(const char *name);

    void TestGetStringFormatById(const char *name, const char *cmp);

    void TestGetStringFormatByName(const char *name, const char *cmp);

    void TestGetStringFormatById(const char *name,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams,  const char *cmp);

    void TestGetStringFormatByName(const char *name,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams,  const char *cmp);

    void TestGetPatternById(const char *name);

    void TestGetPatternByName(const char *name);

    void TestGetThemeById(const char *name);

    void TestGetThemeByName(const char *appTheme, const char *testTheme);

    void TestGetBooleanByName(const char *boolean1, const char *booleanRef);

    void TestGetBooleanById(const char *boolean1, const char *booleanRef);

    void TestGetIntegerByName(const char* integer1, const char* integerRef);

    void TestGetIntegerById(const char* integer1, const char* integerRef);

    void TestGetFloatByName(const char* touchTarget, const char* floatRef);

    void TestGetFloatById(const char* touchTarget, const char* floatRef);

    void TestGetIntArrayById(const char* intarray1);

    void TestGetIntArrayByName(const char* intarray1);

    void TestGetProfileById(HapResource *tmp);

    void TestGetProfileByName(HapResource *tmp);

    void TestGetMediaWithDensityById(HapResource *tmp);

    void TestGetMediaById(HapResource *tmp);

    void TestGetMediaWithDensityByName(HapResource *tmp);

    void TestGetMediaByName(HapResource *tmp);

    void TestGetResourceLimitKeys(uint32_t expectedLimitKeys);
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
