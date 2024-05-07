/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "resource_manager_test.h"

#include "resource_manager_test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class ResourceManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerTest() : rm(nullptr)
    {}

    ~ResourceManagerTest()
    {}

public:
    ResourceManager *rm;
    ResourceManagerTestCommon *rmc;
};

void ResourceManagerTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResourceManagerTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerTest::SetUp(void)
{
    this->rm = CreateResourceManager();
    this->rmc = new ResourceManagerTestCommon(rm);
}

void ResourceManagerTest::TearDown(void)
{
    delete this->rm;
    delete this->rmc;
}

/*
 * @tc.name: ResourceManagerAddResourceTest001
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest001, TestSize.Level1)
{
    // success cases
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
};

/*
 * @tc.name: ResourceManagerAddResourceTest002
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest002, TestSize.Level1)
{
    // error cases
    // file not exist
    bool ret = rm->AddResource("/data/test/do_not_exist.resources");
    ASSERT_TRUE(!ret);
}

/*
 * @tc.name: ResourceManagerAddResourceTest003
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest003, TestSize.Level1)
{
    // error cases
    // reload the same path
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(!ret);
}

/*
 * @tc.name: ResourceManagerAddResourceTest004
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest004, TestSize.Level1)
{
    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl *)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
}

/*
 * @tc.name: ResourceManagerAddResourceTest005
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest005, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl*)rm)->AddResource("notexist/resources.index", overlayPaths);
    ASSERT_FALSE(ret);
}

/*
 * @tc.name: ResourceManagerAddResourceTest006
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerAddResourceTest006, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath("notexist/resources.index"));
    bool ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_FALSE(ret);
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest001
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest001, TestSize.Level1)
{
    // success cases
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_CAR);
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(SUCCESS, state);
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest002
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest002, TestSize.Level1)
{
    // error cases
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(LOCALEINFO_IS_NULL, state);
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest003
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest003, TestSize.Level1)
{
    // error cases
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo(nullptr, nullptr, "US");
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(LOCALEINFO_IS_NULL, state);
}

/*
 * load a hap, defaultConfig set to en, then switch to zh
 * @tc.name: ResourceManagerUpdateResConfigTest004
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest004, TestSize.Level1)
{
    // success case
    bool ret = true;
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    state = rm->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state);
    ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    if (!ret) {
        EXPECT_TRUE(false);
        delete rc;
        return;
    }
    // update to another language, will trigger reload
    // before reload:
    rmc->TestStringByName("app_name", "App Name");
    rc->SetLocaleInfo("zh", nullptr, nullptr);
    state = rm->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state);
    // after reload:
    rmc->TestStringByName("app_name", "应用名称");
#ifdef SUPPORT_GRAPHICS
    // update to App Preferred language,weill trigger reload
    Locale locale = GetLocale("en", nullptr, nullptr);
    rc->SetPreferredLocaleInfo(locale);
    state = rm->UpdateResConfig(*rc);
    
    EXPECT_EQ(SUCCESS, state);
    rmc->TestStringByName("app_name", "App Name");
    locale = GetLocale(nullptr, nullptr, nullptr);
    rc->SetPreferredLocaleInfo(locale);
    state = rm->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state);
    delete rc;
#endif
}

/*
 * @tc.name: ResourceManagerUpdateResConfigTest005
 * @tc.desc: Test UpdateResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerUpdateResConfigTest005, TestSize.Level1)
{
    // error case
    rmc->AddResource("zh", nullptr, nullptr);

    // make a fake hapResource, then reload will fail
    auto hapResource = std::make_shared<HapResource>("/data/test/non_exist", 0, nullptr);
    ((ResourceManagerImpl *)rm)->hapManager_->hapResources_.push_back(hapResource);
    ((ResourceManagerImpl *)rm)->hapManager_->loadedHapPaths_["/data/test/non_exist"] = std::vector<std::string>();
    RState state;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    state = rm->UpdateResConfig(*rc);
    delete rc;
    EXPECT_EQ(SUCCESS, state);
}

/*
 * @tc.name: ResourceManagerGetResConfigTest001
 * @tc.desc: Test GetResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetResConfigTest001, TestSize.Level1)
{
    // success cases
    ResConfigImpl rc;
    rm->GetResConfig(rc);
#ifdef SUPPORT_GRAPHICS
    EXPECT_EQ(nullptr, rc.GetLocaleInfo());
    EXPECT_EQ(nullptr, rc.GetPreferredLocaleInfo());
#endif
    EXPECT_EQ(DIRECTION_NOT_SET, rc.GetDirection());
    EXPECT_EQ(SCREEN_DENSITY_NOT_SET, rc.GetScreenDensity());
    EXPECT_EQ(DEVICE_NOT_SET, rc.GetDeviceType());
}

/*
 * @tc.name: ResourceManagerGetResConfigTest002
 * @tc.desc: Test GetResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetResConfigTest002, TestSize.Level1)
{
    // success cases
    RState state;
    {
        ResConfig *rc = CreateResConfig();
        if (rc == nullptr) {
            EXPECT_TRUE(false);
            return;
        }
        rc->SetLocaleInfo("en", nullptr, "US");
#ifdef SUPPORT_GRAPHICS
        Locale locale = GetLocale("zh", "Hans", "CN");
        rc->SetPreferredLocaleInfo(locale);
#endif
        rc->SetDeviceType(DeviceType::DEVICE_CAR);
        state = rm->UpdateResConfig(*rc);
        delete rc;
        EXPECT_EQ(SUCCESS, state);
    }

    ResConfigImpl rc;
    rm->GetResConfig(rc);
#ifdef SUPPORT_GRAPHICS
    EXPECT_EQ("en", std::string(rc.GetLocaleInfo()->getLanguage()));
    EXPECT_EQ("zh", std::string(rc.GetPreferredLocaleInfo()->getLanguage()));
    EXPECT_EQ("CN", std::string(rc.GetPreferredLocaleInfo()->getCountry()));
    EXPECT_EQ("Hans", std::string(rc.GetPreferredLocaleInfo()->getScript()));
#endif
    EXPECT_EQ(DEVICE_CAR, rc.GetDeviceType());
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest001
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    rmc->TestGetPatternById("base");
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest002
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    int id;
    std::map<std::string, std::string> outValue;
    RState state;

    id = rmc->GetResId("child", ResType::PATTERN);
    state = rm->GetPatternById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest003
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest003, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    int id;
    std::map<std::string, std::string> outValue;
    RState state;

    id = rmc->GetResId("ccchild", ResType::PATTERN);
    state = rm->GetPatternById(id, outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(5), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByIdTest004
 * @tc.desc: Test GetPatternById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByIdTest004, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    // not found case
    state = rm->GetPatternById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest001
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    rmc->TestGetPatternByName("base");
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest002
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    state = rm->GetPatternByName("child", outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest003
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest003, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    state = rm->GetPatternByName("ccchild", outValue);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ(static_cast<size_t>(5), outValue.size());
    PrintMapString(outValue);
}

/*
 * @tc.name: ResourceManagerGetPatternByNameTest004
 * @tc.desc: Test GetPatternByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetPatternByNameTest004, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::map<std::string, std::string> outValue;
    RState state;

    // not found case
    state = rm->GetPatternByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetThemeByIdTest001
 * @tc.desc: Test GetThemeById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetThemeById("app_theme");
}

/*
 * @tc.name: ResourceManagerGetThemeByIdTest002
 * @tc.desc: Test GetThemeById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::map<std::string, std::string> outValue;
    RState state;
    state = rm->GetThemeById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetThemeByNameTest001
 * @tc.desc: Test GetThemeByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetThemeByName("app_theme", "test_theme");
}

/*
 * @tc.name: ResourceManagerGetThemeByNameTest002
 * @tc.desc: Test GetThemeByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetThemeByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::map<std::string, std::string> outValue;
    RState state;
    state = rm->GetThemeByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetBooleanByIdTest001
 * @tc.desc: Test GetBooleanById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetBooleanById("boolean_1", "boolean_ref");
}

/*
 * @tc.name: ResourceManagerGetBooleanByIdTest002
 * @tc.desc: Test GetBooleanById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    bool outValue = true;
    RState state;
    state = rm->GetBooleanById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameTest001
 * @tc.desc: Test GetBooleanByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetBooleanByName("boolean_1", "boolean_ref");
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameTest002
 * @tc.desc: Test GetBooleanByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    bool outValue = true;
    RState state;
    state = rm->GetBooleanByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntegerByIdTest001
 * @tc.desc: Test GetIntegerById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetIntegerById("integer_1", "integer_ref");
}

/*
 * @tc.name: ResourceManagerGetIntegerByIdTest002
 * @tc.desc: Test GetIntegerById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    int outValue;
    RState state;
    state = rm->GetIntegerById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameTest001
 * @tc.desc: Test GetIntegerByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetIntegerByName("integer_1", "integer_ref");
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameTest002
 * @tc.desc: Test GetIntegerByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    int outValue;
    RState state;
    state = rm->GetIntegerByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetFloatByIdTest001
 * @tc.desc: Test GetFloatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetFloatById("width_appBar_backButton_touchTarget", "float_ref");
}

/*
 * @tc.name: ResourceManagerGetFloatByIdTest002
 * @tc.desc: Test GetFloatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    float outValue;
    RState state;
    state = rm->GetFloatById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetFloatByNameTest001
 * @tc.desc: Test GetFloatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetFloatByName("width_appBar_backButton_touchTarget", "float_ref");
}

/*
 * @tc.name: ResourceManagerGetFloatByNameTest002
 * @tc.desc: Test GetFloatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetFloatByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    float outValue;
    RState state;
    state = rm->GetFloatByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntArrayByIdTest001
 * @tc.desc: Test GetIntArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetIntArrayById("intarray_1");
}

/*
 * @tc.name: ResourceManagerGetIntArrayByIdTest002
 * @tc.desc: Test GetIntArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::vector<int> outValue;
    RState state;
    state = rm->GetIntArrayById(NON_EXIST_ID, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetIntArrayByNameTest001
 * @tc.desc: Test GetIntArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    rmc->TestGetIntArrayByName("intarray_1");
}

/*
 * @tc.name: ResourceManagerGetIntArrayByNameTest002
 * @tc.desc: Test GetIntArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntArrayByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::vector<int> outValue;
    RState state;
    state = rm->GetIntArrayByName(g_nonExistName, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetColorByIdTest001
 * @tc.desc: Test GetColorById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    int id = rmc->GetResId("divider_color", ResType::COLOR);
    EXPECT_TRUE(id > 0);
    state = rm->GetColorById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(268435456), outValue); // #10000000

    id = rmc->GetResId("color_aboutPage_title_primary", ResType::COLOR);
    EXPECT_TRUE(id > 0);
    state = rm->GetColorById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(4279834905, outValue); // #191919
}

/*
 * @tc.name: ResourceManagerGetColorByIdTest002
 * @tc.desc: Test GetColorById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    state = rm->GetColorById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest001
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("divider_color", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(268435456), outValue); // #10000000

    state = rm->GetColorByName("color_aboutPage_title_primary", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(4279834905, outValue); // #191919
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest002
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest003
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest003, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_only", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(0), outValue); // #00000000 base resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest004
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest004, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_light", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(572662306), outValue); // #22222222 light resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest005
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest005, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_dark", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(0), outValue); // #00000000 base resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest006
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest006, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_light_dark", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(572662306), outValue); // #22222222 light resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest007
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest007, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, DARK, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_only", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(0), outValue); // #00000000 base resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest008
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest008, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, DARK, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_light", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(0), outValue); // #00000000 base resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest009
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest009, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, DARK, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_dark", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(286331153), outValue); // #11111111 dark resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest010
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest010, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, DARK, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_light_dark", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(286331153), outValue); // #11111111 dark resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest011
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest011, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_light_phone", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(858993459), outValue); // #33333333 phone resource
}

/*
 * @tc.name: ResourceManagerGetColorByNameTest012
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetColorByNameTest012, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    uint32_t outValue;
    RState state;
    state = rm->GetColorByName("base_light_ldpi", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(static_cast<uint32_t>(572662306), outValue); // #22222222 light resource
}

/*
 * @tc.name: ResourceManagerGetProfileByIdTest001
 * @tc.desc: Test GetProfileById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    rmc->TestGetProfileById(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetProfileByIdTest002
 * @tc.desc: Test GetProfileById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetProfileById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetProfileByNameTest001
 * @tc.desc: Test GetProfileByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    rmc->TestGetProfileByName(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetProfileByNameTest002
 * @tc.desc: Test GetProfileByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetProfileByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetProfileByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest001
 * @tc.desc: Test GetStringById, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByIdForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest002
 * @tc.desc: Test GetStringById, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByIdForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("default device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest003
 * @tc.desc: Test GetStringById, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByIdForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("zh_CN phone pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest004
 * @tc.desc: Test GetStringById, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByIdForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest001
 * @tc.desc: Test GetStringByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest002
 * @tc.desc: Test GetStringByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("default device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest003
 * @tc.desc: Test GetStringByName, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("zh_CN phone pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest004
 * @tc.desc: Test GetStringByName, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest005
 * @tc.desc: Test GetStringByName, not match zh_CN-phone-pointingdevice directory which devicetype not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameForInputDeviceTest005, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("zh_CN phone pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest006
 * @tc.desc: Test GetStringByName,match base directory which pointingdevice directory not have res with name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetStringByNameForInputDeviceTest006, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("hello", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("Helloooo", outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest001
 * @tc.desc: Test GetStringByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(888, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest002
 * @tc.desc: Test GetIntegerByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(999, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest003
 * @tc.desc: Test GetIntegerByName, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(777, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest004
 * @tc.desc: Test GetIntegerByName, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetIntegerByNameForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(888, outValue);
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameForInputDeviceTest001
 * @tc.desc: Test GetBooleanByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    bool outValue;
    rState = rm->GetBooleanByName("boolean_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(true, outValue);
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameForInputDeviceTest002
 * @tc.desc: Test GetBooleanByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetBooleanByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    bool outValue;
    rState = rm->GetBooleanByName("boolean_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(false, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest001
 * @tc.desc: Test GetMediaByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/pointingdevice/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest002
 * @tc.desc: Test GetMediaByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/base/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest003
 * @tc.desc: Test GetMediaByName, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByNameForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/zh_CN-phone-pointingdevice/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest004
 * @tc.desc: Test GetMediaByName, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerGetMediaByNameForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/pointingdevice/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

/*
 * @tc.name: ResourceManagerResolveReferenceTest001
 * @tc.desc: Test ResolveReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerResolveReferenceTest001, TestSize.Level1)
{
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rm->UpdateResConfig(*rc);

    rm->AddResource(FormatFullPath(g_resFilePath).c_str());

    int id = rmc->GetResId("integer_1", ResType::INTEGER);
    std::string value(FormatString("$integer:%d", id));
    std::string outValue;
    RState ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("101"), outValue);

    std::string copyright("XXXXXX All rights reserved. ©2011-2019");
    id = rmc->GetResId("copyright_text", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(copyright, outValue);

    id = rmc->GetResId("string_ref", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(copyright, outValue);

    id = rmc->GetResId("boolean_1", ResType::BOOLEAN);
    value.assign(FormatString("$boolean:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("true"), outValue);

    id = rmc->GetResId("grey_background", ResType::COLOR);
    value.assign(FormatString("$color:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("#F5F5F5"), outValue);

    id = rmc->GetResId("aboutPage_minHeight", ResType::FLOAT);
    value.assign(FormatString("$float:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("707vp"), outValue);

    id = rmc->GetResId("base", ResType::PATTERN);
    value.assign(FormatString("$pattern:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(ERROR, ret);
}

/*
 * @tc.name: ResourceManagerResolveReferenceTest002
 * @tc.desc: Test ResolveReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerResolveReferenceTest002, TestSize.Level1)
{
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rm->UpdateResConfig(*rc);
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());

    // reload
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rm->UpdateResConfig(*rc);
    delete rc;

    int id = rmc->GetResId("copyright_text", ResType::STRING);
    std::string value(FormatString("$string:%d", id));
    std::string outValue;
    RState ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string("版权所有 ©2011-2019 XXXX有限公司保留一切权利"), outValue.c_str());
    id = rmc->GetResId("string_ref", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string("$aaaaa"), outValue.c_str());

    // error case
    // wrong id
    value.assign(FormatString("$boolean:%d", NON_EXIST_ID));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(ERROR, ret);
    // wrong type
    id = rmc->GetResId("copyright_text", ResType::STRING);
    value.assign(FormatString("$boolean:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(ERROR, ret);
}

/*
 * @tc.name: ResourceManagerResolveParentReferenceTest001
 * @tc.desc: Test ResolveParentReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerResolveParentReferenceTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    int id;
    std::map<std::string, std::string> outValue;
    RState ret;

    id = rmc->GetResId("base", ResType::PATTERN);
    EXPECT_TRUE(id > 0);
    auto idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);

    HILOG_DEBUG("=====");
    id = rmc->GetResId("child", ResType::PATTERN);
    idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);

    HILOG_DEBUG("=====");
    id = rmc->GetResId("ccchild", ResType::PATTERN);
    idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);
}

/*
 * test res with same name in different resType
 * @tc.name: ResourceManagerSameNameTest001
 * @tc.desc: Test GetStringByName & GetBooleanByName & GetIntegerByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerSameNameTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    std::string outValue;
    std::string name;
    RState state;

    state = rm->GetStringByName("same_name", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(std::string("StringSameName"), outValue);

    bool outValueB = true;
    state = rm->GetBooleanByName("same_name", outValueB);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(false, outValueB);

    int outValueI;
    state = rm->GetIntegerByName("same_name", outValueI);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(999, outValueI);
}

/*
 * test get raw file path interface
 * @tc.name: RawFileTest001
 * @tc.desc: Test GetRawFilePathByName, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, RawFileTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    rmc->TestGetRawFilePathByName("rawfile/test_rawfile.txt",
        "/data/test/all/assets/entry/resources/rawfile/test_rawfile.txt");

    rmc->TestGetRawFilePathByName("test_rawfile.txt",
        "/data/test/all/assets/entry/resources/rawfile/test_rawfile.txt");
}

/*
 * @tc.name: ResourceManagerCreateDefaultResConfigTest001
 * @tc.desc: Test CreateDefaultResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTest, ResourceManagerCreateDefaultResConfigTest001, TestSize.Level1)
{
    ResConfig *rc = CreateDefaultResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_EQ(rc->GetColorMode(), COLOR_MODE_NOT_SET);
    delete(rc);
}
}
