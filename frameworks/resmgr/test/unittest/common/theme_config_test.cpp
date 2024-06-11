/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "theme_config_test.h"

#include "resource_manager_test_common.h"
#include "theme_pack_config.h"
using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class ThemeConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ThemeConfigTest()
    {}

    ~ThemeConfigTest()
    {}

public:
    std::shared_ptr<ThemeConfig> themeConfig;
    std::shared_ptr<ResConfigImpl> resConfig;
};

void ThemeConfigTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ThemeConfigTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ThemeConfigTest::SetUp(void)
{
    this->themeConfig = std::make_shared<ThemeConfig>();
    this->resConfig = std::make_shared<ResConfigImpl>();
}

void ThemeConfigTest::TearDown(void)
{
    RESMGR_HILOGD(RESMGR_TAG, "ThemeConfigTest teardown");
}

/*
 * @tc.name: ThemeConfigSetThemeDirectionTest001
 * @tc.desc: Test SetThemeDirection function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigSetThemeDirectionTest001, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    EXPECT_EQ(this->themeConfig->GetThemeDirection(), DIRECTION_VERTICAL);
}

/*
 * @tc.name: ThemeConfigSetColorModeTest001
 * @tc.desc: Test LoadThemeSkinResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigSetColorModeTest001, TestSize.Level1)
{
    this->themeConfig->SetThemeColorMode(DARK);
    EXPECT_EQ(this->themeConfig->GetThemeColorMode(), DARK);
}

/*
 * @tc.name: ThemeConfigMatchTest001
 * @tc.desc: Test Match function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigMatchTest001, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_NOT_SET);
    this->resConfig->SetDirection(DIRECTION_NOT_SET);
    bool ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_TRUE(ret);

    this->resConfig->SetDirection(DIRECTION_VERTICAL);
    ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_TRUE(ret);

    this->resConfig->SetDirection(DIRECTION_NOT_SET);
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ThemeConfigMatchTest002
 * @tc.desc: Test Match function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigMatchTest002, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_HORIZONTAL);
    this->resConfig->SetDirection(DIRECTION_VERTICAL);
    bool ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_FALSE(ret);

    this->resConfig->SetDirection(DIRECTION_HORIZONTAL);
    ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ThemeConfigMatchTest003
 * @tc.desc: Test Match function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigMatchTest003, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->themeConfig->SetThemeColorMode(DARK);

    this->resConfig->SetDirection(DIRECTION_VERTICAL);
    this->resConfig->SetColorMode(DARK);
    this->resConfig->SetAppColorMode(false);
    this->resConfig->SetAppDarkRes(false);

    bool ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_FALSE(ret);

    this->themeConfig->SetThemeColorMode(COLOR_MODE_NOT_SET);
    ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_TRUE(ret);

    this->themeConfig->SetThemeColorMode(LIGHT);
    this->resConfig->SetAppDarkRes(true);
    ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ThemeConfigMatchTest004
 * @tc.desc: Test Match function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigMatchTest004, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->resConfig->SetDirection(DIRECTION_VERTICAL);

    this->themeConfig->SetThemeColorMode(DARK);
    this->resConfig->SetColorMode(LIGHT);
    this->resConfig->SetAppColorMode(false);
    this->resConfig->SetAppDarkRes(false);

    bool ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ThemeConfigMatchTest005
 * @tc.desc: Test Match function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigMatchTest005, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->resConfig->SetDirection(DIRECTION_VERTICAL);

    this->themeConfig->SetThemeColorMode(DARK);
    this->resConfig->SetColorMode(DARK);
    this->resConfig->SetAppColorMode(true);
    this->resConfig->SetAppDarkRes(false);

    bool ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ThemeConfigMatchTest006
 * @tc.desc: Test Match function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigMatchTest006, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->resConfig->SetDirection(DIRECTION_VERTICAL);

    this->themeConfig->SetThemeColorMode(DARK);
    this->resConfig->SetColorMode(DARK);
    this->resConfig->SetAppColorMode(false);
    this->resConfig->SetAppDarkRes(true);

    bool ret = this->themeConfig->Match(themeConfig, *this->resConfig);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ThemeConfigBestMatchTest001
 * @tc.desc: Test BestMatch function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigBestMatchTest001, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->resConfig->SetDirection(DIRECTION_VERTICAL);

    std::shared_ptr<ThemeConfig> otherThemeConfig = std::make_shared<ThemeConfig>();
    otherThemeConfig->SetThemeDirection(DIRECTION_HORIZONTAL);

    bool ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_TRUE(ret);

    this->themeConfig->SetThemeDirection(DIRECTION_NOT_SET);
    ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ThemeConfigBestMatchTest002
 * @tc.desc: Test BestMatch function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigBestMatchTest002, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->themeConfig->SetThemeColorMode(DARK);
    this->resConfig->SetDirection(DIRECTION_VERTICAL);
    this->resConfig->SetColorMode(DARK);

    std::shared_ptr<ThemeConfig> otherThemeConfig = std::make_shared<ThemeConfig>();
    otherThemeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    otherThemeConfig->SetThemeColorMode(LIGHT);

    bool ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_TRUE(ret);

    this->themeConfig->SetThemeColorMode(COLOR_MODE_NOT_SET);
    ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ThemeConfigBestMatchTest003
 * @tc.desc: Test BestMatch function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigBestMatchTest003, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->themeConfig->SetThemeColorMode(DARK);
    this->resConfig->SetDirection(DIRECTION_VERTICAL);
    this->resConfig->SetColorMode(DARK);

    std::shared_ptr<ThemeConfig> otherThemeConfig = std::make_shared<ThemeConfig>();
    otherThemeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    otherThemeConfig->SetThemeColorMode(DARK);

    bool ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ThemeConfigBestMatchTest004
 * @tc.desc: Test BestMatch function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigBestMatchTest004, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_HORIZONTAL);
    this->resConfig->SetDirection(DIRECTION_NOT_SET);
    this->resConfig->SetColorMode(COLOR_MODE_NOT_SET);

    std::shared_ptr<ThemeConfig> otherThemeConfig = std::make_shared<ThemeConfig>();
    otherThemeConfig->SetThemeDirection(DIRECTION_VERTICAL);

    bool ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_TRUE(ret);

    this->themeConfig->SetThemeDirection(DIRECTION_NOT_SET);
    ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ThemeConfigBestMatchTest005
 * @tc.desc: Test BestMatch function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeConfigTest, ThemeConfigBestMatchTest005, TestSize.Level1)
{
    this->themeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    this->themeConfig->SetThemeColorMode(LIGHT);
    this->resConfig->SetDirection(DIRECTION_NOT_SET);
    this->resConfig->SetColorMode(COLOR_MODE_NOT_SET);

    std::shared_ptr<ThemeConfig> otherThemeConfig = std::make_shared<ThemeConfig>();
    otherThemeConfig->SetThemeDirection(DIRECTION_VERTICAL);
    otherThemeConfig->SetThemeColorMode(DARK);

    bool ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_TRUE(ret);

    this->themeConfig->SetThemeColorMode(COLOR_MODE_NOT_SET);
    ret = this->themeConfig->BestMatch(otherThemeConfig, *this->resConfig);
    EXPECT_FALSE(ret);
}
} // namespace
