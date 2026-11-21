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

#include "theme_skin_ref.h"
#include "theme_pack_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
ThemeSkinRef::~ThemeSkinRef()
{
    Cleanup();
}

void ThemeSkinRef::Bind(int32_t userId, const std::string &bundleName)
{
    Cleanup();
    manager_ = ThemePackManager::GetThemePackManager();
    userId_ = userId;
    bundleName_ = bundleName;
}

void ThemeSkinRef::Load()
{
    auto themePackManager = manager_.lock();
    if (themePackManager == nullptr) {
        return;
    }
    resource_ = themePackManager->LoadThemeBundleSkinResource(bundleName_, userId_);
}

void ThemeSkinRef::Assign(std::shared_ptr<ThemeBundleResource> resource)
{
    resource_ = std::move(resource);
}

void ThemeSkinRef::Cleanup()
{
    auto themePackManager = manager_.lock();
    resource_.reset();
    if (themePackManager != nullptr && !bundleName_.empty()) {
        themePackManager->RemoveThemeBundleSkinResource(bundleName_, userId_);
    }
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
