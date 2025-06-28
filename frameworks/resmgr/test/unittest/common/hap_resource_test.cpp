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

#include "hap_resource_test.h"

#include <climits>
#include <gtest/gtest.h>

#include "hap_parser.h"
#include "hap_parser_v1.h"
#include "hap_parser_v2.h"
#include "hap_resource.h"
#include "test_common.h"
#include "utils/date_utils.h"
#include "utils/errors.h"
#include "utils/string_utils.h"
#include "hap_resource_manager.h"

#define private public

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class HapResourceTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapResourceTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void HapResourceTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void HapResourceTest::SetUp()
{
    // step 3: input testcase setup step
}

void HapResourceTest::TearDown()
{
    // step 4: input testcase teardown step
}

//void TestGetIdValuesByName(const HapResource *pResource)
void TestGetIdValuesByName(const std::shared_ptr<HapResource> pResource)
{
    std::string name = std::string("app_name");
    auto start = CurrentTimeUsec();
    auto idValues = pResource->GetIdValuesByName(name, ResType::STRING);
    auto cost = CurrentTimeUsec() - start;
    // 3 compare with the size of LimitPaths
    EXPECT_EQ(static_cast<size_t>(1), idValues->GetLimitPathsConst().size());
    RESMGR_HILOGD(RESMGR_TAG, "GetIdValues by name cost: %ld us.", cost);
    PrintIdValues(idValues);
    {
        auto limitPath = idValues->GetLimitPathsConst()[0];
        EXPECT_TRUE(limitPath->GetResConfig()->ToString() == "base");
        EXPECT_TRUE(limitPath->GetIdItem()->name_ == "app_name");
        EXPECT_TRUE(limitPath->GetIdItem()->value_ == "About");
    }
}

/*
 * this test shows how to load a hap, defaultConfig set to null
 * @tc.name: HapResourceFuncTest001
 * @tc.desc: Test Load & GetIdValues & GetIdValuesByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest001, TestSize.Level0)
{
    auto start = CurrentTimeUsec();
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    auto pResource = HapResourceManager::GetInstance().Load(FormatFullPath(g_resFilePath).c_str(), rc);
    auto cost = CurrentTimeUsec() - start;
    RESMGR_HILOGD(RESMGR_TAG, "load cost: %ld us.", cost);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    auto idValue = pResource->GetIdValuesByName("app_name", ResType::STRING);
    int id = idValue->GetLimitPathsConst()[0]->GetIdItem()->id_;
    start = CurrentTimeUsec();
    auto idValues = pResource->GetIdValues(id);
    cost = CurrentTimeUsec() - start;
    EXPECT_EQ(static_cast<size_t>(1), idValues->GetLimitPathsConst().size());
    RESMGR_HILOGD(RESMGR_TAG, "GetIdValues by id cost: %ld us.", cost);
    PrintIdValues(idValues);
    {
        auto limitPath = idValues->GetLimitPathsConst()[0];
        EXPECT_TRUE(limitPath->GetResConfig()->ToString() == "base");
        EXPECT_TRUE(limitPath->GetIdItem()->name_ == "app_name");
        EXPECT_TRUE(limitPath->GetIdItem()->value_ == "About");
    }
    TestGetIdValuesByName(pResource);
}

//void GetIdValuesByNameFuncTest002(const HapResource *pResource, int id)
void GetIdValuesByNameFuncTest002(const std::shared_ptr<HapResource> pResource, int id)
{
    std::string name = std::string("app_name");
    auto start = CurrentTimeUsec();
    auto idValues = pResource->GetIdValuesByName(name, ResType::STRING);
    auto cost = CurrentTimeUsec() - start;
    size_t expectSize = 2;
    EXPECT_EQ(expectSize, idValues->GetLimitPathsConst().size());
    RESMGR_HILOGD(RESMGR_TAG, "GetIdValues by name cost: %ld us.", cost);
    PrintIdValues(idValues);

    {
        auto limitPath = idValues->GetLimitPathsConst()[0];
        EXPECT_TRUE(limitPath->GetResConfig()->ToString() == "base");
        EXPECT_TRUE(limitPath->GetIdItem()->id_ == static_cast<uint32_t>(id));
        EXPECT_TRUE(limitPath->GetIdItem()->value_ == "About");
    }
    {
        auto limitPath = idValues->GetLimitPathsConst()[1];
        EXPECT_TRUE(limitPath->GetResConfig()->ToString() == "en_Latn_US");
        EXPECT_TRUE(limitPath->GetIdItem()->id_ == static_cast<uint32_t>(id));
        EXPECT_TRUE(limitPath->GetIdItem()->value_ == "App Name");
    }
}

/*
 * load a hap, set config en_US
 * @tc.name: HapResourceFuncTest002
 * @tc.desc: Test Load & GetIdValues & GetIdValuesByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest002, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    std::string resPath = FormatFullPath(g_resFilePath);
    const char *path = resPath.c_str();

    auto start = CurrentTimeUsec();
    auto pResource = HapResourceManager::GetInstance().Load(path, rc);
    auto cost = CurrentTimeUsec() - start;
    RESMGR_HILOGD(RESMGR_TAG, "load cost: %ld us.", cost);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    auto idValue = pResource->GetIdValuesByName("app_name", ResType::STRING);
    int id = idValue->GetLimitPathsConst()[0]->GetIdItem()->id_;
    start = CurrentTimeUsec();
    auto idValues = pResource->GetIdValues(id);
    cost = CurrentTimeUsec() - start;
    EXPECT_EQ(static_cast<size_t>(2), idValues->GetLimitPathsConst().size());
    RESMGR_HILOGD(RESMGR_TAG, "GetIdValues by id cost: %ld us.", cost);
    PrintIdValues(idValues);

    {
        auto limitPath = idValues->GetLimitPathsConst()[0];
        EXPECT_TRUE(limitPath->GetResConfig()->ToString() == "base");
        EXPECT_TRUE(limitPath->GetIdItem()->name_ == "app_name");
        EXPECT_TRUE(limitPath->GetIdItem()->value_ == "About");
    }
    {
        auto limitPath = idValues->GetLimitPathsConst()[1];
        EXPECT_TRUE(limitPath->GetResConfig()->ToString() == "en_Latn_US");
        EXPECT_TRUE(limitPath->GetIdItem()->name_ == "app_name");
        EXPECT_TRUE(limitPath->GetIdItem()->value_ == "App Name");
    }

    GetIdValuesByNameFuncTest002(pResource, id);
}

/*
 * load a hap, get a value which is ref
 * @tc.name: HapResourceFuncTest003
 * @tc.desc: Test GetIdValuesByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest003, TestSize.Level1)
{
    auto start = CurrentTimeUsec();
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    auto pResource = HapResourceManager::GetInstance().Load(FormatFullPath(g_resFilePath).c_str(), rc);
    auto cost = CurrentTimeUsec() - start;
    RESMGR_HILOGD(RESMGR_TAG, "load cost: %ld us.", cost);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    auto idv = pResource->GetIdValuesByName(std::string("integer_ref"), ResType::INTEGER);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("string_ref"), ResType::STRING);
    PrintIdValues(idv);

    // ref propagation
    idv = pResource->GetIdValuesByName(std::string("string_ref2"), ResType::STRING);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("boolean_ref"), ResType::BOOLEAN);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("color_ref"), ResType::COLOR);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("float_ref"), ResType::FLOAT);
    PrintIdValues(idv);

    // ref in array ,
    idv = pResource->GetIdValuesByName(std::string("intarray_1"), ResType::INTARRAY);
    PrintIdValues(idv);

    // "parent":   was ref too
    idv = pResource->GetIdValuesByName(std::string("child"), ResType::PATTERN);
    PrintIdValues(idv);
}

int32_t LoadFromHap(const char *hapPath)
{
    HapParserV2 hapParser;
    if (!hapParser.Init(hapPath)) {
        RESMGR_HILOGE(RESMGR_TAG, "HapParserV2 Init failed!");
        return UNKNOWN_ERROR;
    } else {
        RESMGR_HILOGD(RESMGR_TAG, "HapParserV2 Init success.");
    }
    return OK;
}

int32_t LoadFromHap(const char *hapPath, std::shared_ptr<ResConfigImpl> defaultConfig,
    const uint32_t &selectedTypes = SELECT_ALL)
{
    HapParserV1 hapParser(defaultConfig, selectedTypes, false);
    hapParser.Init(hapPath);
    int32_t out = hapParser.ParseResHex();
    if (out != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed! retcode:%d", out);
        return out;
    } else {
        RESMGR_HILOGD(RESMGR_TAG, "ParseResHex success:\n%s", hapParser.GetResDesc()->ToString().c_str());
    }
    // construct hapresource
    return OK;
}

/*
 * @tc.name: HapResourceFuncTest004
 * @tc.desc: Test HapParser::GetIndexData function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest004, TestSize.Level1)
{
    // 1. normal case
    int32_t res = LoadFromHap(FormatFullPath("all.hap").c_str(), nullptr);
    ASSERT_TRUE(res == OK);

    // 2. hap file exists, config.json does not exist
    res = LoadFromHap(FormatFullPath("err-config.json-1.hap").c_str(), nullptr);
    ASSERT_TRUE(res != OK);

    // 3. hap file exists, config.json error: missing "moduleName"
    res = LoadFromHap(FormatFullPath("err-config.json-2.hap").c_str(), nullptr);
    ASSERT_TRUE(res != OK);

    // 4. load select string type res
    res = LoadFromHap(FormatFullPath("all.hap").c_str(), nullptr, SELECT_STRING);
    ASSERT_TRUE(res == OK);
}

/*
 * @tc.name: HapResourceFuncTest005
 * @tc.desc: Test HapParser::GetIndexData function in new module, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest005, TestSize.Level1)
{
    int32_t res = LoadFromHap(FormatFullPath("newModule.hap").c_str());
    ASSERT_TRUE(res == OK);
}

/*
 * this test shows how to load a hap, defaultConfig set to null
 * @tc.name: HapResourceFuncTest006
 * @tc.desc: Test Load & GetIdValues & GetIdValuesByName function in new resource module, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest006, TestSize.Level0)
{
    auto start = CurrentTimeUsec();
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    auto pResource = HapResourceManager::GetInstance().Load(FormatFullPath(g_newResFilePath).c_str(), rc);
    auto cost = CurrentTimeUsec() - start;
    RESMGR_HILOGD(RESMGR_TAG, "load cost: %ld us.", cost);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    auto idValue = pResource->GetIdValuesByName("app_name", ResType::STRING);
    int id = idValue->GetLimitPathsConst()[0]->GetIdItem()->id_;
    start = CurrentTimeUsec();
    auto idValues = pResource->GetIdValues(id);
    cost = CurrentTimeUsec() - start;
    EXPECT_EQ(static_cast<size_t>(3), idValues->GetLimitPathsConst().size());
    RESMGR_HILOGD(RESMGR_TAG, "GetIdValues by id cost: %ld us.", cost);
    PrintIdValues(idValues);
}

/*
 * load a hap, set config en_US
 * @tc.name: HapResourceFuncTest007
 * @tc.desc: Test Load & GetIdValues & GetIdValuesByName function in new resource module, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest007, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    std::string resPath = FormatFullPath(g_newResFilePath);
    const char *path = resPath.c_str();

    auto start = CurrentTimeUsec();
    auto pResource = HapResourceManager::GetInstance().Load(path, rc);
    auto cost = CurrentTimeUsec() - start;
    RESMGR_HILOGD(RESMGR_TAG, "load cost: %ld us.", cost);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    auto idValue = pResource->GetIdValuesByName("app_name", ResType::STRING);
    int id = idValue->GetLimitPathsConst()[0]->GetIdItem()->id_;
    start = CurrentTimeUsec();
    auto idValues = pResource->GetIdValues(id);
    cost = CurrentTimeUsec() - start;
    EXPECT_EQ(static_cast<size_t>(3), idValues->GetLimitPathsConst().size());
    RESMGR_HILOGD(RESMGR_TAG, "GetIdValues by id cost: %ld us.", cost);
    PrintIdValues(idValues);
}

/*
 * load a hap, get a value which is ref
 * @tc.name: HapResourceFuncTest008
 * @tc.desc: Test GetIdValuesByName function in new resource module, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceFuncTest008, TestSize.Level1)
{
    auto start = CurrentTimeUsec();
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    auto pResource = HapResourceManager::GetInstance().Load(FormatFullPath(g_newResFilePath).c_str(), rc);
    auto cost = CurrentTimeUsec() - start;
    RESMGR_HILOGD(RESMGR_TAG, "load cost: %ld us.", cost);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    auto idv = pResource->GetIdValuesByName(std::string("integer_ref"), ResType::INTEGER);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("string_ref"), ResType::STRING);
    PrintIdValues(idv);

    // ref propagation
    idv = pResource->GetIdValuesByName(std::string("string_ref2"), ResType::STRING);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("boolean_ref"), ResType::BOOLEAN);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("color_ref"), ResType::COLOR);
    PrintIdValues(idv);

    idv = pResource->GetIdValuesByName(std::string("float_ref"), ResType::FLOAT);
    PrintIdValues(idv);

    // ref in array ,
    idv = pResource->GetIdValuesByName(std::string("intarray_1"), ResType::INTARRAY);
    PrintIdValues(idv);

    // "parent":   was ref too
    idv = pResource->GetIdValuesByName(std::string("child"), ResType::PATTERN);
    PrintIdValues(idv);
}

/*
 * @tc.name: HapResourcePutAndGetResourceTest001
 * @tc.desc: Test HapResourceManager::PutAndGetResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourcePutAndGetResourceTest001, TestSize.Level1)
{
    std::shared_ptr<HapResource> pResource1 = std::make_shared<HapResourceV1>("test1", 1000, nullptr, false, false);
    ASSERT_TRUE(pResource1 != nullptr);
    std::shared_ptr<HapResource> pResource2 =
        HapResourceManager::GetInstance().PutAndGetResource("test1", pResource1);
    EXPECT_EQ(pResource2, pResource1);
    std::shared_ptr<HapResource> pResource3 = std::make_shared<HapResourceV1>("test1", 1000, nullptr, false, false);
    ASSERT_TRUE(pResource3 != nullptr);
    std::shared_ptr<HapResource> pResource4 =
        HapResourceManager::GetInstance().PutAndGetResource("test1", pResource3);
    EXPECT_EQ(pResource4, pResource1);
}

/*
 * @tc.name: HapResourcePutAndGetResourceTest002
 * @tc.desc: Test HapResourceManager::PutAndGetResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourcePutAndGetResourceTest002, TestSize.Level1)
{
    std::shared_ptr<HapResource> pResource1 = std::make_shared<HapResourceV2>("test1", 1000);
    ASSERT_TRUE(pResource1 != nullptr);
    std::shared_ptr<HapResource> pResource2 =
        HapResourceManager::GetInstance().PutAndGetResource("test1", pResource1);
    EXPECT_EQ(pResource2, pResource1);
    std::shared_ptr<HapResource> pResource3 = std::make_shared<HapResourceV2>("test1", 1000);
    ASSERT_TRUE(pResource3 != nullptr);
    std::shared_ptr<HapResource> pResource4 =
        HapResourceManager::GetInstance().PutAndGetResource("test1", pResource3);
    EXPECT_EQ(pResource4, pResource1);
}

/*
 * @tc.name: HapResourcePutAndGetResourceTest003
 * @tc.desc: Test HapResourceManager::PutAndGetResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourcePutAndGetResourceTest003, TestSize.Level1)
{
    std::shared_ptr<HapResource> pResource1 = std::make_shared<HapResourceV1>("test1", 1000, nullptr, false, false);
    ASSERT_TRUE(pResource1 != nullptr);
    std::shared_ptr<HapResource> pResource2 =
        HapResourceManager::GetInstance().PutAndGetResource("test1", pResource1);
    EXPECT_EQ(pResource2, pResource1);
    std::shared_ptr<HapResource> pResource3 = std::make_shared<HapResourceV2>("test2", 1000);
    ASSERT_TRUE(pResource3 != nullptr);
    std::shared_ptr<HapResource> pResource4 =
        HapResourceManager::GetInstance().PutAndGetResource("test2", pResource3);
    EXPECT_EQ(pResource4, pResource3);
}

/*
 * @tc.name: HapResourcePutPatchResourceTest001
 * @tc.desc: Test HapResourceManager::PutPatchResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourcePutPatchResourceTest001, TestSize.Level1)
{
    std::string path = "test";
    std::string patchPath = "testPatch";
    bool result = HapResourceManager::GetInstance().PutPatchResource(path, patchPath);
    ASSERT_FALSE(result);
}

/*
 * @tc.name: HapResourceGetLocaleTest001
 * @tc.desc: Test GetLocale function.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceGetLocaleTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    std::string resPath = FormatFullPath(g_resFilePath);
    const char *path = resPath.c_str();

    auto pResource = HapResourceManager::GetInstance().Load(path, rc);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    std::set<std::string> locales;
    pResource->GetLocales(locales, false);
    EXPECT_EQ(locales.size(), 4);
}

/*
 * @tc.name: HapResourceGetLocaleTest002
 * @tc.desc: Test GetLocale function in new module.
 * @tc.type: FUNC
 */
HWTEST_F(HapResourceTest, HapResourceGetLocaleTest002, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "US");
    std::string resPath = FormatFullPath(g_newResFilePath);
    const char *path = resPath.c_str();

    auto pResource = HapResourceManager::GetInstance().Load(path, rc);

    if (pResource == nullptr) {
        ASSERT_TRUE(false);
    }

    std::set<std::string> locales;
    pResource->GetLocales(locales, false);
    EXPECT_EQ(locales.size(), 4);
}
}
