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
#include <cstring>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <gtest/gtest.h>

#include "hap_parser.h"
#include "rstate.h"
#include "zip64_hap_test.h"

using namespace OHOS::Global::Resource;
using namespace OHOS::Global::Resource::Zip64Test;
using namespace testing::ext;

namespace {

class Zip64HapTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
};

void Zip64HapTest::SetUpTestCase(void)
{
    EnsureZip64Dir();
}

void Zip64HapTest::TearDownTestCase(void)
{
    CleanupAllTestArchives();
}

void Zip64HapTest::SetUp(void) {}

void Zip64HapTest::TearDown(void)
{
    CleanupAllTestArchives();
}

/*
 * @tc.name: QueryResource_IndexBeyond4G_001
 * @tc.desc: Resource index located beyond the 4GB boundary can be loaded and queried.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, QueryResource_IndexBeyond4G_001, TestSize.Level1)
{
    const std::string hapPath = ZIP64_DIR + "big_resource.hap";
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

    std::string value;
    EXPECT_EQ(manager->GetStringByName("ohos_app_name", value), SUCCESS);
    EXPECT_EQ(value, "System");

    uint32_t color = 0;
    EXPECT_EQ(manager->GetColorByName("divider_color", color), SUCCESS);
    EXPECT_EQ(color, DIVIDER_COLOR_VALUE);
    CleanupTestFile(hapPath);
}

/*
 * @tc.name: QueryResource_V2IndexBeyond4G_012
 * @tc.desc: A V2 (mmap) resource index located beyond the 4GB boundary can be loaded and queried.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, QueryResource_V2IndexBeyond4G_012, TestSize.Level1)
{
    const std::string hapPath = ZIP64_DIR + "big_resource_v2.hap";
    ASSERT_TRUE(BuildBigHap(hapPath, false, "", 0, true));
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

    std::string value;
    EXPECT_EQ(manager->GetStringByName("app_name", value), SUCCESS);
    EXPECT_EQ(value, "App Name");

    uint32_t color = 0;
    EXPECT_EQ(manager->GetColorByName("color_aboutPage_title_primary", color), SUCCESS);
    EXPECT_EQ(color, ABOUT_PAGE_TITLE_COLOR);
    CleanupTestFile(hapPath);
}

/*
 * @tc.name: ReadMediaProfile_Beyond4G_002
 * @tc.desc: Media and profile entries located beyond the 4GB boundary return the original bytes.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, ReadMediaProfile_Beyond4G_002, TestSize.Level1)
{
    const std::string hapPath = ZIP64_DIR + "big_media.hap";
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

    std::vector<uint8_t> expectedMedia;
    ASSERT_TRUE(LoadTestData("all/assets/entry/resources/base/media/icon.png", expectedMedia));
    size_t mediaLen = 0;
    std::unique_ptr<uint8_t[]> mediaData;
    EXPECT_EQ(manager->GetMediaDataByName("icon", mediaLen, mediaData), SUCCESS);
    ASSERT_NE(mediaData, nullptr);
    ASSERT_EQ(mediaLen, expectedMedia.size());
    EXPECT_EQ(memcmp(mediaData.get(), expectedMedia.data(), mediaLen), 0);

    std::vector<uint8_t> expectedProfile;
    ASSERT_TRUE(LoadTestData("all/assets/entry/resources/base/profile/test_profile.json", expectedProfile));
    size_t profileLen = 0;
    std::unique_ptr<uint8_t[]> profileData;
    EXPECT_EQ(manager->GetProfileDataByName("test_profile", profileLen, profileData), SUCCESS);
    ASSERT_NE(profileData, nullptr);
    ASSERT_EQ(profileLen, expectedProfile.size());
    EXPECT_EQ(memcmp(profileData.get(), expectedProfile.data(), profileLen), 0);
    CleanupTestFile(hapPath);
}

/*
 * @tc.name: ReadRawFile_Beyond4G_003
 * @tc.desc: Rawfile stored beyond the 4GB boundary can be read back byte by byte.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, ReadRawFile_Beyond4G_003, TestSize.Level1)
{
    const std::string hapPath = ZIP64_DIR + "big_rawfile.hap";
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

    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    EXPECT_EQ(manager->GetRawFileFromHap("rawfile/test_rawfile.txt", len, data), SUCCESS);
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(len, TEST_RAWFILE_LEN);
    EXPECT_EQ(std::string(reinterpret_cast<char *>(data.get()), len), TEST_RAWFILE_CONTENT);
    CleanupTestFile(hapPath);
}

/*
 * @tc.name: RawFileDescriptor64_Beyond4G_004
 * @tc.desc: The 64-bit rawfile descriptor reports the real offset beyond 4GB and a readable fd.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, RawFileDescriptor64_Beyond4G_004, TestSize.Level1)
{
    const std::string hapPath = ZIP64_DIR + "big_descriptor.hap";
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

    ResourceManager::RawFileDescriptor descriptor = { 0 };
    ASSERT_EQ(manager->GetRawFileDescriptorFromHap("rawfile/test_rawfile.txt", descriptor), SUCCESS);
    EXPECT_GT(descriptor.offset, static_cast<int64_t>(FOUR_GB));
    EXPECT_EQ(descriptor.length, static_cast<int64_t>(TEST_RAWFILE_LEN));
    EXPECT_GE(descriptor.fd, 0);
    char buffer[RAWFILE_BUFFER_LEN] = { 0 };
    ssize_t readLen = pread(descriptor.fd, buffer, TEST_RAWFILE_LEN, descriptor.offset);
    EXPECT_EQ(readLen, static_cast<ssize_t>(TEST_RAWFILE_LEN));
    EXPECT_EQ(std::string(buffer, TEST_RAWFILE_LEN), TEST_RAWFILE_CONTENT);
    EXPECT_EQ(manager->CloseRawFileDescriptor("rawfile/test_rawfile.txt"), SUCCESS);
    CleanupTestFile(hapPath);
}

/*
 * @tc.name: RawFileDescriptor_2GTo4G_011
 * @tc.desc: A classic archive with rawfile data above INT32_MAX keeps a positive int64 descriptor.
 *           On 32-bit builds the former static_cast<long> produced a negative offset here;
 *           this case does not depend on zip64 support, so it also runs on the master baseline.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, RawFileDescriptor_2GTo4G_011, TestSize.Level1)
{
    const std::string hapPath = ZIP64_DIR + "classic_3g.hap";
    ASSERT_TRUE(BuildClassicHap(hapPath));
    auto manager = CreateTestResourceManager();
    ASSERT_NE(manager, nullptr);
    ASSERT_TRUE(manager->AddResource(hapPath.c_str()));

    ResourceManager::RawFileDescriptor descriptor;
    ASSERT_EQ(manager->GetRawFileDescriptorFromHap("rawfile/test_rawfile.txt", descriptor), SUCCESS);
    EXPECT_GT(descriptor.offset, static_cast<int64_t>(INT32_MAX));
    EXPECT_EQ(descriptor.length, static_cast<int64_t>(TEST_RAWFILE_LEN));
    EXPECT_GE(descriptor.fd, 0);
    char buffer[RAWFILE_BUFFER_LEN] = { 0 };
    ssize_t readLen = pread(descriptor.fd, buffer, TEST_RAWFILE_LEN, descriptor.offset);
    EXPECT_EQ(readLen, static_cast<ssize_t>(TEST_RAWFILE_LEN));
    EXPECT_EQ(std::string(buffer, TEST_RAWFILE_LEN), TEST_RAWFILE_CONTENT);
    EXPECT_EQ(manager->CloseRawFileDescriptor("rawfile/test_rawfile.txt"), SUCCESS);
    CleanupTestFile(hapPath);
}

/*
 * @tc.name: MalformedZip64_ReturnsPathInvalid_006
 * @tc.desc: Corrupted zip64 structures are rejected through existing error codes without crashing.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, MalformedZip64_ReturnsPathInvalid_006, TestSize.Level1)
{
    const std::string corruptHap = ZIP64_DIR + "corrupt_locator.hap";
    ASSERT_TRUE(BuildBigHap(corruptHap, true));
    auto corruptManager = CreateTestResourceManager();
    ASSERT_NE(corruptManager, nullptr);
    if (!SupportsLargeHap()) {
        // 32-bit platform: a >4GB archive must be rejected deterministically, never loaded.
        EXPECT_FALSE(corruptManager->AddResource(corruptHap.c_str()));
        CleanupTestFile(corruptHap);
        return;
    }
    EXPECT_FALSE(corruptManager->AddResource(corruptHap.c_str()));
    CleanupTestFile(corruptHap);

    const std::string badOffsetHap = ZIP64_DIR + "bad_entry_offset.hap";
    ASSERT_TRUE(BuildBigHap(badOffsetHap, false, "resources/rawfile/test_rawfile.txt",
        BASE_OFFSET + BAD_ENTRY_OFFSET_DELTA));
    auto manager = CreateTestResourceManager();
    ASSERT_NE(manager, nullptr);
    auto loadResult = LoadLargeHap(manager, badOffsetHap);
    if (loadResult == LargeHapLoadResult::UNSUPPORTED) {
        CleanupTestFile(badOffsetHap);
        GTEST_SKIP() << "ability_base without zip64 support: HAP >4GB rejected by container";
    }
    ASSERT_EQ(loadResult, LargeHapLoadResult::LOADED);

    ResourceManager::RawFileDescriptor descriptor = { 0 };
    RState state = manager->GetRawFileDescriptorFromHap("rawfile/test_rawfile.txt", descriptor);
    if (state == SUCCESS) {
        manager->CloseRawFileDescriptor("rawfile/test_rawfile.txt");
    }
    // Single-resource lookup exhausts the resource list and reports the path as invalid.
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);

    ResourceManager::RawFileDescriptor missingDescriptor;
    EXPECT_EQ(manager->GetRawFileDescriptorFromHap("rawfile/no_such_file.txt", missingDescriptor),
        ERROR_CODE_RES_PATH_INVALID);
    CleanupTestFile(badOffsetHap);
}

/*
 * @tc.name: OverlayBeyond4G_007
 * @tc.desc: An overlay HAP larger than 4GB takes effect through AddAppOverlay.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, OverlayBeyond4G_007, TestSize.Level1)
{
    const std::string baseHap = ZIP64_DIR + "overlay_base.hap";
    const std::string overlayHap = ZIP64_DIR + "overlay_beyond4g.hap";
    ASSERT_TRUE(BuildBigHap(baseHap));
    ASSERT_TRUE(BuildOverlayHap(overlayHap));

    auto manager = CreateTestResourceManager();
    ASSERT_NE(manager, nullptr);
    if (!SupportsLargeHap()) {
        // 32-bit platform: a >4GB archive must be rejected deterministically, never loaded.
        EXPECT_FALSE(manager->AddResource(baseHap.c_str()));
        CleanupTestFile(baseHap);
        CleanupTestFile(overlayHap);
        return;
    }
    auto loadResult = LoadLargeHap(manager, baseHap);
    if (loadResult == LargeHapLoadResult::UNSUPPORTED) {
        CleanupTestFile(baseHap);
        CleanupTestFile(overlayHap);
        GTEST_SKIP() << "ability_base without zip64 support: HAP >4GB rejected by container";
    }
    ASSERT_EQ(loadResult, LargeHapLoadResult::LOADED);

    std::string value;
    EXPECT_EQ(manager->GetStringByName("ohos_app_name", value), SUCCESS);
    EXPECT_EQ(value, "System");

    EXPECT_TRUE(manager->AddAppOverlay(overlayHap));
    EXPECT_EQ(manager->GetStringByName("ohos_app_name", value), SUCCESS);
    EXPECT_EQ(value, "SystemOverlay");
    CleanupTestFile(baseHap);
    CleanupTestFile(overlayHap);
}

/*
 * @tc.name: PatchRawFileBeyond4G_008
 * @tc.desc: A HQF patch HAP larger than 4GB overrides the base rawfile content.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, PatchRawFileBeyond4G_008, TestSize.Level1)
{
    const std::string baseHap = ZIP64_DIR + "patch_base.hap";
    const std::string patchHap = ZIP64_DIR + "patch_beyond4g.hap";
    ASSERT_TRUE(BuildBigHap(baseHap));
    ASSERT_TRUE(BuildPatchHap(patchHap, PATCHED_RAWFILE_CONTENT));

    auto manager = CreateTestResourceManager();
    ASSERT_NE(manager, nullptr);
    if (!SupportsLargeHap()) {
        // 32-bit platform: a >4GB archive must be rejected deterministically, never loaded.
        EXPECT_FALSE(manager->AddResource(baseHap.c_str()));
        CleanupTestFile(baseHap);
        CleanupTestFile(patchHap);
        return;
    }
    auto loadResult = LoadLargeHap(manager, baseHap);
    if (loadResult == LargeHapLoadResult::UNSUPPORTED) {
        CleanupTestFile(baseHap);
        CleanupTestFile(patchHap);
        GTEST_SKIP() << "ability_base without zip64 support: HAP >4GB rejected by container";
    }
    ASSERT_EQ(loadResult, LargeHapLoadResult::LOADED);

    size_t baseLen = 0;
    std::unique_ptr<uint8_t[]> baseData;
    ASSERT_EQ(manager->GetRawFileFromHap("rawfile/test_rawfile.txt", baseLen, baseData), SUCCESS);
    ASSERT_EQ(std::string(reinterpret_cast<char *>(baseData.get()), baseLen), TEST_RAWFILE_CONTENT);

    ASSERT_TRUE(manager->AddPatchResource(baseHap.c_str(), patchHap.c_str()));
    size_t patchedLen = 0;
    std::unique_ptr<uint8_t[]> patchedData;
    EXPECT_EQ(manager->GetRawFileFromHap("rawfile/test_rawfile.txt", patchedLen, patchedData), SUCCESS);
    ASSERT_NE(patchedData, nullptr);
    EXPECT_EQ(std::string(reinterpret_cast<char *>(patchedData.get()), patchedLen), PATCHED_RAWFILE_CONTENT);
    CleanupTestFile(baseHap);
    CleanupTestFile(patchHap);
}

/*
 * @tc.name: MalformedOverlayPatch_009
 * @tc.desc: A corrupted overlay HAP fails to mount and leaves the base resources untouched.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, MalformedOverlayPatch_009, TestSize.Level1)
{
    const std::string baseHap = ZIP64_DIR + "malformed_base.hap";
    const std::string badOverlay = ZIP64_DIR + "bad_overlay.hap";
    const std::string badPatch = ZIP64_DIR + "bad_patch.hap";
    const std::string indexOnlyPatch = ZIP64_DIR + "index_only_patch.hap";
    ASSERT_TRUE(BuildBigHap(baseHap));
    ASSERT_TRUE(WriteGarbageFile(badOverlay));
    ASSERT_TRUE(WriteGarbageFile(badPatch));
    ASSERT_TRUE(BuildIndexOnlyHap(indexOnlyPatch));

    auto manager = CreateTestResourceManager();
    ASSERT_NE(manager, nullptr);
    if (!SupportsLargeHap()) {
        // 32-bit platform: a >4GB archive must be rejected deterministically, never loaded.
        EXPECT_FALSE(manager->AddResource(baseHap.c_str()));
        CleanupTestFile(baseHap);
        CleanupTestFile(badOverlay);
        CleanupTestFile(badPatch);
        CleanupTestFile(indexOnlyPatch);
        return;
    }
    auto loadResult = LoadLargeHap(manager, baseHap);
    if (loadResult == LargeHapLoadResult::UNSUPPORTED) {
        CleanupTestFile(baseHap);
        CleanupTestFile(badOverlay);
        CleanupTestFile(badPatch);
        CleanupTestFile(indexOnlyPatch);
        GTEST_SKIP() << "ability_base without zip64 support: HAP >4GB rejected by container";
    }
    ASSERT_EQ(loadResult, LargeHapLoadResult::LOADED);

    EXPECT_FALSE(manager->AddAppOverlay(badOverlay));
    std::string value;
    EXPECT_EQ(manager->GetStringByName("ohos_app_name", value), SUCCESS);
    EXPECT_EQ(value, "System");

    // AddPatchResource only records the patch path; a corrupted patch must not crash the read path.
    EXPECT_TRUE(manager->AddPatchResource(baseHap.c_str(), badPatch.c_str()));
    size_t badLen = 0;
    std::unique_ptr<uint8_t[]> badData;
    RState badState = manager->GetRawFileFromHap("rawfile/test_rawfile.txt", badLen, badData);
    // Single-resource lookup exhausts the resource list and reports the path as invalid.
    EXPECT_EQ(badState, ERROR_CODE_RES_PATH_INVALID);

    // A valid patch without the target entry falls back to the base HAP content.
    EXPECT_TRUE(manager->AddPatchResource(baseHap.c_str(), indexOnlyPatch.c_str()));
    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    EXPECT_EQ(manager->GetRawFileFromHap("rawfile/test_rawfile.txt", len, data), SUCCESS);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(std::string(reinterpret_cast<char *>(data.get()), len), TEST_RAWFILE_CONTENT);
    CleanupTestFile(baseHap);
    CleanupTestFile(badOverlay);
    CleanupTestFile(badPatch);
    CleanupTestFile(indexOnlyPatch);
}

/*
 * @tc.name: IsValidDescriptor_Boundary_010
 * @tc.desc: The offset/length guard accepts pairs whose sum does not exceed INT64_MAX.
 * @tc.type: FUNC
 */
HWTEST_F(Zip64HapTest, IsValidDescriptor_Boundary_010, TestSize.Level1)
{
    const uint64_t maxInt64 = static_cast<uint64_t>(INT64_MAX);
    EXPECT_TRUE(HapParser::IsValidDescriptor(0, 0));
    EXPECT_TRUE(HapParser::IsValidDescriptor(maxInt64 - 1, 1));
    EXPECT_TRUE(HapParser::IsValidDescriptor(0, maxInt64));
    EXPECT_FALSE(HapParser::IsValidDescriptor(maxInt64, 1));
    EXPECT_FALSE(HapParser::IsValidDescriptor(maxInt64, maxInt64));
    EXPECT_FALSE(HapParser::IsValidDescriptor(maxInt64 + 1, 0));
    EXPECT_FALSE(HapParser::IsValidDescriptor(0, maxInt64 + 1));
    EXPECT_FALSE(HapParser::IsValidDescriptor(UINT64_MAX, UINT64_MAX));
    // Bundle manager caps packages above 4GB at 200GB (spec BR-8); the supported maximum must fit.
    EXPECT_TRUE(HapParser::IsValidDescriptor(0, MAX_SUPPORTED_HAP_SIZE));
}

} // namespace
