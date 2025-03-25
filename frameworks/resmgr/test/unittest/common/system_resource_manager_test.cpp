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
}

void SystemResourceManagerTest::TearDown()
{}

TEST_F(SystemResourceManagerTest, CreateSystemResourceTest)
{
    {
        std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
        EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 1);
    }
    EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 0);
}

TEST_F(SystemResourceManagerTest, CreateSystemResourceTest002)
{
    std::shared_ptr<SystemResourceManager> sysResMgr = std::make_shared<SystemResourceManager>();
    EXPECT_TRUE(sysResMgr != nullptr);
}

TEST_F(SystemResourceManagerTest, GetSystemResourceManagerNoSandBoxTest)
{
    {
        SystemResourceManager::GetSystemResourceManagerNoSandBox();
        EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 1);
    }
    EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 1);
    SystemResourceManager::ReleaseSystemResourceManager();
    EXPECT_EQ(SystemResourceManager::weakResourceManager_.use_count(), 0);
}

TEST_F(SystemResourceManagerTest, DoubleCreateSystemResourceTest)
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

TEST_F(SystemResourceManagerTest, SystemResourceCreateBeforeGet)
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

TEST_F(SystemResourceManagerTest, SystemResourceCreateAfterGet)
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
    EXPECT_EQ(weakResource.use_count(), 0);
}

TEST_F(SystemResourceManagerTest, AddSystemResourceTest)
{
    ResourceManagerImpl *appResMgr = nullptr;
    bool result = SystemResourceManager::AddSystemResource(appResMgr);
    EXPECT_FALSE(result);
}
}  // namespace