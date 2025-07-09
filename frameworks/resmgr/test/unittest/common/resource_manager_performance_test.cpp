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

#include "resource_manager_performance_test.h"

#include <chrono>
#include <climits>
#include <cstring>
#include <ctime>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

#define private public

#include "hap_parser.h"
#include "hap_parser_v1.h"
#include "hap_parser_v2.h"
#include "hap_resource.h"
#include "hap_resource_v1.h"
#include "hap_resource_v2.h"
#include "resource_manager.h"
#include "resource_manager_impl.h"
#include "test_common.h"
#include "utils/errors.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
using namespace std;
namespace {
static const char *PERFOR_FEIL_V2_PATH = "all/assets/entry/resourcesV2.index";
class ResourceManagerPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerPerformanceTest() : rm(nullptr)
    {}

    ~ResourceManagerPerformanceTest()
    {}

public:
    ResourceManager *rm;

    int GetResId(std::string name, ResType resType) const;
};

int ResourceManagerPerformanceTest::GetResId(std::string name, ResType resType) const
{
    auto idValues = ((ResourceManagerImpl *)rm)->hapManager_->GetResourceListByName(name.c_str(), resType);
    if (idValues.size() == 0) {
        return -1;
    }

    for (auto &idValue : idValues) {
        if (idValue->GetLimitPathsConst().size() > 0 && !idValue->GetLimitPathsConst()[0]->IsSystemResource()) {
            return idValue->GetLimitPathsConst()[0]->GetIdItem()->id_;
        }
    }
    return OBJ_NOT_FOUND;
}

void ResourceManagerPerformanceTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void ResourceManagerPerformanceTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerPerformanceTest::SetUp(void)
{
    // PerformanceTest need higher log level
    g_logLevel = LOG_INFO;
    this->rm = CreateResourceManager();
    if (rm == nullptr) {
        return;
    }
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, nullptr);
    rm->UpdateResConfig(*rc);
    delete rc;
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "AddResource failed. test would fail.");
    }
}

void ResourceManagerPerformanceTest::TearDown(void)
{
    if (this->rm != nullptr) {
        delete this->rm;
        this->rm = nullptr;
    }
}

int TestLoadFromNewIndex(const char *filePath)
{
    std::string pstr = FormatFullPath(filePath);
    auto start = std::chrono::high_resolution_clock::now();
    for (int k = 0; k < 1000; ++k) {
        HapParserV2 hapParser;
        if (!hapParser.Init(pstr.c_str())) {
            RESMGR_HILOGE(RESMGR_TAG, "ParseResHex failed!");
            return -1;
        }

        std::shared_ptr<HapResource> pResource = hapParser.GetHapResource(pstr.c_str(), false, false);
        if (pResource == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new HapResource failed when LoadFromIndex");
            return -1;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto readFilecost = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    RESMGR_HILOGD(RESMGR_TAG, "read index file cost 002: %lld us", readFilecost);
    return OK;
}

/*
 * @tc.name: ResourceManagerPerformanceFuncTest002
 * @tc.desc: Test UpdateResConfig
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest002, TestSize.Level1)
{
    long long total = 0;
    double average = 0;
    auto tmpRm = CreateResourceManager();
    if (tmpRm == nullptr) {
        ASSERT_TRUE(false);
    }
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
        delete tmpRm;
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_CAR);
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        tmpRm->UpdateResConfig(*rc);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    delete tmpRm;
    delete rc;
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 002: %f us", average);
    EXPECT_LT(average, 2000);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest003
 * @tc.desc: Test GetResConfig
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest003, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDeviceType(DeviceType::DEVICE_CAR);
    ResConfigImpl rci;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetResConfig(rci);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    delete rc;
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 003: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest004
 * @tc.desc: Test GetStringByID
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest004, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    vector<uint32_t> ids;
    int count = 2;
    for (int i = 0; i < count; ++i) {
        int id = GetResId(name[i], ResType::STRING);
        ASSERT_TRUE(id > 0);
        ids.push_back(static_cast<uint32_t>(id));
    }

    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringById(ids[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 004: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest005
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest005, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    int count = 2;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringByName(name[i].c_str(), outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 005: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest006
 * @tc.desc: Test GetStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest006, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"string_ref", "string_ref2"};
    int count = 2;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringByName(name[i].c_str(), outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 006: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest007
 * @tc.desc: Test GetStringFormatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest007, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    vector<uint32_t> ids;
    int count = 2;
    for (int i = 0; i < count; ++i) {
        int id = GetResId(name[i], ResType::STRING);
        ASSERT_TRUE(id > 0);
        ids.push_back(static_cast<uint32_t>(id));
    }

    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringFormatById(outValue, ids[i], 12);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 007: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest008
 * @tc.desc: Test GetStringFormatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest008, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"app_name", "title"};
    int count = 2;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetStringFormatByName(outValue, name[i].c_str(), 123);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 008: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest009
 * @tc.desc: Test GetStringArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest009, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("size", ResType::STRINGARRAY);
    ASSERT_TRUE(id > 0);

    std::vector<std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetStringArrayById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 009: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest010
 * @tc.desc: Test GetStringArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest010, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    std::vector<std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetStringArrayByName("size", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 010: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest011
 * @tc.desc: Test GetPatternById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest011, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"base", "child"};
    vector<uint32_t> ids;
    int count = 2;
    for (int i = 0; i < count; ++i) {
        int id = GetResId(name[i], ResType::PATTERN);
        ASSERT_TRUE(id > 0);
        ids.push_back(static_cast<uint32_t>(id));
    }
    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPatternById(ids[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 011: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest012
 * @tc.desc: Test GetPatternByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest012, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string name[] = {"base", "child"};
    int count = 2;
    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPatternByName(name[i].c_str(), outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 012: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest013
 * @tc.desc: Test GetPluralStringById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest013, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    int id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringById(id, quantity[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 013: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest014
 * @tc.desc: Test GetPluralStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest014, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringByName("eat_apple", quantity[i], outValue);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 014: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest015
 * @tc.desc: Test GetPluralStringByIdFormat
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest015, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    int id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringByIdFormat(outValue, id, quantity[i], quantity[i]);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 015: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest016
 * @tc.desc: Test GetPluralStringByNameFormat
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest016, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int quantity[] = {1, 100};
    int count = 2;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        for (int i = 0; i < count; ++i) {
            auto t1 = std::chrono::high_resolution_clock::now();
            rm->GetPluralStringByNameFormat(outValue, "eat_apple", quantity[i], quantity[i]);
            auto t2 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
    }
    average = total / (1000.0 * count);
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 016: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest017
 * @tc.desc: Test GetThemeById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest017, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("app_theme", ResType::THEME);
    ASSERT_TRUE(id > 0);

    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetThemeById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 017: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest018
 * @tc.desc: Test GetThemeByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest018, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    std::map<std::string, std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetThemeByName("app_theme", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 018: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest019
 * @tc.desc: Test GetBooleanById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest019, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("boolean_1", ResType::BOOLEAN);
    ASSERT_TRUE(id > 0);

    bool outValue = true;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetBooleanById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 019: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest020
 * @tc.desc: Test GetBooleanByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest020, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    bool outValue = true;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetBooleanByName("boolean_1", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 020: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest021
 * @tc.desc: Test GetIntegerById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest021, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("integer_1", ResType::INTEGER);
    ASSERT_TRUE(id > 0);

    int outValue = 0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntegerById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 021: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest022
 * @tc.desc: Test GetIntegerByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest022, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int outValue = 0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntegerByName("integer_ref", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 022: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest023
 * @tc.desc: Test GetFloatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest023, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("width_appBar_backButton_touchTarget", ResType::FLOAT);
    ASSERT_TRUE(id > 0);

    float outValue = 0.0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetFloatById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 023: %f us", average);
    EXPECT_LT(average, 220);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest024
 * @tc.desc: Test GetFloatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest024, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    float outValue = 0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetFloatByName("width_appBar_backButton_touchTarget", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 024: %f us", average);
    EXPECT_LT(average, 160);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest025
 * @tc.desc: Test GetIntArrayById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest025, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("intarray_1", ResType::INTARRAY);
    ASSERT_TRUE(id > 0);

    std::vector<int> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntArrayById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 025: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest026
 * @tc.desc: Test GetIntArrayByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest026, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    std::vector<int> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetIntArrayByName("intarray_1", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 026: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest027
 * @tc.desc: Test GetColorById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest027, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("divider_color", ResType::COLOR);
    ASSERT_TRUE(id > 0);

    uint32_t outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetColorById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 027: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest028
 * @tc.desc: Test GetColorByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest028, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    uint32_t outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetColorByName("divider_color", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 028: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest029
 * @tc.desc: Test GetProfileById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest029, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("test_profile", ResType::PROF);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetProfileById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 029: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest030
 * @tc.desc: Test GetProfileByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest030, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetProfileByName("test_common", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 030: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest031
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest031, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);

    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 031: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest032
 * @tc.desc: Test GetMediaByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest032, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaByName("icon", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 032: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest033
 * @tc.desc: Test GetResConfigById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest033, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("app_name", ResType::STRING);
    ASSERT_TRUE(id > 0);
    ResConfigImpl rci;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetResConfigById(id, rci);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 033: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest034
 * @tc.desc: Test GetResConfigByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest034, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    ResConfigImpl rci;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetResConfigByName("app_name", ResType::STRING, rci);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 034: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest037
 * @tc.desc: Test GetMediaDataById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest037, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaDataById(id, len, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 037: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest038
 * @tc.desc: Test GetMediaDataByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest038, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaDataByName("icon", len, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 038: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest039
 * @tc.desc: Test GetMediaBase64DataById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest039, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaBase64DataById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 039: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest040
 * @tc.desc: Test GetMediaBase64DataByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest040, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetMediaBase64DataByName("icon", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 040: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest041
 * @tc.desc: Test GetProfileDataById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest041, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("test_profile", ResType::PROF);
    EXPECT_TRUE(id > 0);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetProfileDataById(id, len, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 041: %f us", average);
    EXPECT_LT(average, 260);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest042
 * @tc.desc: Test GetProfileDataByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest042, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetProfileDataByName("test_profile", len, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 042: %f us", average);
    EXPECT_LT(average, 260);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest045
 * @tc.desc: Test IsLoadHap
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest045, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    unsigned long long total = 0;
    double average = 0;
    std::string hapPath(FormatFullPath(g_hapPath));
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->IsLoadHap(hapPath);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 045: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest047
 * @tc.desc: Test GetDrawableInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest047, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    int id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    std::string type;
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetDrawableInfoById(id, type, len, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 047: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest048
 * @tc.desc: Test GetDrawableInfoByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest048, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    std::string type;
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetDrawableInfoByName(std::string("icon").c_str(), type, len, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 048: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest049
 * @tc.desc: Test GetResourceLimitKeys
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest049, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetResourceLimitKeys();
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 049: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest051
 * @tc.desc: Test GetResId
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest051, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    uint32_t resId;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetResId("app.string.app_name", resId);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 051: %f us", average);
    EXPECT_LT(average, 380);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest052
 * @tc.desc: Test GetLocales
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest052, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    std::vector<std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetLocales(outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 052: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest053
 * @tc.desc: Test GetSymbolById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest053, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    int id = GetResId("test_symbol", ResType::SYMBOL);
    ASSERT_TRUE(id > 0);
    unsigned long long total = 0;
    double average = 0;
    uint32_t outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetSymbolById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 053: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest054
 * @tc.desc: Test GetSymbolByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest054, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    uint32_t outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetSymbolByName(std::string("test_symbol").c_str(), outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 054: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest056
 * @tc.desc: Test GetStringFormatById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest056, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    int id = GetResId("test_string2", ResType::STRING);
    ASSERT_TRUE(id > 0);

    unsigned long long total = 0;
    double average = 0;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetStringFormatById(outValue, id, 1.00001, "你好");
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 056: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest057
 * @tc.desc: Test GetStringFormatByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest057, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    std::string outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetStringFormatByName(outValue, std::string("test_string2").c_str(), 1.0001, "你好");
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 057: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest058
 * @tc.desc: Test GetFormatPluralStringById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest058, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    int id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);

    unsigned long long total = 0;
    double average = 0;
    double quantity = 1;
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    params.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(quantity)));
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetFormatPluralStringById(outValue, id, quantity, params);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 058: %f us", average);
    EXPECT_LT(average, 200);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest059
 * @tc.desc: Test GetFormatPluralStringByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest059, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    unsigned long long total = 0;
    double average = 0;
    double quantity = 1;
    std::string outValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> params;
    params.push_back(std::make_tuple(ResourceManager::NapiValueType::NAPI_NUMBER, std::to_string(quantity)));
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetFormatPluralStringByName(outValue, std::string("eat_apple").c_str(), quantity, params);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 059: %f us", average);
    EXPECT_LT(average, 200);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest060
 * @tc.desc: Test GetThemeDataByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest060, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    unsigned long long total = 0;
    double average = 0;
    std::map<std::string, ResourceManager::ResData> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetThemeDataByName("ohos_device_theme", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 060: %f us", average);
    EXPECT_LT(average, 150);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest061
 * @tc.desc: Test GetThemeDataById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest061, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    int id = GetResId("ohos_device_theme", ResType::THEME);
    ASSERT_TRUE(id > 0);
    unsigned long long total = 0;
    double average = 0;
    std::map<std::string, ResourceManager::ResData> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetThemeDataById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 061: %f us", average);
    EXPECT_LT(average, 150);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest062
 * @tc.desc: Test GetPatternDataByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest062, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    unsigned long long total = 0;
    double average = 0;
    std::map<std::string, ResourceManager::ResData> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetPatternDataByName("ohos_test_button_pattern", outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 062: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest063
 * @tc.desc: Test GetPatternDataById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest063, TestSize.Level1)
{
    if (rm == nullptr) {
        ASSERT_TRUE(false);
    }
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    int id = GetResId("ohos_test_button_pattern", ResType::PATTERN);
    ASSERT_TRUE(id > 0);
    unsigned long long total = 0;
    double average = 0;
    std::map<std::string, ResourceManager::ResData> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        rm->GetPatternDataById(id, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    g_logLevel = LOG_DEBUG;
    RESMGR_HILOGD(RESMGR_TAG, "avg cost 063: %f us", average);
    EXPECT_LT(average, 100);
};

/*
 * @tc.name: ResourceManagerPerformanceFuncTest064
 * @tc.desc: Test AddResource
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerPerformanceTest, ResourceManagerPerformanceFuncTest064, TestSize.Level1)
{
    int ret = TestLoadFromNewIndex(PERFOR_FEIL_V2_PATH);
    EXPECT_EQ(OK, ret);
};
}
