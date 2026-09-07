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

#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <securec.h>

#include "resource_manager.h"
#include "ohresmgr.h"
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
