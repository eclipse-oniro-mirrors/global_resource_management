/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "resource_manager_test_media.h"

#include "resource_manager_test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class ResourceManagerTestString : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerTestString() : rm(nullptr)
    {}

    ~ResourceManagerTestString()
    {}
public:
    ResourceManager *rm;
    ResourceManagerTestCommon *rmc;
};

void ResourceManagerTestString::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResourceManagerTestString::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerTestString::SetUp(void)
{
    this->rm = CreateResourceManager();
    this->rmc = new ResourceManagerTestCommon(rm);
}

void ResourceManagerTestString::TearDown(void)
{
    delete this->rm;
    delete this->rmc;
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest001
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByIdTest001, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, nullptr);

    rmc->TestStringById("app_name", "App Name");

    rmc->TestStringById("copyright_text", "XXXXXX All rights reserved. ©2011-2019");

    rmc->TestStringById("string_ref", "XXXXXX All rights reserved. ©2011-2019");

    rmc->TestStringById("string_ref2", "XXXXXX All rights reserved. ©2011-2019");
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest002
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    rmc->TestStringById("app_name", "应用名称");

    rmc->TestStringById("copyright_text", "版权所有 ©2011-2019 XXXX有限公司保留一切权利");

    rmc->TestStringById("string_ref", "$aaaaa");

    rmc->TestStringById("string_ref2", "$aaaaa");
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest003
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByIdTest003, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest004
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByIdTest004, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, nullptr);

    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_app_name", "SystemOverlay");
}

/*
 * @tc.name: ResourceManagerGetStringByIdTest005
 * @tc.desc: Test GetStringById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByIdTest005, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    rmc->TestStringById("ohos_lab_answer_call", "overlay接听电话");
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest001
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameTest001, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, nullptr);

    rmc->TestStringByName("app_name", "App Name");

    rmc->TestStringByName("copyright_text", "XXXXXX All rights reserved. ©2011-2019");

    rmc->TestStringByName("string_ref", "XXXXXX All rights reserved. ©2011-2019");

    rmc->TestStringByName("string_ref2", "XXXXXX All rights reserved. ©2011-2019");
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest002
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    rmc->TestStringByName("app_name", "应用名称");

    rmc->TestStringByName("copyright_text", "版权所有 ©2011-2019 XXXX有限公司保留一切权利");

    rmc->TestStringByName("string_ref", "$aaaaa");

    rmc->TestStringByName("string_ref2", "$aaaaa");
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest003
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameTest003, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest004
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameTest004, TestSize.Level1)
{
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_VERTICAL);
    rm->UpdateResConfig(*rc);
    delete rc;
    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl *)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    std::string outValue;
    ((ResourceManagerImpl *)rm)->GetStringByName("ohos_desc_camera", outValue);
    ASSERT_EQ(outValue, "允许应用拍摄照片和视频。");
}

/*
 * @tc.name: ResourceManagerGetStringByNameTest005
 * @tc.desc: Test GetStringByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameTest005, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, nullptr);
    std::vector<std::string> overlayPaths;
    overlayPaths.push_back(FormatFullPath(g_overlayResFilePath).c_str());
    bool ret = ((ResourceManagerImpl *)rm)->AddResource(FormatFullPath(g_resFilePath).c_str(), overlayPaths);
    ASSERT_TRUE(ret);
    std::string outValue;
    ((ResourceManagerImpl *)rm)->GetStringByName("hello", outValue);
    ASSERT_EQ(outValue, "Helloooo");
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest001
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    const char *name = "app_name";
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    RState state = rm->GetStringFormatById(outValue, id, 101);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ("应用名称", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest002
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringFormatById(outValue, NON_EXIST_ID, 101);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest003
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest003, TestSize.Level1)
{
    const char *name = "test_string0";
    const char *cmp = "向前兼容";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest004
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest004, TestSize.Level1)
{
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    std::string outValue;
    RState state = rm->GetStringFormatById(NON_EXIST_ID, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest005
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest005, TestSize.Level1)
{
    const char *name = "test_string1";
    const char *cmp = "%10%d%%d%%";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "10"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest006
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest006, TestSize.Level1)
{
    const char *name = "test_string2";
    const char *cmp = "-9.999 你好";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "-9.999"},
        {ResourceManager::NapiValueType::NAPI_STRING, " 你好"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest007
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest007, TestSize.Level1)
{
    const char *name = "test_string2";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_STRING, " 你好"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "-9.999"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest008
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest008, TestSize.Level1)
{
    const char *name = "test_string2";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "-9.999"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest009
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest009, TestSize.Level1)
{
    const char *name = "test_string9";
    const char *cmp = "%d-%d1$d";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest010
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest010, TestSize.Level1)
{
    const char *name = "test_string10";
    const char *cmp = "%d%%%";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest011
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest011, TestSize.Level1)
{
    const char *name = "test_string11";
    const char *cmp = "-7";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "-7.5"}};
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest012
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest012, TestSize.Level1)
{
    const char *name = "test_string12";
    const char *cmp = "2-1===6.100000-5.000000===你好,world===%hh%%%";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1.000000"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "2.100000"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "5.000000"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "6.100000"},
        {ResourceManager::NapiValueType::NAPI_STRING, "world"},
        {ResourceManager::NapiValueType::NAPI_STRING, "你好,"}};
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest013
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest013, TestSize.Level1)
{
    const char *name = "test_string13";
    const char *cmp = "11";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest014
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest014, TestSize.Level1)
{
    const char *name = "test_string14";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest015
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest015, TestSize.Level1)
{
    const char *name = "test_string15";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_STRING, "hhh"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest016
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest016, TestSize.Level1)
{
    const char *name = "test_string16";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_STRING, "hhh"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest017
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest017, TestSize.Level1)
{
    const char *name = "test_string17";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest018
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest018, TestSize.Level1)
{
    const char *name = "test_string18";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest019
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest019, TestSize.Level1)
{
    const char *name = "test_string19";
    const char *cmp = "11";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "2"}};
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest020
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest020, TestSize.Level1)
{
    const char *name = "test_string20";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest021
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest021, TestSize.Level1)
{
    const char *name = "test_string21";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    int id = rmc->GetResId(name, ResType::STRING);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatById(id, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_ID_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByIdTest022
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByIdTest022, TestSize.Level1)
{
    const char *name = "test_string22";
    const char *cmp = "112";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "2"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "3"}};
    rmc->TestGetStringFormatById(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest001
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    const char *name = "app_name";
    std::string outValue;
    RState state = rm->GetStringFormatByName(outValue, name, 101);
    ASSERT_EQ(SUCCESS, state);
    ASSERT_EQ("应用名称", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest002
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    std::string outValue;
    RState state = rm->GetStringFormatByName(outValue, g_nonExistName, 101);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest003
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest003, TestSize.Level1)
{
    const char *name = "test_string0";
    const char *cmp = "向前兼容";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest004
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest004, TestSize.Level1)
{
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    std::string outValue;
    RState state = rm->GetStringFormatByName("NON_EXIST_NAME", outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest005
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest005, TestSize.Level1)
{
    const char *name = "test_string3";
    const char *cmp = "abc12.3abc%%\"abc\"abc";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "12.3"},
        {ResourceManager::NapiValueType::NAPI_STRING, "\"abc\""}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest006
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest006, TestSize.Level1)
{
    const char *name = "test_string4";
    const char *cmp = "%a-320a%%";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "-320"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest007
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest007, TestSize.Level1)
{
    const char *name = "test_string2";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "-9.999"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "abc"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest008
 * @tc.desc: Test GetStringFormatByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest008, TestSize.Level1)
{
    const char *name = "test_string2";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "-9.999"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest009
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest009, TestSize.Level1)
{
    const char *name = "test_string9";
    const char *cmp = "%d-%d1$d";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest010
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest010, TestSize.Level1)
{
    const char *name = "test_string10";
    const char *cmp = "%d%%%";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest011
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest011, TestSize.Level1)
{
    const char *name = "test_string11";
    const char *cmp = "-7";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "-7.5"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest012
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest012, TestSize.Level1)
{
    const char *name = "test_string12";
    const char *cmp = "2-1===4.100000-3.000000===你好,world===%hh%%%";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1.000000"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "2.100000"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "3.000000"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "4.100000"},
        {ResourceManager::NapiValueType::NAPI_STRING, "world"},
        {ResourceManager::NapiValueType::NAPI_STRING, "你好,"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest013
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest013, TestSize.Level1)
{
    const char *name = "test_string13";
    const char *cmp = "11";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest014
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest014, TestSize.Level1)
{
    const char *name = "test_string14";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest015
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest015, TestSize.Level1)
{
    const char *name = "test_string15";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_STRING, "hhh"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest016
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest016, TestSize.Level1)
{
    const char *name = "test_string16";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_STRING, "hhh"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest017
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest017, TestSize.Level1)
{
    const char *name = "test_string17";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest018
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest018, TestSize.Level1)
{
    const char *name = "test_string18";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest019
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest019, TestSize.Level1)
{
    const char *name = "test_string19";
    const char *cmp = "11";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "2"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest020
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest020, TestSize.Level1)
{
    const char *name = "test_string20";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest021
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest021, TestSize.Level1)
{
    const char *name = "test_string21";
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"}};
    RState state = rm->GetStringFormatByName(name, outValue, jsParams);
    ASSERT_EQ(ERROR_CODE_RES_NAME_FORMAT_ERROR, state);
}

/*
 * @tc.name: ResourceManagerGetStringFormatByNameTest022
 * @tc.desc: Test GetStringFormatById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringFormatByNameTest022, TestSize.Level1)
{
    const char *name = "test_string22";
    const char *cmp = "112";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams =
        {{ResourceManager::NapiValueType::NAPI_NUMBER, "1"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "2"},
        {ResourceManager::NapiValueType::NAPI_NUMBER, "3"}};
    rmc->TestGetStringFormatByName(name, jsParams, cmp);
}

/*
 * @tc.name: ResourceManagerGetStringArrayByIdTest001
 * @tc.desc: Test GetStringArrayById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringArrayByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    rmc->TestGetStringArrayById("size");
}

/*
 * @tc.name: ResourceManagerGetStringArrayByIdTest002
 * @tc.desc: Test GetStringArrayById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringArrayByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    RState state;
    // error case
    // not found case
    std::vector<std::string> outValue;
    state = rm->GetStringArrayById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetStringArrayByNameTest001
 * @tc.desc: Test GetStringArrayByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringArrayByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    rmc->TestGetStringArrayByName("size");
}

/*
 * @tc.name: ResourceManagerGetStringArrayByNameTest002
 * @tc.desc: Test GetStringArrayByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringArrayByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, nullptr);

    RState state;
    // error case
    // not found case
    std::vector<std::string> outValue;
    state = rm->GetStringArrayByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}


/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest001
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdTest001, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "%d apple", false);

    quantity = 101;
    rmc->TestPluralStringById(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest002
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "%d apples", false);

    quantity = 101;
    rmc->TestPluralStringById(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest003
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdTest003, TestSize.Level1)
{
    rmc->AddResource("pl", nullptr, "PL");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "1 jabłko");

    quantity = 2;
    rmc->TestPluralStringById(quantity, "%d jabłka");

    quantity = 23;
    rmc->TestPluralStringById(quantity, "%d jabłka");

    quantity = 12;
    rmc->TestPluralStringById(quantity, "%d jabłek");
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest004
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdTest004, TestSize.Level1)
{
    rmc->AddResource("ar", nullptr, "SA");

    int quantity = 0;
    rmc->TestPluralStringById(quantity, "zero-0");
    quantity = 1;
    rmc->TestPluralStringById(quantity, "one-1");
    quantity = 2;
    rmc->TestPluralStringById(quantity, "two-2");
    quantity = 5;
    rmc->TestPluralStringById(quantity, "few-%d");
    quantity = 12;
    rmc->TestPluralStringById(quantity, "many-%d");
    quantity = 500;
    rmc->TestPluralStringById(quantity, "other-%d");
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdTest005
 * @tc.desc: Test GetPluralStringById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdTest005, TestSize.Level1)
{
    rmc->AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringById(NON_EXIST_ID, 1, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameTest001
 * @tc.desc: Test GetPluralStringByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByNameTest001, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    int quantity = 1;
    rmc->TestPluralStringByName(quantity, "%d apple", false);

    quantity = 101;
    rmc->TestPluralStringByName(quantity, "%d apples", false);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameTest002
 * @tc.desc: Test GetPluralStringByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByNameTest002, TestSize.Level1)
{
    rmc->AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringByName(g_nonExistName, 1, outValue);
    ASSERT_EQ(NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFormatTest001
 * @tc.desc: Test GetPluralStringByIdFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdFormatTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    int quantity = 1;
    rmc->TestPluralStringById(quantity, "1 apples", true);

    quantity = 101;
    rmc->TestPluralStringById(quantity, "101 apples", true);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFormatTest002
 * @tc.desc: Test GetPluralStringByIdFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdFormatTest002, TestSize.Level1)
{
    rmc->AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringByIdFormat(outValue, NON_EXIST_ID, 1, 1);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFormatTest003
 * @tc.desc: Test GetPluralStringByIdFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdFormatTest003, TestSize.Level1)
{
    rmc->AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    int id = rmc->GetResId("app_name", ResType::STRING);
    state = rm->GetPluralStringByIdFormat(outValue, id, 1, 1);
    ASSERT_EQ(ERROR_CODE_RES_NOT_FOUND_BY_ID, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByIdFormatTest004
 * @tc.desc: Test GetPluralStringByIdFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByIdFormatTest004, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    RState state;
    std::string outValue;
    int quantity = 1;
    int id = rmc->GetResId("eat_apple1", ResType::PLURALS);
    state = rm->GetPluralStringByIdFormat(outValue, id, quantity);
    ASSERT_EQ(ERROR_CODE_RES_REF_TOO_MUCH, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFormatTest001
 * @tc.desc: Test GetPluralStringByNameFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByNameFormatTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    int quantity = 1;
    rmc->TestPluralStringByName(quantity, "1 apples", true);

    quantity = 101;
    rmc->TestPluralStringByName(quantity, "101 apples", true);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFormatTest002
 * @tc.desc: Test GetPluralStringByNameFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByNameFormatTest002, TestSize.Level1)
{
    rmc->AddResource("ar", nullptr, "SA");

    RState state;
    std::string outValue;
    state = rm->GetPluralStringByNameFormat(outValue, g_nonExistName, 1, 1);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFormatTest003
 * @tc.desc: Test GetPluralStringByNameFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByNameFormatTest003, TestSize.Level1)
{
    bool ret = ((ResourceManagerImpl*)rm)->AddResource(FormatFullPath(g_overlayResFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string outValue;
    const char* eatApple = "eat_apple";
    RState state = rm->GetPluralStringByNameFormat(outValue, eatApple, 1, 1);
    ASSERT_EQ(ERROR_CODE_RES_NOT_FOUND_BY_NAME, state);
}

/*
 * @tc.name: ResourceManagerGetPluralStringByNameFormatTest004
 * @tc.desc: Test GetPluralStringByNameFormat function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetPluralStringByNameFormatTest004, TestSize.Level1)
{
    rmc->AddColorModeResource(DEVICE_PHONE, LIGHT, SCREEN_DENSITY_LDPI / BASE_DPI);

    std::string outValue;
    const char* eatApple = "eat_apple1";
    int quantity = 1;
    RState state = rm->GetPluralStringByNameFormat(outValue, eatApple, quantity);
    ASSERT_EQ(ERROR_CODE_RES_REF_TOO_MUCH, state);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest001
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_en_us_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest002
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, nullptr);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_zh_cn", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest003
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);

    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest004
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(1);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc001_en", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest005
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest005, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(10);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc010_en", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest006
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest006, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest007
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest007, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_zh", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest008
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest008, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_en_us_phone", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest009
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest009, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_en_us_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest010
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest010, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_phone", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest011
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest011, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest012
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest012, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_en_us_dark", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest013
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest013, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_en_us_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest014
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest014, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_dark", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest015
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest015, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_SDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest016
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest016, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_LDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_en_us_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest017
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest017, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_XLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_en_us_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest018
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest018, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_LDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest019
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest019, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(LIGHT);
    rc->SetScreenDensity(SCREEN_DENSITY_XLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_light", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForMccMncTest020
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestString, ResourceManagerGetStringByNameForMccMncTest020, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("mccmnc_str", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("str_mcc460_mnc101_en_phone_dark_xldpi", outValue);
}
}
