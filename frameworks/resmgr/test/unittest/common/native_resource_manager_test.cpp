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
#include <gtest/gtest.h>
#include <memory>

#include "resource_manager.h"
#include "ohresmgr.h"
#include "resmgr_common.h"
#include "test_common.h"

// Layout-compatible definition of the opaque NativeResourceManager (single member: shared_ptr).
struct NativeResourceManager {
    std::shared_ptr<OHOS::Global::Resource::ResourceManager> resManager = nullptr;
};

using namespace OHOS::Global::Resource;
using namespace testing::ext;

namespace {
constexpr uint32_t DENSITY_DEFAULT = 0;
}

class NativeResourceManagerTest : public testing::Test {
public:
    void SetUp() override
    {
        rm = CreateResourceManager(false);
        ASSERT_NE(rm, nullptr);
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
};

/*
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
