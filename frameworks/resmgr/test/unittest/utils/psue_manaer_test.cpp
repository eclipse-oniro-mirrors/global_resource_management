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

#include <gtest/gtest.h>
#include "utils/psue_manager.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

namespace {
class PsueManagerTest : public testing::Test {
public:
    void SetUp();

private:
    std::shared_ptr<PsueManager> psueManager;
};

void PsueManagerTest::SetUp()
{
    psueManager = std::make_shared<PsueManager>();
}

HWTEST_F(PsueManagerTest, TestBidirectionConvertFunc001, TestSize.Level1)
{
    std::string noAsciiStr = "Hello, 世界!";
    std::string asciiStr = "123qwe 0.*/";
    EXPECT_EQ(psueManager->BidirectionConvert(asciiStr), "\u200F\u202E123qwe\u202C\u200F \u200F\u202E0.*/\u202C\u200F");
    EXPECT_EQ(psueManager->BidirectionConvert(noAsciiStr), noAsciiStr);
}

HWTEST_F(PsueManagerTest, TestConvertFunc001, TestSize.Level1)
{
    std::string noAsciiStr = "Hello, 世界!";
    std::string longStr = "abcdefghijklmnopqrstuvwxyz0123456789 abcdefghijklmnopqrstuvwxyz0123456789";
    std::string outValue = "";
    psueManager->Convert("123", outValue);
    EXPECT_EQ(outValue, "");
    psueManager->Convert("abc", outValue);
    EXPECT_EQ(outValue, "[àbćReÇÉÄß]");
    outValue = "";
    psueManager->Convert("", outValue);
    EXPECT_EQ(outValue, "");
    psueManager->Convert(noAsciiStr, outValue);
    EXPECT_EQ(outValue, "[Helló, 世界!ReÇÉÄßÑ¿ÃóèìжД]");
    psueManager->SetFakeLocaleLevel(3);
    psueManager->Convert("abc {test} %dabc", outValue);
    EXPECT_EQ(psueManager->Convert("abc {test} %dabc", outValue), "");
}

/*
 * @tc.name: TestGetExtendRatio001
 * @tc.desc: Test GetExtendRatio function
 * @tc.type: FUNC
 */
HWTEST_F(PsueManagerTest, TestGetExtendRatio001, TestSize.Level1)
{
    int32_t len = 1000;
    float result = psueManager->GetExtendRatio(len);
    EXPECT_EQ(0.3f, result);
}
}  // namespace
