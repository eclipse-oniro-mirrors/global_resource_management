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

#include <cerrno>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "resource_manager_test_common.h"
#include "theme_pack_manager.h"
#include "theme_skin_ref.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

namespace {
constexpr int32_t DEFAULT_USER_ID = 100;

class ThemeBundleSkinTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDown()
    {
        auto mgr = ThemePackManager::GetThemePackManager();
        if (mgr != nullptr) {
            std::unique_lock<std::shared_mutex> lock(mgr->lockSkin_);
            mgr->skinResource_.clear();
            mgr->systemSkinResource_.clear();
        }
    }
    ThemeBundleSkinTest() {}
    ~ThemeBundleSkinTest() {}

    using SkinContainer = std::shared_ptr<ThemeBundleResource>;

    static SkinContainer Acquire(const std::shared_ptr<ThemePackManager> &mgr, const std::string &bundle,
        int32_t userId = DEFAULT_USER_ID)
    {
        return mgr->LoadThemeBundleSkinResource(bundle, userId);
    }

    static bool HasLiveHolder(const std::shared_ptr<ThemePackManager> &mgr, const std::string &bundle,
        int32_t userId = DEFAULT_USER_ID)
    {
        std::shared_lock<std::shared_mutex> lock(mgr->lockSkin_);
        auto userIt = mgr->skinResource_.find(userId);
        if (userIt == mgr->skinResource_.end()) {
            return false;
        }
        auto it = userIt->second.find(bundle);
        return it != userIt->second.end() && !it->second.expired();
    }

    static bool HasEntry(const std::shared_ptr<ThemePackManager> &mgr, const std::string &bundle,
        int32_t userId = DEFAULT_USER_ID)
    {
        std::shared_lock<std::shared_mutex> lock(mgr->lockSkin_);
        auto userIt = mgr->skinResource_.find(userId);
        return userIt != mgr->skinResource_.end() && userIt->second.find(bundle) != userIt->second.end();
    }
};

/*
 * @tc.name: Container_AutoReleaseOnLastHolder_001
 * @tc.desc: The container is released when its last holder is destroyed.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, Container_AutoReleaseOnLastHolder_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "com.test.autoRelease";
    {
        auto holder = Acquire(mgr, bundle);
        EXPECT_NE(holder, nullptr);
        EXPECT_TRUE(HasLiveHolder(mgr, bundle));
    }
    EXPECT_FALSE(HasLiveHolder(mgr, bundle));
}

/*
 * @tc.name: Container_SharedByMultipleHolders_001
 * @tc.desc: Multiple acquires share one container, released only after all holders are gone.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, Container_SharedByMultipleHolders_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "com.test.multiHolder";
    auto holder1 = Acquire(mgr, bundle);
    auto holder2 = Acquire(mgr, bundle);
    EXPECT_NE(holder1, nullptr);
    EXPECT_EQ(holder1.get(), holder2.get());

    holder1.reset();
    EXPECT_TRUE(HasLiveHolder(mgr, bundle));
    holder2.reset();
    EXPECT_FALSE(HasLiveHolder(mgr, bundle));
}

/*
 * @tc.name: ThemeSwitch_UpdatesAliveContainer_001
 * @tc.desc: Theme switch refreshes the resources of an alive container in place.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, ThemeSwitch_UpdatesAliveContainer_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "ohos.global.test.all";
    auto holder = Acquire(mgr, bundle);
    ASSERT_NE(holder, nullptr);

    std::vector<std::string> rootDirs;
    rootDirs.emplace_back("/data/test/theme/skin/" + bundle);
    mgr->LoadThemeSkinResource(bundle, "entry", rootDirs, DEFAULT_USER_ID);
    EXPECT_TRUE(HasLiveHolder(mgr, bundle));
    EXPECT_TRUE(holder->resource != nullptr);

    // The holder still references the same container after the switch.
    auto holderAfterSwitch = Acquire(mgr, bundle);
    EXPECT_EQ(holder.get(), holderAfterSwitch.get());
}

/*
 * @tc.name: ThemeSwitch_ClearsAliveContainer_001
 * @tc.desc: Theme switch with empty rootDirs clears the alive container but keeps it alive.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, ThemeSwitch_ClearsAliveContainer_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "ohos.global.test.all";
    auto holder = Acquire(mgr, bundle);
    ASSERT_NE(holder, nullptr);
    std::vector<std::string> rootDirs;
    rootDirs.emplace_back("/data/test/theme/skin/" + bundle);
    mgr->LoadThemeSkinResource(bundle, "entry", rootDirs, DEFAULT_USER_ID);
    ASSERT_TRUE(holder->resource != nullptr);

    std::vector<std::string> emptyDirs;
    mgr->LoadThemeSkinResource(bundle, "entry", emptyDirs, DEFAULT_USER_ID);
    EXPECT_TRUE(HasLiveHolder(mgr, bundle));
    EXPECT_TRUE(holder->resource == nullptr);
}

/*
 * @tc.name: ThemeSwitch_PrunesDeadEntry_001
 * @tc.desc: Theme switch removes the cache entry when no holder is left.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, ThemeSwitch_PrunesDeadEntry_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "com.test.pruned";
    {
        auto holder = Acquire(mgr, bundle);
        EXPECT_NE(holder, nullptr);
    }
    EXPECT_TRUE(HasEntry(mgr, bundle));
    EXPECT_FALSE(HasLiveHolder(mgr, bundle));

    std::vector<std::string> emptyDirs;
    mgr->LoadThemeSkinResource(bundle, "entry", emptyDirs, DEFAULT_USER_ID);
    EXPECT_FALSE(HasEntry(mgr, bundle));
}

/*
 * @tc.name: QueryAfterLastHolderReleased_ReturnsEmpty_001
 * @tc.desc: Query returns empty after the last holder of the bundle is destroyed.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, QueryAfterLastHolderReleased_ReturnsEmpty_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "com.test.queryAfterRelease";
    {
        auto holder = Acquire(mgr, bundle);
    }
    EXPECT_FALSE(HasLiveHolder(mgr, bundle));
    std::pair<std::string, std::string> bundInfo(bundle, "entry");
    auto result = mgr->GetThemeResourceList(bundInfo, ResType::COLOR, "base_only", DEFAULT_USER_ID);
    EXPECT_TRUE(result.empty());
}

/*
 * @tc.name: ThemeSkinRef_KeepsContainerAlive_001
 * @tc.desc: ThemeSkinRef keeps the container alive and removes the weak index entry after the last holder.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, ThemeSkinRef_KeepsContainerAlive_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "com.test.skinRef";
    {
        ThemeSkinRef skinRef;
        skinRef.Bind(DEFAULT_USER_ID, bundle);
        skinRef.Load();
        EXPECT_TRUE(HasLiveHolder(mgr, bundle));
    }
    EXPECT_FALSE(HasLiveHolder(mgr, bundle));
    EXPECT_FALSE(HasEntry(mgr, bundle));

    // The entry is kept while another handle still shares the same container.
    {
        ThemeSkinRef first;
        first.Bind(DEFAULT_USER_ID, bundle);
        first.Load();
        {
            ThemeSkinRef second;
            second.Bind(DEFAULT_USER_ID, bundle);
            second.Load();
            EXPECT_TRUE(HasEntry(mgr, bundle));
        }
        EXPECT_TRUE(HasEntry(mgr, bundle));
    }
    EXPECT_FALSE(HasEntry(mgr, bundle));
}

/*
 * @tc.name: MultiBundle_Isolation_001
 * @tc.desc: Multi-bundle containers are independent, releasing one does not affect the other.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, MultiBundle_Isolation_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundleA = "com.test.coexistA";
    const std::string bundleB = "com.test.coexistB";
    auto holderA = Acquire(mgr, bundleA);
    auto holderB = Acquire(mgr, bundleB);
    ASSERT_NE(holderA, nullptr);
    ASSERT_NE(holderB, nullptr);
    EXPECT_NE(holderA.get(), holderB.get());

    holderA.reset();
    EXPECT_FALSE(HasLiveHolder(mgr, bundleA));
    EXPECT_TRUE(HasLiveHolder(mgr, bundleB));
}

/*
 * @tc.name: FirstLoad_KeepsLoadedResource_001
 * @tc.desc: The first load without an existing holder keeps the loaded ThemeResource in the returned container.
 * @tc.type: FUNC
 */
HWTEST_F(ThemeBundleSkinTest, FirstLoad_KeepsLoadedResource_001, TestSize.Level1)
{
    auto mgr = ThemePackManager::GetThemePackManager();
    ASSERT_NE(mgr, nullptr);
    const std::string bundle = "com.test.firstLoad";
    const std::string rootDir = "/data/test/theme/skin/" + bundle;
    errno = 0;
    int ret = mkdir("/data/test/theme", 0755);
    ASSERT_TRUE(ret == 0 || errno == EEXIST);
    errno = 0;
    ret = mkdir("/data/test/theme/skin", 0755);
    ASSERT_TRUE(ret == 0 || errno == EEXIST);
    errno = 0;
    ret = mkdir(rootDir.c_str(), 0755);
    ASSERT_TRUE(ret == 0 || errno == EEXIST);

    std::vector<std::string> rootDirs;
    rootDirs.emplace_back(rootDir);
    auto holder = mgr->LoadThemeSkinResource(bundle, "entry", rootDirs, DEFAULT_USER_ID);
    ASSERT_NE(holder, nullptr);
    // Regression: without a strong reference across LoadSkinResourcesLocked, the assigned
    // ThemeResource was destroyed and this returned container was recreated empty.
    EXPECT_NE(holder->resource, nullptr);

    auto acquired = mgr->LoadThemeBundleSkinResource(bundle, DEFAULT_USER_ID);
    EXPECT_EQ(holder.get(), acquired.get());
    EXPECT_NE(acquired->resource, nullptr);
    EXPECT_EQ(rmdir(rootDir.c_str()), 0);

    // The lazy path (AppendThemeSkinResource) must keep the container alive as well.
    const std::string lazyBundle = "com.test.firstLazyLoad";
    const std::string lazyDir = "/data/test/theme/skin/" + lazyBundle;
    errno = 0;
    ret = mkdir(lazyDir.c_str(), 0755);
    ASSERT_TRUE(ret == 0 || errno == EEXIST);
    std::vector<std::string> lazyDirs;
    lazyDirs.emplace_back(lazyDir);
    std::shared_ptr<ThemeBundleResource> lazyHolder;
    {
        std::unique_lock<std::shared_mutex> lock(mgr->lockSkin_);
        lazyHolder = mgr->AppendThemeSkinResource(lazyBundle, lazyDirs, DEFAULT_USER_ID, "");
    }
    ASSERT_NE(lazyHolder, nullptr);
    EXPECT_NE(lazyHolder->resource, nullptr);
    EXPECT_EQ(rmdir(lazyDir.c_str()), 0);
}
}  // namespace
