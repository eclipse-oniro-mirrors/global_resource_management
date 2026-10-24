/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <string>
#include <unordered_set>

#include "brand_color_manager.h"
#include "hilog_wrapper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Global {
namespace Resource {

// System color resource names and values for test data
// (from system_resources/systemres/main/resources/base/element/color.json)
constexpr uint32_t COLOR_OHOS_ID_EMPHASIZE = 0xFF007DFF;       // ohos_id_color_emphasize
constexpr uint32_t COLOR_OHOS_ID_WARNING = 0xFFFA2A2D;         // ohos_id_color_warning
constexpr uint32_t COLOR_OHOS_ID_ALERT = 0xFFFF7500;           // ohos_id_color_alert
constexpr uint32_t COLOR_OHOS_ID_COMPONENT_NORMAL = 0x19182431; // ohos_id_color_component_normal
constexpr uint32_t COLOR_DIVIDER = 0x10000000;                  // divider_color (from test HAP)
constexpr uint32_t BRAND_COLOR = 0xFFFF0000;                   // red brand color

class BrandColorManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDown()
    {
        auto &inst = BrandColorManager::GetInstance();
        inst.overrideList_.clear();
        inst.loaded_.store(false, std::memory_order_relaxed);
    }

    // Set up override list with names and mark as loaded
    void SetupList(const std::vector<std::string> &names)
    {
        auto &inst = BrandColorManager::GetInstance();
        inst.overrideList_.clear();
        for (const auto &name : names) {
            inst.overrideList_.insert(name);
        }
        inst.loaded_.store(true, std::memory_order_release);
    }
};

// === Basic tests ===

HWTEST_F(BrandColorManagerTest, GetInstanceTest, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    EXPECT_NE(nullptr, &instance);
}

HWTEST_F(BrandColorManagerTest, ApplyBrandColorNotLoaded, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    instance.overrideList_.clear();
    instance.loaded_.store(false, std::memory_order_relaxed);
    uint32_t color = COLOR_OHOS_ID_EMPHASIZE;
    bool result = instance.ApplyBrandColor("ohos_id_color_emphasize", BRAND_COLOR, color);
    EXPECT_FALSE(result);
    EXPECT_EQ(COLOR_OHOS_ID_EMPHASIZE, color);
}

// === RGB replacement + alpha preservation ===

HWTEST_F(BrandColorManagerTest, ReplaceRgbKeepAlphaFF, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_emphasize"});
    uint32_t color = COLOR_OHOS_ID_EMPHASIZE;  // alpha=FF, RGB=007DFF
    bool result = instance.ApplyBrandColor("ohos_id_color_emphasize", BRAND_COLOR, color);
    EXPECT_TRUE(result);
    EXPECT_EQ(0xFFFF0000u, color);  // alpha=FF preserved, RGB=FF0000 (brand)
}

HWTEST_F(BrandColorManagerTest, ReplaceRgbKeepAlpha19, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_component_normal"});
    uint32_t color = COLOR_OHOS_ID_COMPONENT_NORMAL;  // alpha=19, RGB=182431
    bool result = instance.ApplyBrandColor("ohos_id_color_component_normal", BRAND_COLOR, color);
    EXPECT_TRUE(result);
    EXPECT_EQ(0x19FF0000u, color);  // alpha=19 preserved, RGB=FF0000 (brand)
}

HWTEST_F(BrandColorManagerTest, ReplaceRgbKeepAlpha10, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"divider_color"});
    uint32_t color = COLOR_DIVIDER;  // alpha=10, RGB=000000
    bool result = instance.ApplyBrandColor("divider_color", BRAND_COLOR, color);
    EXPECT_TRUE(result);
    EXPECT_EQ(0x10FF0000u, color);  // alpha=10 preserved, RGB=FF0000 (brand)
}

HWTEST_F(BrandColorManagerTest, ReplaceRgbKeepAlpha00, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"base_only"});
    uint32_t color = 0x00000000;  // alpha=00, RGB=000000
    bool result = instance.ApplyBrandColor("base_only", BRAND_COLOR, color);
    EXPECT_TRUE(result);
    EXPECT_EQ(0x00FF0000u, color);  // alpha=00 preserved, RGB=FF0000 (brand)
}

// === Name not in list ===

HWTEST_F(BrandColorManagerTest, NotInList, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_emphasize"});
    uint32_t color = COLOR_OHOS_ID_WARNING;
    bool result = instance.ApplyBrandColor("ohos_id_color_warning", BRAND_COLOR, color);
    EXPECT_FALSE(result);
    EXPECT_EQ(COLOR_OHOS_ID_WARNING, color);  // unchanged
}

HWTEST_F(BrandColorManagerTest, EmptyName, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_emphasize"});
    uint32_t color = COLOR_OHOS_ID_EMPHASIZE;
    bool result = instance.ApplyBrandColor("", BRAND_COLOR, color);
    EXPECT_FALSE(result);
    EXPECT_EQ(COLOR_OHOS_ID_EMPHASIZE, color);
}

// === IsBrandColorName ===

HWTEST_F(BrandColorManagerTest, IsBrandColorNameTrue, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_emphasize", "ohos_id_color_warning"});
    EXPECT_TRUE(instance.IsBrandColorName("ohos_id_color_emphasize"));
    EXPECT_TRUE(instance.IsBrandColorName("ohos_id_color_warning"));
}

HWTEST_F(BrandColorManagerTest, IsBrandColorNameFalse, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_emphasize"});
    EXPECT_FALSE(instance.IsBrandColorName("ohos_id_color_alert"));
    EXPECT_FALSE(instance.IsBrandColorName(""));
}

HWTEST_F(BrandColorManagerTest, IsBrandColorNameNotLoaded, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    instance.overrideList_.clear();
    instance.loaded_.store(false, std::memory_order_relaxed);
    EXPECT_FALSE(instance.IsBrandColorName("ohos_id_color_emphasize"));
}

// === Multiple names in list (reference chain scenario) ===
// Simulates A->B->C reference chain where B is in the list.
// ApplyBrandColorByIdItems iterates idItems and finds the first match.

HWTEST_F(BrandColorManagerTest, MultipleNamesInList, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    // Simulate a reference chain: ohos_id_color_emphasize -> ohos_id_color_warning -> ohos_id_color_alert
    // All three are in the brand color list
    SetupList({"ohos_id_color_emphasize", "ohos_id_color_warning", "ohos_id_color_alert"});

    // The first name found in the chain should be used for replacement
    EXPECT_TRUE(instance.IsBrandColorName("ohos_id_color_emphasize"));
    EXPECT_TRUE(instance.IsBrandColorName("ohos_id_color_warning"));
    EXPECT_TRUE(instance.IsBrandColorName("ohos_id_color_alert"));

    // ApplyBrandColor with the first name in chain
    uint32_t color = COLOR_OHOS_ID_EMPHASIZE;
    EXPECT_TRUE(instance.ApplyBrandColor("ohos_id_color_emphasize", BRAND_COLOR, color));
    EXPECT_EQ(0xFFFF0000u, color);

    // ApplyBrandColor with the second name in chain
    color = COLOR_OHOS_ID_WARNING;
    EXPECT_TRUE(instance.ApplyBrandColor("ohos_id_color_warning", BRAND_COLOR, color));
    EXPECT_EQ(0xFFFF0000u, color);
}

HWTEST_F(BrandColorManagerTest, OnlyMiddleNameInList, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    // Simulate A->B->C where only B (middle) is in the list
    // A="ohos_id_color_emphasize" (not in list)
    // B="ohos_id_color_warning" (in list)
    // C="ohos_id_color_alert" (not in list)
    SetupList({"ohos_id_color_warning"});

    EXPECT_FALSE(instance.IsBrandColorName("ohos_id_color_emphasize"));
    EXPECT_TRUE(instance.IsBrandColorName("ohos_id_color_warning"));
    EXPECT_FALSE(instance.IsBrandColorName("ohos_id_color_alert"));

    // ApplyBrandColor with B should succeed
    uint32_t color = COLOR_OHOS_ID_WARNING;
    EXPECT_TRUE(instance.ApplyBrandColor("ohos_id_color_warning", BRAND_COLOR, color));
    EXPECT_EQ(0xFFFF0000u, color);

    // ApplyBrandColor with A or C should fail (not in list)
    color = COLOR_OHOS_ID_EMPHASIZE;
    EXPECT_FALSE(instance.ApplyBrandColor("ohos_id_color_emphasize", BRAND_COLOR, color));
    EXPECT_EQ(COLOR_OHOS_ID_EMPHASIZE, color);
}

HWTEST_F(BrandColorManagerTest, OnlyLastNameInList, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    // Simulate A->B->C where only C (last) is in the list
    SetupList({"ohos_id_color_alert"});

    EXPECT_FALSE(instance.IsBrandColorName("ohos_id_color_emphasize"));
    EXPECT_FALSE(instance.IsBrandColorName("ohos_id_color_warning"));
    EXPECT_TRUE(instance.IsBrandColorName("ohos_id_color_alert"));

    uint32_t color = COLOR_OHOS_ID_ALERT;
    EXPECT_TRUE(instance.ApplyBrandColor("ohos_id_color_alert", BRAND_COLOR, color));
    EXPECT_EQ(0xFFFF0000u, color);
}

// === Brand color with non-zero alpha (brand color itself can have alpha) ===

HWTEST_F(BrandColorManagerTest, BrandColorWithAlpha, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_emphasize"});
    // Brand color with alpha=80 (semi-transparent red)
    uint32_t brandColor = 0x80FF0000;
    uint32_t color = COLOR_OHOS_ID_EMPHASIZE;  // alpha=FF, RGB=007DFF
    bool result = instance.ApplyBrandColor("ohos_id_color_emphasize", brandColor, color);
    EXPECT_TRUE(result);
    // alpha=FF from original, RGB=FF0000 from brand (brand alpha is ignored, only RGB used)
    EXPECT_EQ(0xFFFF0000u, color);
}

// === Brand color = 0x00000000 (transparent black) ===

HWTEST_F(BrandColorManagerTest, BrandColorTransparentBlack, TestSize.Level0)
{
    auto &instance = BrandColorManager::GetInstance();
    SetupList({"ohos_id_color_emphasize"});
    uint32_t brandColor = 0x00000000;
    uint32_t color = COLOR_OHOS_ID_EMPHASIZE;  // alpha=FF
    bool result = instance.ApplyBrandColor("ohos_id_color_emphasize", brandColor, color);
    EXPECT_TRUE(result);
    // alpha=FF preserved, RGB=000000 (brand RGB = 0)
    EXPECT_EQ(0xFF000000u, color);
}

}  // namespace Resource
}  // namespace Global
}  // namespace OHOS
