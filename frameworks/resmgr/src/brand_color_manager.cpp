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

#include "brand_color_manager.h"

#include <climits>
#include <cstdio>
#include <mutex>

#include "cJSON.h"
#include "hilog_wrapper.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {

namespace {
constexpr const char *BRAND_COLOR_CONFIG_PATH = "/system/etc/brand_color_override.json";
constexpr size_t MAX_CONFIG_SIZE = 1024 * 1024; // 1MB safety cap

// RAII wrapper for std::FILE* so every early-return path auto-closes the handle.
struct FileCloser {
    void operator()(FILE *pf) const
    {
        if (pf != nullptr && std::fclose(pf) != 0) {
            RESMGR_HILOGE(RESMGR_TAG, "fclose failed for brand color config");
        }
    }
};
using UniqueFile = std::unique_ptr<FILE, FileCloser>;

std::unique_ptr<char[]> ReadConfigFile(const std::string &path)
{
    char realPath[PATH_MAX + 1] = {0};
    Utils::CanonicalizePath(path.c_str(), realPath, PATH_MAX);
    if (realPath[0] == '\0') {
        RESMGR_HILOGE(RESMGR_TAG, "canonicalize path failed for brand color config");
        return nullptr;
    }
    UniqueFile pf(std::fopen(realPath, "r"));
    if (pf == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "fopen failed for brand color config");
        return nullptr;
    }
    // /system/etc/brand_color_override.json is treated as untrusted input; every
    // step is validated. fseek may fail on non-seekable files, in which case
    // ftell/fread behavior is undefined, so bail out early.
    if (std::fseek(pf.get(), 0, SEEK_END) != 0) {
        RESMGR_HILOGE(RESMGR_TAG, "fseek SEEK_END failed for brand color config");
        return nullptr;
    }
    long len = ftell(pf.get());
    if (len < 0) {
        RESMGR_HILOGE(RESMGR_TAG, "ftell failed for brand color config");
        return nullptr;
    }
    if (static_cast<size_t>(len) > MAX_CONFIG_SIZE) {
        RESMGR_HILOGE(RESMGR_TAG, "brand color config file too large: %{public}ld", len);
        return nullptr;
    }
    if (std::fseek(pf.get(), 0, SEEK_SET) != 0) {
        RESMGR_HILOGE(RESMGR_TAG, "fseek SEEK_SET failed for brand color config");
        return nullptr;
    }
    auto data = std::make_unique<char[]>(static_cast<size_t>(len) + 1);
    size_t readLen = std::fread(data.get(), 1, static_cast<size_t>(len), pf.get());
    if (readLen != static_cast<size_t>(len)) {
        RESMGR_HILOGE(RESMGR_TAG, "short read for brand color config, read %{public}zu/%{public}ld", readLen, len);
        return nullptr;
    }
    data[readLen] = '\0';
    return data;
}

void ParseOverrideList(BrandColorManager::OverrideList &list, const char *jsonStr)
{
    if (jsonStr == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "brand color config json string is null");
        return;
    }
    cJSON *root = cJSON_Parse(jsonStr);
    if (root == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "brand color config JSON parse failed");
        return;
    }
    cJSON *colors = cJSON_GetObjectItem(root, "colors");
    if (colors != nullptr && cJSON_IsArray(colors)) {
        cJSON *item = nullptr;
        cJSON_ArrayForEach(item, colors) {
            if (cJSON_IsString(item) && item->valuestring != nullptr) {
                list.insert(std::string(item->valuestring));
            }
        }
    }
    cJSON_Delete(root);
}
} // namespace

BrandColorManager& BrandColorManager::GetInstance()
{
    static BrandColorManager instance;
    return instance;
}

BrandColorManager::BrandColorManager() {}

void BrandColorManager::LoadConfig()
{
    std::call_once(loadOnceFlag_, [this]() {
        auto data = ReadConfigFile(BRAND_COLOR_CONFIG_PATH);
        if (data == nullptr) {
            RESMGR_HILOGI(RESMGR_TAG, "brand color config not loaded (file absent or unreadable)");
            return;
        }
        ParseOverrideList(overrideList_, data.get());
        loaded_.store(true, std::memory_order_release);
        RESMGR_HILOGI(RESMGR_TAG, "brand color config loaded, override list size: %{public}zu",
            overrideList_.size());
    });
}

bool BrandColorManager::ApplyBrandColor(const std::string &name, uint32_t brandColor, uint32_t &color) const
{
    if (!IsBrandColorName(name)) {
        return false;
    }
    // Replace lower 24 bits (RGB) with brand color RGB, keep original alpha
    uint32_t alpha = color & 0xFF000000;
    uint32_t brandRgb = brandColor & 0x00FFFFFF;
    color = alpha | brandRgb;
    return true;
}

bool BrandColorManager::IsBrandColorName(const std::string &name) const
{
    if (!loaded_.load(std::memory_order_acquire)) {
        return false;
    }
    return overrideList_.find(name) != overrideList_.end();
}

} // namespace Resource
} // namespace Global
} // namespace OHOS
