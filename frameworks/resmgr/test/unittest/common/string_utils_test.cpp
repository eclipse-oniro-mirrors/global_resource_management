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

#include "string_utils_test.h"
#include <climits>
#include <gtest/gtest.h>
#include <thread>
#include "test_common.h"
#include "utils/string_utils.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class StringUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void StringUtilsTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void StringUtilsTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void StringUtilsTest::SetUp()
{
    // step 3: input testcase setup step
}

void StringUtilsTest::TearDown()
{
    // step 4: input testcase teardown step
}

/*
 * @tc.name: StringUtilsFuncTest001
 * @tc.desc: Test FormatString, none file case.
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, StringUtilsFuncTest001, TestSize.Level1)
{
    std::string result = FormatString("%d", 10001);
    EXPECT_EQ("10001", result);

    result = FormatString("I'm %s, I'm %d", "cici", 5);
    EXPECT_EQ("I'm cici, I'm 5", result);
}

void FuncMultiThread(int* num, std::mutex* mutex)
{
    std::lock_guard<std::mutex> lock(*mutex);
    auto tmp = (*num);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    (*num) = tmp + 1;
}

void TestThread(int* num, int threadNum, std::mutex* mutex)
{
    std::vector<std::thread> threads;
    for (int i = 0; i < threadNum; ++i) {
        threads.push_back(std::thread(FuncMultiThread, num, mutex));
    }
    for (auto &thread : threads) {
        thread.join();
    }
}

/*
 * @tc.name: LockFuncTest001
 * @tc.desc: Test lock, none file case.
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, LockFuncTest001, TestSize.Level1)
{
    int num = 0;
    int threadNum = 3;
    int result = num + threadNum;
    std::mutex mutex;
    TestThread(&num, threadNum, &mutex);
    EXPECT_EQ(result, num);
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest001
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest001, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo(nullptr, nullptr, "CN");
    std::string inputValue;
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "1" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest002
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest002, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("", nullptr, "CN");
    std::string inputValue = "length is %d";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "1" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest003
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest003, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "");
    std::string inputValue = "length is %d";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "1" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "length is 1");

    inputValue = "length is %d";
    rc.SetLocaleInfo("zh", nullptr, "CN");
    ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "length is 1");
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest004
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest004, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    std::string inputValue = "percent is %f";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "0.5" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "percent is 0.5");
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest005
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest005, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    std::string inputValue = "percent is %1$.2f, count is %2$d, name is %3$s";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "59.557" },
        { ResourceManager::NapiValueType::NAPI_NUMBER, "100" },
        { ResourceManager::NapiValueType::NAPI_STRING, "hello" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "percent is 59.56, count is 100, name is hello");
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest006
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest006, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    std::string inputValue = "percent is %.2f, count is %d, name is %s";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "59.557" },
        { ResourceManager::NapiValueType::NAPI_NUMBER, "100" },
        { ResourceManager::NapiValueType::NAPI_STRING, "hello" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "percent is 59.56, count is 100, name is hello");
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest007
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest007, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    std::string inputValue = "percent is %%f, count is %d, name is %s";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "59.557" },
        { ResourceManager::NapiValueType::NAPI_NUMBER, "100" },
        { ResourceManager::NapiValueType::NAPI_STRING, "hello" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest008
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest008, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    std::string inputValue = "percent is %%f, count is %2$d, name is %3$s";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "59.557" },
        { ResourceManager::NapiValueType::NAPI_NUMBER, "100" },
        { ResourceManager::NapiValueType::NAPI_STRING, "hello" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "percent is %f, count is 100, name is hello");
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest009
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest009, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    std::string inputValue = "percent is %%f, count is %1$d, name is %2$s";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "100" },
        { ResourceManager::NapiValueType::NAPI_STRING, "hello" }
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "percent is %f, count is 100, name is hello");
}

/*
 * @tc.name: ReplacePlaceholderWithParamsTest010
 * @tc.desc: Test ReplacePlaceholderWithParams function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ReplacePlaceholderWithParamsTest010, TestSize.Level1)
{
    ResConfigImpl rc;
    rc.SetLocaleInfo("zh", nullptr, "CN");
    std::string inputValue = "percent is %.0f";
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams = {
        { ResourceManager::NapiValueType::NAPI_NUMBER, "59.557" },
    };
    bool ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "percent is 60");

    inputValue = "percent is %.6f";
    ret = ReplacePlaceholderWithParams(inputValue, rc, jsParams);
    EXPECT_TRUE(ret);
    EXPECT_EQ(inputValue, "percent is 59.557000");
}

/*
 * @tc.name: FormatStringTest001
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, FormatStringTest001, TestSize.Level1)
{
    std::string result = FormatString(nullptr);
    EXPECT_EQ(result, "");

    va_list args;
    result = FormatString(nullptr, args);
    EXPECT_EQ(result, "");
}

bool TestParseArgs(const std::string inputOutputValue, ...)
{
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    va_list args;
    va_start(args, inputOutputValue);
    bool ret = parseArgs(inputOutputValue, args, jsParams);
    va_end(args);
    return ret;
}

/*
 * @tc.name: ParseArgsTest001
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ParseArgsTest001, TestSize.Level1)
{
    std::string inputOutputValue = "";
    bool ret = TestParseArgs(inputOutputValue);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: ParseArgsTest002
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ParseArgsTest002, TestSize.Level1)
{
    std::string inputOutputValue = "%184467440737095516150$d";
    bool ret = TestParseArgs(inputOutputValue);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ParseArgsTest003
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ParseArgsTest003, TestSize.Level1)
{
    std::string inputOutputValue = "%0$d";
    bool ret = TestParseArgs(inputOutputValue);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ParseArgsTest004
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ParseArgsTest004, TestSize.Level1)
{
    std::string inputOutputValue = "%1$d%s";
    bool ret = TestParseArgs(inputOutputValue, "1", "2");
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ParseArgsTest005
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ParseArgsTest005, TestSize.Level1)
{
    std::string inputOutputValue = "%2$d";
    bool ret = TestParseArgs(inputOutputValue, 1);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ParseArgsTest006
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ParseArgsTest006, TestSize.Level1)
{
    std::string inputOutputValue = "%18446744073709551615$d";
    bool ret = TestParseArgs(inputOutputValue);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ParseArgsTest007
 * @tc.desc: Test FormatString function
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, ParseArgsTest007, TestSize.Level1)
{
    std::string inputOutputValue = "%18446744073709551616$d";
    bool ret = TestParseArgs(inputOutputValue);
    EXPECT_FALSE(ret);
}
}