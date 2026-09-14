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

#include <fcntl.h>
#include "res_config.h"
#include "resource_manager_test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class ResourceManagerDeviceTest : public testing::Test {
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

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest003
 * @tc.desc: Test GetStringById, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByIdForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("zh_CN phone pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest004
 * @tc.desc: Test GetStringById, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByIdForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest001
 * @tc.desc: Test GetStringByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest002
 * @tc.desc: Test GetStringByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("default device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest003
 * @tc.desc: Test GetStringByName, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByNameForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("zh_CN phone pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest004
 * @tc.desc: Test GetStringByName, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByNameForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest005
 * @tc.desc: Test GetStringByName, not match zh_CN-phone-pointingdevice directory which devicetype not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByNameForInputDeviceTest005, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("point_test", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("zh_CN phone pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByNameForInputDeviceTest006
 * @tc.desc: Test GetStringByName,match base directory which pointingdevice directory not have res with name
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByNameForInputDeviceTest006, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    std::string outValue;
    rState = rm->GetStringByName("hello", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("Helloooo", outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest001
 * @tc.desc: Test GetStringByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetIntegerByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(888, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest002
 * @tc.desc: Test GetIntegerByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetIntegerByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(999, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest003
 * @tc.desc: Test GetIntegerByName, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetIntegerByNameForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(777, outValue);
}

/*
 * @tc.name: ResourceManagerGetIntegerByNameForInputDeviceTest004
 * @tc.desc: Test GetIntegerByName, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetIntegerByNameForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int outValue;
    rState = rm->GetIntegerByName("integer_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(888, outValue);
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameForInputDeviceTest001
 * @tc.desc: Test GetBooleanByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetBooleanByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    bool outValue;
    rState = rm->GetBooleanByName("boolean_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(true, outValue);
}

/*
 * @tc.name: ResourceManagerGetBooleanByNameForInputDeviceTest002
 * @tc.desc: Test GetBooleanByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetBooleanByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    bool outValue;
    rState = rm->GetBooleanByName("boolean_input_device", outValue);;
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ(false, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest001
 * @tc.desc: Test GetMediaByName, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetMediaByNameForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/pointingdevice/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest002
 * @tc.desc: Test GetMediaByName, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetMediaByNameForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/base/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest003
 * @tc.desc: Test GetMediaByName, to match zh_CN-phone-pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetMediaByNameForInputDeviceTest003, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/zh_CN-phone-pointingdevice/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameForInputDeviceTest004
 * @tc.desc: Test GetMediaByName, not match zh_CN-phone-pointingdevice directory which language not match
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetMediaByNameForInputDeviceTest004, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    std::string res;
    res.append("/data/test/all/assets/entry/resources/pointingdevice/media/input_device.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    std::string outValue;
    RState state;
    state = rm->GetMediaByName("input_device", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
}


/*
 * @tc.name: ResourceManagerResolveReferenceTest001
 * @tc.desc: Test ResolveReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerResolveReferenceTest001, TestSize.Level1)
{
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rm->UpdateResConfig(*rc);

    rm->AddResource(FormatFullPath(g_resFilePath).c_str());

    int id = rmc->GetResId("integer_1", ResType::INTEGER);
    std::string value(FormatString("$integer:%d", id));
    std::string outValue;
    RState ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("101"), outValue);

    std::string copyright("XXXXXX All rights reserved. ©2011-2019");
    id = rmc->GetResId("copyright_text", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(copyright, outValue);

    id = rmc->GetResId("string_ref", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(copyright, outValue);

    id = rmc->GetResId("boolean_1", ResType::BOOLEAN);
    value.assign(FormatString("$boolean:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("true"), outValue);

    id = rmc->GetResId("grey_background", ResType::COLOR);
    value.assign(FormatString("$color:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("#F5F5F5"), outValue);

    id = rmc->GetResId("aboutPage_minHeight", ResType::FLOAT);
    value.assign(FormatString("$float:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(std::string("707vp"), outValue);

    id = rmc->GetResId("base", ResType::PATTERN);
    value.assign(FormatString("$pattern:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    EXPECT_EQ(ERROR, ret);
}

/*
 * @tc.name: ResourceManagerResolveReferenceTest002
 * @tc.desc: Test ResolveReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerResolveReferenceTest002, TestSize.Level1)
{
    ResConfig *rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    rm->UpdateResConfig(*rc);
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());

    // reload
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rm->UpdateResConfig(*rc);
    delete rc;

    int id = rmc->GetResId("copyright_text", ResType::STRING);
    std::string value(FormatString("$string:%d", id));
    std::string outValue;
    RState ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string("版权所有 ©2011-2019 XXXX有限公司保留一切权利"), outValue.c_str());
    id = rmc->GetResId("string_ref", ResType::STRING);
    value.assign(FormatString("$string:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(SUCCESS, ret);
    ASSERT_EQ(std::string("$aaaaa"), outValue.c_str());

    // error case
    // wrong id
    value.assign(FormatString("$boolean:%d", NON_EXIST_ID));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(ERROR, ret);
    // wrong type
    id = rmc->GetResId("copyright_text", ResType::STRING);
    value.assign(FormatString("$boolean:%d", id));
    ret = ((ResourceManagerImpl *)rm)->ResolveReference(value, outValue);
    ASSERT_EQ(ERROR, ret);
}

/*
 * @tc.name: ResourceManagerResolveParentReferenceTest001
 * @tc.desc: Test ResolveParentReference function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerResolveParentReferenceTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    int id;
    std::map<std::string, std::string> outValue;
    RState ret;

    id = rmc->GetResId("base", ResType::PATTERN);
    EXPECT_TRUE(id > 0);
    auto idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);

    RESMGR_HILOGD(RESMGR_TAG, "=====");
    id = rmc->GetResId("child", ResType::PATTERN);
    idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);

    RESMGR_HILOGD(RESMGR_TAG, "=====");
    id = rmc->GetResId("ccchild", ResType::PATTERN);
    idItem = ((ResourceManagerImpl *)rm)->hapManager_->FindResourceById(id);
    ASSERT_TRUE(idItem != nullptr);
    ret = ((ResourceManagerImpl *)rm)->ResolveParentReference(idItem, outValue);
    ASSERT_EQ(SUCCESS, ret);
    PrintMapString(outValue);
}

/*
 * test res with same name in different resType
 * @tc.name: ResourceManagerSameNameTest001
 * @tc.desc: Test GetStringByName & GetBooleanByName & GetIntegerByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerSameNameTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    std::string outValue;
    std::string name;
    RState state;

    state = rm->GetStringByName("same_name", outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(std::string("StringSameName"), outValue);

    bool outValueB = true;
    state = rm->GetBooleanByName("same_name", outValueB);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(false, outValueB);

    int outValueI;
    state = rm->GetIntegerByName("same_name", outValueI);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(999, outValueI);
}

/*
 * @tc.name: ResourceManagerGetSymbolByIdTest001
 * @tc.desc: Test GetSymbolById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetSymbolByIdTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    uint32_t id = rmc->GetResId("test_symbol", ResType::SYMBOL);
    uint32_t outValue;
    RState state = rm->GetSymbolById(id, outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(983040, outValue);
}

/*
 * @tc.name: ResourceManagerGetSymbolByIdTest002
 * @tc.desc: Test GetSymbolById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetSymbolByIdTest002, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    uint32_t id = rmc->GetResId("app_name", ResType::STRING);
    uint32_t outValue;
    RState state = rm->GetSymbolById(id, outValue);
    EXPECT_TRUE(state == ERROR_CODE_RES_NOT_FOUND_BY_ID);

    id = 1992503;
    state = rm->GetSymbolById(id, outValue);
    EXPECT_TRUE(state == ERROR_CODE_RES_ID_NOT_FOUND);
}

/*
 * @tc.name: ResourceManagerGetSymbolByNameTest001
 * @tc.desc: Test GetSymbolByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetSymbolByNameTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    std::string name("test_symbol");
    uint32_t outValue;
    RState state = rm->GetSymbolByName(name.c_str(), outValue);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(983040, outValue);
}

/*
 * @tc.name: ResourceManagerGetSymbolByNameTest002
 * @tc.desc: Test GetSymbolByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetSymbolByNameTest002, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    std::string name("test_symbol_tmp");
    uint32_t outValue;
    RState state = rm->GetSymbolByName(name.c_str(), outValue);
    EXPECT_TRUE(state == ERROR_CODE_RES_NAME_NOT_FOUND);
}

/*
 * test get raw file path interface
 * @tc.name: RawFileTest001
 * @tc.desc: Test GetRawFilePathByName, file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, RawFileTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    rmc->TestGetRawFilePathByName("rawfile/test_rawfile.txt",
        "/data/test/all/assets/entry/resources/rawfile/test_rawfile.txt");

    rmc->TestGetRawFilePathByName("test_rawfile.txt",
        "/data/test/all/assets/entry/resources/rawfile/test_rawfile.txt");
}

/*
 * @tc.name: ResourceManagerCreateDefaultResConfigTest001
 * @tc.desc: Test CreateDefaultResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerCreateDefaultResConfigTest001, TestSize.Level1)
{
    ResConfig *rc = CreateDefaultResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    EXPECT_EQ(rc->GetColorMode(), COLOR_MODE_NOT_SET);
    delete(rc);
}

/*
 * @tc.name: ResourceManagerGetOverrideResourceManager001
 * @tc.desc: Test GetOverrideResourceManager function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetOverrideResourceManager001, TestSize.Level1)
{
    std::shared_ptr<ResConfigImpl> rc = std::make_shared<ResConfigImpl>();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    std::shared_ptr<ResourceManager> manager = rm->GetOverrideResourceManager(rc);
    EXPECT_TRUE(manager != nullptr);
}

/*
 * @tc.name: ResourceManagerGetOverrideResourceManager002
 * @tc.desc: Test GetOverrideResourceManager function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetOverrideResourceManager002, TestSize.Level1)
{
    std::shared_ptr<ResConfigImpl> rc = std::make_shared<ResConfigImpl>();
    rc->SetLocaleInfo("zh-CN");
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetDirection(DIRECTION_HORIZONTAL);
    rc->SetColorMode(DARK);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rc->SetMcc(460);
    rc->SetMnc(1);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI);
    std::shared_ptr<ResourceManager> manager = rm->GetOverrideResourceManager(rc);
    EXPECT_TRUE(manager != nullptr);
    bool addRet = manager->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(addRet);

    // test override resource manager
    ResourceManagerTestCommon *overrideRmc = new ResourceManagerTestCommon(manager);
    std::map<std::string, std::string> outValue;
    RState state;
    state = manager->GetPatternByName("child", outValue);
    EXPECT_EQ(state, SUCCESS);

    int id = overrideRmc->GetResId("child", ResType::PATTERN);
    state = manager->GetPatternById(id, outValue);
    EXPECT_EQ(state, SUCCESS);
    ASSERT_EQ(static_cast<size_t>(4), outValue.size());

    // not found case
    state = manager->GetPatternByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);

    delete overrideRmc;
}

/*
 * @tc.name: ResourceManagerGetOverrideResConfig001
 * @tc.desc: Test GetOverrideResConfig function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetOverrideResConfig001, TestSize.Level1)
{
    std::unique_ptr<ResConfig> config(CreateResConfig());
    rm->GetOverrideResConfig(*config);
    EXPECT_EQ(config->GetScreenDensity(), 0);
    config->SetScreenDensity(SCREEN_DENSITY_XXLDPI);
    RState state = rm->UpdateOverrideResConfig(*config);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerIsLoadHap001
 * @tc.desc: Test IsLoadHap function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerIsLoadHap001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::string fullPath = FormatFullPath(g_hapPath);
    RState state = rm->IsLoadHap(fullPath);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerIsLoadHap002
 * @tc.desc: Test IsLoadHap function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerIsLoadHap002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath("no_exist.hap").c_str());
    ASSERT_FALSE(ret);
    std::string fullPath = FormatFullPath(g_hapPath);
    RState state = rm->IsLoadHap(fullPath);
    EXPECT_TRUE(state != SUCCESS);
}

/*
 * @tc.name: ResourceManagerIsRawDirFromHap001
 * @tc.desc: Test IsRawDirFromHap function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerIsRawDirFromHap001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    RState state = rm->IsRawDirFromHap("test_rawfile.txt", ret);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerIsRawDirFromHap002
 * @tc.desc: Test IsRawDirFromHap function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerIsRawDirFromHap002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    RState state = rm->IsRawDirFromHap("no_test_rawfile.txt", ret);
    EXPECT_TRUE(state != SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetRawFileList001
 * @tc.desc: Test GetRawFile function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetRawFileList001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rawfileList;
    RState state = rm->GetRawFileList("", rawfileList);
    EXPECT_TRUE(state == SUCCESS);
}

/*
 * @tc.name: ResourceManagerGetRawFileList002
 * @tc.desc: Test GetRawFile function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetRawFileList002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_hapPath).c_str());
    ASSERT_TRUE(ret);
    std::vector<std::string> rawfileList;
    RState state = rm->GetRawFileList("no_exist_dir", rawfileList);
    EXPECT_EQ(state, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: CreateResourceManagerTest001
 * @tc.desc: Test CreateResourceManager function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, CreateResourceManagerTest001, TestSize.Level1)
{
    ResConfigImpl *rc = new ResConfigImpl;
    std::string hapPath;
    std::vector<std::string> overlayPath;
    int32_t appType = 1;
    int32_t userId = 100; // userId is 100
    std::shared_ptr<ResourceManager> bundleRm =
        CreateResourceManager("ohos.global.test.all", "entry", hapPath, overlayPath, *rc, appType, userId);
    EXPECT_TRUE(bundleRm == nullptr);

    bundleRm = CreateResourceManager("", "entry", hapPath, overlayPath, *rc, appType, userId);
    EXPECT_TRUE(bundleRm == nullptr);
    delete rc;
}

/*
 * @tc.name: CreateResourceManagerTest002
 * @tc.desc: Test CreateResourceManager function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, CreateResourceManagerTest002, TestSize.Level1)
{
    ResConfigImpl *rc = new ResConfigImpl;
    std::string hapPath;
    std::vector<std::string> overlayPath;
    int32_t appType = 0;
    int32_t userId = 100; // userId is 100
    std::shared_ptr<ResourceManager> bundleRm =
        CreateResourceManager("ohos.global.test.all", "entry", hapPath, overlayPath, *rc, appType, userId);
    EXPECT_TRUE(bundleRm != nullptr);

    bundleRm = CreateResourceManager("", "entry", hapPath, overlayPath, *rc, appType, userId);
    EXPECT_TRUE(bundleRm == nullptr);
    delete rc;
}

/*
 * @tc.name: GetSystemResourceManagerTest001
 * @tc.desc: Test CreateResourceManager function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, GetSystemResourceManagerTest001, TestSize.Level1)
{
    ResourceManager *bundleRm = GetSystemResourceManagerNoSandBox();
    EXPECT_TRUE(bundleRm != nullptr);
    EXPECT_TRUE(GetSystemResourceManager() != nullptr);
    bundleRm = nullptr;
}

/*
 * @tc.name: AddPatchResourcePathTest001
 * @tc.desc: Test AddPatchResource function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, AddPatchResourcePathTest001, TestSize.Level1)
{
    std::string path = FormatFullPath(g_resFilePath);
    ASSERT_TRUE(rm->AddResource(path.c_str()));
    EXPECT_TRUE(rm->AddPatchResource(path.c_str(), "testPatch"));
    EXPECT_FALSE(rm->AddPatchResource("not_exit_path", "testPatch"));
}

/*
 * @tc.name: ReleaseSystemResourceManagerTest001
 * @tc.desc: Test ReleaseSystemResourceManager function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ReleaseSystemResourceManagerTest001, TestSize.Level1)
{
    ResourceManager *bundleRm = GetSystemResourceManagerNoSandBox();
    EXPECT_TRUE(bundleRm != nullptr);
    ReleaseSystemResourceManager();
    bundleRm = nullptr;
}

/*
 * @tc.name: CloseRawFileDescriptorTest001
 * @tc.desc: Test CloseRawFileDescriptor function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, CloseRawFileDescriptorTest001, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_hapPath).c_str());
    string name = "test_rawfile.txt";
    OHOS::Global::Resource::ResourceManager::RawFileDescriptor descriptor;
    RState state = rm->GetRawFileDescriptor(name, descriptor);
    EXPECT_EQ(SUCCESS, state);
    state = rm->CloseRawFileDescriptor(name);
    EXPECT_EQ(SUCCESS, state);
}

/*
 * @tc.name: CloseRawFileDescriptorTest002
 * @tc.desc: Test CloseRawFileDescriptor function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, CloseRawFileDescriptorTest002, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_hapPath).c_str());
    string name = "test_rawfile.txt";
    OHOS::Global::Resource::ResourceManager::RawFileDescriptor descriptor;
    RState state = rm->GetRawFileDescriptor(name, descriptor);
    EXPECT_EQ(SUCCESS, state);

    close(descriptor.fd);

    state = rm->CloseRawFileDescriptor(name);
    EXPECT_EQ(ERROR_CODE_RES_PATH_INVALID, state);
}

/*
 * @tc.name: CloseRawFileDescriptorTest003
 * @tc.desc: Test CloseRawFileDescriptor function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, CloseRawFileDescriptorTest003, TestSize.Level1)
{
    rm->AddResource(FormatFullPath(g_hapPath).c_str());
    string name = "test_rawfile.txt";
    OHOS::Global::Resource::ResourceManager::RawFileDescriptor descriptor;
    RState state = rm->GetRawFileDescriptor(name, descriptor);
    EXPECT_EQ(SUCCESS, state);

    close(descriptor.fd);
    int fd = open(name.c_str(), O_RDONLY);

    state = rm->CloseRawFileDescriptor(name);
    EXPECT_EQ(ERROR_CODE_RES_PATH_INVALID, state);

    close(fd);
}

/*
 * @tc.name: AddSystemResourceTest001
 * @tc.desc: Test AddSystemResource function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, AddSystemResourceTest001, TestSize.Level1)
{
    std::shared_ptr<ResourceManagerImpl> sysMgr = nullptr;
    bool result = ((ResourceManagerImpl *)rm)->AddSystemResource(sysMgr);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: AddSystemResourceTest002
 * @tc.desc: Test AddSystemResource function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, AddSystemResourceTest002, TestSize.Level1)
{
    ResourceManagerImpl* sysMgr = SystemResourceManager::GetSystemResourceManager();
    ((ResourceManagerImpl *)rm)->AddSystemResource(sysMgr);
    uint32_t outValue;
    RState state = rm->GetColorById(125831026, outValue);
    EXPECT_EQ(state, SUCCESS);
    SystemResourceManager::ReleaseSystemResourceManager();
}

/*
 * @tc.name: GetMediaBase64DataByNameTest001
 * @tc.desc: Test GetMediaBase64DataByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, GetMediaBase64DataByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::string outValue;
    RState state;
    state = rm->GetMediaBase64DataByName("app_name", outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: GetProfileDataByIdTest001
 * @tc.desc: Test GetProfileDataById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, GetProfileDataByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetProfileDataById(NON_EXIST_ID, len, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NOT_FOUND_BY_ID, state);
}

/*
 * @tc.name: GetDrawableInfoByIdTest001
 * @tc.desc: Test GetDrawableInfoById function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, GetDrawableInfoByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::tuple<std::string, size_t, std::string> drawableInfo;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetDrawableInfoById(NON_EXIST_ID, drawableInfo, outValue, 0, 1000);
    ASSERT_EQ(ERROR_CODE_INVALID_INPUT_PARAMETER, state);
}

/*
 * @tc.name: GetDrawableInfoByNameTest001
 * @tc.desc: Test GetDrawableInfoByName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, GetDrawableInfoByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::tuple<std::string, size_t, std::string> drawableInfo;
    std::unique_ptr<uint8_t[]> outValue;
    RState state = rm->GetDrawableInfoByName("app_name", drawableInfo, outValue, 0, 1000);
    ASSERT_EQ(ERROR_CODE_INVALID_INPUT_PARAMETER, state);
}

/*
 * @tc.name: ResourceManagerInitTest001
 * @tc.desc: Test Init function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerInitTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = nullptr;
    bool result = ((ResourceManagerImpl *)rm)->Init(hapManager);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: ResourceManagerGetResIdTest001
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetResIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    uint32_t resId;
    RState state = rm->GetResId("sys.string.ohos_id_text_font_family_regular", resId);
    ASSERT_EQ(state, SUCCESS);
    ASSERT_EQ(resId, 125829694);
}

/*
 * @tc.name: ResourceManagerGetResIdTest002
 * @tc.desc: Test GetResId function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetResIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    uint32_t resId;
    RState state = rm->GetResId("app.string.app_name", resId);
    ASSERT_EQ(state, SUCCESS);
    ASSERT_EQ(resId, 16777216);
}

/*
 * @tc.name: ResourceManagerGetResNameTest001
 * @tc.desc: Test GetResName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetResNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::string resName;
    RState state = rm->GetResName(125829694, resName);
    ASSERT_EQ(state, SUCCESS);
    ASSERT_EQ(resName, "ohos_id_text_font_family_regular");
}

/*
 * @tc.name: ResourceManagerGetResNameTest002
 * @tc.desc: Test GetResName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetResNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::string resName;
    RState state = rm->GetResName(16777216, resName);
    ASSERT_EQ(state, SUCCESS);
    ASSERT_EQ(resName, "app_name");
}

/*
 * @tc.name: ResourceManagerGetResNameTest003
 * @tc.desc: Test GetResName function
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetResNameTest003, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");
    std::string resName;
    RState state = rm->GetResName(0, resName);
    ASSERT_EQ(state, ERROR_CODE_RES_ID_NOT_FOUND);
    ASSERT_EQ(resName, "");
}

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest001
 * @tc.desc: Test GetStringById, to match pointingdevice directory which rc is set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByIdForInputDeviceTest001, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_POINTINGDEVICE);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("pointing device test", outValue);
}

/*
 * @tc.name: ResourceManagerGetStringByIdForInputDeviceTest002
 * @tc.desc: Test GetStringById, to match base directory which rc is not set
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerDeviceTest, ResourceManagerGetStringByIdForInputDeviceTest002, TestSize.Level1)
{
    bool ret = rm->AddResource(FormatFullPath(g_resFilePath).c_str());
    ASSERT_TRUE(ret);
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, nullptr);
    rc->SetDirection(DIRECTION_NOT_SET);
    rc->SetDeviceType(DEVICE_NOT_SET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rc->SetInputDevice(INPUTDEVICE_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    RState rState;
    int id = rmc->GetResId("point_test", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    rState = rm->GetStringById(id, outValue);
    ASSERT_EQ(SUCCESS, rState);
    ASSERT_EQ("default device test", outValue);
}
