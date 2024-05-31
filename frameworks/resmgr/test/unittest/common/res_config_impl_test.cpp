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
#include "res_config_impl_test.h"

#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "res_config_impl.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
class ResConfigImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ResConfigImplTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void ResConfigImplTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResConfigImplTest::SetUp()
{
}

void ResConfigImplTest::TearDown()
{
}

std::shared_ptr<ResConfigImpl> CreateResConfigImpl(const char *language, const char *script, const char *region)
{
    auto resConfigImpl = std::make_shared<ResConfigImpl>();
    resConfigImpl->SetLocaleInfo(language, script, region);
    resConfigImpl->SetAppDarkRes(true);
    return resConfigImpl;
}

/*
 * @tc.name: ResConfigImplMatchTest001
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest001, TestSize.Level1)
{
    auto other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto current = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest002
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest002, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, "CA");
    auto current = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));

    Locale locale;
    locale = GetLocale("en", nullptr, "CA");
    current->SetPreferredLocaleInfo(locale);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest003
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest003, TestSize.Level1)
{
    auto other = CreateResConfigImpl("tl", nullptr, "PH");
    auto current = CreateResConfigImpl("fil", nullptr, "PH");
    EXPECT_TRUE(current->Match(other));

    Locale locale;
    locale = GetLocale("en", nullptr, "CA");
    current->SetPreferredLocaleInfo(locale);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest004
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest004, TestSize.Level1)
{
    auto other = CreateResConfigImpl("qaa", nullptr, nullptr);
    auto current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest005
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest005, TestSize.Level1)
{
    auto other = CreateResConfigImpl("qaa", nullptr, "CA");
    auto current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest006
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest006, TestSize.Level1)
{
    auto other = CreateResConfigImpl("az", nullptr, nullptr);
    auto current = CreateResConfigImpl("az", "Latn", nullptr);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest007
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest007, TestSize.Level1)
{
    auto other = CreateResConfigImpl("az", nullptr, "IR");
    auto current = CreateResConfigImpl("az", "Arab", nullptr);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest008
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest008, TestSize.Level1)
{
    auto other = CreateResConfigImpl("ar", nullptr, "EG");
    auto current = CreateResConfigImpl("ar", nullptr, "TN");
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest009
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest009, TestSize.Level1)
{
    auto other = CreateResConfigImpl("qaa", "Latn", "FR");
    auto current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));

    Locale locale;
    locale = GetLocale("qaa", "Latn", "FR");
    current->SetPreferredLocaleInfo(locale);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest010
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest010, TestSize.Level1)
{
    auto other = CreateResConfigImpl("qaa", nullptr, "FR");
    auto current = CreateResConfigImpl("qaa", "Latn", "CA");
    EXPECT_FALSE(current->Match(other));

    Locale locale;
    locale = GetLocale("qaa", nullptr, "FR");
    current->SetPreferredLocaleInfo(locale);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest011
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest011, TestSize.Level1)
{
    auto other = CreateResConfigImpl("az", nullptr, nullptr);
    auto current = CreateResConfigImpl("az", "Cyrl", nullptr);
    EXPECT_FALSE(current->Match(other));

    Locale locale;
    locale = GetLocale("az", nullptr, nullptr);
    current->SetPreferredLocaleInfo(locale);
    EXPECT_TRUE(current->Match(other));

    locale = GetLocale("qaa", "Latn", "CA");
    current->SetPreferredLocaleInfo(locale);
    EXPECT_FALSE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest012
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest012, TestSize.Level1)
{
    auto other = CreateResConfigImpl("az", nullptr, nullptr);
    auto current = CreateResConfigImpl("az", nullptr, "IR");
    EXPECT_FALSE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest013
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest013, TestSize.Level1)
{
    auto other = CreateResConfigImpl("qaa", nullptr, "FR");
    auto current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest014
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest014, TestSize.Level1)
{
    auto other = CreateResConfigImpl("he", nullptr, nullptr);
    auto current = CreateResConfigImpl("iw", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
};

/*
 * @tc.name: ResConfigImplMatchTest015
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest015, TestSize.Level1)
{
    auto other = CreateResConfigImpl("ji", nullptr, nullptr);
    auto current = CreateResConfigImpl("yi", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
};

/*
 * @tc.name: ResConfigImplMatchTest016
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest016, TestSize.Level1)
{
    auto other = CreateResConfigImpl("jw", nullptr, nullptr);
    auto current = CreateResConfigImpl("jv", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
};

/*
 * @tc.name: ResConfigImplMatchTest017
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest017, TestSize.Level1)
{
    auto other = CreateResConfigImpl("in", nullptr, nullptr);
    auto current = CreateResConfigImpl("id", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
};

/*
 * @tc.name: ResConfigImplMatchTest018
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest018, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", "Latn", nullptr);
    auto current = CreateResConfigImpl("en", "Qaag", nullptr);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest019
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest019, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDeviceType(DeviceType::DEVICE_CAR);
    other->SetDeviceType(DeviceType::DEVICE_CAR);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest020
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest020, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDeviceType(DeviceType::DEVICE_CAR);
    other->SetDeviceType(DeviceType::DEVICE_PAD);
    EXPECT_FALSE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest021
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest021, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    other->SetDeviceType(DeviceType::DEVICE_PAD);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest022
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest022, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDeviceType(DeviceType::DEVICE_PAD);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest023
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest023, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest024
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest024, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetDirection(Direction::DIRECTION_VERTICAL);
    EXPECT_FALSE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest025
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest025, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest026
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest026, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest027
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest027, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_VERTICAL);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest028
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest028, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest029
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest029, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_FALSE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest030
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest030, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetColorMode(ColorMode::LIGHT);
    other->SetColorMode(ColorMode::DARK);
    EXPECT_FALSE(current->Match(other));
    EXPECT_FALSE(other->Match(current));
};

/*
 * @tc.name: ResConfigImplMatchTest031
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest031, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetColorMode(ColorMode::LIGHT);
    other->SetColorMode(ColorMode::LIGHT);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest032
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest032, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetColorMode(ColorMode::LIGHT);
    other->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
};

/*
 * @tc.name: ResConfigImplMatchTest033
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest033, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetColorMode(ColorMode::DARK);
    other->SetColorMode(ColorMode::DARK);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest034
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest034, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetColorMode(ColorMode::DARK);
    other->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
};

/*
 * @tc.name: ResConfigImplMatchTest035
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest035, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    other->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest036
 * @tc.desc: Test set preferred locale ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest036, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, "CA");
    auto current = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));

    Locale locale;
    locale = GetLocale(nullptr, nullptr, nullptr);
    current->SetPreferredLocaleInfo(locale);
    EXPECT_FALSE(current->Match(other));

    locale = GetLocale("fr", nullptr, "CA");
    current->SetPreferredLocaleInfo(locale);
    EXPECT_FALSE(current->Match(other));

    locale = GetLocale("en", nullptr, "CA");
    current->SetPreferredLocaleInfo(locale);
    EXPECT_TRUE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplMatchTest037
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest037, TestSize.Level1)
{
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetColorMode(ColorMode::DARK);
    current->SetAppDarkRes(false);
    other->SetColorMode(ColorMode::DARK);
    EXPECT_FALSE(current->Match(other));
};

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest001
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest001, TestSize.Level1)
{
    auto request = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto current = CreateResConfigImpl("fr", nullptr, "FR");
    auto other = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest002
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest002, TestSize.Level1)
{
    auto request = CreateResConfigImpl("fr", nullptr, "CA");
    auto current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));

    Locale locale;
    locale = GetLocale(nullptr, nullptr, nullptr);
    request->SetPreferredLocaleInfo(locale);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest003
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest003, TestSize.Level1)
{
    auto request = CreateResConfigImpl("fr", nullptr, "CA");
    auto current = CreateResConfigImpl("fr", nullptr, "FR");
    auto other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest004
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest004, TestSize.Level1)
{
    auto request = CreateResConfigImpl("fil", nullptr, "PH");
    auto current = CreateResConfigImpl("tl", nullptr, "PH");
    auto other = CreateResConfigImpl("fil", nullptr, "US");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));

    Locale locale;
    locale = GetLocale("fil", nullptr, "US");
    request->SetPreferredLocaleInfo(locale);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest005
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest005, TestSize.Level1)
{
    auto request = CreateResConfigImpl("fil", nullptr, "PH");
    auto current = CreateResConfigImpl("fil", nullptr, "PH");
    auto other = CreateResConfigImpl("tl", nullptr, "PH");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest006
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest006, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, "419");
    auto other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest007
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest007, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, "AR");
    auto other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest008
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest008, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, "419");
    auto other = CreateResConfigImpl("es", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest009
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest009, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, "419");
    auto other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest010
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest010, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, nullptr);
    auto other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest011
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest011, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, "PE");
    auto other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));

    Locale locale;
    locale = GetLocale("es", nullptr, "ES");
    request->SetPreferredLocaleInfo(locale);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest012
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest012, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, "AR");
    auto other = CreateResConfigImpl("es", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest013
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest013, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "AR");
    auto current = CreateResConfigImpl("es", nullptr, "US");
    auto other = CreateResConfigImpl("es", nullptr, "BO");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest014
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest014, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "IC");
    auto current = CreateResConfigImpl("es", nullptr, "ES");
    auto other = CreateResConfigImpl("es", nullptr, "GQ");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest015
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest015, TestSize.Level1)
{
    auto request = CreateResConfigImpl("es", nullptr, "GQ");
    auto current = CreateResConfigImpl("es", nullptr, "IC");
    auto other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest016
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest016, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "GB");
    auto current = CreateResConfigImpl("en", nullptr, "001");
    auto other = CreateResConfigImpl("en", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest017
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest017, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "PR");
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    auto other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest018
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest018, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "DE");
    auto current = CreateResConfigImpl("en", nullptr, "150");
    auto other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest019
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest019, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "IN");
    auto current = CreateResConfigImpl("en", nullptr, "AU");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest020
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest020, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "PR");
    auto current = CreateResConfigImpl("en", nullptr, "001");
    auto other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest021
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest021, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "IN");
    auto current = CreateResConfigImpl("en", nullptr, "GB");
    auto other = CreateResConfigImpl("en", nullptr, "AU");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest022
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest022, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "IN");
    auto current = CreateResConfigImpl("en", nullptr, "AU");
    auto other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest023
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest023, TestSize.Level1)
{
    auto request = CreateResConfigImpl("pt", nullptr, "MZ");
    auto current = CreateResConfigImpl("pt", nullptr, "PT");
    auto other = CreateResConfigImpl("pt", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest024
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest024, TestSize.Level1)
{
    auto request = CreateResConfigImpl("pt", nullptr, "MZ");
    auto current = CreateResConfigImpl("pt", nullptr, "PT");
    auto other = CreateResConfigImpl("pt", nullptr, "BR");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest025
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest025, TestSize.Level1)
{
    auto request = CreateResConfigImpl("zh", "Hant", "MO");
    auto current = CreateResConfigImpl("zh", "Hant", "HK");
    auto other = CreateResConfigImpl("zh", "Hant", "TW");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));

    Locale locale;
    locale = GetLocale("zh", "Hant", "US");
    request->SetPreferredLocaleInfo(locale);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest026
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest026, TestSize.Level1)
{
    auto request = CreateResConfigImpl("zh", "Hant", "US");
    auto current = CreateResConfigImpl("zh", "Hant", "TW");
    auto other = CreateResConfigImpl("zh", "Hant", "HK");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest027
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest027, TestSize.Level1)
{
    auto request = CreateResConfigImpl("ar", nullptr, "DZ");
    auto current = CreateResConfigImpl("ar", nullptr, "015");
    auto other = CreateResConfigImpl("ar", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest028
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest028, TestSize.Level1)
{
    auto request = CreateResConfigImpl("ar", nullptr, "EG");
    auto current = CreateResConfigImpl("ar", nullptr, nullptr);
    auto other = CreateResConfigImpl("ar", nullptr, "015");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest029
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest029, TestSize.Level1)
{
    auto request = CreateResConfigImpl("ar", nullptr, "QA");
    auto current = CreateResConfigImpl("ar", nullptr, "EG");
    auto other = CreateResConfigImpl("ar", nullptr, "BH");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest030
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest030, TestSize.Level1)
{
    auto request = CreateResConfigImpl("ar", nullptr, "QA");
    auto current = CreateResConfigImpl("ar", nullptr, "SA");
    auto other = CreateResConfigImpl("ar", nullptr, "015");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest031
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest031, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest032
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest032, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest033
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest033, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "PR");
    auto current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest034
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest034, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    auto other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest035
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest035, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "PR");
    auto current = CreateResConfigImpl("en", nullptr, nullptr);
    auto other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest036
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest036, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto other = CreateResConfigImpl("en", nullptr, "PR");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest037
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest037, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "CN");
    auto current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest038
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest038, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", "Qaag", nullptr);
    auto current = CreateResConfigImpl("en", nullptr, "GB");
    auto other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));

    Locale locale;
    locale = GetLocale("en", nullptr, "CA");
    request->SetPreferredLocaleInfo(locale);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest039
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest039, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", "Qaag", nullptr);
    auto current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    auto other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest040
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest040, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "CN");
    auto other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest041
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest041, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest042
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest042, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest043
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest043, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest044
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest044, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest045
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest045, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_XLDPI / BASE_DPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_XXLDPI / BASE_DPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest046
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest046, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_MDPI / BASE_DPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_SDPI / BASE_DPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest047
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest047, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_SDPI / BASE_DPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest048
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest048, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest049
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest049, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest050
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest050, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_SDPI / BASE_DPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest051
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest051, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::LIGHT);
    current->SetColorMode(ColorMode::LIGHT);
    other->SetColorMode(ColorMode::LIGHT);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest052
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest052, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::LIGHT);
    current->SetColorMode(ColorMode::LIGHT);
    other->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest053
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest053, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::LIGHT);
    current->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    other->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest054
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest054, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::DARK);
    current->SetColorMode(ColorMode::DARK);
    other->SetColorMode(ColorMode::DARK);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest055
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest055, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::DARK);
    current->SetColorMode(ColorMode::DARK);
    other->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest056
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest056, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::DARK);
    current->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    other->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest057
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest057, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::LIGHT);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetColorMode(ColorMode::LIGHT);
    other->SetDeviceType(DeviceType::DEVICE_NOT_SET);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest058
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest058, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::LIGHT);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    current->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    other->SetColorMode(ColorMode::LIGHT);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_NOT_SET);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest059
 * @tc.desc: Test ResConfig IsMoreSuitable, match ColorMode earlier than InputDevice
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest059, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetColorMode(ColorMode::LIGHT);
    request->SetInputDevice(InputDevice::INPUTDEVICE_POINTINGDEVICE);
    current->SetColorMode(ColorMode::COLOR_MODE_NOT_SET);
    current->SetInputDevice(InputDevice::INPUTDEVICE_POINTINGDEVICE);
    other->SetColorMode(ColorMode::LIGHT);
    other->SetInputDevice(InputDevice::INPUTDEVICE_NOT_SET);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest060
 * @tc.desc: Test ResConfig IsMoreSuitable, match InputDevice earlier than ScreenDensity
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest060, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "US");
    auto current = CreateResConfigImpl("en", nullptr, "US");
    auto other = CreateResConfigImpl("en", nullptr, "US");
    request->SetInputDevice(InputDevice::INPUTDEVICE_POINTINGDEVICE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    current->SetInputDevice(InputDevice::INPUTDEVICE_NOT_SET);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI / BASE_DPI);
    other->SetInputDevice(InputDevice::INPUTDEVICE_POINTINGDEVICE);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_NOT_SET);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest061
 * @tc.desc: Test ResConfig IsMoreSuitable, match preferredLocale
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest061, TestSize.Level1)
{
    auto request = CreateResConfigImpl("en", nullptr, "IN");
    auto current = CreateResConfigImpl("en", nullptr, "AU");
    auto other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));

    Locale currentLocale;
    currentLocale = GetLocale("en", nullptr, "CA");
    request->SetPreferredLocaleInfo(currentLocale);
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
}
}