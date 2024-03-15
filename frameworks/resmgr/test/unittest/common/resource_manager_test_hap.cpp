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

#include "resource_manager_test_hap.h"

#include "resource_manager_test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
static constexpr uint32_t DATA_ALL_HAP_LIMIT_KEYS = 1503;
static constexpr uint32_t SYSTEM_RES_LIMIT_KEYS = 1503;
class ResourceManagerTestHap : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerTestHap() : rm(nullptr)
    {}

    ~ResourceManagerTestHap()
    {}
public:
    ResourceManager *rm;
    ResourceManagerTestCommon *rmc;
};

void ResourceManagerTestHap::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResourceManagerTestHap::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerTestHap::SetUp(void)
{
    this->rm = CreateResourceManager();
    this->rmc = new ResourceManagerTestCommon(rm);
}

void ResourceManagerTestHap::TearDown(void)
{
    delete this->rm;
    delete this->rmc;
}

/*
 * @tc.name: ResourceManagerAddResourceFromHapTest001
 * @tc.desc: Test AddResource function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerAddResourceFromHapTest001, TestSize.Level1)
{
    // error cases
    // file not exist
    bool ret = rm->AddResource("/data/test/do_not_exist.hap");
    ASSERT_TRUE(!ret);
}

/*
 * @tc.name: ResourceManagerGetStringByIdFromHapTest002
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringByIdFromHapTest002, TestSize.Level1)
{
    rmc->AddHapResource("en", nullptr, "US");

    rmc->TestStringById("app_name", "App Name");
    rmc->TestStringById("copyright_text", "XXXXXX All rights reserved. ©2011-2019");
}

/*
 * @tc.name: ResourceManagerGetStringByIdFromHapTest003
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringByIdFromHapTest003, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);

    rmc->TestStringById("app_name", "应用名称");
    rmc->TestStringById("copyright_text", "版权所有 ©2011-2019 XXXX有限公司保留一切权利");
}

/*
 * @tc.name: ResourceManagerGetStringByNameFromHapTest004
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringByNameFromHapTest004, TestSize.Level1)
{
    rmc->AddHapResource("en", nullptr, nullptr);

    rmc->TestStringByName("app_name", "App Name");
    rmc->TestStringByName("copyright_text", "XXXXXX All rights reserved. ©2011-2019");
}

/*
 * @tc.name: ResourceManagerGetStringByNameFromHapTest005
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringByNameFromHapTest005, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);

    rmc->TestStringByName("app_name", "应用名称");
    rmc->TestStringByName("copyright_text", "版权所有 ©2011-2019 XXXX有限公司保留一切权利");
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdFromHapTest006
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringFormatByIdFromHapTest006, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);

    rmc->TestGetStringFormatById("app_name", "应用名称");
}


/*
 * @tc.name: ResourceManagerGetStringFormatByNameFromHapTest007
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringFormatByNameFromHapTest007, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);

    rmc->TestGetStringFormatByName("app_name", "应用名称");
}

/*
 * @tc.name: ResourceManagerGetStringArrayByIdFromHapTest008
 * @tc.desc: Test GetStringArrayById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringArrayByIdFromHapTest008, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);

    rmc->TestGetStringArrayById("size");
}

/*
 * @tc.name: ResourceManagerGetStringArrayByNameFromHapTest009
 * @tc.desc: Test GetStringArrayByName function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetStringArrayByNameFromHapTest009, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);
 
    rmc->TestGetStringArrayByName("size");
}

/*
 * @tc.name: ResourceManagerGetPatternByIdFromHapTest0010
 * @tc.desc: Test GetPatternById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPatternByIdFromHapTest0010, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);
    rmc->TestGetPatternById("base");
}

/*
 * @tc.name: ResourceManagerGetPatternByNameFromHapTest0011
 * @tc.desc: Test GetPatternByName function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPatternByNameFromHapTest0011, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, nullptr);
    rmc->TestGetPatternByName("base");
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFromHapTest0012
 * @tc.desc: Test GetPluralStringById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPluralStringByIdFromHapTest0012, TestSize.Level1)
{
    rmc->AddHapResource("en", nullptr, "US");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "%d apple", false);
    quantity = 101;
    rmc->TestPluralStringById(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFromHapTest0013
 * @tc.desc: Test GetPluralStringById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPluralStringByIdFromHapTest0013, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "%d apples", false);
    quantity = 101;
    rmc->TestPluralStringById(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFromHapTest0014
 * @tc.desc: Test GetPluralStringById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPluralStringByIdFromHapTest0014, TestSize.Level1)
{
    rmc->AddHapResource("ar", nullptr, "SA");

    int quantity = 0;
    rmc->TestPluralStringById(quantity, "zero-0");
    quantity = 1;
    rmc->TestPluralStringById(quantity, "one-1");
    quantity = 5;
    rmc->TestPluralStringById(quantity, "few-%d");
    quantity = 500;
    rmc->TestPluralStringById(quantity, "other-%d");
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFromHapTest0015
 * @tc.desc: Test GetPluralStringById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPluralStringByIdFromHapTest0015, TestSize.Level1)
{
    rmc->AddHapResource("pl", nullptr, "PL");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "1 jabłko");
    quantity = 2;
    rmc->TestPluralStringById(quantity, "%d jabłka");
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFromHapTest0016
 * @tc.desc: Test GetPluralStringByName function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPluralStringByNameFromHapTest0016, TestSize.Level1)
{
    rmc->AddHapResource("en", nullptr, "US");

    int quantity = 1;
    rmc->TestPluralStringByName(quantity, "%d apple", false);
    quantity = 101;
    rmc->TestPluralStringByName(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFormatFromHapTest0017
 * @tc.desc: Test GetPluralStringById function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPluralStringByIdFormatFromHapTest0017, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "1 apples", true);
    quantity = 101;
    rmc->TestPluralStringById(quantity, "101 apples", true);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFormatFromHapTest0018
 * @tc.desc: Test GetPluralStringByName function
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetPluralStringByNameFormatFromHapTest0018, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    int quantity = 1;
    rmc->TestPluralStringByName(quantity, "1 apples", true);
    quantity = 101;
    rmc->TestPluralStringByName(quantity, "101 apples", true);
}

/*
 * @tc.name: ResourceManagerGetThemeByIdFromHapTest0019
 * @tc.desc: Test GetThemeById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetThemeByIdFromHapTest0019, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetThemeById("app_theme");
}

/*
 * @tc.name: ResourceManagerGetThemeByNameFromHapTest0020
 * @tc.desc: Test GetThemeByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetThemeByNameFromHapTest0020, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetThemeByName("app_theme", "test_theme");
}

/*
 * @tc.name: ResourceManagerGetBooleanByIdFromHapTest0021
 * @tc.desc: Test GetBooleanById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetBooleanByIdFromHapTest0021, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetBooleanById("boolean_1", "boolean_ref");
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameFromHapTest0022
 * @tc.desc: Test GetBooleanByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetBooleanByNameFromHapTest0022, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetBooleanByName("boolean_1", "boolean_ref");
}

/*
 * @tc.name: ResourceManagerGetIntegerByIdFromHapTest0023
 * @tc.desc: Test GetIntegerById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetIntegerByIdFromHapTest0023, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetIntegerById("integer_1", "integer_ref");
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameFromHapTest0024
 * @tc.desc: Test GetIntegerByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetIntegerByNameFromHapTest0024, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetIntegerByName("integer_1", "integer_ref");
}

/*
 * @tc.name: ResourceManagerGetFloatByIdFromHapTest0025
 * @tc.desc: Test GetFloatById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetFloatByIdFromHapTest0025, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetFloatById("width_appBar_backButton_touchTarget", "float_ref");
}

/*
 * @tc.name: ResourceManagerGetFloatByNameFromHapTest0026
 * @tc.desc: Test GetFloatByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetFloatByNameFromHapTest0026, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetFloatByName("width_appBar_backButton_touchTarget", "float_ref");
}

/*
 * @tc.name: ResourceManagerGetIntArrayByIdFromHapTest0027
 * @tc.desc: Test GetIntArrayById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetIntArrayByIdFromHapTest0027, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetIntArrayById("intarray_1");
}

/*
 * @tc.name: ResourceManagerGetIntArrayByNameFromHapTest0028
 * @tc.desc: Test GetIntArrayByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetIntArrayByNameFromHapTest0028, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    rmc->TestGetIntArrayByName("intarray_1");
}

/*
 * @tc.name: ResourceManagerGetProfileByIdFromHapTest0029
 * @tc.desc: Test GetProfileById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetProfileByIdFromHapTest0029, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_hapPath).c_str(), 0, nullptr);
    rmc->TestGetProfileById(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetProfileByNameFromHapTest0030
 * @tc.desc: Test GetProfileByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetProfileByNameFromHapTest0030, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_hapPath).c_str(), 0, nullptr);
    rmc->TestGetProfileByName(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdFromHapTest0031
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaByIdFromHapTest0031, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_hapPath).c_str(), 0, nullptr);
    rmc->TestGetMediaById(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdFromHapTest0032
 * @tc.desc: Test GetMediaById, to match sdpi determinder
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaByIdFromHapTest0032, TestSize.Level1)
{
    rmc->AddHapResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_hapPath).c_str(), 0, nullptr);
    rmc->TestGetMediaWithDensityById(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameFromHapTest0033
 * @tc.desc: Test GetMediaByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaByNameFromHapTest0033, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_hapPath).c_str(), 0, nullptr);
    rmc->TestGetMediaByName(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameFromHapTest0034
 * @tc.desc: Test GetMediaByName, to match sdpi determinder
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaByNameFromHapTest0034, TestSize.Level1)
{
    rmc->AddHapResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_hapPath).c_str(), 0, nullptr);
    rmc->TestGetMediaWithDensityByName(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetProfileDataByIdFromHapTest0035
 * @tc.desc: Test GetProfileDataByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetProfileDataByIdFromHapTest0035, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    std::unique_ptr<uint8_t[]> outValue;
    int id = rmc->GetResId("test_profile", ResType::PROF);
    EXPECT_TRUE(id > 0);
    size_t len;
    RState state = rm->GetProfileDataById(id, len, outValue);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetProfileDataByNameFromHapTest0036
 * @tc.desc: Test GetProfileDataByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetProfileDataByNameFromHapTest0036, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    std::unique_ptr<uint8_t[]> outValue;
    size_t len;
    RState state = rm->GetProfileDataByName("test_profile", len, outValue);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaDataByNameFromHapTest0037
 * @tc.desc: Test GetMediaDataByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaDataByNameFromHapTest0037, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    std::unique_ptr<uint8_t[]> outValue;
    RState state;
    size_t len;
    state = rm->GetMediaDataByName("icon1", len, outValue);
    EXPECT_EQ(len, static_cast<size_t>(5997)); // the length of icon1
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaDataByIdFromHapTest0038
 * @tc.desc: Test GetMediaDataById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaDataByIdFromHapTest0038, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    std::unique_ptr<uint8_t[]> outValue;
    RState state;
    int id = rmc->GetResId("icon1", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    size_t len;
    state = rm->GetMediaDataById(id, len, outValue);
    EXPECT_EQ(len, static_cast<size_t>(5997)); // the length of icon1
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaDataBase64ByNameFromHapTest0039
 * @tc.desc: Test GetMediaBase64DataByName
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaDataBase64ByNameFromHapTest0039, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetMediaBase64DataByName("icon1", outValue);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaDataBase64ByIdFromHapTest0040
 * @tc.desc: Test GetMediaDataBase64ById
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaDataBase64ByIdFromHapTest0040, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

    std::string outValue;
    int id = rmc->GetResId("icon1", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    RState state;
    state = rm->GetMediaBase64DataById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaDataByIdFromHapTest0041
 * @tc.desc: Test GetMediaDataById, to match sdpi determinder
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaDataByIdFromHapTest0041, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

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
    std::unique_ptr<uint8_t[]> outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    size_t len;
    state = rm->GetMediaDataById(id, len, outValue, density);
    EXPECT_EQ(static_cast<int>(len), 6790); // the length of icon
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaDataByNameFromHapTest0042
 * @tc.desc: Test GetMediaBase64DataByName, to match sdpi determinder
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaDataByNameFromHapTest0042, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

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
    std::unique_ptr<uint8_t[]> outValue;
    RState state;
    size_t len;
    state = rm->GetMediaDataByName("icon", len, outValue, density);
    EXPECT_EQ(static_cast<int>(len), 6790); // the length of icon
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaBase64DataByIdFromHapTest0043
 * @tc.desc: Test GetMediaBase64DataById, to match sdpi determinder
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaBase64DataByIdFromHapTest0043, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

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
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaBase64DataById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetMediaBase64DataByNameFromHapTest0044
 * @tc.desc: Test GetMediaDataById, to match sdpi determinder
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetMediaBase64DataByNameFromHapTest0044, TestSize.Level1)
{
    rmc->AddHapResource("zh", nullptr, "CN");

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
    RState state;
    state = rm->GetMediaBase64DataByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * test get raw file path interface
 * @tc.name: RawFileTestFromHap0045
 * @tc.desc: Test GetRawFileFromHap & AddResource function, file case.
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, RawFileTestFromHap0045, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    RState state;
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    state = rm->GetRawFileFromHap("test_rawfile.txt", len, outValue);
    ASSERT_EQ(static_cast<int>(len), 17); // 17 means the length of "for raw file test" in "test_rawfile.txt"
    EXPECT_TRUE(state == SUCCESS);

    size_t len2;
    std::unique_ptr<uint8_t[]> outValue2;
    state = rm->GetRawFileFromHap("rawfile/test_rawfile.txt", len2, outValue2);
    ASSERT_EQ(static_cast<int>(len2), 17); // 17 means the length of "for raw file test" in "test_rawfile.txt"
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * test get raw file path interface
 * @tc.name: RawFileTestFromHap0046
 * @tc.desc: Test GetRawFileFromHap & AddResource function, file case.
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, RawFileTestFromHap0046, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    ResourceManager::RawFileDescriptor descriptor;
    RState state;
    state = rm->GetRawFileDescriptorFromHap("test_rawfile.txt", descriptor);
    EXPECT_TRUE(state == SUCCESS);
    ASSERT_EQ(descriptor.length, 17); // 17 means the length of test_rawfile.txt after compressed
    state = rm->CloseRawFileDescriptor("test_rawfile.txt");
    EXPECT_TRUE(state == SUCCESS);

    state = rm->GetRawFileDescriptorFromHap("rawfile/test_rawfile.txt", descriptor);
    EXPECT_TRUE(state == SUCCESS);
    ASSERT_EQ(descriptor.length, 17); // 17 means the length of test_rawfile.txt after compressed
    state = rm->CloseRawFileDescriptor("rawfile/test_rawfile.txt");
    EXPECT_TRUE(state == SUCCESS);

    state = rm->CloseRawFileDescriptor("noexist.txt");
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * test get raw file path interface
 * @tc.name: RawFileTestFromHap0047
 * @tc.desc: Test GetRawFileFromHap & AddResource function, file case.
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, RawFileTestFromHap0047, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetRawFileFromHap("test_rawfile.txt", len, outValue);
    ASSERT_EQ(static_cast<int>(len), 17); // 17 means the length of "for raw file test" in "test_rawfile.txt"
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * test get raw file path interface
 * @tc.name: RawFileTestFromHap0048
 * @tc.desc: Test GetRawFileDescriptorFromHap & AddResource function, file case.
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, RawFileTestFromHap0048, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    std::unique_ptr<uint8_t[]> outValue;
    ResourceManager::RawFileDescriptor descriptor;
    RState state;
    state = rm->GetRawFileDescriptorFromHap("test_rawfile.txt", descriptor);
    ASSERT_EQ(descriptor.length, 17);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerOverlayTest001
 * @tc.desc: Test overlay AddResource & UpdateResConfig function, file case.
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerOverlayTest001, TestSize.Level1)
{
    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl *)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rm->UpdateResConfig(*rc);
    delete rc;
}

/*
 * @tc.name: ResourceManagerOverlayTest002
 * @tc.desc: Test overlay function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerOverlayTest002, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, nullptr);

    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    // repeat add the same overlayPaths
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_app_name", "SystemOverlay");

    std::vector<std::string> removePaths;
    removePaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    ret = ((ResourceManagerImpl*)rm)->RemoveResource(FormatFullPath(g_resFilePath).c_str(), removePaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_app_name", "System");

    ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_app_name", "SystemOverlay");
}

/*
 * @tc.name: ResourceManagerOverlayTest003
 * @tc.desc: Test overlay function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerOverlayTest003, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, nullptr);

    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_app_name", "SystemOverlay");

    std::vector<std::string> removePaths;
    removePaths.push_back("notexist.index");
    removePaths.push_back("notexist2.index");
    removePaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    ret = ((ResourceManagerImpl*)rm)->RemoveResource(FormatFullPath(g_resFilePath).c_str(), removePaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_app_name", "System");
}

/*
 * @tc.name: ResourceManagerOverlayTest004
 * @tc.desc: Test overlay function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerOverlayTest004, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_lab_answer_call", "overlay接听电话");

    std::vector<std::string> removePaths;
    removePaths.push_back("notexist.index");
    removePaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    ret = ((ResourceManagerImpl*)rm)->RemoveResource(FormatFullPath(g_resFilePath).c_str(), removePaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_lab_answer_call", "接听电话");

    ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_lab_answer_call", "overlay接听电话");
}

/*
 * @tc.name: ResourceManagerUtilsTest001
 * @tc.desc: Test endWithTail;
 * @tc.type: FUNC
 * @tc.require: issueI5LHLP
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerUtilsTest001, TestSize.Level1)
{
    std::string path = "";
    std::string tail = ".hap";
    bool ret = Utils::endWithTail(path, tail);
    ASSERT_FALSE(ret);
}

/*
 * @tc.name: ResourceManagerGetResourceLimitKeysTest001
 * @tc.desc: Test GetResourceLimitKeys function
 * @tc.type: FUNC
 * @tc.require: issueI73ZQ8
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResourceLimitKeysTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    // all/assets/entry/resources.index contains limit keys for LANGUAGE/REGION/
    // SCREEN_DENSITY/DEVICETYPE/COLORMODE/MCC/MNC/INPUTDEVICE
    rmc->TestGetResourceLimitKeys(DATA_ALL_HAP_LIMIT_KEYS);
}

/*
 * @tc.name: ResourceManagerGetResourceLimitKeysTest002
 * @tc.desc: Test GetResourceLimitKeys function
 * @tc.type: FUNC
 * @tc.require: issueI73ZQ8
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResourceLimitKeysTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    // all.hap contains limit keys for LANGUAGE/REGION/
    // SCREEN_DENSITY/DEVICETYPE/COLORMODE/MCC/MNC/INPUTDEVICE
    rmc->TestGetResourceLimitKeys(DATA_ALL_HAP_LIMIT_KEYS);
}

/*
 * @tc.name: ResourceManagerGetResourceLimitKeysTest003
 * @tc.desc: Test GetResourceLimitKeys function
 * @tc.type: FUNC
 * @tc.require: issueI73ZQ8
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResourceLimitKeysTest003, TestSize.Level1)
{
    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl *)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    // all/assets/entry/resources.index and overlay/assets/entry/resources.index contains
    // LANGUAGE/REGION/DIRECTION/COLORMODE
    rmc->TestGetResourceLimitKeys(SYSTEM_RES_LIMIT_KEYS);
}

/*
 * @tc.name: ResourceManagerOverlayTest005
 * @tc.desc: Test overlay function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerOverlayTest005, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);
    // test add invalid overlay path
    bool ret = ((ResourceManagerImpl*)rm)->AddAppOverlay("noexist.hap");
    ASSERT_FALSE(ret);

    ret = ((ResourceManagerImpl*)rm)->AddAppOverlay(FormatFullPath(g_overlayResFilePath).c_str());
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_lab_answer_call", "overlay接听电话");
}

/*
 * @tc.name: ResourceManagerOverlayTest006
 * @tc.desc: Test overlay function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerOverlayTest006, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);
    // test remove invalid overlay path
    bool ret = ((ResourceManagerImpl*)rm)->RemoveAppOverlay("noexist.hap");
    ASSERT_FALSE(ret);

    ret = ((ResourceManagerImpl*)rm)->AddAppOverlay(FormatFullPath(g_overlayResFilePath).c_str());
    ASSERT_TRUE(ret);
    ret = ((ResourceManagerImpl*)rm)->RemoveAppOverlay(FormatFullPath(g_overlayResFilePath).c_str());
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_lab_answer_call", "接听电话");
}

/*
 * @tc.name: ResourceManagerGetResIdTest001
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResIdTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    const std::string resTypeName = "app.string.entry_MainAbility";
    uint32_t id = 0;
    RState state = rm->GetResId(resTypeName, id);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(id, 16777226);
}

/*
 * @tc.name: ResourceManagerGetResIdTest002
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResIdTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    const std::string resTypeName = "app.media.icon";
    uint32_t id = 0;
    RState state = rm->GetResId(resTypeName, id);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(id, 16777321);
}

/*
 * @tc.name: ResourceManagerGetResIdTest003
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResIdTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    const std::string resTypeName = "app.color.color_aboutPage_title_primary";
    uint32_t id = 0;
    RState state = rm->GetResId(resTypeName, id);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(id, 16777273);
}

/*
 * @tc.name: ResourceManagerGetResIdTest004
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResIdTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    const std::string resTypeName = "app.string.xxx";
    uint32_t id = 0;
    RState state = rm->GetResId(resTypeName, id);
    EXPECT_TRUE(state == NOT_FOUND);
    EXPECT_EQ(id, 0);
}

/*
 * @tc.name: ResourceManagerGetResIdTest005
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResIdTest005, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    const std::string resTypeName = "xxx.string.entry_MainAbility";
    uint32_t id = 0;
    RState state = rm->GetResId(resTypeName, id);
    EXPECT_TRUE(state == NOT_FOUND);
    EXPECT_EQ(id, 0);
}

/*
 * @tc.name: ResourceManagerGetResIdTest006
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetResIdTest006, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    const std::string resTypeName = "app.xxx.entry_MainAbility";
    uint32_t id = 0;
    RState state = rm->GetResId(resTypeName, id);
    EXPECT_TRUE(state == NOT_FOUND);
    EXPECT_EQ(id, 0);
}

/*
 * @tc.name: ResourceManagerGetLocalesTest001
 * @tc.desc: Test GetLocales function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetLocalesTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    std::vector<std::string> outValue;
    rm->GetLocales(outValue);
    EXPECT_EQ(static_cast<size_t>(4), outValue.size());
}

/*
 * @tc.name: ResourceManagerGetLocalesTest002
 * @tc.desc: Test GetLocales function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetLocalesTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    std::vector<std::string> outValue;
    rm->GetLocales(outValue, false);
    EXPECT_EQ(static_cast<size_t>(4), outValue.size());
}

/*
 * @tc.name: ResourceManagerGetLocalesTest003
 * @tc.desc: Test GetLocales function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestHap, ResourceManagerGetLocalesTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    std::vector<std::string> outValue;
    rm->GetLocales(outValue, true);
    EXPECT_EQ(static_cast<size_t>(4), outValue.size());
}
}
