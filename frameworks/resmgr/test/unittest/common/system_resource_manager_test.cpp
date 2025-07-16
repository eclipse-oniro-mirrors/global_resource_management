/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include <memory>
#include "gtest/gtest.h"
#include "hap_resource.h"
#include "resource_manager_test_common.h"
#include "system_resource_manager.h"

using namespace testing::ext;
using namespace OHOS::Global::Resource;

namespace {
class SystemResourceManagerTest : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    static constexpr int refCountTwo = 2;
    static constexpr int refCountFour = 4;
};

void SystemResourceManagerTest::SetUp()
{
    SystemResourceManager::ReleaseSystemResourceManager();
    ReleaseSysResourceManager();
}

void SystemResourceManagerTest::TearDown()
{}

HWTEST_F(SystemResourceManagerTest, CreateSystemResourceTest, TestSize.Level1)
{
    {
        std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
        EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 1);
    }
    EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 0);
}

HWTEST_F(SystemResourceManagerTest, CreateSystemResourceTest002, TestSize.Level1)
{
    std::shared_ptr<SystemResourceManager> sysResMgr = std::make_shared<SystemResourceManager>();
    EXPECT_TRUE(sysResMgr != nullptr);
}

HWTEST_F(SystemResourceManagerTest, GetSystemResourceManagerNoSandBoxTest, TestSize.Level1)
{
    {
        SystemResourceManager::GetSystemResourceManagerNoSandBox();
        EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 1);
    }
    EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 1);
    SystemResourceManager::ReleaseSystemResourceManager();
    EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 0);
}

HWTEST_F(SystemResourceManagerTest, DoubleCreateSystemResourceTest, TestSize.Level1)
{
    std::weak_ptr<HapResource> weakResource;
    {
        std::shared_ptr<ResourceManager> resMgr1(CreateResourceManager());
        std::shared_ptr<ResourceManager> resMgr2(CreateResourceManager());
        EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), refCountTwo);
        auto sysResMgr = SystemResourceManager::weakResourceManager_.lock();
        ASSERT_TRUE(sysResMgr != nullptr);
        auto hapResources = sysResMgr->hapManager_->GetHapResource();
        ASSERT_GT(hapResources.size(), 0);
        weakResource = hapResources[0];
        EXPECT_EQ(weakResource.use_count(), refCountFour);
    }
    EXPECT_EQ(weakResource.use_count(), 0);
}

HWTEST_F(SystemResourceManagerTest, SystemResourceCreateBeforeGet, TestSize.Level1)
{
    std::weak_ptr<HapResource> weakResource;
    {
        std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
        SystemResourceManager::GetSystemResourceManagerNoSandBox();
        EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), refCountTwo);
        auto sysResMgr = SystemResourceManager::weakResourceManager_.lock();
        ASSERT_TRUE(sysResMgr != nullptr);
        auto hapResources = sysResMgr->hapManager_->GetHapResource();
        ASSERT_GT(hapResources.size(), 0);
        weakResource = hapResources[0];
        EXPECT_EQ(weakResource.use_count(), refCountFour);
    }
    EXPECT_EQ(weakResource.use_count(), refCountTwo);
    SystemResourceManager::ReleaseSystemResourceManager();
    EXPECT_EQ(weakResource.use_count(), 0);
}

HWTEST_F(SystemResourceManagerTest, SystemResourceCreateAfterGet, TestSize.Level1)
{
    std::weak_ptr<HapResource> weakResource;
    {
        SystemResourceManager::GetSystemResourceManagerNoSandBox();
        std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
        EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), refCountTwo);
        auto sysResMgr = SystemResourceManager::weakResourceManager_.lock();
        ASSERT_TRUE(sysResMgr != nullptr);
        auto hapResources = sysResMgr->hapManager_->GetHapResource();
        ASSERT_GT(hapResources.size(), 0);
        weakResource = hapResources[0];
        EXPECT_EQ(weakResource.use_count(), refCountFour);
    }
    EXPECT_EQ(weakResource.use_count(), refCountTwo);
    SystemResourceManager::ReleaseSystemResourceManager();
    ReleaseSysResourceManager();
    EXPECT_EQ(weakResource.use_count(), 0);
}

HWTEST_F(SystemResourceManagerTest, AddSystemResourceTest, TestSize.Level1)
{
    ResourceManagerImpl *appResMgr = nullptr;
    bool result = SystemResourceManager::AddSystemResource(appResMgr);
    EXPECT_FALSE(result);
}

HWTEST_F(SystemResourceManagerTest, CreateSysResourceManagerTest001, TestSize.Level1)
{
    std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
    ASSERT_TRUE(resMgr != nullptr);
    auto sysResMgr = SystemResourceManager::CreateSysResourceManager();
    ASSERT_TRUE(sysResMgr != nullptr);
    bool ret = resMgr->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    rc.SetColorMode(ColorMode::DARK);
    resMgr->UpdateResConfig(rc);
    std::string rmOutValue;
    resMgr->GetStringByName("ohos_desc_write_calendar", rmOutValue);
    std::string sysOutValue;
    sysResMgr->GetStringByName("ohos_desc_write_calendar", sysOutValue);
    EXPECT_EQ(rmOutValue, sysOutValue);
}

HWTEST_F(SystemResourceManagerTest, CreateSysResourceManagerTest002, TestSize.Level1)
{
    std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
    ASSERT_TRUE(resMgr != nullptr);
    auto sysResMgr = SystemResourceManager::CreateSysResourceManager();
    ASSERT_TRUE(sysResMgr != nullptr);
    bool ret = resMgr->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ResConfigImpl rc;
    rc.SetLocaleInfo("en", nullptr, "US"); 
    rc.SetColorMode(ColorMode::DARK);
    resMgr->UpdateResConfig(rc);
    uint32_t rmOutValue;
    resMgr->GetColorByName("ohos_id_color_foregroud", rmOutValue);
    uint32_t sysOutValue;
    sysResMgr->GetColorByName("ohos_id_color_foregroud", sysOutValue);
    EXPECT_EQ(rmOutValue, sysOutValue);
}

HWTEST_F(SystemResourceManagerTest, CreateSysResourceManagerTest003, TestSize.Level1)
{
    std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
    ASSERT_TRUE(resMgr != nullptr);
    auto sysResMgr = SystemResourceManager::CreateSysResourceManager();
    ASSERT_TRUE(sysResMgr != nullptr);
    bool ret = resMgr->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ResConfigImpl rc;
    rc.SetLocaleInfo("en", nullptr, "US");
    rc.SetScreenDensity(2.00);
    rc.SetDeviceType(DeviceType::DEVICE_WEARABLE);
    resMgr->UpdateResConfig(rc);
    uint32_t rmOutValue;
    resMgr->GetColorByName("ohos_id_color_text_primary_contrary", rmOutValue);
    uint32_t sysOutValue;
    sysResMgr->GetColorByName("ohos_id_color_text_primary_contrary", sysOutValue);
    EXPECT_EQ(rmOutValue, sysOutValue);
}

HWTEST_F(SystemResourceManagerTest, CreateSysResourceManagerTest004, TestSize.Level1)
{
    std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
    ASSERT_TRUE(resMgr != nullptr);
    auto sysResMgr = SystemResourceManager::CreateSysResourceManager();
    ASSERT_TRUE(sysResMgr != nullptr);
    bool ret = resMgr->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ResConfigImpl rc;
    rc.SetLocaleInfo("en", nullptr, "US");
    rc.SetScreenDensity(3.25);
    resMgr->UpdateResConfig(rc);
    float rmOutValue;
    resMgr->GetFloatByName("ohos_fa_corner_radius_card", rmOutValue);
    float sysOutValue;
    sysResMgr->GetFloatByName("ohos_fa_corner_radius_card", sysOutValue);
    EXPECT_EQ(rmOutValue, sysOutValue);
}

HWTEST_F(SystemResourceManagerTest, CreateSysResourceManagerTest005, TestSize.Level1)
{
    std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
    ASSERT_TRUE(resMgr != nullptr);
    auto sysResMgr = SystemResourceManager::CreateSysResourceManager();
    ASSERT_TRUE(sysResMgr != nullptr);
    bool ret = resMgr->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ResConfigImpl rc;
    rc.SetLocaleInfo("en", nullptr, "US");
    rc.SetDirection(Direction::DIRECTION_VERTICAL);
    resMgr->UpdateResConfig(rc);
    uint32_t rmOutValue;
    resMgr->GetSymbolByName("ohos_wifi", rmOutValue);
    uint32_t sysOutValue;
    sysResMgr->GetSymbolByName("ohos_wifi", sysOutValue);
    EXPECT_EQ(rmOutValue, sysOutValue);
}

HWTEST_F(SystemResourceManagerTest, CreateSysResourceManagerTest006, TestSize.Level1)
{
    std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
    ASSERT_TRUE(resMgr != nullptr);
    auto sysResMgr = SystemResourceManager::CreateSysResourceManager();
    ASSERT_TRUE(sysResMgr != nullptr);
    bool ret = resMgr->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    ResConfigImpl rc;
    rc.SetLocaleInfo("en", nullptr, "US");
    rc.SetMcc(460);
    rc.SetMnc(001);
    resMgr->UpdateResConfig(rc);
    int rmOutValue;
    resMgr->GetIntegerByName("ohos_id_alpha_content_primary", rmOutValue);
    int sysOutValue;
    sysResMgr->GetIntegerByName("ohos_id_alpha_content_primary", sysOutValue);
    EXPECT_EQ(rmOutValue, sysOutValue);
}
}  // namespace