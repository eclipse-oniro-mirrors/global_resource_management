/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "utils/utils.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class UtilsTest : public testing::Test {};

/*
 * @tc.name: TestConvertColorToUInt32Func001
 * @tc.desc: Test ConvertColorToUInt32 function
 * @tc.type: FUNC
 */
HWTEST_F(UtilsTest, TestConvertColorToUInt32Func001, TestSize.Level1)
{
    uint32_t outValue;
    EXPECT_EQ(Utils::ConvertColorToUInt32(nullptr, outValue), RState::INVALID_FORMAT);
    EXPECT_EQ(Utils::ConvertColorToUInt32("", outValue), RState::INVALID_FORMAT);
    EXPECT_EQ(Utils::ConvertColorToUInt32("123", outValue), RState::INVALID_FORMAT);
    EXPECT_EQ(Utils::ConvertColorToUInt32("#12", outValue), RState::INVALID_FORMAT);
    EXPECT_EQ(Utils::ConvertColorToUInt32("##12", outValue), RState::INVALID_FORMAT);
    EXPECT_EQ(Utils::ConvertColorToUInt32("#123", outValue), RState::SUCCESS);
    EXPECT_EQ(outValue, 4279312947);
    EXPECT_EQ(Utils::ConvertColorToUInt32("#1233", outValue), RState::SUCCESS);
    EXPECT_EQ(outValue, 287454003);
    EXPECT_EQ(Utils::ConvertColorToUInt32("#12334", outValue), RState::INVALID_FORMAT);
    EXPECT_EQ(Utils::ConvertColorToUInt32("#123344", outValue), RState::SUCCESS);
    EXPECT_EQ(outValue, 4279382852);
    EXPECT_EQ(Utils::ConvertColorToUInt32("#12334455", outValue), RState::SUCCESS);
    EXPECT_EQ(outValue, 305349717);
    EXPECT_EQ(Utils::ConvertColorToUInt32("#1a3344ff", outValue), RState::SUCCESS);
    EXPECT_EQ(outValue, 439567615);
}

/*
 * @tc.name: TestGetMediaBase64Data001
 * @tc.desc: Test GetMediaBase64Data function
 * @tc.type: FUNC
 */
HWTEST_F(UtilsTest, TestGetMediaBase64Data001, TestSize.Level1)
{
    std::string iconPath = "/data/test/all/assets/entry/resources/base/media/icon.png";
    std::string base64Data;
    RState state = Utils::GetMediaBase64Data(iconPath, base64Data);
    EXPECT_EQ(SUCCESS, state);
}

/*
 * @tc.name: TestGetMediaBase64Data002
 * @tc.desc: Test GetMediaBase64Data function
 * @tc.type: FUNC
 */
HWTEST_F(UtilsTest, TestGetMediaBase64Data002, TestSize.Level1)
{
    std::string iconPath = "/data/test/all/assets/entry/resources/base/media/icon_no_exist.png";
    std::string base64Data;
    RState state = Utils::GetMediaBase64Data(iconPath, base64Data);
    EXPECT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: TestCanonicalizePath001
 * @tc.desc: Test CanonicalizePath function
 * @tc.type: FUNC
 */
HWTEST_F(UtilsTest, TestCanonicalizePath001, TestSize.Level1)
{
    char *outPath = nullptr;
    size_t len = 0;
    Utils::CanonicalizePath(nullptr, outPath, len);
    EXPECT_EQ(outPath, nullptr);
}

/*
 * @tc.name: TestCanonicalizePath002
 * @tc.desc: Test CanonicalizePath function
 * @tc.type: FUNC
 */
HWTEST_F(UtilsTest, TestCanonicalizePath002, TestSize.Level1)
{
    std::string iconPath = "/data/test/all/assets/entry/resources/base/media/icon.png";
    char *outPath = nullptr;
    size_t len = 1;
    Utils::CanonicalizePath(iconPath.c_str(), outPath, len);
    EXPECT_EQ(outPath, nullptr);
}

/*
 * @tc.name: TestGetFiles001
 * @tc.desc: Test GetFiles function
 * @tc.type: FUNC
 */
HWTEST_F(UtilsTest, TestGetFiles001, TestSize.Level1)
{
    std::string currentDir = "/data/test/all/assets/entry/resources/";
    std::vector<std::string> vFiles;
    RState state = Utils::GetFiles(currentDir, vFiles);
    EXPECT_EQ(SUCCESS, state);
}

/*
 * @tc.name: TestGetFiles002
 * @tc.desc: Test GetFiles function
 * @tc.type: FUNC
 */
HWTEST_F(UtilsTest, TestGetFiles002, TestSize.Level1)
{
    std::string currentDir = "/data/test/all/assets/entry/resources/no_exist_dir_123456";
    std::vector<std::string> vFiles;
    RState state = Utils::GetFiles(currentDir, vFiles);
    EXPECT_EQ(ERROR_CODE_RES_PATH_INVALID, state);
}
}  // namespace
