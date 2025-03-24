/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "theme_pack_resource.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

namespace {
class ThemeResourceTest : public testing::Test {
    protected:
    void SetUp() override;
    void TearDown() override;
};

void ThemeResourceTest::SetUp()
{}

void ThemeResourceTest::TearDown()
{}

/*
 * @tc.name: ThemeResourceInitThemeResTest001
 * @tc.desc: Test InitThemeRes function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceInitThemeResTest001, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::pair<std::string, std::string> bundleInfo;
    cJSON *root = nullptr;
    std::shared_ptr<ThemeConfig> themeConfig = std::make_shared<ThemeConfig>();
    std::string resTypeStr = "";
    themeResource->InitThemeRes(bundleInfo, root, themeConfig, resTypeStr);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceInitThemeResTest002
 * @tc.desc: Test InitThemeRes function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceInitThemeResTest002, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::pair<std::string, std::string> bundleInfo;
    cJSON *root = cJSON_CreateObject();
    cJSON *array = cJSON_CreateArray();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name1", "bbbbb");
    cJSON_AddItemToArray(array, item);
    cJSON_AddItemToObject(root, "color", array);
    std::shared_ptr<ThemeConfig> themeConfig = std::make_shared<ThemeConfig>();
    std::string resTypeStr = "color";
    themeResource->InitThemeRes(bundleInfo, root, themeConfig, resTypeStr);
    cJSON_Delete(root);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceInitThemeResTest003
 * @tc.desc: Test InitThemeRes function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceInitThemeResTest003, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::pair<std::string, std::string> bundleInfo;
    cJSON *root = cJSON_CreateObject();
    cJSON *array = cJSON_CreateArray();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "value1", "bbbbb");
    cJSON_AddItemToArray(array, item);
    cJSON_AddItemToObject(root, "color", array);
    std::shared_ptr<ThemeConfig> themeConfig = std::make_shared<ThemeConfig>();
    std::string resTypeStr = "color";
    themeResource->InitThemeRes(bundleInfo, root, themeConfig, resTypeStr);
    cJSON_Delete(root);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseJsonTest001
 * @tc.desc: Test ParseJson function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseJsonTest001, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string jsonPath = "/data/test/theme/dark/media/foreground.png";
    themeResource->ParseJson(bundleName, moduleName, jsonPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseJsonTest002
 * @tc.desc: Test ParseJson function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseJsonTest002, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string jsonPath = "/data/test/theme/horizontal/media/foreground.png";
    themeResource->ParseJson(bundleName, moduleName, jsonPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseJsonTest003
 * @tc.desc: Test ParseJson function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseJsonTest003, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string jsonPath = "/data/test/theme/horizontal-dark/media/foreground.png";
    themeResource->ParseJson(bundleName, moduleName, jsonPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseJsonTest004
 * @tc.desc: Test ParseJson function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseJsonTest004, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string jsonPath = "/data/test/theme/base/media/foreground.png";
    themeResource->ParseJson(bundleName, moduleName, jsonPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseJsonTest005
 * @tc.desc: Test ParseJson function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseJsonTest005, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string jsonPath = "data/test/foreground.png";
    themeResource->ParseJson(bundleName, moduleName, jsonPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseIconTest001
 * @tc.desc: Test ParseIcon function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseIconTest001, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string iconPath = "data/test";
    themeResource->ParseIcon(bundleName, moduleName, iconPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseIconTest002
 * @tc.desc: Test ParseIcon function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseIconTest002, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string iconPath = "foreground.png";
    themeResource->ParseIcon(bundleName, moduleName, iconPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceParseIconTest003
 * @tc.desc: Test ParseIcon function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceParseIconTest003, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    std::string moduleName = "entry";
    std::string iconPath = "foreground.png/";
    themeResource->ParseIcon(bundleName, moduleName, iconPath);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceGetThemeValuesTest001
 * @tc.desc: Test GetThemeValues function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceGetThemeValuesTest001, TestSize.Level1)
{
    std::shared_ptr<ThemeResource> themeResource = std::make_shared<ThemeResource>("");
    EXPECT_TRUE(themeResource != nullptr);
    std::pair<std::string, std::string> bundInfo;
    ResType resType = ResType::VALUES;
    std::string name("test");
    themeResource->GetThemeValues(bundInfo, resType, name);
    EXPECT_TRUE(themeResource->themeValueVec_.size() == 0);
}

/*
 * @tc.name: ThemeResourceLoadThemeIconResourceTest001
 * @tc.desc: Test LoadThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceLoadThemeIconResourceTest001, TestSize.Level1)
{
    std::string iconPath("");
    std::shared_ptr<ThemeResource> themeResource = ThemeResource::LoadThemeIconResource(iconPath);
    EXPECT_TRUE(themeResource == nullptr);

    iconPath = "abc";
    themeResource = ThemeResource::LoadThemeIconResource(iconPath);
    EXPECT_TRUE(themeResource != nullptr);
}

/*
 * @tc.name: ThemeResourceLoadThemeIconResourceTest002
 * @tc.desc: Test LoadThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceLoadThemeIconResourceTest002, TestSize.Level1)
{
    std::string iconPath("/data/test/theme/themereource/");
    std::shared_ptr<ThemeResource> themeResource = ThemeResource::LoadThemeIconResource(iconPath);
    EXPECT_TRUE(themeResource != nullptr);
}

/*
 * @tc.name: ThemeResourceLoadThemeIconResourceTest003
 * @tc.desc: Test LoadThemeIconResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceLoadThemeIconResourceTest003, TestSize.Level1)
{
    std::string iconPath("/data/test/theme/icons/dynamic_icons/");
    std::shared_ptr<ThemeResource> themeResource = ThemeResource::LoadThemeIconResource(iconPath);
    EXPECT_TRUE((themeResource != nullptr && themeResource->iconValues_.size() != 0));
}

/*
 * @tc.name: ThemeResourceGetThemeAppIconByAbilityNameTest001
 * @tc.desc: Test GetThemeAppIconByAbilityName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceGetThemeAppIconByAbilityNameTest001, TestSize.Level1)
{
    std::string iconPath("/data/test/theme/icons/dynamic_icons/ohos.global.test.all/");
    std::shared_ptr<ThemeResource> themeResource = ThemeResource::LoadThemeIconResource(iconPath);
    EXPECT_TRUE(themeResource != nullptr);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test";
    bundleInfo.second = "entry";
    std::string iconName = "foreground";
    std::string abilityName = "MainAbility";
    std::string result = themeResource->GetThemeAppIconByAbilityName(bundleInfo, iconName, abilityName);
    EXPECT_EQ(result, std::string(""));
}

/*
 * @tc.name: ThemeResourceGetThemeAppIconByAbilityNameTest002
 * @tc.desc: Test GetThemeAppIconByAbilityName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceGetThemeAppIconByAbilityNameTest002, TestSize.Level1)
{
    std::string iconPath("/data/test/theme/icons/ohos.global.test.all/entry/");
    std::shared_ptr<ThemeResource> themeResource = ThemeResource::LoadThemeIconResource(iconPath);
    EXPECT_TRUE(themeResource != nullptr);
    std::pair<std::string, std::string> bundleInfo;
    bundleInfo.first = "ohos.global.test";
    bundleInfo.second = "entry";
    std::string iconName = "foreground";
    std::string abilityName = "MainAbility";
    std::string result = themeResource->GetThemeAppIconByAbilityName(bundleInfo, iconName, abilityName);
    EXPECT_EQ(result, std::string(""));
}

/*
 * @tc.name: ThemeResourceHasIconInThemeTest001
 * @tc.desc: Test HasIconInTheme function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeResourceTest, ThemeResourceHasIconInThemeTest001, TestSize.Level1)
{
    std::string iconPath("/data/test/theme/icons/dynamic_icons/ohos.global.test.all/");
    std::shared_ptr<ThemeResource> themeResource = ThemeResource::LoadThemeIconResource(iconPath);
    EXPECT_TRUE(themeResource != nullptr);
    std::string bundleName = "ohos.global.test.all";
    bool result = themeResource->HasIconInTheme(bundleName);
    EXPECT_FALSE(result);
}
}
