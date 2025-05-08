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

#include "hap_manager_test.h"

#include <gtest/gtest.h>

#include "test_common.h"
#include "utils/string_utils.h"

#define private public

#include "hap_manager.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
namespace {
static const int NON_EXIST_ID = 1111;
class HapManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapManagerTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void HapManagerTest::TearDownTestCase()
{
    // step 2: input testsuit teardown step
}

void HapManagerTest::SetUp()
{
    // step 3: input testcase setup step
    RESMGR_HILOGD(RESMGR_TAG, "HapManagerTest setup");
}

void HapManagerTest::TearDown()
{
    // step 4: input testcase teardown step
    RESMGR_HILOGD(RESMGR_TAG, "HapManagerTest teardown");
}

/*
 * this test shows how to load a hap, then find value list by id
 * @tc.name: HapManagerFuncTest001
 * @tc.desc: Test AddResourcePath & GetResourceList function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFuncTest001, TestSize.Level1)
{
    HapManager *hapManager = new HapManager(std::make_shared<ResConfigImpl>());
    bool ret = hapManager->AddResourcePath(FormatFullPath(g_resFilePath).c_str());

    EXPECT_TRUE(ret);

    int id = 16777217;
    auto idValues = hapManager->GetResourceList(id);
    if (idValues.size() == 0) {
        delete hapManager;
        ASSERT_TRUE(false);
    }

    PrintIdValues(idValues[0]);
    delete hapManager;
}

/*
 * this test shows how to reload a hap
 * @tc.name: HapManagerFuncTest002
 * @tc.desc: Test UpdateResConfig & AddResourcePath function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFuncTest002, TestSize.Level1)
{
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    std::string resPath = FormatFullPath(g_resFilePath);
    const char *path = resPath.c_str();
    HapManager *hapManager = new HapManager(std::make_shared<ResConfigImpl>());
    if (hapManager == nullptr) {
        delete (rc);
        ASSERT_TRUE(false);
    }
    hapManager->UpdateResConfig(*rc);
    bool ret = hapManager->AddResourcePath(path);

    EXPECT_TRUE(ret);

    uint32_t id = 16777238; // 16777238 means string_ref
    auto idValues = hapManager->GetResourceList(id);
    if (idValues.size() == 0) {
        delete (hapManager);
        delete (rc);
        ASSERT_TRUE(false);
    }

    EXPECT_EQ(static_cast<size_t>(1), idValues[0]->GetLimitPathsConst().size());
    PrintIdValues(idValues[0]);

    // reload

    auto rc2 = CreateResConfig();
    if (rc2 == nullptr) {
        delete (hapManager);
        delete (rc);
        ASSERT_TRUE(false);
    }

    rc2->SetLocaleInfo("zh", nullptr, "CN");
    hapManager->UpdateResConfig(*rc2);
    do {
        idValues = hapManager->GetResourceList(id);
        if (idValues.size() == 0) {
            EXPECT_TRUE(false);
            break;
        }

        EXPECT_EQ(static_cast<size_t>(2), idValues[0]->GetLimitPathsConst().size());

        PrintIdValues(idValues[0]);
    } while (false);
    delete (hapManager);
    delete (rc2);
    delete (rc);
}

/*
 * this test shows how to load resources from hap, then find value list by id
 * @tc.name: HapManagerFuncTest003
 * @tc.desc: Test AddResourcePath & GetResourceList function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFuncTest003, TestSize.Level1)
{
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("zh", nullptr, "CN");
    HapManager *hapManager = new HapManager(std::make_shared<ResConfigImpl>());
    hapManager->UpdateResConfig(*rc);
    hapManager->ReloadAll();
    if (hapManager == nullptr) {
        ASSERT_TRUE(false);
    }
    bool ret = hapManager->AddResourcePath(FormatFullPath(g_hapPath).c_str());

    EXPECT_TRUE(ret);

    int id = 16777221; // 16777221 means 'aboutPage_title_primary'
    auto idValues = hapManager->GetResourceList(id);
    if (idValues.size() == 0) {
        delete hapManager;
        ASSERT_TRUE(false);
    }
    PrintIdValues(idValues[0]);

    idValues = hapManager->GetResourceList(NON_EXIST_ID);
    if (idValues.size() == 0) {
        delete hapManager;
        EXPECT_TRUE(true);
        return;
    }
    delete hapManager;
}

/*
 * this test shows how to load resources from hap, then find value list by id
 * @tc.name: HapManagerFuncTest004
 * @tc.desc: Test AddResourcePath & GetResourceList function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFuncTest004, TestSize.Level1)
{
    auto rc = CreateResConfig();
    if (rc == nullptr) {
        ASSERT_TRUE(false);
    }
    rc->SetLocaleInfo("en", nullptr, "US");
    HapManager *hapManager = new HapManager(std::make_shared<ResConfigImpl>());
    EXPECT_FALSE(hapManager == nullptr);
    hapManager->UpdateResConfig(*rc);
    hapManager->ReloadAll();
    bool ret = hapManager->AddResourcePath(FormatFullPath(g_hapPath).c_str());
    EXPECT_TRUE(ret);
    int id = 16777221; // 16777221 means 'AboutPageFA' 'aboutPage_title_primary'
    auto idValues = hapManager->GetResourceList(id);
    if (idValues.size() == 0) {
        delete hapManager;
        ASSERT_TRUE(false);
    }
    PrintIdValues(idValues[0]);

    idValues = hapManager->GetResourceList(NON_EXIST_ID);
    if (idValues.size() == 0) {
        delete hapManager;
        EXPECT_TRUE(true);
        return;
    }
    delete hapManager;
}

/*
 * @tc.name: HapManagerGetPluralRulesAndSelectTest001
 * @tc.desc: Test GetPluralRulesAndSelect function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetPluralRulesAndSelectTest001, TestSize.Level1)
{
    std::shared_ptr<ResConfigImpl> resConfig = std::make_shared<ResConfigImpl>();
    ASSERT_TRUE(resConfig != nullptr);
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(resConfig, false);
    ASSERT_TRUE(hapManager != nullptr);
    ResourceManager::Quantity quantity;
    std::string result = hapManager->GetPluralRulesAndSelect(quantity, false);
    EXPECT_TRUE(result == "other");
}

/*
 * @tc.name: HapManagerGetBestMatchResourceTest001
 * @tc.desc: Test GetBestMatchResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetBestMatchResourceTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::vector<std::shared_ptr<HapResource::IdValues>> candidates;
    auto result = hapManager->GetBestMatchResource(candidates, 0, false);
    EXPECT_TRUE(result == nullptr);
}

/*
 * @tc.name: HapManagerFindRawFileTest001
 * @tc.desc: Test FindRawFile function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFindRawFileTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::string name = "";
    std::string outValue = "";
    RState result = hapManager->FindRawFile(name, outValue);
    EXPECT_EQ(result, ERROR_CODE_RES_PATH_INVALID);
    EXPECT_TRUE(outValue == "");
}

/*
 * @tc.name: HapManagerGetMediaDataFromHapTest001
 * @tc.desc: Test GetMediaDataFromHap function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetMediaDataFromHapTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<ResKey> resKey = std::make_shared<ResKey>();
    ASSERT_TRUE(resKey != nullptr);
    resKey->resConfig_ = std::make_shared<ResConfigImpl>();
    std::pair<std::string, std::string> resPath;
    std::shared_ptr<HapResource::ValueUnderQualifierDir> qd =
        std::make_shared<HapResource::ValueUnderQualifierDir>(resKey, nullptr, resPath);
    ASSERT_TRUE(qd != nullptr);
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState result = hapManager->GetMediaDataFromHap(qd, len, outValue);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerGetMediaDataFromHapTest002
 * @tc.desc: Test GetMediaDataFromHap function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetMediaDataFromHapTest002, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<ResKey> resKey = std::make_shared<ResKey>();
    ASSERT_TRUE(resKey != nullptr);
    resKey->resConfig_ = std::make_shared<ResConfigImpl>();
    std::pair<std::string, std::string> resPath;
    std::shared_ptr<HapResource::ValueUnderQualifierDir> qd =
        std::make_shared<HapResource::ValueUnderQualifierDir>(resKey, nullptr, resPath);
    ASSERT_TRUE(qd != nullptr);
    qd->indexPath_ = "/data/test/all.hap";
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState result = hapManager->GetMediaDataFromHap(qd, len, outValue);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerGetMediaDataFromIndexTest001
 * @tc.desc: Test GetMediaDataFromIndex function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetMediaDataFromIndexTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<HapResource::ValueUnderQualifierDir> qd = nullptr;
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState result = hapManager->GetMediaDataFromIndex(qd, len, outValue);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerGetMediaBase64DataFromHapTest001
 * @tc.desc: Test GetMediaBase64DataFromHap function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetMediaBase64DataFromHapTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<ResKey> resKey = std::make_shared<ResKey>();
    ASSERT_TRUE(resKey != nullptr);
    resKey->resConfig_ = std::make_shared<ResConfigImpl>();
    std::pair<std::string, std::string> resPath;
    std::shared_ptr<HapResource::ValueUnderQualifierDir> qd =
        std::make_shared<HapResource::ValueUnderQualifierDir>(resKey, nullptr, resPath);
    ASSERT_TRUE(qd != nullptr);
    std::string outValue;
    RState result = hapManager->GetMediaBase64DataFromHap(qd, outValue);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerGetMediaBase64DataFromHapTest002
 * @tc.desc: Test GetMediaBase64DataFromHap function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetMediaBase64DataFromHapTest002, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<ResKey> resKey = std::make_shared<ResKey>();
    ASSERT_TRUE(resKey != nullptr);
    resKey->resConfig_ = std::make_shared<ResConfigImpl>();
    std::pair<std::string, std::string> resPath;
    std::shared_ptr<HapResource::ValueUnderQualifierDir> qd =
        std::make_shared<HapResource::ValueUnderQualifierDir>(resKey, nullptr, resPath);
    ASSERT_TRUE(qd != nullptr);
    qd->indexPath_ = "/data/test/all.hap";
    std::string outValue;
    RState result = hapManager->GetMediaBase64DataFromHap(qd, outValue);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerGetMediaBase64DataFromIndexTest001
 * @tc.desc: Test GetMediaBase64DataFromIndex function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetMediaBase64DataFromIndexTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<HapResource::ValueUnderQualifierDir> qd = nullptr;
    std::string outValue;
    RState result = hapManager->GetMediaBase64DataFromIndex(qd, outValue);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerGetValidIndexPathTest001
 * @tc.desc: Test GetValidIndexPath function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetValidIndexPathTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::string indexPath;
    int32_t result = hapManager->GetValidIndexPath(indexPath);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerGetValidIndexPathTest002
 * @tc.desc: Test GetValidIndexPath function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetValidIndexPathTest002, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<HapResource> pResource = std::make_shared<HapResource>("/data/test/systemres/resources.index",
        1000, nullptr, false, false);
    ASSERT_TRUE(pResource != nullptr);
    hapManager->hapResources_.emplace_back(pResource);
    std::string indexPath;
    int32_t result = hapManager->GetValidIndexPath(indexPath);
    EXPECT_EQ(result, NOT_FOUND);
}

/*
 * @tc.name: HapManagerFindRawFileFromHapTest001
 * @tc.desc: Test FindRawFileFromHap function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFindRawFileFromHapTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<HapResource> pResource = std::make_shared<HapResource>("/data/test/all.hap",
        1000, nullptr, false, false);
    ASSERT_TRUE(pResource != nullptr);
    pResource->isPatch_ = true;
    pResource->patchPath_ = "";
    hapManager->hapResources_.emplace_back(pResource);
    std::string rawFileName = "";
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState result = hapManager->FindRawFileFromHap(rawFileName, len, outValue);
    EXPECT_EQ(result, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: HapManagerFindRawFileFromHapTest002
 * @tc.desc: Test FindRawFileFromHap function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFindRawFileFromHapTest002, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<HapResource> pResource = std::make_shared<HapResource>(
        "/data/test/all/assets/entry/resources.index", 1000, nullptr, false, false);
    ASSERT_TRUE(pResource != nullptr);
    hapManager->hapResources_.emplace_back(pResource);
    std::string rawFileName = "/test";
    size_t len;
    std::unique_ptr<uint8_t[]> outValue;
    RState result = hapManager->FindRawFileFromHap(rawFileName, len, outValue);
    EXPECT_EQ(result, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: HapManagerGetRawFdTest001
 * @tc.desc: Test GetRawFd function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetRawFdTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<HapResource> pResource = std::make_shared<HapResource>("/data/test/all.hap",
        1000, nullptr, false, false);
    ASSERT_TRUE(pResource != nullptr);
    pResource->isPatch_ = true;
    pResource->patchPath_ = "";
    hapManager->hapResources_.emplace_back(pResource);
    std::string rawFileName = "";
    ResourceManager::RawFileDescriptor descriptor;
    RState result = hapManager->GetRawFd(rawFileName, descriptor);
    EXPECT_EQ(result, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: HapManagerGetRawFileListTest001
 * @tc.desc: Test GetRawFileList function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetRawFileListTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::shared_ptr<HapResource> pResource = std::make_shared<HapResource>("/data/test/all.hap",
        1000, nullptr, false, false);
    ASSERT_TRUE(pResource != nullptr);
    pResource->isPatch_ = true;
    pResource->patchPath_ = "/test";
    hapManager->hapResources_.emplace_back(pResource);
    std::string rawDirPath = "";
    std::vector<std::string> fileList;
    RState result = hapManager->GetRawFileList(rawDirPath, fileList);
    EXPECT_EQ(result, SUCCESS);
    EXPECT_TRUE(fileList.size() > 0);
}

/*
 * @tc.name: HapManagerGetRawFileListTest002
 * @tc.desc: Test GetRawFileList function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetRawFileListTest002, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::string rawDirPath = "";
    std::vector<std::string> fileList;
    RState result = hapManager->GetRawFileList(rawDirPath, fileList);
    EXPECT_EQ(result, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: HapManagerFindRawFileDescriptorTest001
 * @tc.desc: Test FindRawFileDescriptor function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerFindRawFileDescriptorTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::string name = "";
    ResourceManager::RawFileDescriptor descriptor;
    RState result = hapManager->FindRawFileDescriptor(name, descriptor);
    EXPECT_EQ(result, ERROR_CODE_RES_PATH_INVALID);
}

/*
 * @tc.name: HapManagerRemoveResourceTest001
 * @tc.desc: Test RemoveResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerRemoveResourceTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::string path;
    std::vector<std::string> overlayPaths;
    bool result = hapManager->RemoveResource(path, overlayPaths);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: HapManagerRemoveResourceTest002
 * @tc.desc: Test RemoveResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerRemoveResourceTest002, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    std::string path = "/data/test/all.hap";
    std::vector<std::string> overlayPaths;
    hapManager->loadedHapPaths_[path] = overlayPaths;
    bool result = hapManager->RemoveResource(path, overlayPaths);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: HapManagerAddSystemResourceTest001
 * @tc.desc: Test AddSystemResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerAddSystemResourceTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    hapManager->AddSystemResource(nullptr);
    EXPECT_TRUE(hapManager->hapResources_.size() == 0);
}

/*
 * @tc.name: HapManagerAddSystemResourceTest002
 * @tc.desc: Test AddSystemResource function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerAddSystemResourceTest002, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, false);
    ASSERT_TRUE(hapManager != nullptr);
    hapManager->AddSystemResource(hapManager);
    EXPECT_TRUE(hapManager->hapResources_.size() == 0);
}

/*
 * @tc.name: HapManagerGetLocalesTest001
 * @tc.desc: Test GetLocales function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(HapManagerTest, HapManagerGetLocalesTest001, TestSize.Level1)
{
    std::shared_ptr<HapManager> hapManager = std::make_shared<HapManager>(nullptr, true);
    ASSERT_TRUE(hapManager != nullptr);
    std::vector<std::string> outValue;
    hapManager->GetLocales(outValue, false);
    EXPECT_TRUE(outValue.size() == 0);
}
}