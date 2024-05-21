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
class ResourceManagerTestMedia : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();

    ResourceManagerTestMedia() : rm(nullptr)
    {}

    ~ResourceManagerTestMedia()
    {}
public:
    ResourceManager *rm;
    ResourceManagerTestCommon *rmc;
};

void ResourceManagerTestMedia::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResourceManagerTestMedia::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResourceManagerTestMedia::SetUp(void)
{
    this->rm = CreateResourceManager();
    this->rmc = new ResourceManagerTestCommon(rm);
}

void ResourceManagerTestMedia::TearDown(void)
{
    delete this->rm;
    delete this->rmc;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest001
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    rmc->TestGetMediaById(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest002
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetMediaById(NON_EXIST_ID, outValue);
    ASSERT_EQ(ERROR_CODE_RES_ID_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest003
 * @tc.desc: Test GetMediaById, to match sdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest003, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    rmc->TestGetMediaWithDensityById(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest004
 * @tc.desc: Test GetMediaById, to match mdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest004, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TV);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 160;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest005
 * @tc.desc: Test GetMediaById, to match ldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest005, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/ldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 240;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest006
 * @tc.desc: Test GetMediaById, to match xldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest006, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/xldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 320;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest007
 * @tc.desc: Test GetMediaById, to match xxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest007, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/xxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 480;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest008
 * @tc.desc: Test GetMediaById, to match xxxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest008, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/xxxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 640;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest009
 * @tc.desc: Test GetMediaById, to match unsupport density
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest009, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    uint32_t density1 = 420;
    uint32_t density2 = 800;
    uint32_t density3 = 10;
    std::string outValue;
    RState state1;
    RState state2;
    RState state3;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state1 = rm->GetMediaById(id, outValue, density1);
    state2 = rm->GetMediaById(id, outValue, density2);
    state3 = rm->GetMediaById(id, outValue, density3);
    EXPECT_TRUE(state1 == ERROR_CODE_INVALID_INPUT_PARAMETER);
    EXPECT_TRUE(state2 == ERROR_CODE_INVALID_INPUT_PARAMETER);
    EXPECT_TRUE(state3 == ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest010
 * @tc.desc: Test GetMediaById, to match with no density param
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest010, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    uint32_t density = 0;
    std::string outValue1;
    std::string outValue2;
    RState state1;
    RState state2;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    state1 = rm->GetMediaById(id, outValue1, density);
    state2 = rm->GetMediaById(id, outValue2);
    EXPECT_TRUE(state1 == SUCCESS);
    EXPECT_TRUE(state2 == SUCCESS);
    EXPECT_EQ(outValue1, outValue2);
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest011
 * @tc.desc: Test GetMediaById, to match zh_CN-sdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest011, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-sdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 120;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest012
 * @tc.desc: Test GetMediaById, to match zh_CN-mdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest012, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-mdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 160;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest013
 * @tc.desc: Test GetMediaById, to match zh_CN-ldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest013, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-ldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 240;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest014
 * @tc.desc: Test GetMediaById, to match zh_CN-xldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest014, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-xldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 320;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest015
 * @tc.desc: Test GetMediaById, to match zh_CN-xxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest015, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-xxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 480;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest016
 * @tc.desc: Test GetMediaById, to match zh_CN-xxxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest016, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-xxxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 640;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest017
 * @tc.desc: Test GetMediaById, to match mcc460_mnc101-zh_CN-phone-dark-sdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest017, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-sdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 120;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest018
 * @tc.desc: Test GetMediaById, to match mcc460_mnc101-zh_CN-phone-dark-mdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest018, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-mdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 160;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest019
 * @tc.desc: Test GetMediaById, to match mcc460_mnc101-zh_CN-phone-dark-ldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest019, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-ldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 240;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest020
 * @tc.desc: Test GetMediaById, to match mcc460_mnc101-zh_CN-phone-dark-xldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest020, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-xldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 320;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest021
 * @tc.desc: Test GetMediaById, to match mcc460_mnc101-zh_CN-phone-dark-xxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest021, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-xxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 480;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest022
 * @tc.desc: Test GetMediaById, to match mcc460_mnc101-zh_CN-phone-dark-xxxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest022, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-xxxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 640;
    std::string outValue;
    RState state;
    int id = rmc->GetResId("icon", ResType::MEDIA);
    EXPECT_TRUE(id > 0);
    state = rm->GetMediaById(id, outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByIdTest023
 * @tc.desc: Test GetMediaById
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByIdTest023, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, nullptr);

    int id = rmc->GetResId("app_name", ResType::STRING);
    EXPECT_TRUE(id > 0);
    std::string outValue;
    RState state = rm->GetMediaById(id, outValue);
    EXPECT_EQ(state, ERROR_CODE_RES_NOT_FOUND_BY_ID);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest001
 * @tc.desc: Test GetMediaByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest001, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    rmc->TestGetMediaByName(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest002
 * @tc.desc: Test GetMediaByName
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest002, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    std::string outValue;
    RState state;
    state = rm->GetMediaByName(g_nonExistName, outValue);
    ASSERT_EQ(ERROR_CODE_RES_NAME_NOT_FOUND, state);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest003
 * @tc.desc: Test GetMediaByName, to match sdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest003, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    rmc->TestGetMediaWithDensityByName(tmp);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest004
 * @tc.desc: Test GetMediaByName, to match mdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest004, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    uint32_t density = 160;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest005
 * @tc.desc: Test GetMediaByName, to match ldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest005, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/ldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    uint32_t density = 240;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest006
 * @tc.desc: Test GetMediaByName, to match xldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest006, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/xldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    uint32_t density = 320;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest007
 * @tc.desc: Test GetMediaByName, to match xxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest007, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/xxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    uint32_t density = 480;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest008
 * @tc.desc: Test GetMediaByName, to match xxxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest008, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/xxxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    uint32_t density = 640;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest009
 * @tc.desc: Test GetMediaByName, to match unsupport density
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest009, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    uint32_t density1 = 420;
    uint32_t density2 = 800;
    uint32_t density3 = 10;
    std::string outValue;
    RState state1;
    RState state2;
    RState state3;
    state1 = rm->GetMediaByName("icon", outValue, density1);
    state2 = rm->GetMediaByName("icon", outValue, density2);
    state3 = rm->GetMediaByName("icon", outValue, density3);
    EXPECT_TRUE(state1 == ERROR_CODE_INVALID_INPUT_PARAMETER);
    EXPECT_TRUE(state2 == ERROR_CODE_INVALID_INPUT_PARAMETER);
    EXPECT_TRUE(state3 == ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest010
 * @tc.desc: Test GetMediaByName, to match with no density param
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest010, TestSize.Level1)
{
    rmc->AddResource("en", nullptr, "US");

    uint32_t density = 0;
    std::string outValue1;
    std::string outValue2;
    RState state1;
    RState state2;
    state1 = rm->GetMediaByName("icon", outValue1, density);
    state2 = rm->GetMediaByName("icon", outValue2);
    EXPECT_TRUE(state1 == SUCCESS);
    EXPECT_TRUE(state2 == SUCCESS);
    EXPECT_EQ(outValue1, outValue2);
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest011
 * @tc.desc: Test GetMediaByName, to match zh_CN-sdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest011, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-sdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 120;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest012
 * @tc.desc: Test GetMediaByName, to match zh_CN-mdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest012, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-mdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 160;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest013
 * @tc.desc: Test GetMediaByName, to match zh_CN-ldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest013, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-ldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 240;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest014
 * @tc.desc: Test GetMediaByName, to match zh_CN-xldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest014, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-xldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 320;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest015
 * @tc.desc: Test GetMediaByName, to match zh_CN-xxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest015, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-xxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 480;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest016
 * @tc.desc: Test GetMediaByName, to match zh_CN-xxxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest016, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/zh_CN-xxxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetDeviceType(DEVICE_TABLET);
    rc->SetColorMode(COLOR_MODE_NOT_SET);
    rc->SetScreenDensity(SCREEN_DENSITY_NOT_SET);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 640;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest017
 * @tc.desc: Test GetMediaByName, to match mcc460_mnc101-zh_CN-phone-dark-sdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest017, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-sdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 120;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest018
 * @tc.desc: Test GetMediaByName, to match mcc460_mnc101-zh_CN-phone-dark-mdpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest018, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-mdpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 160;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest019
 * @tc.desc: Test GetMediaByName, to match mcc460_mnc101-zh_CN-phone-dark-ldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest019, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-ldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 240;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest020
 * @tc.desc: Test GetMediaByName, to match mcc460_mnc101-zh_CN-phone-dark-xldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest020, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-xldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 320;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest021
 * @tc.desc: Test GetMediaByName, to match mcc460_mnc101-zh_CN-phone-dark-xxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest021, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-xxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 480;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}

/*
 * @tc.name: ResourceManagerGetMediaByNameTest022
 * @tc.desc: Test GetMediaByName, to match mcc460_mnc101-zh_CN-phone-dark-xxxldpi determinder
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerTestMedia, ResourceManagerGetMediaByNameTest022, TestSize.Level1)
{
    rmc->AddResource("zh", nullptr, "CN");

    HapResource *tmp = new HapResource(FormatFullPath(g_resFilePath).c_str(), 0, nullptr);
    tmp->Init(rmc->defaultResConfig);
    std::string res = tmp->GetResourcePath();
    res.append("entry/resources/mcc460_mnc101-zh_CN-phone-dark-xxxldpi/media/icon.png");

    auto rc = CreateResConfig();
    if (rc == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    rc->SetMcc(460);
    rc->SetMnc(101);
    rc->SetLocaleInfo("zh", nullptr, "CN");
    rc->SetDeviceType(DEVICE_PHONE);
    rc->SetColorMode(DARK);
    rc->SetScreenDensity(SCREEN_DENSITY_XXXLDPI / BASE_DPI);
    rm->UpdateResConfig(*rc);
    delete rc;

    int density = 640;
    std::string outValue;
    RState state;
    state = rm->GetMediaByName("icon", outValue, density);
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_EQ(res, outValue);
    delete tmp;
}
}
