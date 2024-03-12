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

#include "theme_manager_test.h"

#include "resource_manager_test_common.h"
#include "theme_pack_manager.h"
using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class ThemeManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ThemeManagerTest()
    {}

    ~ThemeManagerTest()
    {}

public:
    std::shared_ptr<ResourceManager> rm;
    std::shared_ptr<ResourceManagerTestCommon> rmc;
    std::shared_ptr<ThemePackManager> tm;
};

void ThemeManagerTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ThemeManagerTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ThemeManagerTest::SetUp(void)
{
    this->tm = ThemePackManager::GetThemePackManager();
    std::string hapPath;
    std::vector<std::string> overlayPath;
    ResConfigImpl *rc = new ResConfigImpl;
    int32_t appType = 0;
    int32_t userId = 100; // userId is 100
    this->rm = CreateResourceManager("ohos.global.test.all", "entry", hapPath, overlayPath, *rc, appType, userId);
    this->rmc = std::make_shared<ResourceManagerTestCommon>(rm);
}

void ThemeManagerTest::TearDown(void)
{
    HILOG_DEBUG("ThemeManagerTest teardown");
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceTest001
 * @tc.desc: Test LoadThemeSkinResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeSkinResourceTest001, TestSize.Level1)
{
   // success cases
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId("base_only", ResType::COLOR);
    uint32_t outValue;
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/skin/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    const std::string themeFlag = "e";
    int32_t userId = 100; // userId is 100
    bool isAbsolutePath = false;
    // the bundlename is invalid
    tm->LoadThemeSkinResource("ohos.global.test", "entry", rootDirs, themeFlag, isAbsolutePath, userId);
    rm->GetColorById(id, outValue);
    ASSERT_EQ(0, outValue); // base_only APP value is #000000
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceTest003
 * @tc.desc: Test AddResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeSkinResourceTest002, TestSize.Level1)
{
   // success cases
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId("base_only", ResType::COLOR);
    uint32_t outValue;
    rm->GetColorById(id, outValue);
    ASSERT_EQ(0, outValue); // base_only APP value is #000000

    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/skin/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    const std::string themeFlag = "e";
    int32_t userId = 100; // userId is 100
    bool isAbsolutePath = false;
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, themeFlag, isAbsolutePath, userId);
    rm->GetColorById(id, outValue);
    ASSERT_EQ(4294967295, outValue); // base_only theme value is #ffffff(4294967295)
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest001
 * @tc.desc: Test GetThemeIcons function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/ohos.global.test.all";
    uint32_t resId = 16777306;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
    RState state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_TRUE(state == ERROR_CODE_RES_ID_NOT_FOUND);

    rootDirs.emplace_back(rootDir);
    const std::string themeFlag = "e";
    int32_t userId = 100; // userId is 100
    bool isAbsolutePath = false;
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", rootDirs, themeFlag, isAbsolutePath, userId);
    state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_TRUE(state == SUCCESS);
}
}
