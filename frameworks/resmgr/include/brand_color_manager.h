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

#ifndef OHOS_RESOURCE_MANAGER_BRAND_COLOR_MANAGER_H
#define OHOS_RESOURCE_MANAGER_BRAND_COLOR_MANAGER_H

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>

namespace OHOS {
namespace Global {
namespace Resource {

/**
 * BrandColorManager manages the global brand color override list.
 *
 * It loads a JSON config file from /system/etc/brand_color_override.json
 * that contains a list of color resource names which should be replaced
 * with the brand color. The brand color value itself is per-ResourceManager,
 * not stored here.
 *
 * The override list is a direct member guarded by an atomic<bool> loaded
 * flag: read paths (ApplyBrandColor) check the flag (lock-free atomic
 * load) and access the list directly (read-only after call_once).
 */
class BrandColorManager final {
public:
    using OverrideList = std::unordered_set<std::string>;

    /**
     * Get the singleton instance of BrandColorManager.
     * @return the reference to the singleton instance.
     */
    static BrandColorManager& GetInstance();

    ~BrandColorManager() = default;

    /**
     * Load the override list from the config file. Only loads once;
     * subsequent calls are no-ops. Thread-safe via std::call_once.
     */
    void LoadConfig();

    /**
     * Apply brand color override to a color value if the resource name
     * is in the override list. The lower 24 bits (RGB) are replaced with
     * the brand color's RGB, keeping the original alpha channel.
     * Lock-free on the read path (atomic<bool> load + unordered_set find).
     * @param name the color resource name.
     * @param brandColor the brand color value (0xaarrggbb).
     * @param color the original color value (0xaarrggbb), replaced on output if applicable.
     * @return true if the color was replaced with brand color.
     */
    bool ApplyBrandColor(const std::string &name, uint32_t brandColor, uint32_t &color) const;

    /**
     * Check if a resource name is in the brand color override list.
     * Lock-free on the read path.
     */
    bool IsBrandColorName(const std::string &name) const;

private:
    BrandColorManager();
    BrandColorManager(const BrandColorManager&) = delete;
    BrandColorManager& operator=(const BrandColorManager&) = delete;
    BrandColorManager(BrandColorManager&&) = delete;
    BrandColorManager& operator=(BrandColorManager&&) = delete;

    OverrideList overrideList_;

    std::once_flag loadOnceFlag_;

    std::atomic<bool> loaded_ = false;
};

} // namespace Resource
} // namespace Global
} // namespace OHOS

#endif // OHOS_RESOURCE_MANAGER_BRAND_COLOR_MANAGER_H
