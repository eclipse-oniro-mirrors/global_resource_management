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

#include "resource_manager_test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

ResourceManagerTestCommon::ResourceManagerTestCommon(ResourceManager *rm) : rm(rm)
{}

ResourceManagerTestCommon::ResourceManagerTestCommon(std::shared_ptr<ResourceManager> rm)
{
    this->rm = rm.get();
}

ResourceManagerTestCommon::~ResourceManagerTestCommon()
{}

void ResourceManagerTestCommon::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResourceManagerTestCommon::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerTestCommon::SetUp(void)
{
    this->rm = CreateResourceManager();
}

void ResourceManagerTestCommon::TearDown(void)
{
    delete this->rm;
}

int ResourceManagerTestCommon::GetResId(const std::string &name, ResType resType)
{
    auto idv = ((ResourceManagerImpl *)rm)->hapManager_->GetResourceListByName(name.c_str(), resType);
    if (idv.size() == 0) {
        return -1;
    }

    PrintIdValues(idv[0]);
    if (idv[0]->GetLimitPathsConst().size() > 0) {
        return idv[0]->GetLimitPathsConst()[0]->GetIdItem()->id_;
    }
    return OBJ_NOT_FOUND;
}

void ResourceManagerTestCommon::TestStringByName(const char *name, const char *cmp)
{
    std::string outValue;
    RState rState = rm->GetStringByName(name, outValue);
    ASSERT_EQ(SUCCESS, rState);
    HILOG_DEBUG("%s : %s", name, outValue.c_str());
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTestCommon::TestStringById(const char *name, const char *cmp)
{
    std::string outValue;
    int id = GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    RState rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTestCommon::AddResource(const char *language, const char *script, const char *region)
{
    if (language != nullptr || region != nullptr) {
        auto rc = CreateResConfig();
        if (rc == nullptr) {
            EXPECT_TRUE(false);
            return;
        }
        rc->SetLocaleInfo(language, script, region);
        rm->UpdateResConfig(*rc);
        delete rc;
    }
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
}

void ResourceManagerTestCommon::AddHapResource(const char *language, const char *script, const char *region)
{
    if (language != nullptr || region != nullptr) {
        auto rc = CreateResConfig();
        if (rc == nullptr) {
            EXPECT_TRUE(false);
            return;
        }
        rc->SetLocaleInfo(language, script, region);
        rm->UpdateResConfig(*rc);
        delete rc;
    }
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
}

void ResourceManagerTestCommon::AddColorModeResource(DeviceType deviceType, ColorMode colorMode,
    float screenDensity)
{
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, nullptr);
    rc->SetDeviceType(deviceType);
    rc->SetColorMode(colorMode);
    rc->SetScreenDensity(screenDensity);
    rm->UpdateResConfig(*rc);
    delete rc;
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
}

void ResourceManagerTestCommon::TestPluralStringById(int quantity, const char *cmp, bool format)
{
    RState ret;
    std::string outValue;
    int id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);
    if (format) {
        ret = rm->GetPluralStringByIdFormat(outValue, id, quantity, quantity);
    } else {
        ret = rm->GetPluralStringById(id, quantity, outValue);
    }

    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTestCommon::TestPluralStringByName(int quantity, const char *cmp, bool format)
{
    RState ret;
    std::string outValue;
    const char *name = "eat_apple";
    if (format) {
        ret = rm->GetPluralStringByNameFormat(outValue, name, quantity, quantity);
    } else {
        ret = rm->GetPluralStringByName(name, quantity, outValue);
    }

    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string(cmp), outValue);
}

void ResourceManagerTestCommon::TestGetRawFilePathByName(const std::string &name, const std::string &cmp)
{
    std::string outValue;
    rm->GetRawFilePathByName(name, outValue);
    HILOG_DEBUG("%s : %s", name.c_str(), outValue.c_str());
    ASSERT_EQ(cmp, outValue);
}

void ResourceManagerTestCommon::TestGetProfileById(HapResource *tmp)
{
    tmp->Init(this->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/profile/test_profile.json");

    std::string outValue;
    RState state;
    int id = GetResId("test_profile", ResType::PROF);
    EXPECT_TRUE(id > 0);
    state = rm->GetProfileById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

void ResourceManagerTestCommon::TestGetProfileByName(HapResource *tmp)
{
    tmp->Init(this->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/profile/test_profile.json");

    std::string outValue;
    RState state = rm->GetProfileByName("test_profile", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

void ResourceManagerTestCommon::TestGetMediaById(HapResource *tmp)
{
    tmp->Init(this->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/media/icon1.png");

    std::string outValue;
    int id = GetResId("icon1", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    RState state = rm->GetMediaById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

void ResourceManagerTestCommon::TestGetMediaWithDensityById(HapResource *tmp)
{
    tmp->Init(this->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/sdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 120;
    std::string outValue;
    int id = GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    RState state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

void ResourceManagerTestCommon::TestGetMediaByName(HapResource *tmp)
{
    tmp->Init(this->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/base/media/icon1.png");

    std::string outValue;
    RState state = rm->GetMediaByName("icon1", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

void ResourceManagerTestCommon::TestGetMediaWithDensityByName(HapResource *tmp)
{
    tmp->Init(this->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/sdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    uint32_t density = 120;
    std::string outValue;
    RState state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

void ResourceManagerTestCommon::TestGetStringFormatById(const char *name, const char *cmp)
{
    int id = GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    RState state = rm->GetStringFormatById(outValue, id, 101); // 101 means the format number
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(cmp, outValue);
}

void ResourceManagerTestCommon::TestGetStringFormatByName(const char *name, const char *cmp)
{
    std::string outValue;
    RState state = rm->GetStringFormatByName(outValue, name, 101); // 101 means the format number
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(cmp, outValue);
}

void ResourceManagerTestCommon::TestGetStringArrayById(const char *name)
{
    std::vector<std::string> outValue;
    int id = GetResId(name, ResType::STRINGARRAY);
    RState state = rm->GetStringArrayById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size()); // 4 means the size of string array resource
    PrintVectorString(outValue);
}

void ResourceManagerTestCommon::TestGetStringFormatById(const char *name,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams,  const char *cmp)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    uint32_t id = GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(cmp, outValue);
}

void ResourceManagerTestCommon::TestGetStringFormatByName(const char *name,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams,  const char *cmp)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(cmp, outValue);
}

void ResourceManagerTestCommon::TestGetStringArrayByName(const char *name)
{
    std::vector<std::string> outValue;
    RState state = rm->GetStringArrayByName(name, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size()); // 4 means the size of string array resource
    PrintVectorString(outValue);
}

void ResourceManagerTestCommon::TestGetPatternById(const char *name)
{
    std::map<std::string, std::string> outValue;
    int id = GetResId(name, ResType::PATTERN);
    RState state = rm->GetPatternById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(3), outValue.size()); // 3 means the size of pattern resource
    PrintMapString(outValue);
}

void ResourceManagerTestCommon::TestGetPatternByName(const char *name)
{
    std::map<std::string, std::string> outValue;
    RState state = rm->GetPatternByName(name, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(3), outValue.size()); // 3 means the size of pattern resource
    PrintMapString(outValue);
}

void ResourceManagerTestCommon::TestGetThemeById(const char *name)
{
    std::map<std::string, std::string> outValue;
    int id = GetResId(name, ResType::THEME);
    ASSERT_TRUE(id > 0);
    RState state = rm->GetThemeById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
}

void ResourceManagerTestCommon::TestGetThemeByName(const char *appTheme, const char *testTheme)
{
    std::map<std::string, std::string> outValue;
    RState state = rm->GetThemeByName(appTheme, outValue);
    ASSERT_EQ(SUCCESS, state);
    PrintMapString(outValue);

    state = rm->GetThemeByName(testTheme, outValue);
    ASSERT_EQ(SUCCESS, state);
}

void ResourceManagerTestCommon::TestGetBooleanById(const char* boolean1, const char* booleanRef)
{
    bool outValue = true;
    int id = GetResId(boolean1, ResType::BOOLEAN);
    ASSERT_TRUE(id > 0);
    RState state = rm->GetBooleanById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);

    id = GetResId(booleanRef, ResType::BOOLEAN);
    ASSERT_TRUE(id > 0);
    state = rm->GetBooleanById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);
}

void ResourceManagerTestCommon::TestGetBooleanByName(const char* boolean1, const char* booleanRef)
{
    bool outValue = true;
    RState state = rm->GetBooleanByName(boolean1, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);

    state = rm->GetBooleanByName(booleanRef, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_TRUE(outValue);
}

void ResourceManagerTestCommon::TestGetIntegerById(const char* integer1, const char* integerRef)
{
    int outValue;
    int id = GetResId(integer1, ResType::INTEGER);
    ASSERT_TRUE(id > 0);
    RState state = rm->GetIntegerById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue); // 101 means the result of int resource

    id = GetResId(integerRef, ResType::INTEGER);
    ASSERT_TRUE(id > 0);
    state = rm->GetIntegerById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue); // 101 means the result of int resource
}

void ResourceManagerTestCommon::TestGetIntegerByName(const char* integer1, const char* integerRef)
{
    int outValue;
    RState state = rm->GetIntegerByName(integer1, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue); // 101 means the result of int resource


    state = rm->GetIntegerByName(integerRef, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(101, outValue); // 101 means the result of int resource
}

void ResourceManagerTestCommon::TestGetFloatById(const char* touchTarget, const char* floatRef)
{
    float outValue;
    int id = GetResId(touchTarget, ResType::FLOAT);
    ASSERT_TRUE(id > 0);
    RState state = rm->GetFloatById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(48, outValue); // 48vp

    std::string unit;
    state = rm->GetFloatById(id, outValue, unit);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(48, outValue); // 48vp
    EXPECT_EQ("vp", unit);

    id = GetResId(floatRef, ResType::FLOAT);
    ASSERT_TRUE(id > 0);
    state = rm->GetFloatById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(707, outValue); // 707vp
}

void ResourceManagerTestCommon::TestGetFloatByName(const char* touchTarget, const char* floatRef)
{
    float outValue;
    RState state = rm->GetFloatByName(touchTarget, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(48, outValue); // 48vp

    std::string unit;
    state = rm->GetFloatByName(touchTarget, outValue, unit);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(48, outValue); // 48vp
    EXPECT_EQ("vp", unit);

    state = rm->GetFloatByName(floatRef, outValue);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(707, outValue); // 707vp
}

void ResourceManagerTestCommon::TestGetIntArrayById(const char* intarray1)
{
    std::vector<int> outValue;
    int id = GetResId(intarray1, ResType::INTARRAY);
    EXPECT_TRUE(id > 0);
    RState state = rm->GetIntArrayById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(3), outValue.size()); // 3 means the size of int array resource
    EXPECT_EQ(100, outValue[0]); // 100 means the first value of int array
    EXPECT_EQ(200, outValue[1]); // 200 means the second value of int array
    EXPECT_EQ(101, outValue[2]); // 101 means the third value of int array
}

void ResourceManagerTestCommon::TestGetIntArrayByName(const char* intarray1)
{
    std::vector<int> outValue;
    RState state = rm->GetIntArrayByName(intarray1, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(3), outValue.size()); // 3 means the size of int array resource
    EXPECT_EQ(100, outValue[0]); // 100 means the first value of int array
    EXPECT_EQ(200, outValue[1]); // 200 means the second value of int array
    EXPECT_EQ(101, outValue[2]); // 101 means the third value of int array
}

void ResourceManagerTestCommon::TestGetResourceLimitKeys(uint32_t expectedLimitKeys)
{
    uint32_t limitKeys = rm->GetResourceLimitKeys();
    limitKeys &= expectedLimitKeys;
    EXPECT_EQ(limitKeys, expectedLimitKeys);
}
