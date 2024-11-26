/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "resmgr_fuzzer.h"

#include <string>
#include <securec.h>
#include <vector>
#include "resource_manager.h"

#undef private

using namespace std;
using namespace OHOS::Global::Resource;

namespace OHOS {

    constexpr int SINGULAR_NUM = 1;
    constexpr int PLURAL_NUM = 2;

    bool AddResourceFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            result = rm->AddResource(testName.c_str());
        }
        return result;
    }

    bool RemoveResourceFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        std::vector<std::string> overlayPaths;
        if (size > 0) {
            std::string testName(data, size);
            result = rm->RemoveResource(testName.c_str(), overlayPaths);
        }
        return result;
    }

    bool AddAppOverlayFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            result = rm->AddAppOverlay(testName.c_str());
        }
        return result;
    }

    bool RemoveAppOverlayFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            result = rm->RemoveAppOverlay(testName.c_str());
        }
        return result;
    }

    bool GetStringByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::string extraInfo = "";
            result = rm->GetStringById(testId, extraInfo);
        }
        return result;
    }

    bool GetStringByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::string extraInfo = "";
            result = rm->GetStringByName(testName.c_str(), extraInfo);
        }
        return result;
    }

    bool GetStringArrayByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::vector<std::string> outValue;
            result = rm->GetStringArrayById(testId, outValue);
        }
        return result;
    }

    bool GetStringArrayByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::vector<std::string> outValue;
            result = rm->GetStringArrayByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetPatternByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::map<std::string, std::string> outValue;
            result = rm->GetPatternById(testId, outValue);
        }
        return result;
    }

    bool GetPatternByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::map<std::string, std::string> outValue;
            result = rm->GetPatternByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetPluralStringByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::string outValue = "";
            result = rm->GetPluralStringById(testId, SINGULAR_NUM, outValue);
        }
        return result;
    }

    bool GetPluralStringByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::string outValue = "";
            result = rm->GetPluralStringByName(testName.c_str(), PLURAL_NUM, outValue);
        }
        return result;
    }

    bool GetThemeByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::map<std::string, std::string> outValue;
            result = rm->GetThemeById(testId, outValue);
        }
        return result;
    }

    bool GetThemeByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::map<std::string, std::string> outValue;
            result = rm->GetThemeByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetIntegerByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            int outValue;
            result = rm->GetIntegerById(testId, outValue);
        }
        return result;
    }

    bool GetIntegerByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            int outValue;
            result = rm->GetIntegerByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetBooleanByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            bool outValue;
            result = rm->GetBooleanById(testId, outValue);
        }
        return result;
    }

    bool GetBooleanByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            bool outValue = false;
            result = rm->GetBooleanByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetFloatByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            float outValue;
            result = rm->GetFloatById(testId, outValue);
        }
        return result;
    }

    bool GetFloatByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            float outValue = 0.0f;
            result = rm->GetFloatByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetIntArrayByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::vector<int> outValue;
            result = rm->GetIntArrayById(testId, outValue);
        }
        return result;
    }

    bool GetIntArrayByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::vector<int> outValue;
            result = rm->GetIntArrayByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetMediaByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::string outValue = "";
            result = rm->GetMediaById(testId, outValue);
        }
        return result;
    }

    bool GetMediaByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::string outValue = "";
            result = rm->GetMediaByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetRawFilePathByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::string outValue = "";
            result = rm->GetRawFilePathByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetRawFileDescriptorFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            ResourceManager::RawFileDescriptor outValue;
            result = rm->GetRawFileDescriptor(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetRawFdNdkFromHapFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            ResourceManager::RawFileDescriptor outValue;
            result = rm->GetRawFdNdkFromHap(testName.c_str(), outValue);
        }
        return result;
    }

    bool CloseRawFileDescriptorFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            result = rm->CloseRawFileDescriptor(testName.c_str());
        }
        return result;
    }

    bool GetMediaDataByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            size_t len = 0;
            std::unique_ptr<uint8_t[]> outValue;
            result = rm->GetMediaDataById(testId, len, outValue);
        }
        return result;
    }

    bool GetMediaDataByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            size_t len = 0;
            std::unique_ptr<uint8_t[]> outValue;
            result = rm->GetMediaDataByName(testName.c_str(), len, outValue);
        }
        return result;
    }

    bool GetMediaBase64DataByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::string outValue;
            result = rm->GetMediaBase64DataById(testId, outValue);
        }
        return result;
    }

    bool GetMediaBase64DataByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::string outValue;
            result = rm->GetMediaBase64DataByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetProfileDataByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            size_t len = 0;
            std::unique_ptr<uint8_t[]> outValue;
            result = rm->GetProfileDataById(testId, len, outValue);
        }
        return result;
    }

    bool GetProfileDataByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            size_t len = 0;
            std::unique_ptr<uint8_t[]> outValue;
            result = rm->GetProfileDataByName(testName.c_str(), len, outValue);
        }
        return result;
    }

    bool GetRawFileFromHapFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            size_t len = 0;
            std::unique_ptr<uint8_t[]> outValue;
            result = rm->GetRawFileFromHap(testName.c_str(), len, outValue);
        }
        return result;
    }

    bool GetRawFileDescriptorFromHapFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            ResourceManager::RawFileDescriptor outValue;
            result = rm->GetRawFileDescriptorFromHap(testName.c_str(), outValue);
        }
        return result;
    }

    bool IsLoadHapFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            result = rm->IsLoadHap(testName);
        }
        return result;
    }

    bool GetRawFileListFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::vector<std::string> outValue;
            result = rm->GetRawFileList(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetResIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::string testName(data, size);
            result = rm->GetResId(testName.c_str(), testId);
        }
        return result;
    }

    bool GetThemeIconsFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::pair<std::unique_ptr<uint8_t[]>, size_t> foregroundInfo;
            std::pair<std::unique_ptr<uint8_t[]>, size_t> backgroundInfo;
            result = rm->GetThemeIcons(testId, foregroundInfo, backgroundInfo);
        }
        return result;
    }

    bool GetStringFormatByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::string outValue = "";
            result = rm->GetStringFormatById(outValue, testId);
        }
        return result;
    }

    bool GetColorByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            uint32_t outValue = 0;
            result = rm->GetColorById(testId, outValue);
        }
        return result;
    }

    bool GetColorByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            uint32_t outValue = 0;
            result = rm->GetColorByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetSymbolByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            uint32_t outValue = 0;
            result = rm->GetSymbolById(testId, outValue);
        }
        return result;
    }

    bool GetSymbolByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            uint32_t outValue = 0;
            result = rm->GetSymbolByName(testName.c_str(), outValue);
        }
        return result;
    }

    bool GetProfileByIdFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            uint32_t testId = static_cast<uint32_t>(atoi(data));
            std::string outValue;
            result = rm->GetProfileById(testId, outValue);
        }
        return result;
    }

    bool GetProfileByNameFuzzTest(const char* data, size_t size, ResourceManager *rm)
    {
        bool result = false;
        if (size > 0) {
            std::string testName(data, size);
            std::string outValue;
            result = rm->GetProfileByName(testName.c_str(), outValue);
        }
        return result;
    }

    void ResourceManagerImplAdd(const char* data, size_t size, ResourceManager *rm)
    {
        AddResourceFuzzTest(data, size, rm);
        RemoveResourceFuzzTest(data, size, rm);
        AddAppOverlayFuzzTest(data, size, rm);
        RemoveAppOverlayFuzzTest(data, size, rm);
        GetStringByNameFuzzTest(data, size, rm);
        GetStringArrayByNameFuzzTest(data, size, rm);
        GetPatternByIdFuzzTest(data, size, rm);
        GetPatternByNameFuzzTest(data, size, rm);
        GetPluralStringByIdFuzzTest(data, size, rm);
        GetPluralStringByNameFuzzTest(data, size, rm);
        GetThemeByIdFuzzTest(data, size, rm);
        GetThemeByNameFuzzTest(data, size, rm);
        GetBooleanByNameFuzzTest(data, size, rm);
        GetFloatByNameFuzzTest(data, size, rm);
        GetIntArrayByIdFuzzTest(data, size, rm);
        GetIntArrayByNameFuzzTest(data, size, rm);
        GetRawFilePathByNameFuzzTest(data, size, rm);
        GetRawFileDescriptorFuzzTest(data, size, rm);
        GetRawFdNdkFromHapFuzzTest(data, size, rm);
        CloseRawFileDescriptorFuzzTest(data, size, rm);
        GetMediaDataByIdFuzzTest(data, size, rm);
        GetMediaDataByNameFuzzTest(data, size, rm);
        GetMediaBase64DataByIdFuzzTest(data, size, rm);
        GetMediaBase64DataByNameFuzzTest(data, size, rm);
    }

    void ResourceManagerImplFuzzTest(const char* data, size_t size)
    {
        ResourceManager *rm = CreateResourceManager();
        if (rm == nullptr) {
            return;
        }
        ResourceManagerImplAdd(data, size, rm);
        GetProfileDataByIdFuzzTest(data, size, rm);
        GetProfileDataByNameFuzzTest(data, size, rm);
        GetRawFileFromHapFuzzTest(data, size, rm);
        GetRawFileDescriptorFromHapFuzzTest(data, size, rm);
        IsLoadHapFuzzTest(data, size, rm);
        GetRawFileListFuzzTest(data, size, rm);
        GetResIdFuzzTest(data, size, rm);
        GetThemeIconsFuzzTest(data, size, rm);
        GetColorByNameFuzzTest(data, size, rm);
        GetSymbolByIdFuzzTest(data, size, rm);
        GetSymbolByNameFuzzTest(data, size, rm);
        GetProfileByIdFuzzTest(data, size, rm);
        GetProfileByNameFuzzTest(data, size, rm);
        GetStringByIdFuzzTest(data, size, rm);
        GetStringArrayByIdFuzzTest(data, size, rm);
        GetIntegerByIdFuzzTest(data, size, rm);
        GetIntegerByNameFuzzTest(data, size, rm);
        GetBooleanByIdFuzzTest(data, size, rm);
        GetFloatByIdFuzzTest(data, size, rm);
        GetMediaByIdFuzzTest(data, size, rm);
        GetMediaByNameFuzzTest(data, size, rm);
        GetStringFormatByIdFuzzTest(data, size, rm);
        GetColorByIdFuzzTest(data, size, rm);
        delete rm;
        return;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    char* ch = (char *)malloc(size + 1);
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size, 0x00, size);
    if (memcpy_s(ch, size, data, size) != 0) {
        free(ch);
        ch = nullptr;
        return 0;
    }
    OHOS::ResourceManagerImplFuzzTest(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}

