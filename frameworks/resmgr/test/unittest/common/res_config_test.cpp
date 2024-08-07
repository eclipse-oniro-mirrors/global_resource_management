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
#include "res_config_test.h"

#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "hap_resource.h"
#include "res_config.h"
#include "resource_manager_impl.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class ResConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ResConfigTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResConfigTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResConfigTest::SetUp()
{
}

void ResConfigTest::TearDown()
{
}

/*
 * @tc.name: ResConfigFuncTest001
 * @tc.desc: Test Config function, non file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigTest, ResConfigFuncTest001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("en", nullptr, "AU");
    auto current = std::make_shared<ResConfigImpl>();
    current->SetLocaleInfo("en", nullptr, "GB");
    auto target = std::make_shared<ResConfigImpl>();
    target->SetLocaleInfo("zh", nullptr, "CN");
    EXPECT_TRUE(rc->Match(current));
    EXPECT_TRUE(!(rc->Match(target)));
};

/*
 * @tc.name: TestScreenDensity001
 * @tc.desc: Test ScreenDensity.
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigTest, TestScreenDensity001, TestSize.Level1)
{
    auto rc = std::make_shared<ResConfigImpl>();
    rc->SetScreenDensityDpi(OHOS::Global::Resource::SCREEN_DENSITY_MDPI);
    EXPECT_EQ(OHOS::Global::Resource::SCREEN_DENSITY_MDPI, rc->GetScreenDensityDpi());
    EXPECT_FLOAT_EQ(1, rc->GetScreenDensity());
}

/*
 * @tc.name: TestCompleteScript001
 * @tc.desc: Test CompleteScript.
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigTest, TestCompleteScript001, TestSize.Level1)
{
    auto target = std::make_shared<ResConfigImpl>();
    target->SetLocaleInfo("zh", nullptr, "CN");
    target->CompleteScript();
    EXPECT_TRUE(target->IsCompletedScript());
    target->CompleteScript();
}
}