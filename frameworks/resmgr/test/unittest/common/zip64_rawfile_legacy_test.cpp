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

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "raw_file.h"
#include "raw_file_manager.h"
#include "rstate.h"
#include "zip64_hap_test.h"

using namespace OHOS::Global::Resource;
using namespace OHOS::Global::Resource::Zip64Test;
using namespace testing::ext;

struct NativeResourceManager {
    std::shared_ptr<OHOS::Global::Resource::ResourceManager> resManager = nullptr;
};

namespace {

class Zip64RawFileLegacyTest : public testing::Test {
public:
    void TearDown() override
    {
        CleanupAllTestArchives();
    }
};

/*
 * @tc.name: LegacyDescriptor_64BitPlatform_005
 * @tc.desc: On LP64 platforms the legacy C descriptor reports the same values as the 64-bit one.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64RawFileLegacyTest, LegacyDescriptor_64BitPlatform_005, TestSize.Level1)
{
    const std::string hapPath = ZIP64_DIR + "big_legacy.hap";
    ASSERT_TRUE(BuildBigHap(hapPath));
    auto manager = CreateTestResourceManager();
    ASSERT_NE(manager, nullptr);
    if (!SupportsLargeHap()) {
        // 32-bit platform: a >4GB archive must be rejected deterministically, never loaded.
        EXPECT_FALSE(manager->AddResource(hapPath.c_str()));
        CleanupTestFile(hapPath);
        return;
    }
    auto loadResult = LoadLargeHap(manager, hapPath);
    if (loadResult == LargeHapLoadResult::UNSUPPORTED) {
        CleanupTestFile(hapPath);
        GTEST_SKIP() << "ability_base without zip64 support: HAP >4GB rejected by container";
    }
    ASSERT_EQ(loadResult, LargeHapLoadResult::LOADED);

    NativeResourceManager nativeManager;
    nativeManager.resManager = manager;

    RawFile64 *rawFile64 = OH_ResourceManager_OpenRawFile64(&nativeManager, "rawfile/test_rawfile.txt");
    ASSERT_NE(rawFile64, nullptr);
    RawFileDescriptor64 descriptor64 = { 0 };
    if (!OH_ResourceManager_GetRawFileDescriptor64(rawFile64, &descriptor64)) {
        OH_ResourceManager_CloseRawFile64(rawFile64);
        CleanupTestFile(hapPath);
        FAIL() << "GetRawFileDescriptor64 failed";
    }
    EXPECT_GT(descriptor64.start, static_cast<int64_t>(FOUR_GB));
    EXPECT_EQ(descriptor64.length, static_cast<int64_t>(TEST_RAWFILE_LEN));
    EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptor64(&descriptor64));

    if (sizeof(long) == sizeof(int64_t)) {
        RawFile *rawFile = OH_ResourceManager_OpenRawFile(&nativeManager, "rawfile/test_rawfile.txt");
        ASSERT_NE(rawFile, nullptr);
        RawFileDescriptor descriptor = { 0 };
        if (!OH_ResourceManager_GetRawFileDescriptor(rawFile, descriptor)) {
            OH_ResourceManager_CloseRawFile(rawFile);
            OH_ResourceManager_CloseRawFile64(rawFile64);
            CleanupTestFile(hapPath);
            FAIL() << "GetRawFileDescriptor failed";
        }
        EXPECT_EQ(static_cast<int64_t>(descriptor.start), descriptor64.start);
        EXPECT_EQ(static_cast<int64_t>(descriptor.length), descriptor64.length);
        EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptor(descriptor));
        OH_ResourceManager_CloseRawFile(rawFile);
    }

    OH_ResourceManager_CloseRawFile64(rawFile64);
    CleanupTestFile(hapPath);
}
} // namespace
