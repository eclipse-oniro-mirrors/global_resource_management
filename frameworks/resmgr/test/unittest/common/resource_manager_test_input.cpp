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

#include "resource_manager_test.h"
#include "system_resource_manager.h"

#include <cstdarg>
#include <fcntl.h>
#include "res_config.h"
#include "resource_manager_test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {

class ResourceManagerSecurityTest : public testing::Test {
public:
    static void SetUpTestCase(void) { g_logLevel = LOG_DEBUG; }
    static void TearDownTestCase(void) {}
    void SetUp()
    {
        this->rm = CreateResourceManager();
        this->rmc = new ResourceManagerTestCommon(rm);
    }
    void TearDown()
    {
        delete this->rm;
        delete this->rmc;
        if (SystemResourceManager::resourceManager_) {
            delete SystemResourceManager::resourceManager_;
            SystemResourceManager::resourceManager_ = nullptr;
        }
    }
    ResourceManager *rm;
    ResourceManagerTestCommon *rmc;
};

bool TestParseArgsWrapper(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string fmtStr(fmt);
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> jsParams;
    bool ret = ParseArgs(fmtStr, args, jsParams);
    va_end(args);
    return ret;
}

/*
 * @tc.name: ResMgrSecGetProfileDataByNameNullNameTest001
 * @tc.desc: Test GetProfileDataByName with null name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecGetProfileDataByNameNullNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    size_t len = 0;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetProfileDataByName(nullptr, len, outValue);
    EXPECT_EQ(state, ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResMgrSecGetDrawableInfoByNameNullNameTest001
 * @tc.desc: Test GetDrawableInfoByName (5-arg) with null name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecGetDrawableInfoByNameNullNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::string type;
    size_t len = 0;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetDrawableInfoByName(nullptr, type, len, outValue, 0);
    EXPECT_EQ(state, ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResMgrSecGetDrawableInfoByNameNullNameTest002
 * @tc.desc: Test GetDrawableInfoByName (6-arg) with null name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecGetDrawableInfoByNameNullNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::tuple<std::string, size_t, std::string> drawableInfo;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetDrawableInfoByName(nullptr, drawableInfo, outValue, 0, 0);
    EXPECT_EQ(state, ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResMgrSecGetMediaByNameNullNameTest001
 * @tc.desc: Test GetMediaByName with null name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecGetMediaByNameNullNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::string outValue;
    RState state = rm->GetMediaByName(nullptr, outValue);
    EXPECT_EQ(state, ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResMgrSecGetMediaDataByNameNullNameTest001
 * @tc.desc: Test GetMediaDataByName with null name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecGetMediaDataByNameNullNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    size_t len = 0;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetMediaDataByName(nullptr, len, outValue);
    EXPECT_EQ(state, ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResMgrSecGetMediaBase64DataByNameNullNameTest001
 * @tc.desc: Test GetMediaBase64DataByName with null name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecGetMediaBase64DataByNameNullNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::string outValue;
    RState state = rm->GetMediaBase64DataByName(nullptr, outValue);
    EXPECT_EQ(state, ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResMgrSecParseArgsNullCharPtrTest001
 * @tc.desc: Test ParseArgs with null char* for %s (paramsWithOutNum path)
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecParseArgsNullCharPtrTest001, TestSize.Level1)
{
    bool ret = TestParseArgsWrapper("hello %s", static_cast<char *>(nullptr));
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: ResMgrSecParseArgsNullCharPtrTest002
 * @tc.desc: Test ParseArgs with null char* for %1$s (paramsWithNum path)
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerSecurityTest, ResMgrSecParseArgsNullCharPtrTest002, TestSize.Level1)
{
    bool ret = TestParseArgsWrapper("hello %1$s", static_cast<char *>(nullptr));
    EXPECT_FALSE(ret);
}
} // namespace
