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

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gtest/gtest.h>

#include "resource_manager.h"
#include "resource_manager_test_common.h"
#include "system_resource_manager.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class RawFilePathTest : public testing::Test {
public:
    void SetUp() override
    {
        rm = CreateResourceManager();
        ASSERT_NE(rm, nullptr);
    }

    void TearDown() override
    {
        delete rm;
        rm = nullptr;
    }

    ResourceManager *rm = nullptr;
};

/*
 * @tc.name: RawFileTraversalTest001
 * @tc.desc: Test GetRawFilePathByName with traversal/absolute paths (path validation reverted,
 *           realpath resolves the path; success depends on whether the resolved file exists).
 * @tc.type: FUNC
 */
HWTEST_F(RawFilePathTest, RawFileTraversalTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());

    std::string outValue;
    outValue.clear();
    RState state = rm->GetRawFilePathByName("../base/media/icon.png", outValue);
    EXPECT_EQ(state, SUCCESS);

    outValue.clear();
    state = rm->GetRawFilePathByName("../../base/media/icon.png", outValue);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
    EXPECT_TRUE(outValue.empty());

    outValue.clear();
    state = rm->GetRawFilePathByName("subdir/../test_rawfile.txt", outValue);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);

    outValue.clear();
    state = rm->GetRawFilePathByName("test_rawfile.txt", outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_EQ(outValue, "/data/test/all/assets/entry/resources/rawfile/test_rawfile.txt");
}

/*
 * @tc.name: RawFileTraversalListTest001
 * @tc.desc: Test GetRawFileList/IsRawDirFromHap with traversal/absolute dir paths
 *           (path validation reverted; traversal paths resolve via realpath).
 * @tc.type: FUNC
 */
HWTEST_F(RawFilePathTest, RawFileTraversalListTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());

    std::vector<std::string> rawfileList;
    RState state = rm->GetRawFileList("../", rawfileList);
    EXPECT_EQ(state, SUCCESS);

    rawfileList.clear();
    state = rm->GetRawFileList("/etc", rawfileList);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);

    bool isDir = false;
    state = rm->IsRawDirFromHap("../", isDir);
    EXPECT_EQ(state, SUCCESS);

    state = rm->IsRawDirFromHap("/etc", isDir);
    EXPECT_EQ(state, SUCCESS);

    rawfileList.clear();
    state = rm->GetRawFileList("", rawfileList);
    EXPECT_EQ(state, SUCCESS);
}

/*
 * @tc.name: RawFileSymlinkTraversalTest001
 * @tc.desc: Test FindRawFile follows symlinks (O_NOFOLLOW reverted; symlinks are followed).
 * @tc.type: FUNC
 */
HWTEST_F(RawFilePathTest, RawFileSymlinkTraversalTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    const std::string rawfileDir = "/data/test/all/assets/entry/resources/rawfile";

    // 1. symlink in rawfile pointing outside rawfile dir -> followed (no O_NOFOLLOW)
    std::string escapeLink = rawfileDir + "/symlink_escape";
    ASSERT_EQ(symlink("/etc", escapeLink.c_str()), 0);
    std::string outValue;
    RState state = rm->GetRawFilePathByName("symlink_escape", outValue);
    EXPECT_EQ(state, SUCCESS);
    unlink(escapeLink.c_str());

    // 2. symlink in rawfile pointing to a file inside rawfile -> followed
    std::string internalLink = rawfileDir + "/symlink_internal";
    ASSERT_EQ(symlink((rawfileDir + "/test_rawfile.txt").c_str(), internalLink.c_str()), 0);
    outValue.clear();
    state = rm->GetRawFilePathByName("symlink_internal", outValue);
    EXPECT_EQ(state, SUCCESS);
    unlink(internalLink.c_str());

    // 3. regular file still works (regression)
    outValue.clear();
    state = rm->GetRawFilePathByName("test_rawfile.txt", outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_EQ(outValue, rawfileDir + "/test_rawfile.txt");
}

/*
 * @tc.name: RawFileInterfaceCoverage001
 * @tc.desc: Cover all rawfile interfaces. Traversal path "../" resolves via realpath
 *           (path validation reverted); valid path succeeds.
 * @tc.type: FUNC
 */
HWTEST_F(RawFilePathTest, RawFileInterfaceCoverage001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    const std::string validName = "test_rawfile.txt";

    // 1. GetRawFilePathByName
    std::string outValue;
    EXPECT_EQ(rm->GetRawFilePathByName("../base/media/icon.png", outValue), SUCCESS);
    EXPECT_EQ(rm->GetRawFilePathByName(validName, outValue), SUCCESS);

    // 2. GetRawFileDescriptor
    ResourceManager::RawFileDescriptor descriptor{};
    EXPECT_EQ(rm->GetRawFileDescriptor("../no_such_file", descriptor), ERROR_CODE_RES_PATH_INVALID);
    EXPECT_EQ(rm->GetRawFileDescriptor(validName, descriptor), SUCCESS);
    EXPECT_GT(descriptor.fd, 0);
    EXPECT_EQ(rm->CloseRawFileDescriptor(validName), SUCCESS);

    // 3. GetRawFileFromHap
    size_t len = 0;
    std::unique_ptr<uint8_t[]> dataBuf;
    EXPECT_EQ(rm->GetRawFileFromHap("../no_such_file", len, dataBuf), ERROR_CODE_RES_PATH_INVALID);
    EXPECT_EQ(rm->GetRawFileFromHap(validName, len, dataBuf), SUCCESS);
    EXPECT_NE(dataBuf, nullptr);

    // 4. GetRawFileDescriptorFromHap
    ResourceManager::RawFileDescriptor desc2{};
    EXPECT_EQ(rm->GetRawFileDescriptorFromHap("../no_such_file", desc2), ERROR_CODE_RES_PATH_INVALID);
    EXPECT_EQ(rm->GetRawFileDescriptorFromHap(validName, desc2), SUCCESS);
    EXPECT_GT(desc2.fd, 0);
    EXPECT_EQ(rm->CloseRawFileDescriptor(validName), SUCCESS);

    // 5. GetRawFileList
    std::vector<std::string> rawfileList;
    EXPECT_EQ(rm->GetRawFileList("../", rawfileList), SUCCESS);
    EXPECT_EQ(rm->GetRawFileList("", rawfileList), SUCCESS);

    // 6. IsRawDirFromHap
    bool isDir = false;
    EXPECT_EQ(rm->IsRawDirFromHap("../", isDir), SUCCESS);
}

/*
 * @tc.name: RawFileRelativeWithinRawfile001
 * @tc.desc: A relative path containing ".." that resolves BACK INTO the rawfile directory must be
 *           ALLOWED (realpath resolves the path; path validation reverted).
 * @tc.type: FUNC
 */
HWTEST_F(RawFilePathTest, RawFileRelativeWithinRawfile001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    const std::string rawfileDir = "/data/test/all/assets/entry/resources/rawfile";
    const std::string validName = "test_rawfile.txt";

    // Case 1: "./../rawfile/test_rawfile.txt" — contains ".." but resolves back into rawfile.
    std::string outValue;
    RState state = rm->GetRawFilePathByName("./../rawfile/test_rawfile.txt", outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_EQ(outValue, rawfileDir + "/" + validName);

    // Case 2: "subdir/../test_rawfile.txt" — subdir exists (created below), ".." resolves back.
    std::string subdir = rawfileDir + "/subdir_for_test";
    mode_t prevMask = umask(0);
    int mkdirRet = mkdir(subdir.c_str(), 0777);
    umask(prevMask);
    ASSERT_EQ(mkdirRet, 0);
    outValue.clear();
    state = rm->GetRawFilePathByName("subdir_for_test/../test_rawfile.txt", outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_EQ(outValue, rawfileDir + "/" + validName);

    // GetRawFileDescriptor also allows within-rawfile relative
    ResourceManager::RawFileDescriptor descriptor{};
    EXPECT_EQ(rm->GetRawFileDescriptor("subdir_for_test/../test_rawfile.txt", descriptor), SUCCESS);
    EXPECT_GT(descriptor.fd, 0);
    EXPECT_EQ(rm->CloseRawFileDescriptor("subdir_for_test/../test_rawfile.txt"), SUCCESS);

    rmdir(subdir.c_str());
}
