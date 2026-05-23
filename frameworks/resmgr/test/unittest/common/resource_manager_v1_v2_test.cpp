/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include <fcntl.h>
#include "res_config.h"
#include "res_config_impl.h"
#include "resource_manager_test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class ResourceManagerV1V2Test : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerV1V2Test()
    {}

    ~ResourceManagerV1V2Test()
    {}
};

void ResourceManagerV1V2Test::SetUpTestCase(void)
{}

void ResourceManagerV1V2Test::TearDownTestCase(void)
{}

void ResourceManagerV1V2Test::SetUp(void)
{}

void ResourceManagerV1V2Test::TearDown(void)
{}

static void CompareV1V2ResourceManagerResult(ResourceManager *rmV1, ResourceManager *rmV2, const std::string &name)
{
    std::string valueV1;
    std::string valueV2;
    RState stateV1 = rmV1->GetStringByName(name.c_str(), valueV1);
    RState stateV2 = rmV2->GetStringByName(name.c_str(), valueV2);

    EXPECT_EQ(stateV1, stateV2) << name << " state should match";
    if (stateV1 == SUCCESS && stateV2 == SUCCESS) {
        EXPECT_EQ(valueV1, valueV2) << name << " value should match";
    }
}

static void TestV1V2ResourceManagerConsistency(std::shared_ptr<ResConfigImpl> rc, const std::string &testName)
{
    ResourceManager *rmV1 = CreateResourceManager();
    ResourceManager *rmV2 = CreateResourceManager();
    ASSERT_TRUE(rmV1 != nullptr) << testName << " V1 ResourceManager create failed";
    ASSERT_TRUE(rmV2 != nullptr) << testName << " V2 ResourceManager create failed";

    RState state = rmV1->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state) << testName << " V1 UpdateResConfig failed";
    state = rmV2->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state) << testName << " V2 UpdateResConfig failed";

    bool ret = rmV1->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret) << testName << " V1 AddResource failed";
    ret = rmV2->AddResource(FormatFullPath(g_newResFilePath).c_str());
    ASSERT_TRUE(ret) << testName << " V2 AddResource failed";

    CompareV1V2ResourceManagerResult(rmV1, rmV2, "app_name");
    CompareV1V2ResourceManagerResult(rmV1, rmV2, "integer_ref");

    delete rmV1;
    delete rmV2;
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyDeviceTypeNotSetTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency when deviceType is DEVICE_NOT_SET
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyDeviceTypeNotSetTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_NOT_SET);
    TestV1V2ResourceManagerConsistency(rc, "DeviceTypeNotSetTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyDeviceTypePhoneTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency when deviceType is DEVICE_PHONE
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyDeviceTypePhoneTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_PHONE);
    TestV1V2ResourceManagerConsistency(rc, "DeviceTypePhoneTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyDeviceTypeTabletTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency when deviceType is DEVICE_TABLET
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyDeviceTypeTabletTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_TABLET);
    TestV1V2ResourceManagerConsistency(rc, "DeviceTypeTabletTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyColorModeDarkTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with colorMode DARK
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyColorModeDarkTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetColorMode(ColorMode::DARK);
    TestV1V2ResourceManagerConsistency(rc, "ColorModeDarkTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyColorModeLightTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with colorMode LIGHT
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyColorModeLightTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetColorMode(ColorMode::LIGHT);
    TestV1V2ResourceManagerConsistency(rc, "ColorModeLightTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyDirectionVerticalTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with direction VERTICAL
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyDirectionVerticalTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(Direction::DIRECTION_VERTICAL);
    TestV1V2ResourceManagerConsistency(rc, "DirectionVerticalTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyDirectionHorizontalTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with direction HORIZONTAL
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyDirectionHorizontalTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(Direction::DIRECTION_HORIZONTAL);
    TestV1V2ResourceManagerConsistency(rc, "DirectionHorizontalTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyLocaleZhCnTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with locale zh_CN
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyLocaleZhCnTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("zh", nullptr, "CN");
    TestV1V2ResourceManagerConsistency(rc, "LocaleZhCnTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyLocaleEnGbTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with locale en_GB
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyLocaleEnGbTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "GB");
    TestV1V2ResourceManagerConsistency(rc, "LocaleEnGbTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyConfigFullTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with full config
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyConfigFullTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDeviceType(DeviceType::DEVICE_PHONE);
    rc->SetColorMode(ColorMode::DARK);
    rc->SetDirection(Direction::DIRECTION_VERTICAL);
    TestV1V2ResourceManagerConsistency(rc, "ConfigFullTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyInputDeviceTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with inputDevice POINTINGDEVICE
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyInputDeviceTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetInputDevice(InputDevice::INPUTDEVICE_POINTINGDEVICE);
    TestV1V2ResourceManagerConsistency(rc, "InputDeviceTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyMccMncTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with mcc and mnc set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyMccMncTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetMcc(460);
    rc->SetMnc(1);
    TestV1V2ResourceManagerConsistency(rc, "MccMncTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyScreenDensitySdpiTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with screenDensity SDPI
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyScreenDensitySdpiTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetScreenDensityDpi(ScreenDensity::SCREEN_DENSITY_SDPI);
    TestV1V2ResourceManagerConsistency(rc, "ScreenDensitySdpiTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyScreenDensityXxldpiTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with screenDensity XXLDPI
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyScreenDensityXxldpiTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetScreenDensityDpi(ScreenDensity::SCREEN_DENSITY_XXLDPI);
    TestV1V2ResourceManagerConsistency(rc, "ScreenDensityXxldpiTest");
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyMultipleConfigTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency with multiple config combinations
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyMultipleConfigTest001, TestSize.Level1)
{
    std::vector<std::pair<std::shared_ptr<ResConfigImpl>, std::string>> configs;

    auto rc1 = std::make_shared<ResConfigImpl>();
    rc1->SetLocaleInfo("en", nullptr, "US");
    rc1->SetColorMode(ColorMode::DARK);
    configs.push_back({ rc1, "Config1" });

    auto rc2 = std::make_shared<ResConfigImpl>();
    rc2->SetLocaleInfo("zh", nullptr, "CN");
    rc2->SetColorMode(ColorMode::LIGHT);
    configs.push_back({ rc2, "Config2" });

    auto rc3 = std::make_shared<ResConfigImpl>();
    rc3->SetLocaleInfo("en", nullptr, "US");
    rc3->SetDirection(Direction::DIRECTION_HORIZONTAL);
    configs.push_back({ rc3, "Config3" });

    auto rc4 = std::make_shared<ResConfigImpl>();
    rc4->SetLocaleInfo("en", nullptr, "US");
    rc4->SetDeviceType(DeviceType::DEVICE_PHONE);
    configs.push_back({ rc4, "Config4" });

    auto rc5 = std::make_shared<ResConfigImpl>();
    rc5->SetLocaleInfo("zh", nullptr, "CN");
    rc5->SetDeviceType(DeviceType::DEVICE_TABLET);
    configs.push_back({ rc5, "Config5" });

    for (const auto &pair : configs) {
        TestV1V2ResourceManagerConsistency(pair.first, pair.second);
    }
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyByIdTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency by resource id
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyByIdTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");

    ResourceManager *rmV1 = CreateResourceManager();
    ResourceManager *rmV2 = CreateResourceManager();
    ASSERT_TRUE(rmV1 != nullptr);
    ASSERT_TRUE(rmV2 != nullptr);

    RState state = rmV1->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state);
    state = rmV2->UpdateResConfig(*rc);
    EXPECT_EQ(SUCCESS, state);

    bool ret = rmV1->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ret = rmV2->AddResource(FormatFullPath(g_newResFilePath).c_str());
    ASSERT_TRUE(ret);

    std::string valueV1;
    std::string valueV2;
    state = rmV1->GetStringByName("app_name", valueV1);
    ASSERT_EQ(SUCCESS, state);
    state = rmV2->GetStringByName("app_name", valueV2);
    ASSERT_EQ(SUCCESS, state);
    EXPECT_EQ(valueV1, valueV2);

    uint32_t idV1 = 0;
    state = rmV1->GetResId("$app:string.app_name", idV1);
    if (state == SUCCESS) {
        uint32_t idV2 = 0;
        state = rmV2->GetResId("$app:string.app_name", idV2);
        if (state == SUCCESS) {
            std::string valueByIdV1;
            std::string valueByIdV2;
            state = rmV1->GetStringById(idV1, valueByIdV1);
            EXPECT_EQ(SUCCESS, state);
            state = rmV2->GetStringById(idV2, valueByIdV2);
            EXPECT_EQ(SUCCESS, state);
            EXPECT_EQ(valueByIdV1, valueByIdV2);
        }
    }

    delete rmV1;
    delete rmV2;
}

/*
 * @tc.name: ResourceManagerV1V2ConsistencyUpdateResConfigTest001
 * @tc.desc: Test V1 and V2 ResourceManager result consistency after UpdateResConfig
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerV1V2Test, ResourceManagerV1V2ConsistencyUpdateResConfigTest001, TestSize.Level1)
{
    ResourceManager *rmV1 = CreateResourceManager();
    ResourceManager *rmV2 = CreateResourceManager();
    ASSERT_TRUE(rmV1 != nullptr);
    ASSERT_TRUE(rmV2 != nullptr);

    auto rc1 = std::make_shared<ResConfigImpl>();
    rc1->SetLocaleInfo("en", nullptr, "US");

    RState state = rmV1->UpdateResConfig(*rc1);
    EXPECT_EQ(SUCCESS, state);
    state = rmV2->UpdateResConfig(*rc1);
    EXPECT_EQ(SUCCESS, state);

    bool ret = rmV1->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ret = rmV2->AddResource(FormatFullPath(g_newResFilePath).c_str());
    ASSERT_TRUE(ret);

    CompareV1V2ResourceManagerResult(rmV1, rmV2, "app_name");

    auto rc2 = std::make_shared<ResConfigImpl>();
    rc2->SetLocaleInfo("zh", nullptr, "CN");
    state = rmV1->UpdateResConfig(*rc2);
    EXPECT_EQ(SUCCESS, state);
    state = rmV2->UpdateResConfig(*rc2);
    EXPECT_EQ(SUCCESS, state);

    CompareV1V2ResourceManagerResult(rmV1, rmV2, "app_name");

    delete rmV1;
    delete rmV2;
}
}