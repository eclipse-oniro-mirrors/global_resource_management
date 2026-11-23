/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <securec.h>
#include <vector>

#include "resource_manager.h"
#include "ohresmgr.h"
#include "raw_file.h"
#include "raw_file_manager.h"
#include "resmgr_common.h"
#include "resource_manager_impl.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

namespace {
constexpr uint32_t DENSITY_DEFAULT = 0;
}

struct NativeResourceManager {
    std::shared_ptr<OHOS::Global::Resource::ResourceManager> resManager = nullptr;
};

class NativeResourceManagerTest : public testing::Test {
public:
    void SetUp() override
    {
        rm = CreateResourceManager(false);
        ASSERT_NE(rm, nullptr);
        auto rc = CreateResConfig();
        rc->SetLocaleInfo("en", nullptr, "US");
        rm->UpdateResConfig(*rc);
        delete rc;
        bool ret = rm->AddResource(FormatFullPath(g_newResFilePath).c_str());
        ASSERT_TRUE(ret);
        nativeMgr.resManager = std::shared_ptr<ResourceManager>(rm, [](ResourceManager *) {});
    }

    void TearDown() override
    {
        nativeMgr.resManager.reset();
        delete rm;
        rm = nullptr;
    }

    uint32_t GetResId(const char *name, ResType resType)
    {
        auto idValues = ((ResourceManagerImpl *)rm)->hapManager_->GetResourceListByName(name, resType);
        if (idValues.empty()) {
            return 0;
        }
        return idValues[0]->GetLimitPathsConst()[0]->GetIdItem()->id_;
    }

    ResourceManager *rm = nullptr;
    NativeResourceManager nativeMgr;
};

class NativeRawFileHapTest : public testing::Test {
public:
    void SetUp() override
    {
        rm = CreateResourceManager(false);
        ASSERT_NE(rm, nullptr);
        auto rc = CreateResConfig();
        rc->SetLocaleInfo("en", nullptr, "US");
        rm->UpdateResConfig(*rc);
        delete rc;
        bool ret = rm->AddResource(FormatFullPath(g_newModuleHapPath).c_str());
        ASSERT_TRUE(ret);
        nativeMgr.resManager = std::shared_ptr<ResourceManager>(rm, [](ResourceManager *) {});
    }

    void TearDown() override
    {
        nativeMgr.resManager.reset();
        delete rm;
        rm = nullptr;
    }

    ResourceManager *rm = nullptr;
    NativeResourceManager nativeMgr;
};/*
 * @tc.name: Native_ByNameNullResName001
 * @tc.desc: All *ByName Native APIs must reject resName==nullptr with ERROR_CODE_INVALID_INPUT_PARAMETER,
 *           no crash. Covers the same-family consistency fix (GetIntPluralStringByName / GetMediaDataByName
 *           / GetMediaBase64DataByName) plus the already-guarded siblings.
 *           Note: these tests do NOT access resManager (null resName short-circuits before it),
 *           so no test resources are needed.
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_ByNameNullResName001, TestSize.Level1)
{
    ASSERT_NE(nativeMgr.resManager, nullptr);
    NativeResourceManager *mgr = &nativeMgr;
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;

    uint8_t *out8 = nullptr;
    char *outc = nullptr;
    char **outArr = nullptr;
    uint64_t len64 = 0;
    uint32_t outu32 = 0;
    int outInt = 0;
    float outFloat = 0.0f;
    bool outBool = false;

    EXPECT_EQ(OH_ResourceManager_GetMediaDataByName(mgr, nullptr, &out8, &len64, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetMediaBase64DataByName(mgr, nullptr, &outc, &len64, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetStringByName(mgr, nullptr, &outc), badParam);
    EXPECT_EQ(OH_ResourceManager_GetStringArrayByName(mgr, nullptr, &outArr, &outu32), badParam);
    EXPECT_EQ(OH_ResourceManager_GetPluralStringByName(mgr, nullptr, 1, &outc), badParam);
    EXPECT_EQ(OH_ResourceManager_GetColorByName(mgr, nullptr, &outu32), badParam);
    EXPECT_EQ(OH_ResourceManager_GetIntByName(mgr, nullptr, &outInt), badParam);
    EXPECT_EQ(OH_ResourceManager_GetFloatByName(mgr, nullptr, &outFloat), badParam);
    EXPECT_EQ(OH_ResourceManager_GetBoolByName(mgr, nullptr, &outBool), badParam);
    EXPECT_EQ(OH_ResourceManager_GetSymbolByName(mgr, nullptr, &outu32), badParam);
    EXPECT_EQ(OH_ResourceManager_GetIntPluralStringByName(mgr, nullptr, 1, &outc), badParam);
    EXPECT_EQ(OH_ResourceManager_GetDoublePluralStringByName(mgr, nullptr, 1.0, &outc), badParam);
}

/*
 * @tc.name: Native_NullMgr001
 * @tc.desc: mgr==nullptr is rejected by every entry guard.
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_NullMgr001, TestSize.Level1)
{
    uint8_t *out8 = nullptr;
    uint64_t len64 = 0;
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_GetMediaDataByName(nullptr, "icon", &out8, &len64, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetMediaBase64DataByName(nullptr, "icon", nullptr, &len64, DENSITY_DEFAULT),
        badParam);
}

/*
 * @tc.name: Native_NullResultValue001
 * @tc.desc: resultValue==nullptr (with valid mgr + resName) is rejected.
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_NullResultValue001, TestSize.Level1)
{
    ASSERT_NE(nativeMgr.resManager, nullptr);
    NativeResourceManager *mgr = &nativeMgr;
    uint64_t len64 = 0;
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_GetMediaDataByName(mgr, "icon", nullptr, &len64, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetMediaBase64DataByName(mgr, "icon", nullptr, &len64, DENSITY_DEFAULT),
        badParam);
}

/*
 * @tc.name: Native_NullResultLen001
 * @tc.desc: resultLen==nullptr (with valid mgr + resName) is rejected.
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_NullResultLen001, TestSize.Level1)
{
    ASSERT_NE(nativeMgr.resManager, nullptr);
    NativeResourceManager *mgr = &nativeMgr;
    uint8_t *out8 = nullptr;
    char *outc = nullptr;
    char **outArr = nullptr;
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_GetMediaDataByName(mgr, "icon", &out8, nullptr, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetMediaBase64DataByName(mgr, "icon", &outc, nullptr, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetStringArrayByName(mgr, "str_arr", &outArr, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetLocales(mgr, &outArr, nullptr, true), badParam);
}

/*
 * @tc.name: Native_NullResNameDrawableDescriptor001
 * @tc.desc: GetDrawableDescriptorDataByName with resName==nullptr is rejected.
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_NullResNameDrawableDescriptor001, TestSize.Level1)
{
    ASSERT_NE(nativeMgr.resManager, nullptr);
    NativeResourceManager *mgr = &nativeMgr;
    ArkUI_DrawableDescriptor *drawable = nullptr;
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_GetDrawableDescriptorDataByName(mgr, nullptr, &drawable, DENSITY_DEFAULT, 0),
        badParam);
}

/*
 * @tc.name: Native_GetStringByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetString with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetStringByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("app_name", ResType::STRING);
    ASSERT_TRUE(id > 0);
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetString(mgr, id, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "App Name");
    free(result);
}

/*
 * @tc.name: Native_GetStringByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetStringByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetStringByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetStringByName(mgr, "app_name", &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "App Name");
    free(result);
}

/*
 * @tc.name: Native_GetColorByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetColor with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetColorByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("color_aboutPage_title_primary", ResType::COLOR);
    ASSERT_TRUE(id > 0);
    uint32_t result = 0;
    auto ret = OH_ResourceManager_GetColor(mgr, id, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_EQ(result, static_cast<uint32_t>(0xFF191919));
}

/*
 * @tc.name: Native_GetColorByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetColorByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetColorByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t result = 0;
    auto ret = OH_ResourceManager_GetColorByName(mgr, "color_aboutPage_title_primary", &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_EQ(result, static_cast<uint32_t>(0xFF191919));
}

/*
 * @tc.name: Native_GetIntByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetInt with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetIntByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("integer_1", ResType::INTEGER);
    ASSERT_TRUE(id > 0);
    int result = 0;
    auto ret = OH_ResourceManager_GetInt(mgr, id, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_EQ(result, 101);
}

/*
 * @tc.name: Native_GetIntByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetIntByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetIntByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    int result = 0;
    auto ret = OH_ResourceManager_GetIntByName(mgr, "integer_1", &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_EQ(result, 101);
}

/*
 * @tc.name: Native_GetFloatByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetFloat with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetFloatByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("height_appBar", ResType::FLOAT);
    ASSERT_TRUE(id > 0);
    float result = 0.0f;
    auto ret = OH_ResourceManager_GetFloat(mgr, id, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_FLOAT_EQ(result, 56.0f);
}

/*
 * @tc.name: Native_GetFloatByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetFloatByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetFloatByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    float result = 0.0f;
    auto ret = OH_ResourceManager_GetFloatByName(mgr, "height_appBar", &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_FLOAT_EQ(result, 56.0f);
}

/*
 * @tc.name: Native_GetBoolByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetBool with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetBoolByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("boolean_1", ResType::BOOLEAN);
    ASSERT_TRUE(id > 0);
    bool result = false;
    auto ret = OH_ResourceManager_GetBool(mgr, id, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: Native_GetBoolByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetBoolByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetBoolByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    bool result = false;
    auto ret = OH_ResourceManager_GetBoolByName(mgr, "boolean_1", &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: Native_GetStringArrayByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetStringArray with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetStringArrayByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("size", ResType::STRINGARRAY);
    ASSERT_TRUE(id > 0);
    char **result = nullptr;
    uint32_t resultLen = 0;
    auto ret = OH_ResourceManager_GetStringArray(mgr, id, &result, &resultLen);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(resultLen, static_cast<uint32_t>(4));
    EXPECT_STREQ(result[0], "small");
    OH_ResourceManager_ReleaseStringArray(&result, resultLen);
}

/*
 * @tc.name: Native_GetStringArrayByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetStringArrayByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetStringArrayByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char **result = nullptr;
    uint32_t resultLen = 0;
    auto ret = OH_ResourceManager_GetStringArrayByName(mgr, "size", &result, &resultLen);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(resultLen, static_cast<uint32_t>(4));
    EXPECT_STREQ(result[0], "small");
    OH_ResourceManager_ReleaseStringArray(&result, resultLen);
}

/*
 * @tc.name: Native_GetPluralStringByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetPluralString with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetPluralStringByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetPluralString(mgr, id, 1, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetPluralStringByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetPluralStringByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetPluralStringByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetPluralStringByName(mgr, "eat_apple", 1, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetSymbolByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetSymbol with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetSymbolByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("test_symbol", ResType::SYMBOL);
    ASSERT_TRUE(id > 0);
    uint32_t result = 0;
    auto ret = OH_ResourceManager_GetSymbol(mgr, id, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_EQ(result, static_cast<uint32_t>(0xF0000));
}

/*
 * @tc.name: Native_GetSymbolByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetSymbolByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetSymbolByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t result = 0;
    auto ret = OH_ResourceManager_GetSymbolByName(mgr, "test_symbol", &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_EQ(result, static_cast<uint32_t>(0xF0000));
}

/*
 * @tc.name: Native_GetMediaDataByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetMediaData with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaDataByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    uint8_t *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMediaData(mgr, id, &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_GT(resultLen, static_cast<uint64_t>(0));
    free(result);
}

/*
 * @tc.name: Native_GetMediaDataByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetMediaDataByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaDataByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint8_t *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMediaDataByName(mgr, "icon", &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_GT(resultLen, static_cast<uint64_t>(0));
    free(result);
}

/*
 * @tc.name: Native_GetMediaBase64DataByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetMediaBase64Data with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaBase64DataByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    char *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMediaBase64Data(mgr, id, &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_GT(resultLen, static_cast<uint64_t>(0));
    free(result);
}

/*
 * @tc.name: Native_GetMediaBase64DataByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetMediaBase64DataByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaBase64DataByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMediaBase64DataByName(mgr, "icon", &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_GT(resultLen, static_cast<uint64_t>(0));
    free(result);
}

/*
 * @tc.name: Native_GetIntPluralStringByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetIntPluralString with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetIntPluralStringByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetIntPluralString(mgr, id, 1, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetDoublePluralStringByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetDoublePluralString with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetDoublePluralStringByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("eat_apple", ResType::PLURALS);
    ASSERT_TRUE(id > 0);
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetDoublePluralString(mgr, id, 1.0, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetIntPluralStringByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetIntPluralStringByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetIntPluralStringByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetIntPluralStringByName(mgr, "eat_apple", 1, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetDoublePluralStringByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetDoublePluralStringByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetDoublePluralStringByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char *result = nullptr;
    auto ret = OH_ResourceManager_GetDoublePluralStringByName(mgr, "eat_apple", 1.0, &result);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetLocalesTest001
 * @tc.desc: Test OH_ResourceManager_GetLocales with valid mgr
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetLocalesTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char **result = nullptr;
    uint32_t resultLen = 0;
    auto ret = OH_ResourceManager_GetLocales(mgr, &result, &resultLen, true);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    EXPECT_GT(resultLen, static_cast<uint32_t>(0));
    OH_ResourceManager_ReleaseStringArray(&result, resultLen);
}

/*
 * @tc.name: Native_GetConfigurationTest001
 * @tc.desc: Test OH_ResourceManager_GetConfiguration with valid mgr
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetConfigurationTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    ResourceManager_Configuration config;
    errno_t memRet = memset_s(&config, sizeof(config), 0, sizeof(config));
    ASSERT_EQ(memRet, EOK);
    auto ret = OH_ResourceManager_GetConfiguration(mgr, &config);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    OH_ResourceManager_ReleaseConfiguration(&config);
}

/*
 * @tc.name: Native_GetResourceConfigurationTest001
 * @tc.desc: Test OH_ResourceManager_GetResourceConfiguration with valid mgr
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetResourceConfigurationTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    ResourceManager_Configuration config;
    errno_t memRet = memset_s(&config, sizeof(config), 0, sizeof(config));
    ASSERT_EQ(memRet, EOK);
    auto ret = OH_ResourceManager_GetResourceConfiguration(mgr, &config);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    OH_ResourceManager_ReleaseConfiguration(&config);
}

/*
 * @tc.name: Native_GetConfigurationNullTest001
 * @tc.desc: Test OH_ResourceManager_GetConfiguration with null configuration
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetConfigurationNullTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_GetConfiguration(mgr, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetResourceConfiguration(mgr, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetConfiguration(nullptr, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_ReleaseConfiguration(nullptr), badParam);
}

/*
 * @tc.name: Native_ReleaseStringArrayTest001
 * @tc.desc: Test OH_ResourceManager_ReleaseStringArray with null input
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_ReleaseStringArrayTest001, TestSize.Level1)
{
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_ReleaseStringArray(nullptr, 0), badParam);
    char **nullArr = nullptr;
    EXPECT_EQ(OH_ResourceManager_ReleaseStringArray(&nullArr, 0), badParam);
}

/*
 * @tc.name: Native_GetStringArrayNullResultLenTest001
 * @tc.desc: OH_ResourceManager_GetStringArray rejects resultValueLen==nullptr
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_GetStringArrayNullResultLenTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("size", ResType::STRINGARRAY);
    ASSERT_TRUE(id > 0);
    char **result = nullptr;
    EXPECT_EQ(OH_ResourceManager_GetStringArray(mgr, id, &result, nullptr),
        ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER);
    EXPECT_EQ(result, nullptr);

    uint32_t resultLen = 0;
    EXPECT_EQ(OH_ResourceManager_GetStringArray(mgr, id, nullptr, &resultLen),
        ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER);
}

/*
 * @tc.name: Native_InitResourceManagerNullParamTest001
 * @tc.desc: OH_ResourceManager_InitNativeResourceManager rejects null env or jsResMgr
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_InitResourceManagerNullParamTest001, TestSize.Level1)
{
    EXPECT_EQ(OH_ResourceManager_InitNativeResourceManager(nullptr, nullptr), nullptr);
}

/*
 * @tc.name: Native_NotFoundIdTest001
 * @tc.desc: Test APIs with non-existent resource id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_NotFoundIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t invalidId = 999999999;
    char *outStr = nullptr;
    uint32_t outU32 = 0;
    int outInt = 0;
    float outFloat = 0.0f;
    bool outBool = false;
    auto notFound = ResourceManager_ErrorCode::ERROR_CODE_RES_ID_NOT_FOUND;
    (void)notFound;

    EXPECT_NE(OH_ResourceManager_GetString(mgr, invalidId, &outStr), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetColor(mgr, invalidId, &outU32), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetInt(mgr, invalidId, &outInt), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetFloat(mgr, invalidId, &outFloat), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetBool(mgr, invalidId, &outBool), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetSymbol(mgr, invalidId, &outU32), ResourceManager_ErrorCode::SUCCESS);
}

/*
 * @tc.name: Native_NotFoundNameTest001
 * @tc.desc: Test APIs with non-existent resource name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_NotFoundNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char *outStr = nullptr;
    uint32_t outU32 = 0;
    int outInt = 0;
    float outFloat = 0.0f;
    bool outBool = false;
    char **outArr = nullptr;
    uint32_t arrLen = 0;
    uint8_t *outMedia = nullptr;
    uint64_t mediaLen = 0;

    EXPECT_NE(OH_ResourceManager_GetStringByName(mgr, "nonexistent", &outStr), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetColorByName(mgr, "nonexistent", &outU32), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetIntByName(mgr, "nonexistent", &outInt), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetFloatByName(mgr, "nonexistent", &outFloat), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetBoolByName(mgr, "nonexistent", &outBool), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetSymbolByName(mgr, "nonexistent", &outU32), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetStringArrayByName(mgr, "nonexistent", &outArr, &arrLen),
        ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetPluralStringByName(mgr, "nonexistent", 1, &outStr),
        ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetMediaDataByName(mgr, "nonexistent", &outMedia, &mediaLen, DENSITY_DEFAULT),
        ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetMediaBase64DataByName(mgr, "nonexistent",
        reinterpret_cast<char **>(&outMedia), &mediaLen, DENSITY_DEFAULT), ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetIntPluralStringByName(mgr, "nonexistent", 1, &outStr),
        ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(OH_ResourceManager_GetDoublePluralStringByName(mgr, "nonexistent", 1.0, &outStr),
        ResourceManager_ErrorCode::SUCCESS);
}

/*
 * @tc.name: Native_GetDrawableDescriptorByIdTest001
 * @tc.desc: Test OH_ResourceManager_GetDrawableDescriptor with valid id
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetDrawableDescriptorByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    ArkUI_DrawableDescriptor *drawable = nullptr;
    auto ret = OH_ResourceManager_GetDrawableDescriptor(mgr, id, &drawable, DENSITY_DEFAULT, 0);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(drawable, nullptr);
}

/*
 * @tc.name: Native_GetDrawableDescriptorByNameTest001
 * @tc.desc: Test OH_ResourceManager_GetDrawableDescriptorByName with valid name
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetDrawableDescriptorByNameTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    ArkUI_DrawableDescriptor *drawable = nullptr;
    auto ret = OH_ResourceManager_GetDrawableDescriptorByName(mgr, "icon", &drawable, DENSITY_DEFAULT, 0);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(drawable, nullptr);
}

/*
 * @tc.name: Native_NullParamByIdTest001
 * @tc.desc: Test ById APIs reject null resultValue
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_NullParamByIdTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    uint32_t id = GetResId("app_name", ResType::STRING);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(OH_ResourceManager_GetString(mgr, id, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetColor(mgr, id, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetInt(mgr, id, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetFloat(mgr, id, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetBool(mgr, id, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetSymbol(mgr, id, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetStringArray(mgr, id, nullptr, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetPluralString(mgr, id, 1, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetIntPluralString(mgr, id, 1, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetDoublePluralString(mgr, id, 1.0, nullptr), badParam);
    EXPECT_EQ(OH_ResourceManager_GetMediaData(mgr, id, nullptr, nullptr, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetMediaBase64Data(mgr, id, nullptr, nullptr, DENSITY_DEFAULT), badParam);
    EXPECT_EQ(OH_ResourceManager_GetDrawableDescriptor(mgr, id, nullptr, DENSITY_DEFAULT, 0), badParam);
}

/*
 * @tc.name: Native_GetMediaBase64WrapperTest001
 * @tc.desc: Test OH_ResourceManager_GetMediaBase64 wrapper delegates to GetMediaBase64Data
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaBase64WrapperTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    char *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMediaBase64(mgr, id, &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetMediaBase64ByNameWrapperTest001
 * @tc.desc: Test OH_ResourceManager_GetMediaBase64ByName wrapper delegates to GetMediaBase64DataByName
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaBase64ByNameWrapperTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMediaBase64ByName(mgr, "icon", &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetMediaWrapperTest001
 * @tc.desc: Test OH_ResourceManager_GetMedia wrapper delegates to GetMediaData
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaWrapperTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    uint8_t *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMedia(mgr, id, &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_GetMediaByNameWrapperTest001
 * @tc.desc: Test OH_ResourceManager_GetMediaByName wrapper delegates to GetMediaDataByName
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetMediaByNameWrapperTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint8_t *result = nullptr;
    uint64_t resultLen = 0;
    auto ret = OH_ResourceManager_GetMediaByName(mgr, "icon", &result, &resultLen, DENSITY_DEFAULT);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    free(result);
}

/*
 * @tc.name: Native_AddResourceTest001
 * @tc.desc: Test OH_ResourceManager_AddResource with null params
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_AddResourceTest001, TestSize.Level1)
{
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_AddResource(nullptr, "/data/test/overlay/assets/entry/resourcesV2.index"),
        badParam);
    EXPECT_EQ(OH_ResourceManager_AddResource(&nativeMgr, nullptr), badParam);
}

/*
 * @tc.name: Native_AddResourceTest002
 * @tc.desc: Test OH_ResourceManager_AddResource with valid overlay path
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_AddResourceTest002, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    std::string overlayPath = FormatFullPath("overlay/assets/entry/resourcesV2.index");
    auto ret = OH_ResourceManager_AddResource(mgr, overlayPath.c_str());
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
}

/*
 * @tc.name: Native_RemoveResourceTest001
 * @tc.desc: Test OH_ResourceManager_RemoveResource with null params
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_RemoveResourceTest001, TestSize.Level1)
{
    auto badParam = ResourceManager_ErrorCode::ERROR_CODE_INVALID_INPUT_PARAMETER;
    EXPECT_EQ(OH_ResourceManager_RemoveResource(nullptr, "/data/test/overlay/assets/entry/resourcesV2.index"),
        badParam);
    EXPECT_EQ(OH_ResourceManager_RemoveResource(&nativeMgr, nullptr), badParam);
}

/*
 * @tc.name: Native_RemoveResourceTest002
 * @tc.desc: Test OH_ResourceManager_RemoveResource with valid overlay path
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_RemoveResourceTest002, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    std::string overlayPath = FormatFullPath("overlay/assets/entry/resourcesV2.index");
    OH_ResourceManager_AddResource(mgr, overlayPath.c_str());
    auto ret = OH_ResourceManager_RemoveResource(mgr, overlayPath.c_str());
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
}

/*
 * @tc.name: Native_GetDrawableDescriptorThemeIconTest001
 * @tc.desc: Test OH_ResourceManager_GetDrawableDescriptor with type=1 (theme icon path)
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetDrawableDescriptorThemeIconTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    uint32_t id = GetResId("icon", ResType::MEDIA);
    ASSERT_TRUE(id > 0);
    ArkUI_DrawableDescriptor *drawable = nullptr;
    auto ret = OH_ResourceManager_GetDrawableDescriptor(mgr, id, &drawable, DENSITY_DEFAULT, 1);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(drawable, nullptr);
}

/*
 * @tc.name: Native_GetDrawableDescriptorByNameThemeIconTest001
 * @tc.desc: Test OH_ResourceManager_GetDrawableDescriptorByName with type=1 (theme icon path)
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetDrawableDescriptorByNameThemeIconTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    ArkUI_DrawableDescriptor *drawable = nullptr;
    auto ret = OH_ResourceManager_GetDrawableDescriptorByName(mgr, "icon", &drawable, DENSITY_DEFAULT, 1);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    EXPECT_NE(drawable, nullptr);
}

/*
 * @tc.name: Native_GetLocalesWrapperTest001
 * @tc.desc: Test OH_ResourceManager_GetLocales wrapper delegates to GetLocalesData
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_GetLocalesWrapperTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    char **result = nullptr;
    uint32_t resultLen = 0;
    auto ret = OH_ResourceManager_GetLocales(mgr, &result, &resultLen, true);
    EXPECT_EQ(ret, ResourceManager_ErrorCode::SUCCESS);
    ASSERT_NE(result, nullptr);
    OH_ResourceManager_ReleaseStringArray(&result, resultLen);
}

/*
 * @tc.name: Native_SeekRawFileNegativeOffset001
 * @tc.desc: Test SeekRawFile rejects negative SEEK_SET offset and rolls back state on
 *           out-of-bounds SEEK_CUR, verifying no negative offset persists in actualOffset.
 *           Uses HAP mode (buffer != nullptr) so ReadRawFile advances actualOffset.
 * @tc.type: FUNC
 */
HWTEST_F(NativeRawFileHapTest, Native_SeekRawFileNegativeOffset001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    RawFile *rawFile = OH_ResourceManager_OpenRawFile(mgr, "test_rawfile.txt");
    ASSERT_NE(rawFile, nullptr);
    long fileSize = OH_ResourceManager_GetRawFileSize(rawFile);
    ASSERT_GT(fileSize, 0);

    // SEEK_SET negative must be rejected at guard
    int ret = OH_ResourceManager_SeekRawFile(rawFile, -5, SEEK_SET);
    EXPECT_EQ(ret, -1);

    // State not corrupted: offset still 0, remaining length not inflated
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(rawFile), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileRemainingLength(rawFile), fileSize);

    // Read 3 bytes — HAP mode advances actualOffset via memcpy_s path
    char buf[32] = {0};
    int bytesRead = OH_ResourceManager_ReadRawFile(rawFile, buf, 3);
    EXPECT_EQ(bytesRead, 3);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(rawFile), 3);

    // SEEK_CUR resulting in negative must be rejected and state rolled back
    ret = OH_ResourceManager_SeekRawFile(rawFile, -4, SEEK_CUR);
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(rawFile), 3);

    // SEEK_CUR with valid backward (within bounds) should succeed
    ret = OH_ResourceManager_SeekRawFile(rawFile, -2, SEEK_CUR);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(rawFile), 1);

    OH_ResourceManager_CloseRawFile(rawFile);
}

/*
 * @tc.name: Native_ReadRawFile64NegativeLength001
 * @tc.desc: Test ReadRawFile64 rejects negative length without crash or state corruption
 * @tc.type: FUNC
 */
HWTEST_F(NativeRawFileHapTest, Native_ReadRawFile64NegativeLength001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    RawFile64 *rawFile = OH_ResourceManager_OpenRawFile64(mgr, "test_rawfile.txt");
    ASSERT_NE(rawFile, nullptr);

    char buf[32] = {0};

    // Negative length returns 0 (invalid parameter, logged), not crash
    int64_t ret = OH_ResourceManager_ReadRawFile64(rawFile, buf, -1);
    EXPECT_EQ(ret, 0);

    // Zero length returns 0 (EOF / empty read, unchanged)
    ret = OH_ResourceManager_ReadRawFile64(rawFile, buf, 0);
    EXPECT_EQ(ret, 0);

    // Offset unchanged after rejected calls
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(rawFile), 0);

    // Positive length works normally (regression)
    ret = OH_ResourceManager_ReadRawFile64(rawFile, buf, 3);
    EXPECT_EQ(ret, 3);

    OH_ResourceManager_CloseRawFile64(rawFile);
}

/*
 * @tc.name: Native_NullDescriptorTest001
 * @tc.desc: Descriptor get/release APIs reject null descriptor or raw file input,
 *           and valid descriptor get/release is verified as control
 * @tc.type: SECURITY
 */
HWTEST_F(NativeRawFileHapTest, Native_NullDescriptorTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    RawFile *rawFile = OH_ResourceManager_OpenRawFile(mgr, "test_rawfile.txt");
    ASSERT_NE(rawFile, nullptr);
    RawFile64 *rawFile64 = OH_ResourceManager_OpenRawFile64(mgr, "test_rawfile.txt");
    if (rawFile64 == nullptr) {
        OH_ResourceManager_CloseRawFile(rawFile);
        ASSERT_NE(rawFile64, nullptr);
    }

    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptorData(rawFile, nullptr));
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptor64(rawFile64, nullptr));
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptorData(nullptr, nullptr));
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptor64(nullptr, nullptr));

    RawFileDescriptor nullRawFileDescriptor = {};
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptorData(nullptr, &nullRawFileDescriptor));
    RawFileDescriptor64 nullRawFileDescriptor64 = {};
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptor64(nullptr, &nullRawFileDescriptor64));

    EXPECT_FALSE(OH_ResourceManager_ReleaseRawFileDescriptorData(nullptr));
    EXPECT_FALSE(OH_ResourceManager_ReleaseRawFileDescriptor64(nullptr));

    RawFileDescriptor descriptor = {};
    EXPECT_TRUE(OH_ResourceManager_GetRawFileDescriptorData(rawFile, &descriptor));
    EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptorData(&descriptor));

    RawFileDescriptor64 descriptor64 = {};
    EXPECT_TRUE(OH_ResourceManager_GetRawFileDescriptor64(rawFile64, &descriptor64));
    EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptor64(&descriptor64));

    OH_ResourceManager_CloseRawFile(rawFile);
    OH_ResourceManager_CloseRawFile64(rawFile64);
}

/*
 * @tc.name: Native_RawFileNullParamTest001
 * @tc.desc: Rawfile/rawdir APIs reject null pointers and a manager without a resource manager
 * @tc.type: SECURITY
 */
HWTEST_F(NativeResourceManagerTest, Native_RawFileNullParamTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    NativeResourceManager emptyMgr;
    char buf[8] = {0};
    RawFileDescriptor descriptor = {};
    RawFileDescriptor64 descriptor64 = {};

    EXPECT_EQ(OH_ResourceManager_OpenRawDir(nullptr, "rawfile"), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawDir(mgr, nullptr), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawDir(&emptyMgr, "rawfile"), nullptr);
    EXPECT_EQ(OH_ResourceManager_GetRawFileCount(nullptr), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileName(nullptr, 0), nullptr);
    OH_ResourceManager_CloseRawDir(nullptr);

    EXPECT_EQ(OH_ResourceManager_OpenRawFile(nullptr, "test_rawfile.txt"), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawFile(mgr, nullptr), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawFile(&emptyMgr, "test_rawfile.txt"), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawFile64(nullptr, "test_rawfile.txt"), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawFile64(mgr, nullptr), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawFile64(&emptyMgr, "test_rawfile.txt"), nullptr);

    EXPECT_EQ(OH_ResourceManager_ReadRawFile(nullptr, buf, sizeof(buf)), 0);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile(nullptr, 0, SEEK_SET), -1);
    EXPECT_EQ(OH_ResourceManager_GetRawFileSize(nullptr), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileRemainingLength(nullptr), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(nullptr), 0);
    OH_ResourceManager_CloseRawFile(nullptr);

    EXPECT_EQ(OH_ResourceManager_ReadRawFile64(nullptr, buf, sizeof(buf)), 0);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(nullptr, 0, SEEK_SET), -1);
    EXPECT_EQ(OH_ResourceManager_GetRawFileSize64(nullptr), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileRemainingLength64(nullptr), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(nullptr), 0);
    OH_ResourceManager_CloseRawFile64(nullptr);

    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptorData(nullptr, &descriptor));
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptor(nullptr, descriptor));
    EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptor(descriptor));
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptor64(nullptr, &descriptor64));
    EXPECT_FALSE(OH_ResourceManager_ReleaseRawFileDescriptor64(nullptr));

    EXPECT_EQ(OH_ResourceManager_InitNativeResourceManager(nullptr, nullptr), nullptr);
    OH_ResourceManager_ReleaseNativeResourceManager(nullptr);

    EXPECT_FALSE(OH_ResourceManager_IsRawDir(nullptr, "rawfile"));
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(mgr, nullptr));
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(&emptyMgr, "rawfile"));
}

/*
 * @tc.name: Native_RawDirFromIndexTest001
 * @tc.desc: RawDir traversal on an uncompressed (index) manager
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_RawDirFromIndexTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;

    RawDir *rawDir = OH_ResourceManager_OpenRawDir(mgr, "");
    EXPECT_NE(rawDir, nullptr);
    int rawFileCount = OH_ResourceManager_GetRawFileCount(rawDir);
    EXPECT_GT(rawFileCount, 0);
    bool foundRawFile = false;
    bool foundSubDir = false;
    for (int i = 0; i < rawFileCount; i++) {
        const char *name = OH_ResourceManager_GetRawFileName(rawDir, i);
        EXPECT_NE(name, nullptr);
        if (name == nullptr) {
            continue;
        }
        if (strstr(name, "test_rawfile.txt") != nullptr) {
            foundRawFile = true;
        }
        if (strstr(name, "test_dir") != nullptr) {
            foundSubDir = true;
        }
    }
    EXPECT_TRUE(foundRawFile);
    EXPECT_TRUE(foundSubDir);
    EXPECT_EQ(OH_ResourceManager_GetRawFileName(rawDir, -1), nullptr);
    EXPECT_EQ(OH_ResourceManager_GetRawFileName(rawDir, rawFileCount), nullptr);
    OH_ResourceManager_CloseRawDir(rawDir);

    RawDir *subDir = OH_ResourceManager_OpenRawDir(mgr, "test_dir");
    EXPECT_NE(subDir, nullptr);
    EXPECT_EQ(OH_ResourceManager_GetRawFileCount(subDir), 1);
    OH_ResourceManager_CloseRawDir(subDir);

    RawDir *missingDir = OH_ResourceManager_OpenRawDir(mgr, "no_such_dir");
    EXPECT_NE(missingDir, nullptr);
    EXPECT_EQ(OH_ResourceManager_GetRawFileCount(missingDir), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileName(missingDir, 0), nullptr);
    OH_ResourceManager_CloseRawDir(missingDir);
}

/*
 * @tc.name: Native_RawFileFromIndexTest001
 * @tc.desc: RawFile read/seek/descriptor on an uncompressed (index) manager
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_RawFileFromIndexTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    EXPECT_EQ(OH_ResourceManager_OpenRawFile(mgr, "no_such_file.txt"), nullptr);

    RawFile *rawFile = OH_ResourceManager_OpenRawFile(mgr, "test_rawfile.txt");
    EXPECT_NE(rawFile, nullptr);
    long fileSize = OH_ResourceManager_GetRawFileSize(rawFile);
    EXPECT_GE(fileSize, 4);

    RawFileDescriptor descriptor = {};
    EXPECT_TRUE(OH_ResourceManager_GetRawFileDescriptor(rawFile, descriptor));
    EXPECT_GT(descriptor.fd, 0);
    EXPECT_EQ(descriptor.length, fileSize);
    EXPECT_EQ(descriptor.start, 0);
    EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptor(descriptor));

    char buf[32] = {0};
    EXPECT_EQ(OH_ResourceManager_ReadRawFile(rawFile, nullptr, 4), 0);
    EXPECT_EQ(OH_ResourceManager_ReadRawFile(rawFile, buf, 0), 0);
    EXPECT_EQ(OH_ResourceManager_ReadRawFile(rawFile, buf, 4), 4);

    EXPECT_EQ(OH_ResourceManager_SeekRawFile(rawFile, 4, SEEK_SET), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(rawFile), 4);
    EXPECT_EQ(OH_ResourceManager_GetRawFileRemainingLength(rawFile), fileSize - 4);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile(rawFile, fileSize, SEEK_CUR), -1);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(rawFile), 4);

    EXPECT_EQ(OH_ResourceManager_SeekRawFile(rawFile, -1, SEEK_SET), -1);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile(rawFile, 0, 0xFF), -1);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile(rawFile, fileSize + 1, SEEK_SET), -1);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile(rawFile, -fileSize, SEEK_END), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset(rawFile), 0);

    OH_ResourceManager_CloseRawFile(rawFile);
}

/*
 * @tc.name: Native_RawFile64FromIndexTest001
 * @tc.desc: RawFile64 read/seek/descriptor on an uncompressed (index) manager
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_RawFile64FromIndexTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    EXPECT_EQ(OH_ResourceManager_OpenRawFile64(mgr, "no_such_file.txt"), nullptr);

    RawFile64 *rawFile = OH_ResourceManager_OpenRawFile64(mgr, "test_rawfile.txt");
    EXPECT_NE(rawFile, nullptr);
    int64_t fileSize = OH_ResourceManager_GetRawFileSize64(rawFile);
    EXPECT_GE(fileSize, 4);

    RawFileDescriptor64 descriptor = {};
    EXPECT_TRUE(OH_ResourceManager_GetRawFileDescriptor64(rawFile, &descriptor));
    EXPECT_GT(descriptor.fd, 0);
    EXPECT_EQ(descriptor.length, fileSize);
    EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptor64(&descriptor));

    char buf[32] = {0};
    EXPECT_EQ(OH_ResourceManager_ReadRawFile64(rawFile, nullptr, 4), 0);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, -1, SEEK_SET), -1);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, 0, 0xFF), -1);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, fileSize + 1, SEEK_SET), -1);

    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, 0, SEEK_SET), 0);
    EXPECT_EQ(OH_ResourceManager_ReadRawFile64(rawFile, buf, 4), 4);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(rawFile), 4);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, -1, SEEK_CUR), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(rawFile), 3);

    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, -4, SEEK_CUR), -1);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(rawFile), 3);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, fileSize, SEEK_CUR), -1);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(rawFile), 3);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, -3, SEEK_END), 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(rawFile), fileSize - 3);
    EXPECT_EQ(OH_ResourceManager_GetRawFileRemainingLength64(rawFile), 3);

    std::vector<char> bigBuffer(static_cast<size_t>(fileSize) * 2, 0);
    EXPECT_EQ(OH_ResourceManager_SeekRawFile64(rawFile, 0, SEEK_SET), 0);
    EXPECT_EQ(OH_ResourceManager_ReadRawFile64(rawFile, bigBuffer.data(), fileSize * 2), fileSize);
    EXPECT_EQ(OH_ResourceManager_GetRawFileOffset64(rawFile), fileSize);
    EXPECT_EQ(OH_ResourceManager_ReadRawFile64(rawFile, buf, 4), 0);

    OH_ResourceManager_CloseRawFile64(rawFile);
}

/*
 * @tc.name: Native_IsRawDirFromIndexTest001
 * @tc.desc: IsRawDir on an uncompressed (index) manager, which resolves the given path directly
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_IsRawDirFromIndexTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;

    EXPECT_TRUE(OH_ResourceManager_IsRawDir(mgr,
        FormatFullPath("all/assets/entry/resources/rawfile").c_str()));
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(mgr,
        FormatFullPath("all/assets/entry/resources/rawfile/test_rawfile.txt").c_str()));
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(mgr,
        FormatFullPath("all/assets/entry/resources/rawfile/no_such_dir").c_str()));
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(mgr, ""));
}

/*
 * @tc.name: Native_ReleaseNativeResourceManager001
 * @tc.desc: ReleaseNativeResourceManager accepts null/heap manager and keeps the original manager usable
 * @tc.type: FUNC
 */
HWTEST_F(NativeResourceManagerTest, Native_ReleaseNativeResourceManager001, TestSize.Level1)
{
    OH_ResourceManager_ReleaseNativeResourceManager(nullptr);

    NativeResourceManager *mgr = new NativeResourceManager();
    mgr->resManager = nativeMgr.resManager;
    OH_ResourceManager_ReleaseNativeResourceManager(mgr);

    RawFile *rawFile = OH_ResourceManager_OpenRawFile(&nativeMgr, "test_rawfile.txt");
    EXPECT_NE(rawFile, nullptr);
    EXPECT_GT(OH_ResourceManager_GetRawFileSize(rawFile), 0);
    OH_ResourceManager_CloseRawFile(rawFile);
}

/*
 * @tc.name: Native_RawDirFromHapTest001
 * @tc.desc: RawDir traversal on a HAP manager, empty dirName lists the rawfile root
 * @tc.type: FUNC
 */
HWTEST_F(NativeRawFileHapTest, Native_RawDirFromHapTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;

    RawDir *rawDir = OH_ResourceManager_OpenRawDir(mgr, "");
    EXPECT_NE(rawDir, nullptr);
    int rawFileCount = OH_ResourceManager_GetRawFileCount(rawDir);
    EXPECT_GT(rawFileCount, 0);
    EXPECT_EQ(OH_ResourceManager_GetRawFileName(rawDir, rawFileCount), nullptr);
    OH_ResourceManager_CloseRawDir(rawDir);

    EXPECT_EQ(OH_ResourceManager_OpenRawDir(mgr, "no_such_dir"), nullptr);
}

/*
 * @tc.name: Native_RawPathFromHapTest001
 * @tc.desc: HAP mode: existing raw file opens, missing raw file and non-directory paths are rejected
 * @tc.type: FUNC
 */
HWTEST_F(NativeRawFileHapTest, Native_RawPathFromHapTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;

    RawFile *rawFile = OH_ResourceManager_OpenRawFile(mgr, "test_rawfile.txt");
    EXPECT_NE(rawFile, nullptr);
    EXPECT_GT(OH_ResourceManager_GetRawFileSize(rawFile), 0);
    OH_ResourceManager_CloseRawFile(rawFile);

    EXPECT_EQ(OH_ResourceManager_OpenRawFile(mgr, "no_such_file.txt"), nullptr);
    EXPECT_EQ(OH_ResourceManager_OpenRawFile64(mgr, "no_such_file.txt"), nullptr);
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(mgr, "rawfile/test_rawfile.txt"));
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(mgr, "rawfile/no_such_dir"));
    EXPECT_FALSE(OH_ResourceManager_IsRawDir(mgr, ""));
}

/*
 * @tc.name: Native_RawFileDescriptorFromHapFailTest001
 * @tc.desc: Descriptor query returns false when the hap can no longer provide the file after the handle is opened,
 *           and succeeds again once the patch is recovered
 * @tc.type: FUNC
 */
HWTEST_F(NativeRawFileHapTest, Native_RawFileDescriptorFromHapFailTest001, TestSize.Level1)
{
    NativeResourceManager *mgr = &nativeMgr;
    std::string hapPath = FormatFullPath(g_newModuleHapPath);

    RawFile *rawFile = OH_ResourceManager_OpenRawFile(mgr, "test_rawfile.txt");
    EXPECT_NE(rawFile, nullptr);
    RawFile64 *rawFile64 = OH_ResourceManager_OpenRawFile64(mgr, "test_rawfile.txt");
    EXPECT_NE(rawFile64, nullptr);

    // A patch path that cannot be opened makes GetRawFdNdkFromHap fail while the handles stay valid.
    EXPECT_TRUE(rm->AddPatchResource(hapPath.c_str(), "/data/test/no_such_patch.hap"));
    RawFileDescriptor descriptor = {};
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptorData(rawFile, &descriptor));
    RawFileDescriptor64 descriptor64 = {};
    EXPECT_FALSE(OH_ResourceManager_GetRawFileDescriptor64(rawFile64, &descriptor64));

    EXPECT_TRUE(rm->AddPatchResource(hapPath.c_str(), ""));
    RawFileDescriptor recovered = {};
    EXPECT_TRUE(OH_ResourceManager_GetRawFileDescriptorData(rawFile, &recovered));
    EXPECT_GT(recovered.fd, 0);
    EXPECT_TRUE(OH_ResourceManager_ReleaseRawFileDescriptorData(&recovered));

    OH_ResourceManager_CloseRawFile(rawFile);
    OH_ResourceManager_CloseRawFile64(rawFile64);
}
