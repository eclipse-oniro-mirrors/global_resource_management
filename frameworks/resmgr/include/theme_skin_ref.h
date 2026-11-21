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

#ifndef OHOS_RESOURCE_MANAGER_THEMESKINREF_H
#define OHOS_RESOURCE_MANAGER_THEMESKINREF_H

#include <cstdint>
#include <memory>
#include <string>

namespace OHOS {
namespace Global {
namespace Resource {
struct ThemeBundleResource;
class ThemePackManager;

/**
 * RAII handle of the theme skin resources of one bundle.
 *
 * The handle keeps the bundle's ThemeBundleResource alive while ThemePackManager only
 * holds a weak reference, so the resources are released as soon as the last handle of
 * the bundle is destroyed. The destructor also removes the bundle's weak index entry
 * when this handle was the last holder, so transient ResourceManagers do not leave a
 * stale entry until the next theme switch.
 */
class ThemeSkinRef {
public:
    ThemeSkinRef() = default;
    ~ThemeSkinRef();

    ThemeSkinRef(const ThemeSkinRef &) = delete;
    ThemeSkinRef &operator=(const ThemeSkinRef &) = delete;

    /**
     * Bind the handle to a bundle and user.
     * The ThemePackManager is acquired here and kept as a weak reference for cleanup.
     * The previously held resources, if any, are released first.
     *
     * @param userId the user id
     * @param bundleName the hap bundleName
     */
    void Bind(int32_t userId, const std::string &bundleName);

    /**
     * Load the theme skin resources of the bound bundle.
     * Reuses the resources already held for the bundle, or loads them on first use.
     */
    void Load();

    /**
     * Assign an already loaded theme skin resource.
     * Used by the resource manager factory to avoid loading the same bundle twice.
     * Bind must be called before Assign so the weak index entry can be cleaned up.
     *
     * @param resource the container returned by ThemePackManager::LoadThemeRes
     */
    void Assign(std::shared_ptr<ThemeBundleResource> resource);

private:
    void Cleanup();

    std::weak_ptr<ThemePackManager> manager_;
    int32_t userId_ = 0;
    std::string bundleName_;
    std::shared_ptr<ThemeBundleResource> resource_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif // OHOS_RESOURCE_MANAGER_THEMESKINREF_H