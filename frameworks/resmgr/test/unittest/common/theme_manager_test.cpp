/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include <cstdio>
using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
void CreateFlagFile(const std::string &path)
{
    FILE *fp = fopen(path.c_str(), "w");
    if (fp != nullptr) {
        fclose(fp);
    }
}

void SetupTestFlags()
{
    CreateFlagFile("/data/test/theme/100/a/app/flag");
    CreateFlagFile("/data/test/theme/103/a/app/flag");
    CreateFlagFile("/data/test/theme/100/b/app/flag");
}

void CleanupTestFlags()
{
    if (remove("/data/test/theme/100/a/app/flag") != 0) {}
    if (remove("/data/test/theme/103/a/app/flag") != 0) {}
    if (remove("/data/test/theme/100/b/app/flag") != 0) {}
}

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
    RESMGR_HILOGD(RESMGR_TAG, "ThemeManagerTest teardown");
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
    int32_t userId = 100; // userId is 100
    // the bundlename is invalid
    tm->LoadThemeSkinResource("ohos.global.test", "entry", rootDirs, userId);
    rm->GetColorById(id, outValue);
    ASSERT_EQ(0, outValue); // base_only APP value is #000000
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceTest002
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
    int32_t userId = 100; // userId is 100
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    rm->GetColorById(id, outValue);
    ASSERT_EQ(4294967295, outValue); // base_only theme value is #ffffff(4294967295)
}

/*
 * @tc.name: ThemeManagerTestChangeSkinResourceStatusTest001
 * @tc.desc: Test ChangeSkinResourceStatus function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestChangeSkinResourceStatusTest001, TestSize.Level1)
{
   // success cases
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);

    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/skin/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    uint32_t outValue;
    int id = rmc->GetResId("base_only", ResType::COLOR);
    rm->GetColorById(id, outValue);
    ASSERT_EQ(4294967295, outValue); // base_only theme value is #ffffff(4294967295)

    tm->ChangeSkinResourceStatus(userId);
    tm->ClearSkinResource();

    rm->GetColorById(id, outValue);
    ASSERT_EQ(0, outValue); // base_only APP value is #000000
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceTest003
 * @tc.desc: Test GetThemeDataById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeSkinResourceTest003, TestSize.Level1)
{
   // success cases
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    int32_t userId = 100; // userId is 100
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    int id = rmc->GetResId("ohos_device_theme", ResType::THEME);
    std::map<std::string, ResourceManager::ResData> outValue;
    rm->GetThemeDataById(id, outValue);
    ASSERT_EQ(ResType::FLOAT, outValue["width1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["width2"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["height2"].resType);
    ASSERT_EQ(ResType::COLOR, outValue["textColor1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["textColor2"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["button_pattern"].resType);
    ASSERT_EQ(ResType::THEME, outValue["ohosTheme"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["text_pattern"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["list_pattern"].resType);
    ASSERT_EQ("100fp", outValue["width1"].value);
    ASSERT_EQ("100fp", outValue["width2"].value);
    ASSERT_EQ("100vp", outValue["height1"].value);
    ASSERT_EQ("100vp", outValue["height2"].value);
    ASSERT_EQ("#000000FF", outValue["textColor1"].value);
    ASSERT_EQ("#000000FF", outValue["textColor2"].value);
    ASSERT_EQ("16777333", outValue["button_pattern"].value);
    ASSERT_EQ("16777328", outValue["ohosTheme"].value);
    ASSERT_EQ("16777332", outValue["text_pattern"].value);
    ASSERT_EQ("16777331", outValue["list_pattern"].value);

    std::string rootDir = "/data/test/theme/skin/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    rm->GetThemeDataById(id, outValue);
    ASSERT_EQ(ResType::FLOAT, outValue["width1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["width2"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["height2"].resType);
    ASSERT_EQ(ResType::COLOR, outValue["textColor1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["textColor2"].resType);
    ASSERT_EQ("1000fp", outValue["width1"].value);
    ASSERT_EQ("100fp", outValue["width2"].value);
    ASSERT_EQ("1000vp", outValue["height1"].value);
    ASSERT_EQ("100vp", outValue["height2"].value);
    ASSERT_EQ("#000000AA", outValue["textColor1"].value);
    ASSERT_EQ("#000000FF", outValue["textColor2"].value);
    ASSERT_EQ("16777333", outValue["button_pattern"].value);
    ASSERT_EQ("16777328", outValue["ohosTheme"].value);
    ASSERT_EQ("16777332", outValue["text_pattern"].value);
    ASSERT_EQ("16777331", outValue["list_pattern"].value);
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceTest004
 * @tc.desc: Test GetThemeDataByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeSkinResourceTest004, TestSize.Level1)
{
   // success cases
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    int32_t userId = 100; // userId is 100
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    std::map<std::string, ResourceManager::ResData> outValue;
    rm->GetThemeDataByName("ohos_device_theme", outValue);
    ASSERT_EQ(ResType::FLOAT, outValue["width1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["width2"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["height2"].resType);
    ASSERT_EQ(ResType::COLOR, outValue["textColor1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["textColor2"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["button_pattern"].resType);
    ASSERT_EQ(ResType::THEME, outValue["ohosTheme"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["text_pattern"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["list_pattern"].resType);
    ASSERT_EQ("100fp", outValue["width1"].value);
    ASSERT_EQ("100fp", outValue["width2"].value);
    ASSERT_EQ("100vp", outValue["height1"].value);
    ASSERT_EQ("100vp", outValue["height2"].value);
    ASSERT_EQ("#000000FF", outValue["textColor1"].value);
    ASSERT_EQ("#000000FF", outValue["textColor2"].value);
    ASSERT_EQ("16777333", outValue["button_pattern"].value);
    ASSERT_EQ("16777328", outValue["ohosTheme"].value);
    ASSERT_EQ("16777332", outValue["text_pattern"].value);
    ASSERT_EQ("16777331", outValue["list_pattern"].value);

    std::string rootDir = "/data/test/theme/skin/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    rm->GetThemeDataByName("ohos_device_theme", outValue);
    ASSERT_EQ(ResType::FLOAT, outValue["width1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["width2"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["height2"].resType);
    ASSERT_EQ(ResType::COLOR, outValue["textColor1"].resType);
    ASSERT_EQ(ResType::STRING, outValue["textColor2"].resType);
    ASSERT_EQ("1000fp", outValue["width1"].value);
    ASSERT_EQ("100fp", outValue["width2"].value);
    ASSERT_EQ("1000vp", outValue["height1"].value);
    ASSERT_EQ("100vp", outValue["height2"].value);
    ASSERT_EQ("#000000AA", outValue["textColor1"].value);
    ASSERT_EQ("#000000FF", outValue["textColor2"].value);
    ASSERT_EQ("16777333", outValue["button_pattern"].value);
    ASSERT_EQ("16777328", outValue["ohosTheme"].value);
    ASSERT_EQ("16777332", outValue["text_pattern"].value);
    ASSERT_EQ("16777331", outValue["list_pattern"].value);
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceTest005
 * @tc.desc: Test GetPatternDataByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeSkinResourceTest005, TestSize.Level1)
{
   // success cases
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    int32_t userId = 100; // userId is 100
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    std::map<std::string, ResourceManager::ResData> outValue;
    rm->GetPatternDataByName("ohos_test_button_pattern", outValue);
    ASSERT_EQ(ResType::STRING, outValue["width"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height"].resType);
    ASSERT_EQ(ResType::STRING, outValue["bgColor"].resType);
    ASSERT_EQ(ResType::STRING, outValue["fgColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["fontPattern"].resType);
    ASSERT_EQ(ResType::STRING, outValue["marginColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["baseFontPattern"].resType);
    ASSERT_EQ("10vp", outValue["width"].value);
    ASSERT_EQ("66vp", outValue["height"].value);
    ASSERT_EQ("#FF0000BB", outValue["bgColor"].value);
    ASSERT_EQ("#FF0000CC", outValue["fgColor"].value);
    ASSERT_EQ("16777334", outValue["fontPattern"].value);
    ASSERT_EQ("#FF00AA00", outValue["marginColor"].value);
    ASSERT_EQ("16777335", outValue["baseFontPattern"].value);

    std::string rootDir = "/data/test/theme/skin/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    rm->GetPatternDataByName("ohos_test_button_pattern", outValue);
    ASSERT_EQ(ResType::STRING, outValue["width"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height"].resType);
    ASSERT_EQ(ResType::STRING, outValue["bgColor"].resType);
    ASSERT_EQ(ResType::STRING, outValue["fgColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["fontPattern"].resType);
    ASSERT_EQ(ResType::STRING, outValue["marginColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["baseFontPattern"].resType);
    ASSERT_EQ("10vp", outValue["width"].value);
    ASSERT_EQ("666vp", outValue["height"].value);
    ASSERT_EQ("#FF0000BB", outValue["bgColor"].value);
    ASSERT_EQ("#FF0000CC", outValue["fgColor"].value);
    ASSERT_EQ("16777334", outValue["fontPattern"].value);
    ASSERT_EQ("#FF00AA00", outValue["marginColor"].value);
    ASSERT_EQ("16777335", outValue["baseFontPattern"].value);
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceTest006
 * @tc.desc: Test GetPatternDataById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeSkinResourceTest006, TestSize.Level1)
{
   // success cases
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    int32_t userId = 100; // userId is 100
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    int id = rmc->GetResId("ohos_test_button_pattern", ResType::PATTERN);
    std::map<std::string, ResourceManager::ResData> outValue;
    rm->GetPatternDataById(id, outValue);
    ASSERT_EQ(ResType::STRING, outValue["width"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height"].resType);
    ASSERT_EQ(ResType::STRING, outValue["bgColor"].resType);
    ASSERT_EQ(ResType::STRING, outValue["fgColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["fontPattern"].resType);
    ASSERT_EQ(ResType::STRING, outValue["marginColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["baseFontPattern"].resType);
    ASSERT_EQ("10vp", outValue["width"].value);
    ASSERT_EQ("66vp", outValue["height"].value);
    ASSERT_EQ("#FF0000BB", outValue["bgColor"].value);
    ASSERT_EQ("#FF0000CC", outValue["fgColor"].value);
    ASSERT_EQ("16777334", outValue["fontPattern"].value);
    ASSERT_EQ("#FF00AA00", outValue["marginColor"].value);
    ASSERT_EQ("16777335", outValue["baseFontPattern"].value);

    std::string rootDir = "/data/test/theme/skin/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId);
    rm->GetPatternDataById(id, outValue);
    ASSERT_EQ(ResType::STRING, outValue["width"].resType);
    ASSERT_EQ(ResType::FLOAT, outValue["height"].resType);
    ASSERT_EQ(ResType::STRING, outValue["bgColor"].resType);
    ASSERT_EQ(ResType::STRING, outValue["fgColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["fontPattern"].resType);
    ASSERT_EQ(ResType::STRING, outValue["marginColor"].resType);
    ASSERT_EQ(ResType::PATTERN, outValue["baseFontPattern"].resType);
    ASSERT_EQ("10vp", outValue["width"].value);
    ASSERT_EQ("666vp", outValue["height"].value);
    ASSERT_EQ("#FF0000BB", outValue["bgColor"].value);
    ASSERT_EQ("#FF0000CC", outValue["fgColor"].value);
    ASSERT_EQ("16777334", outValue["fontPattern"].value);
    ASSERT_EQ("#FF00AA00", outValue["marginColor"].value);
    ASSERT_EQ("16777335", outValue["baseFontPattern"].value);
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
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", rootDirs, userId);
    state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_TRUE(state == SUCCESS);
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, userId);
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest002
 * @tc.desc: Test GetThemeIcons function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    uint32_t resId = 16777306;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
    RState state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_EQ(state, ERROR_CODE_RES_ID_NOT_FOUND);

    std::vector<std::string> rootDirs;
    rootDirs.emplace_back("/data/test/theme/icons/dynamic_icons");
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", rootDirs, userId);
    state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_EQ(state, SUCCESS);
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, userId);
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest003
 * @tc.desc: Test GetThemeIcons function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/ohos.global.test.all";
    uint32_t resId = 16777306;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
    RState state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo, 0, "ohos.global.test.all.EntryAbility");
    EXPECT_EQ(state, ERROR_CODE_RES_ID_NOT_FOUND);

    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", rootDirs, userId);
    state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo, 0, "ohos.global.test.all.EntryAbility");
    EXPECT_EQ(state, SUCCESS);
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, userId);
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest004
 * @tc.desc: Test FindThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest004, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/theme1/icons/ohos.global.test1";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test1", "entry", rootDirs, userId);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test1";
    std::string iconName("foreground");
    std::string iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/background.png");
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest005
 * @tc.desc: Test FindThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest005, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/theme1/icons/ohos.global.test1";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test1", "entry", rootDirs, userId);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test1";
    std::string iconName("foreground");
    std::string iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/background.png");

    rootDirs.clear();
    rootDirs.emplace_back("/data/test/theme/theme2/icons/ohos.global.test2");
    tm->LoadThemeIconsResource("ohos.global.test2", "entry", rootDirs, userId);
    bundleInfo.first = "ohos.global.test2";
    iconName = "foreground";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme2/icons/ohos.global.test2/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme2/icons/ohos.global.test2/entry/background.png");
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest006
 * @tc.desc: Test FindThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest006, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/theme1/icons/ohos.global.test1";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test1", "entry", rootDirs, userId);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test1";
    std::string iconName("foreground");
    std::string iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/background.png");

    rootDirs.clear();
    rootDirs.emplace_back("/data/test/theme/theme2/icons/ohos.global.test2");
    tm->LoadThemeIconsResource("ohos.global.test2", "entry", rootDirs, userId);
    iconName = "foreground";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest007
 * @tc.desc: Test FindThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest007, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/theme1/icons";
    rootDirs = tm->GetRootDir(rootDir, "", 100);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test1", "entry", rootDirs, userId);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test1";
    std::string iconName("foreground");
    std::string iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/background.png");

    tm->LoadThemeIconRes("ohos.global.test1", "entry", userId);
    iconName = "foreground";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceTest008
 * @tc.desc: Test FindThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceTest008, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/theme1/icons";
    rootDirs = tm->GetRootDir(rootDir, "", 100);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test1", "entry", rootDirs, userId);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test1";
    std::string iconName("foreground");
    std::string iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/background.png");

    tm->LoadThemeIconRes("ohos.global.test1", "entry", userId);
    iconName = "foreground";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");

    rootDirs.clear();
    rootDirs = tm->GetRootDir("/data/test/theme/theme2/icons", "", 100);
    tm->LoadThemeIconsResource("ohos.global.test2", "entry", rootDirs, userId);
    bundleInfo.first = "ohos.global.test2";
    iconName = "foreground";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme2/icons/ohos.global.test2/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme2/icons/ohos.global.test2/entry/background.png");
}

/*
 * @tc.name: ThemeManagerTestChangeIconResourceStatusTest001
 * @tc.desc: Test ChangeIconResourceStatus function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestChangeIconResourceStatusTest001, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/theme1/icons";
    rootDirs = tm->GetRootDir(rootDir, "", 100);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test1", "entry", rootDirs, userId);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test1";
    std::string iconName("foreground");
    std::string iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/foreground.png");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "/data/test/theme/theme1/icons/ohos.global.test1/entry/background.png");

    tm->ChangeIconResourceStatus(userId);
    tm->ClearIconResource(100);

    iconName = "foreground";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");
    iconName = "background";
    iconPath = tm->FindThemeIconResource(bundleInfo, iconName, userId);
    EXPECT_EQ(iconPath, "");
}

/*
 * @tc.name: ThemeManagerTestHasIconInThemeTest001
 * @tc.desc: Test HasIconInTheme function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestHasIconInThemeTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/ohos.global.test.all";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", rootDirs, userId);
    bool result = rm->HasIconInTheme("ohos.global.test");
    EXPECT_TRUE(result == false);

    result = rm->HasIconInTheme("ohos.global.test.all");
    EXPECT_TRUE(result == true);
}

/*
 * @tc.name: ThemeManagerTestGetOtherIconsInfoTest001
 * @tc.desc: Test GetOtherIconsInfo function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetOtherIconsInfoTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/other_icons";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("other_icons", "", rootDirs, userId);
    std::unique_ptr<uint8_t[]> outValue;
    size_t len;
    RState state = rm->GetOtherIconsInfo("other_icons", outValue, len, true);
    EXPECT_EQ(state, ERROR_CODE_RES_NOT_FOUND_BY_NAME);

    state = rm->GetOtherIconsInfo("foreground", outValue, len, false);
    EXPECT_EQ(state, SUCCESS);
}

/*
 * @tc.name: ThemeManagerTestGetOtherIconsInfoTest002
 * @tc.desc: Test GetOtherIconsInfo function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetOtherIconsInfoTest002, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/";
    rootDirs = tm->GetRootDir(rootDir, "", 100);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("other_icons", "", rootDirs, userId);
    std::unique_ptr<uint8_t[]> outValue;
    size_t len;
    RState state = rm->GetOtherIconsInfo("icon_mask", outValue, len, true);
    EXPECT_EQ(state, SUCCESS);

    state = rm->GetOtherIconsInfo("icon_mask", outValue, len, false);
    EXPECT_EQ(state, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
}

/*
 * @tc.name: ThemeManagerTestGetOtherIconsInfoTest003
 * @tc.desc: Test GetOtherIconsInfo function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetOtherIconsInfoTest003, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/";
    rootDirs = tm->GetRootDir(rootDir, "", 100);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("other_icons", "", rootDirs, userId);
    std::unique_ptr<uint8_t[]> outValue;
    size_t len;
    RState state = rm->GetOtherIconsInfo("icon_mask", outValue, len, true);
    EXPECT_EQ(state, SUCCESS);

    RState state1 = tm->GetThemeIconFromCache("global_icon_mask", outValue, len, 100);
    EXPECT_EQ(state1, SUCCESS);

    RState state2 = rm->GetOtherIconsInfo("icon_mask", outValue, len, true);
    EXPECT_EQ(state2, SUCCESS);
}

/*
 * @tc.name: ThemeManagerTestGetThemeIconFromCacheTest001
 * @tc.desc: Test GetThemeIconFromCache function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetThemeIconFromCacheTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/other_icons";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("other_icons", "", rootDirs, userId);
    std::unique_ptr<uint8_t[]> outValue;
    size_t len;
    RState state = tm->GetThemeIconFromCache("other_icons_background", outValue, len, 100);
    EXPECT_EQ(state, NOT_FOUND);
    state = tm->GetOtherIconsInfo("background", outValue, len, false, userId);
    EXPECT_EQ(state, SUCCESS);
    state = tm->GetThemeIconFromCache("other_icons_background", outValue, len, 100);
    EXPECT_EQ(state, SUCCESS);
}

/*
 * @tc.name: ThemeManagerTestGetThemeIconFromCacheTest002
 * @tc.desc: Test GetThemeIconFromCache function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetThemeIconFromCacheTest002, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/other_icons";
    rootDirs.emplace_back(rootDir);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("other_icons", "", rootDirs, userId);
    std::unique_ptr<uint8_t[]> outValue;
    size_t len;
    RState state = tm->GetOtherIconsInfo("background", outValue, len, false, userId);
    EXPECT_EQ(state, SUCCESS);
    state = tm->GetThemeIconFromCache("global_icon_mask", outValue, len, 100);
    EXPECT_EQ(state, NOT_FOUND);
    state = tm->GetOtherIconsInfo("icon_mask", outValue, len, true, userId);
    EXPECT_EQ(state, SUCCESS);
    state = tm->GetThemeIconFromCache("global_icon_mask", outValue, len, 100);
    EXPECT_EQ(state, SUCCESS);
}

/*
 * @tc.name: ThemeManagerTestGetHighlightIconTest001
 * @tc.desc: Test GetHighlightIconInfo function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetHighlightIconTest001, TestSize.Level1)
{
    std::vector<std::string> rootDirs;
    std::string rootDir = "/data/test/theme/icons/";
    rootDirs = tm->GetRootDir(rootDir, "", 100);
    int32_t userId = 100; // userId is 100
    tm->LoadThemeIconsResource("other_icons", "", rootDirs, userId);
    std::unique_ptr<uint8_t[]> outValue;
    size_t len;
    RState state = rm->GetOtherIconsInfo("icon_highlightstroke", outValue, len, true);
    EXPECT_EQ(len, 4897);
    EXPECT_EQ(state, SUCCESS);

    std::unique_ptr<uint8_t[]> outValue2;
    size_t len2;
    // from cache
    state = rm->GetOtherIconsInfo("icon_highlightstroke", outValue2, len2, true);
    EXPECT_EQ(len2, 4897);
    EXPECT_EQ(state, SUCCESS);

    std::unique_ptr<uint8_t[]> outValue3;
    size_t len3;
    state = rm->GetOtherIconsInfo("icon_highlightstroke", outValue3, len3, false);
    EXPECT_EQ(len3, 0);
    EXPECT_EQ(state, ERROR_CODE_RES_NOT_FOUND_BY_NAME);

    std::unique_ptr<uint8_t[]> outValue4;
    size_t len4;
    // mock update theme, clear highlight icon
    tm->ClearHighlightIcon(100);
    // this root dir does not contain icon_highlightstroke
    tm->GetRootDir("/data/test/theme/theme2/icons", "", 100);
    state = rm->GetOtherIconsInfo("icon_highlightstroke", outValue4, len4, true);
    EXPECT_EQ(len4, 0);
    EXPECT_EQ(state, ERROR_CODE_RES_NOT_FOUND_BY_NAME);
}

/*
 * @tc.name: ThemeManagerTestIsUpdateByUserIdTest001
 * @tc.desc: Test IsUpdateByUserId function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestIsUpdateByUserIdTest001, TestSize.Level1)
{
    int32_t userId = 100;
    tm->UpdateUserId(userId);
    bool result = tm->IsUpdateByUserId(userId);
    ASSERT_FALSE(result);

    userId = 101;
    result = tm->IsUpdateByUserId(userId);
    ASSERT_TRUE(result);
}

/*
 * @tc.name: ThemeManagerTestIsSameResourceByUserIdTest001
 * @tc.desc: Test IsSameResourceByUserId function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestIsSameResourceByUserIdTest001, TestSize.Level1)
{
    std::string path("/data/test/theme/100/a/app/icons/ohos.global.test2/entry/foreground.png");
    int32_t userId = 100;
    bool result = tm->IsSameResourceByUserId(path, userId);
    EXPECT_TRUE(result == true);

    userId = 101;
    result = tm->IsSameResourceByUserId(path, userId);
    EXPECT_TRUE(result == false);
}

/*
 * @tc.name: ThemeManagerTestIsSameResourceByUserIdTest002
 * @tc.desc: Test IsSameResourceByUserId function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestIsSameResourceByUserIdTest002, TestSize.Level1)
{
    std::string path("/data/test/theme/100");
    int32_t userId = 100;
    bool result = tm->IsSameResourceByUserId(path, userId);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: ThemeManagerTestIsSameResourceByUserIdTest003
 * @tc.desc: Test IsSameResourceByUserId function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestIsSameResourceByUserIdTest003, TestSize.Level1)
{
    std::string path("/data/test/theme/abc/");
    int32_t userId = 100;
    bool result = tm->IsSameResourceByUserId(path, userId);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: ThemeManagerTestGetMaskStringTest001
 * @tc.desc: Test GetMaskString function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetMaskStringTest001, TestSize.Level1)
{
    std::string path("/data/test/theme/100/a/app/icons/ohos.global.test2/entry/foreground.png");
    std::string maskPath = tm->GetMaskString(path);
    EXPECT_EQ(maskPath, "100/a/app/icons/ohos.global.test2/entry/foreground.png");

    path = "/data/test/theme/theme2/icons/ohos.global.test2/entry/foreground.png";
    maskPath = tm->GetMaskString(path);
    EXPECT_EQ(maskPath, "theme2/icons/ohos.global.test2/entry/foreground.png");
}

/*
 * @tc.name: ThemeManagerTestGetRootDirBadPathTest001
 * @tc.desc: Test GetRootDir with non-existent path
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetRootDirBadPathTest001, TestSize.Level1)
{
    std::vector<std::string> rootDirs = tm->GetRootDir("/nonexistent/path/xyz", "", 100);
    EXPECT_TRUE(rootDirs.empty());
}

/*
 * @tc.name: ThemeManagerTestFindThemeResourceNullIdItemTest001
 * @tc.desc: Test FindThemeResource with nullptr in idItems vector
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestFindThemeResourceNullIdItemTest001, TestSize.Level1)
{
    std::pair<std::string, std::string> bundleInfo("ohos.global.test.all", "entry");
    std::vector<std::shared_ptr<IdItem>> idItems;
    idItems.push_back(nullptr);
    ResConfigImpl resConfig;
    resConfig.SetLocaleInfo("zh", nullptr, "CN");
    int32_t userId = 100;
    std::string result = tm->FindThemeResource(bundleInfo, idItems, resConfig, userId);
    EXPECT_TRUE(result.empty());
}

/*
 * @tc.name: ThemeManagerTestLoadThemeSkinResourceWithBasePathTest001
 * @tc.desc: Test LoadThemeSkinResource with basePath, resources stored as relative path and reconstructed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeSkinResourceWithBasePathTest001, TestSize.Level1)
{
    SetupTestFlags();
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    tm->UpdateBasePath(100);
    std::string basePath = tm->GetBasePath(100);
    ASSERT_FALSE(basePath.empty());
    std::vector<std::string> rootDirs;
    rootDirs.emplace_back(basePath + "/skin/ohos.global.test.all");
    int32_t userId = 100;
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs, userId, basePath);
    int id = rmc->GetResId("base_only", ResType::COLOR);
    uint32_t outValue;
    rm->GetColorById(id, outValue);
    CleanupTestFlags();
    ASSERT_EQ(4294967295, outValue); // base_only theme value is #ffffff
}

/*
 * @tc.name: ThemeManagerTestLoadThemeIconsResourceWithBasePathTest001
 * @tc.desc: Test LoadThemeIconsResource with basePath, icons stored as relative path and reconstructed correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestLoadThemeIconsResourceWithBasePathTest001, TestSize.Level1)
{
    SetupTestFlags();
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    tm->UpdateBasePath(100);
    std::string basePath = tm->GetBasePath(100);
    ASSERT_FALSE(basePath.empty());
    std::vector<std::string> rootDirs;
    rootDirs.emplace_back(basePath + "/app/icons/ohos.global.test.all");
    int32_t userId = 100;
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", rootDirs, userId, basePath);
    uint32_t resId = 16777306;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
    RState state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_TRUE(state == SUCCESS);
    CleanupTestFlags();
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, userId);
}

/*
 * @tc.name: ThemeManagerTestUpdateThemeIdTest001
 * @tc.desc: Test UpdateThemeId returns true when themeId changes.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestUpdateThemeIdTest001, TestSize.Level1)
{
    SetupTestFlags();
    bool result = tm->UpdateThemeId(999, 100);
    EXPECT_TRUE(result);
    result = tm->UpdateThemeId(999, 100);
    EXPECT_FALSE(result);
    result = tm->UpdateThemeId(0, 100);
    EXPECT_FALSE(result);
    result = tm->UpdateThemeId(1000, 100);
    CleanupTestFlags();
    EXPECT_TRUE(result);
}

/*
 * @tc.name: ThemeManagerTestMultiUserThemeIdTest001
 * @tc.desc: Test per-user themeIdMap_ isolation: same themeId for different users should not block each other.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestMultiUserThemeIdTest001, TestSize.Level1)
{
    SetupTestFlags();
    // User 100 sets theme 999
    bool result100 = tm->UpdateThemeId(999, 100);
    EXPECT_TRUE(result100);
    // User 103 sets same theme 999 — should still be true (themeIdMap_[103] was 0, not 999)
    bool result103 = tm->UpdateThemeId(999, 103);
    EXPECT_TRUE(result103);
    // User 100 sets same theme again — false (no change for user 100)
    result100 = tm->UpdateThemeId(999, 100);
    EXPECT_FALSE(result100);
    // User 100 changes to 1000
    result100 = tm->UpdateThemeId(1000, 100);
    EXPECT_TRUE(result100);
    // User 103 sets 1000 — should be true (themeIdMap_[103] is 999, not 1000)
    result103 = tm->UpdateThemeId(1000, 103);
    EXPECT_TRUE(result103);
    // User 100 sets 1000 again — false
    result100 = tm->UpdateThemeId(1000, 100);
    CleanupTestFlags();
    EXPECT_FALSE(result100);
}

/*
 * @tc.name: ThemeManagerTestMultiUserBasePathTest001
 * @tc.desc: Test per-user basePathMap_ isolation: different users have independent basePath values.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestMultiUserBasePathTest001, TestSize.Level1)
{
    SetupTestFlags();
    tm->UpdateBasePath(100);
    tm->UpdateBasePath(103);
    std::string basePath100 = tm->GetBasePath(100);
    std::string basePath103 = tm->GetBasePath(103);
    ASSERT_FALSE(basePath100.empty());
    ASSERT_FALSE(basePath103.empty());
    // SA path includes userId, different users get different basePaths
    EXPECT_NE(basePath100, basePath103);
    EXPECT_NE(basePath100.find("/100/"), std::string::npos);
    EXPECT_NE(basePath103.find("/103/"), std::string::npos);
    // User 200 has no basePath (never updated)
    std::string basePath200 = tm->GetBasePath(200);
    CleanupTestFlags();
    EXPECT_TRUE(basePath200.empty());
}

/*
 * @tc.name: ThemeManagerTestMultiUserResourceAccessTest001
 * @tc.desc: Test multi-user resource access isolation.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestMultiUserResourceAccessTest001, TestSize.Level1)
{
    SetupTestFlags();
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    // User 100 loads skin resources
    tm->UpdateBasePath(100);
    std::string basePath100 = tm->GetBasePath(100);
    ASSERT_FALSE(basePath100.empty());
    std::vector<std::string> rootDirs100;
    rootDirs100.emplace_back(basePath100 + "/skin/ohos.global.test.all");
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs100, 100, basePath100);
    // User 103 loads skin resources (different basePath)
    tm->UpdateBasePath(103);
    std::string basePath103 = tm->GetBasePath(103);
    ASSERT_FALSE(basePath103.empty());
    EXPECT_NE(basePath100, basePath103);
    std::vector<std::string> rootDirs103;
    rootDirs103.emplace_back(basePath103 + "/skin/ohos.global.test.all");
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirs103, 103, basePath103);
    // User 100 still gets correct color value (not affected by user 103's load)
    int id = rmc->GetResId("base_only", ResType::COLOR);
    uint32_t outValue;
    rm->GetColorById(id, outValue);
    ASSERT_EQ(4294967295, outValue); // base_only theme value is #ffffff
    // User 100 loads icon resources
    std::vector<std::string> iconDirs100;
    iconDirs100.emplace_back(basePath100 + "/app/icons/ohos.global.test.all");
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", iconDirs100, 100, basePath100);
    // User 103 loads icon resources
    std::vector<std::string> iconDirs103;
    iconDirs103.emplace_back(basePath103 + "/app/icons/ohos.global.test.all");
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", iconDirs103, 103, basePath103);
    // User 100 still gets correct icons (not affected by user 103's load)
    uint32_t resId = 16777306;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
    RState state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_TRUE(state == SUCCESS);
    // Cleanup
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, 100);
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", {}, 100);
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, 103);
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", {}, 103);
    CleanupTestFlags();
}

/*
 * @tc.name: ThemeManagerTestGetLogPathTest001
 * @tc.desc: Test GetLogPath strips basePath correctly with flag file set.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetLogPathTest001, TestSize.Level1)
{
    SetupTestFlags();
    tm->UpdateBasePath(100);
    std::string basePath = tm->GetBasePath(100);
    ASSERT_FALSE(basePath.empty());
    std::string path = basePath + "/app/icons/icon_mask.png";
    std::string result = tm->GetLogPath(path, 100);
    CleanupTestFlags();
    EXPECT_EQ(result, "a/app/icons/icon_mask.png");
}

/*
 * @tc.name: ThemeManagerTestGetMaskTest001
 * @tc.desc: Test GetMask returns non-empty after loading icons with flag file and basePath set.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestGetMaskTest001, TestSize.Level1)
{
    SetupTestFlags();
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    tm->UpdateBasePath(100);
    std::string basePath = tm->GetBasePath(100);
    ASSERT_FALSE(basePath.empty());
    int32_t userId = 100;
    // LoadThemeIconRes calls GetRootDir with basePath, sets themeMask_ as relative path
    tm->LoadThemeIconRes("ohos.global.test.all", "entry", userId);
    std::string mask = tm->GetMask(100);
    EXPECT_FALSE(mask.empty());
    EXPECT_NE(mask.find(basePath), std::string::npos);
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, userId);
    CleanupTestFlags();
}

/*
 * @tc.name: ThemeManagerTestThemeSwitchABTest001
 * @tc.desc: Test theme directory switch from a to b: basePath updates and resources accessible after switch.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeManagerTest, ThemeManagerTestThemeSwitchABTest001, TestSize.Level1)
{
    SetupTestFlags();
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    int32_t userId = 100;
    // Step 1: flag at a, basePath points to a
    tm->UpdateBasePath(userId);
    std::string basePathA = tm->GetBasePath(userId);
    ASSERT_FALSE(basePathA.empty());
    EXPECT_NE(basePathA.find("/a"), std::string::npos);
    // Load skin resources from a
    std::vector<std::string> rootDirsA;
    rootDirsA.emplace_back(basePathA + "/skin/ohos.global.test.all");
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirsA, userId, basePathA);
    int id = rmc->GetResId("base_only", ResType::COLOR);
    uint32_t outValue;
    rm->GetColorById(id, outValue);
    ASSERT_EQ(4294967295, outValue); // #ffffff

    // Step 2: simulate theme switch a->b: remove a flag, UpdateBasePath falls to b
    if (remove("/data/test/theme/100/a/app/flag") != 0) {}
    tm->UpdateBasePath(userId);
    std::string basePathB = tm->GetBasePath(userId);
    ASSERT_FALSE(basePathB.empty());
    EXPECT_NE(basePathB.find("/b"), std::string::npos);
    EXPECT_NE(basePathA, basePathB);
    // Load skin resources from b (relative paths stored, reconstructed with new basePath)
    std::vector<std::string> rootDirsB;
    rootDirsB.emplace_back(basePathB + "/skin/ohos.global.test.all");
    tm->LoadThemeSkinResource("ohos.global.test.all", "entry", rootDirsB, userId, basePathB);
    rm->GetColorById(id, outValue);
    ASSERT_EQ(4294967295, outValue); // still #ffffff

    // Step 3: load icons from b
    std::vector<std::string> iconDirsB;
    iconDirsB.emplace_back(basePathB + "/app/icons/ohos.global.test.all");
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", iconDirsB, userId, basePathB);
    uint32_t resId = 16777306;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
    std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
    RState state = rm->GetThemeIcons(resId, foregroundInfo, backgroundInfo);
    EXPECT_TRUE(state == SUCCESS);

    // Cleanup
    tm->LoadThemeIconsResource("ohos.global.test.all", "entry", {}, userId);
    CleanupTestFlags();
}
}
