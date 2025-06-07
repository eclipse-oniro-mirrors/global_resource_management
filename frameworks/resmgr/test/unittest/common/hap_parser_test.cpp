/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "hap_parser_test.h"

#include <gtest/gtest.h>

#include "test_common.h"
#include "utils/errors.h"
#include "utils/string_utils.h"

#include "hap_parser.h"
#include "hap_parser_v1.h"
#include "hap_parser_v2.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class HapParserTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapParserTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void HapParserTest::TearDownTestCase()
{
    // step 2: input testsuit teardown step
}

void HapParserTest::SetUp()
{
    // step 3: input testcase setup step
}

void HapParserTest::TearDown()
{
    // step 4: input testcase teardown step
}

#ifdef SUPPORT_GRAPHICS
/*
 * @tc.name: HapParserFuncTest001
 * @tc.desc: Test CreateResConfigFromKeyParams
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest001, TestSize.Level1)
{
    std::vector<std::shared_ptr<KeyParam>> keyParams;
    auto kp = std::make_shared<KeyParam>();
    kp->type_ = LANGUAGES;
    kp->value_ = 31336;
    kp->InitStr();
    keyParams.push_back(kp);
    auto kp1 = std::make_shared<KeyParam>();
    kp1->type_ = REGION;
    kp1->value_ = 17230;
    kp1->InitStr();
    keyParams.push_back(kp1);
    auto kp2 = std::make_shared<KeyParam>();
    kp2->type_ = SCREEN_DENSITY;
    kp2->value_ = SCREEN_DENSITY_SDPI;
    kp->InitStr();
    keyParams.push_back(kp2);
    auto kp3 = std::make_shared<KeyParam>();
    kp3->type_ = DEVICETYPE;
    kp3->value_ = DEVICE_CAR;
    kp3->InitStr();
    keyParams.push_back(kp3);
    auto kp4 = std::make_shared<KeyParam>();
    kp4->type_ = DIRECTION;
    kp4->value_ = DIRECTION_VERTICAL;
    kp4->InitStr();
    keyParams.push_back(kp4);
    auto kp5 = std::make_shared<KeyParam>();
    kp5->type_ = COLORMODE;
    kp5->value_ = DARK;
    kp5->InitStr();
    keyParams.push_back(kp5);
    auto config = HapParser::CreateResConfigFromKeyParams(keyParams);
    if (config != nullptr) {
        EXPECT_EQ(std::string("zh"), config->GetLocaleInfo()->getLanguage());
        EXPECT_EQ(std::string("CN"), config->GetLocaleInfo()->getCountry());
        EXPECT_EQ(std::string("Hans"), config->GetLocaleInfo()->getScript());
        EXPECT_EQ(DEVICE_CAR, config->GetDeviceType());
        EXPECT_EQ(DIRECTION_VERTICAL, config->GetDirection());
        EXPECT_EQ(SCREEN_DENSITY_SDPI / BASE_DPI, config->GetScreenDensity());
        EXPECT_EQ(DARK, config->GetColorMode());
    } else {
        EXPECT_TRUE(false);
    }
    keyParams.clear();
}
#endif

/*
 * @tc.name: HapParserFuncTest002
 * @tc.desc: Test GetDeviceType
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest002, TestSize.Level1)
{
    ASSERT_EQ(DEVICE_CAR, HapParser::GetDeviceType(DEVICE_CAR));
    ASSERT_EQ(DEVICE_PAD, HapParser::GetDeviceType(DEVICE_PAD));
    ASSERT_EQ(DEVICE_PHONE, HapParser::GetDeviceType(DEVICE_PHONE));
    ASSERT_EQ(DEVICE_TABLET, HapParser::GetDeviceType(DEVICE_TABLET));
    ASSERT_EQ(DEVICE_TV, HapParser::GetDeviceType(DEVICE_TV));
    ASSERT_EQ(DEVICE_WEARABLE, HapParser::GetDeviceType(DEVICE_WEARABLE));
    ASSERT_EQ(DEVICE_NOT_SET, HapParser::GetDeviceType(1000000));
}

/*
 * @tc.name: HapParserFuncTest003
 * @tc.desc: Test GetScreenDensity
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest003, TestSize.Level1)
{
    ASSERT_EQ(SCREEN_DENSITY_SDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_SDPI));
    ASSERT_EQ(SCREEN_DENSITY_MDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_MDPI));
    ASSERT_EQ(SCREEN_DENSITY_LDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_LDPI));
    ASSERT_EQ(SCREEN_DENSITY_XLDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_XLDPI));
    ASSERT_EQ(SCREEN_DENSITY_XXLDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_XXLDPI));
    ASSERT_EQ(SCREEN_DENSITY_XXXLDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_XXXLDPI));
    ASSERT_EQ(SCREEN_DENSITY_NOT_SET, HapParser::GetScreenDensity(10000000));
}

/*
 * @tc.name: HapParserFuncTest004
 * @tc.desc: Test GetColorMode
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest004, TestSize.Level1)
{
    ASSERT_EQ(DARK, HapParser::GetColorMode(DARK));
    ASSERT_EQ(LIGHT, HapParser::GetColorMode(LIGHT));
}

/*
 * @tc.name: ReadIndexFromFileTest001
 * @tc.desc: Test GetIndexData
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadIndexFromFileTest001, TestSize.Level1)
{
    std::string zipFile = FormatFullPath("not_exist_all.hap");
    std::unique_ptr<uint8_t[]> buffer;
    size_t len;
    bool ret = HapParser::GetIndexData(zipFile.c_str(), buffer, len);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: ReadIndexFromFileTest002
 * @tc.desc: Test GetIndexData
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadIndexFromFileTest002, TestSize.Level1)
{
    std::string zipFile = FormatFullPath(g_hapPath);
    std::unique_ptr<uint8_t[]> buffer;
    size_t len;
    bool ret = HapParser::GetIndexData(zipFile.c_str(), buffer, len);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: ReadIndexFromFileTest002
 * @tc.desc: Test GetIndexData
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadIndexFromFileTest003, TestSize.Level1)
{
    std::string zipFile = FormatFullPath(HAP_PATH_FA);
    std::unique_ptr<uint8_t[]> buffer;
    size_t len;
    bool ret = HapParser::GetIndexData(zipFile.c_str(), buffer, len);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: ReadRawFileFromHapTest001
 * @tc.desc: Test ReadRawFileFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileFromHapTest001, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = HapParser::ReadRawFileFromHap(hapPath, "", "no_exist_file.txt", len, outValue);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: ReadRawFileFromHapTest002
 * @tc.desc: Test ReadRawFileFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileFromHapTest002, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = HapParser::ReadRawFileFromHap(hapPath, "", "test_rawfile.txt", len, outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_EQ(static_cast<int>(len), 17);
}

/*
 * @tc.name: ReadRawFileFromHapTest003
 * @tc.desc: Test ReadRawFileFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileFromHapTest003, TestSize.Level1)
{
    std::string hapPath = FormatFullPath("all_fa_no_exist.hap");
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = HapParser::ReadRawFileFromHap(hapPath, "", "test_rawfile.txt", len, outValue);
    EXPECT_EQ(state, NOT_FOUND);
}

/*
 * @tc.name: ReadRawFileFromHapTest004
 * @tc.desc: Test ReadRawFileFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileFromHapTest004, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = HapParser::ReadRawFileFromHap(hapPath, FormatFullPath("no_patch.hap"), "test_rawfile.txt", len,
        outValue);
    EXPECT_EQ(state, NOT_FOUND);
}

/*
 * @tc.name: ReadRawFileFromHapTest005
 * @tc.desc: Test ReadRawFileFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileFromHapTest005, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = HapParser::ReadRawFileFromHap(hapPath, hapPath, "test_rawfile.txt", len, outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_EQ(static_cast<int>(len), 17);
}

/*
 * @tc.name: ReadRawFileFromHapTest006
 * @tc.desc: Test ReadRawFileFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileFromHapTest006, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = HapParser::ReadRawFileFromHap(hapPath, hapPath, "no_exist_file.txt", len, outValue);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: ReadRawFileDescriptorTest001
 * @tc.desc: Test ReadRawFileDescriptor
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileDescriptorTest001, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    ResourceManager::RawFileDescriptor descriptor;
    RState state = HapParser::ReadRawFileDescriptor(hapPath.c_str(), "", "no_exist_file.txt", descriptor);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: ReadRawFileDescriptorTest002
 * @tc.desc: Test ReadRawFileDescriptor
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileDescriptorTest002, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    ResourceManager::RawFileDescriptor descriptor;
    RState state = HapParser::ReadRawFileDescriptor(hapPath.c_str(), "", "test_rawfile.txt", descriptor);
    EXPECT_EQ(state, SUCCESS);
    if (descriptor.fd > 0) {
        close(descriptor.fd);
    }
}

/*
 * @tc.name: ReadRawFileDescriptorTest003
 * @tc.desc: Test ReadRawFileDescriptor
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileDescriptorTest003, TestSize.Level1)
{
    std::string hapPath = FormatFullPath("all_fa_no_exist.hap");
    ResourceManager::RawFileDescriptor descriptor;
    RState state = HapParser::ReadRawFileDescriptor(hapPath.c_str(), "", "test_rawfile.txt", descriptor);
    EXPECT_EQ(state, NOT_FOUND);
}

/*
 * @tc.name: ReadRawFileDescriptorTest004
 * @tc.desc: Test ReadRawFileDescriptor
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileDescriptorTest004, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    ResourceManager::RawFileDescriptor descriptor;
    RState state = HapParser::ReadRawFileDescriptor(hapPath.c_str(), "/data/test/no_patch.hap", "1.txt", descriptor);
    EXPECT_EQ(state, NOT_FOUND);
}

/*
 * @tc.name: ReadRawFileDescriptorTest005
 * @tc.desc: Test ReadRawFileDescriptor
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileDescriptorTest005, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    ResourceManager::RawFileDescriptor descriptor;
    RState state = HapParser::ReadRawFileDescriptor(hapPath.c_str(), hapPath.c_str(), "test_rawfile.txt", descriptor);
    EXPECT_EQ(state, SUCCESS);
    if (descriptor.fd > 0) {
        close(descriptor.fd);
    }
}

/*
 * @tc.name: ReadRawFileDescriptorTest006
 * @tc.desc: Test ReadRawFileDescriptor
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ReadRawFileDescriptorTest006, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(HAP_PATH_FA);
    ResourceManager::RawFileDescriptor descriptor;
    RState state = HapParser::ReadRawFileDescriptor(hapPath.c_str(), hapPath.c_str(), "111.txt", descriptor);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: GetRawFileListUnCompressedTest001
 * @tc.desc: Test GetRawFileListUnCompressed
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, GetRawFileListUnCompressedTest001, TestSize.Level1)
{
    std::string indexPath = FormatFullPath(g_resFilePath);
    std::vector<std::string> fileList;
    RState state = HapParser::GetRawFileListUnCompressed(indexPath, "", fileList);
    EXPECT_EQ(state, SUCCESS);
}

/*
 * @tc.name: IsRawDirFromHapTest001
 * @tc.desc: Test IsRawDirFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, IsRawDirFromHapTest001, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(g_hapPath);
    bool outValue;
    RState state = HapParser::IsRawDirFromHap(hapPath.c_str(), "", outValue);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
    EXPECT_FALSE(outValue);
}

/*
 * @tc.name: IsRawDirFromHapTest002
 * @tc.desc: Test IsRawDirFromHap
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, IsRawDirFromHapTest002, TestSize.Level1)
{
    std::string hapPath = FormatFullPath("no_exist_hap");
    bool outValue;
    RState state = HapParser::IsRawDirFromHap(hapPath.c_str(), "test", outValue);
    EXPECT_EQ(state, NOT_FOUND);
    EXPECT_FALSE(outValue);
}

/*
 * @tc.name: IsRawDirUnCompressedTest001
 * @tc.desc: Test IsRawDirUnCompressed
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, IsRawDirUnCompressedTest001, TestSize.Level1)
{
    std::string hapPath = FormatFullPath(g_hapPath);
    bool outValue;
    RState state = HapParser::IsRawDirUnCompressed(hapPath.c_str(), outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_FALSE(outValue);
}

/*
 * @tc.name: IsRawDirUnCompressedTest002
 * @tc.desc: Test IsRawDirUnCompressed
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, IsRawDirUnCompressedTest002, TestSize.Level1)
{
    std::string hapPath = FormatFullPath("no_exist_hap");
    bool outValue;
    RState state = HapParser::IsRawDirUnCompressed(hapPath.c_str(), outValue);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
    EXPECT_FALSE(outValue);
}

/*
 * @tc.name: IsRawDirUnCompressedTest003
 * @tc.desc: Test IsRawDirUnCompressed
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, IsRawDirUnCompressedTest003, TestSize.Level1)
{
    std::string pathName = "/data/test/all/assets/entry/resources/base/media";
    bool outValue;
    RState state = HapParser::IsRawDirUnCompressed(pathName, outValue);
    EXPECT_EQ(state, SUCCESS);
    EXPECT_TRUE(outValue);
}

/*
 * @tc.name: ParseModuleNameTest001
 * @tc.desc: Test ParseModuleName
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ParseModuleNameTest001, TestSize.Level1)
{
    std::shared_ptr<OHOS::AbilityBase::Extractor> extractor = nullptr;
    std::string result = HapParser::ParseModuleName(extractor);
    EXPECT_EQ(result, std::string());
}

/*
 * @tc.name: ParseModuleNameTest002
 * @tc.desc: Test ParseModuleName
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ParseModuleNameTest002, TestSize.Level1)
{
    std::shared_ptr<OHOS::AbilityBase::Extractor> extractor = std::make_shared<OHOS::AbilityBase::Extractor>("");
    std::string result = HapParser::ParseModuleName(extractor);
    EXPECT_EQ(result, std::string());
}

/*
 * @tc.name: GetRawFileListTest001
 * @tc.desc: Test GetRawFileList
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, GetRawFileListTest001, TestSize.Level1)
{
    std::string hapPath = "";
    std::string rawDirPath = "";
    std::set<std::string> fileSet;
    RState state = HapParser::GetRawFileList(hapPath, rawDirPath, fileSet);
    EXPECT_EQ(state, NOT_FOUND);
}

/*
 * @tc.name: ParseResHex001
 * @tc.desc: Test ParseResHex
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ParseResHex001, TestSize.Level1)
{
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    HapParserV1 hapParser(rc, SELECT_ALL, false);
    int32_t state = hapParser.ParseResHex();
    EXPECT_EQ(state, SYS_ERROR);
}

/*
 * @tc.name: ParseResHex002
 * @tc.desc: Test ParseResHex
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, ParseResHex002, TestSize.Level1)
{
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    HapParserV2 hapParser;
    int32_t state = hapParser.ParseResHex();
    EXPECT_EQ(state, SYS_ERROR);
}

/*
 * @tc.name: BuildResConfig001
 * @tc.desc: Test BuildResConfig
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, BuildResConfig001, TestSize.Level1)
{
    HapParser::ResConfigKey *configKey = nullptr;
    std::shared_ptr<ResConfigImpl> resConfig = HapParser::BuildResConfig(configKey);
    EXPECT_EQ(resConfig, nullptr);
}

/*
 * @tc.name: BuildResConfig002
 * @tc.desc: Test BuildResConfig
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, BuildResConfig002, TestSize.Level1)
{
    HapParser::ResConfigKey configKey;
    configKey.language = std::string("abc").c_str();
    std::shared_ptr<ResConfigImpl> resConfig = HapParser::BuildResConfig(&configKey);
    EXPECT_TRUE(resConfig != nullptr);
}
}
