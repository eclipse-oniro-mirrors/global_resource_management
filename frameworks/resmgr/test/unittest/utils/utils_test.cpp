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

HWTEST_F(UtilsTest, TestConvertColorToUInt32Func, TestSize.Level1)
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
}  // namespace
