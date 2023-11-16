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
#include "theme_pack_manager.h"

#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "hilog_wrapper.h"
#include "theme_pack_resource.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
static std::shared_ptr<ThemePackManager> themeMgr = nullptr;
static std::once_flag themeMgrFlag;
const std::string sysResIdPreFix = "125";
const std::string themeFlagA = "data/themes/a/app/flag";
const std::string themeFlagB = "data/themes/b/app/flag";
ThemePackManager::ThemePackManager()
{}

ThemePackManager::~ThemePackManager()
{
    for (size_t i = 0; i < skinResource_.size(); i++) {
        if (skinResource_[i] != nullptr) {
            delete skinResource_[i];
            skinResource_[i] = nullptr;
        }
    }

    for (size_t i = 0; i < iconResource_.size(); i++) {
        if (iconResource_[i] != nullptr) {
            delete iconResource_[i];
            iconResource_[i] = nullptr;
        }
    }
}

std::shared_ptr<ThemePackManager> ThemePackManager::GetThemePackManager()
{
    std::call_once(themeMgrFlag, [&] {
        themeMgr = std::shared_ptr<ThemePackManager>(new ThemePackManager());
    });
    return themeMgr;
}

std::vector<std::string> GetRootDir(const std::string &strCurrentDir, std::string& themeMask)
{
    std::vector<std::string> vDir;
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    DIR *dir;
    struct dirent *pDir;
    if ((dir = opendir(strCurrentDir.c_str())) == nullptr) {
        return vDir;
    }
    while ((pDir = readdir(dir)) != nullptr) {
        if (strcmp(pDir->d_name, ".") == 0 || strcmp(pDir->d_name, "..") == 0) {
            continue;
        } else if (pDir->d_type == 4) { // 4 means dir
            std::string strNextDir = strCurrentDir + "/" + pDir->d_name;
            vDir.emplace_back(strNextDir);
        } else if (pDir->d_type == 8) { // 8 means file
            std::string filePath = strCurrentDir + "/" + pDir->d_name;
            if (filePath.find("icon_mask") != std::string::npos) {
                themeMask = filePath;
            }
        }
    }
    closedir(dir);
#endif
    return vDir;
}

void ThemePackManager::LoadThemeSkinResource(const std::string &bundleName, const std::string &moduleName,
    const std::vector<std::string> &rootDirs)
{
    if (rootDirs.empty()) {
        return;
    }
    for (auto dir : rootDirs) {
        auto pos = dir.rfind('/');
        if (pos == std::string::npos) {
            HILOG_ERROR("invalid dir = %{public}s in LoadThemeSkinResource", dir.c_str());
            continue;
        }
        std::string tempBundleName = dir.substr(pos + 1);
        if (tempBundleName != bundleName && tempBundleName != "systemRes") {
            continue;
        }
        const ThemeResource *pThemeResource = ThemeResource::LoadThemeResource(dir);
        if (pThemeResource != nullptr) {
            this->skinResource_.emplace_back(const_cast<ThemeResource *>(pThemeResource));
        }
    }
}

void ThemePackManager::LoadThemeRes(const std::string &bundleName, const std::string &moduleName,
    std::string& themeMask)
{
    const std::string themeSkinA = "/data/themes/a/app/skin";
    const std::string themeSkinB = "/data/themes/b/app/skin";
    const std::string themeIconsA = "/data/themes/a/app/icons";
    const std::string themeIconsB = "/data/themes/b/app/icons";
    std::vector<std::string> rootDirs;
    std::vector<std::string> iconDirs;
    if (Utils::IsFileExist(themeFlagA)) {
        rootDirs = GetRootDir(themeSkinA, themeMask);
        iconDirs = GetRootDir(themeIconsA, themeMask);
    } else if (Utils::IsFileExist(themeFlagB)) {
        rootDirs = GetRootDir(themeSkinB, themeMask);
        iconDirs = GetRootDir(themeIconsB, themeMask);
    } else {
        return;
    }
    LoadThemeSkinResource(bundleName, moduleName, rootDirs);
    LoadThemeIconsResource(bundleName, moduleName, iconDirs);
    return;
}

const std::string ThemePackManager::FindThemeResource(const std::pair<std::string, std::string> &bundleInfo,
    std::vector<const IdItem *> idItems, const ResConfigImpl &resConfig)
{
    std::string result;
    for (size_t i = 0; i < idItems.size(); i++) {
        std::string resName = idItems[i]->GetItemResName();
        uint32_t id = idItems[i]->GetItemResId();
        ResType resType = idItems[i]->GetItemResType();
        if (Utils::IsPrefix(sysResIdPreFix, std::to_string(id))) {
            std::pair<std::string, std::string> tempInfo("systemRes", "entry");
            result = GetThemeResource(tempInfo, resType, resName, resConfig);
        } else {
            result = GetThemeResource(bundleInfo, resType, resName, resConfig);
        }
        if (!result.empty()) {
            break;
        }
    }
    return result;
}

const std::string ThemePackManager::GetThemeResource(const std::pair<std::string, std::string> &bundInfo,
    const ResType &resType, const std::string &resName, const ResConfigImpl &resConfig)
{
    auto themeQualifierValue = GetThemeQualifierValue(bundInfo, resType, resName, resConfig);
    if (themeQualifierValue == nullptr) {
        HILOG_ERROR("themeQualifierValue == nullptr");
        return std::string("");
    }
    return themeQualifierValue->GetResValue();
}

std::vector<const ThemeResource::ThemeValue *> ThemePackManager::GetThemeResourceList(
    const std::pair<std::string, std::string> &bundInfo, const ResType &resType, const std::string &resName) const
{
    std::vector<const ThemeResource::ThemeValue *> result;
    for (size_t i = 0; i < skinResource_.size(); ++i) {
        ThemeResource *pThemeResource = skinResource_[i];
        if (pThemeResource == nullptr) {
            HILOG_ERROR("pThemeResource == nullptr");
            return result;
        }
        std::string bundleName = pThemeResource->GetThemeResBundleName(pThemeResource->themePath_);
        if (bundleName != bundInfo.first) {
            continue;
        }
        result = pThemeResource->GetThemeValues(bundInfo, resType, resName);
    }
    return result;
}

const ThemeResource::ThemeQualifierValue *ThemePackManager::GetThemeQualifierValue(
    const std::pair<std::string, std::string> &bundInfo, const ResType &resType,
    const std::string &resName, const ResConfigImpl &resConfig)
{
    auto candidates = this->GetThemeResourceList(bundInfo, resType, resName);
    if (candidates.size() == 0) {
        return nullptr;
    }
    return GetBestMatchThemeResource(candidates, resConfig);
}

const ThemeResource::ThemeQualifierValue* ThemePackManager::GetBestMatchThemeResource(
    const std::vector<const ThemeResource::ThemeValue *> candidates, const ResConfigImpl &resConfig)
{
    const ThemeResource::ThemeQualifierValue *result = nullptr;
    const ThemeConfig *bestThemeConfig = nullptr;
    for (auto iter = candidates.rbegin(); iter != candidates.rend(); iter++) {
        const std::vector<ThemeResource::ThemeQualifierValue *> ThemePaths = (*iter)->GetThemeLimitPathsConst();
        size_t len = ThemePaths.size();
        for (size_t i = 0; i < len; i++) {
            ThemeResource::ThemeQualifierValue *path = ThemePaths[i];
            const ThemeConfig *themeConfig = path->GetThemeConfig();
            if (!ThemeConfig::Match(themeConfig, resConfig)) {
                continue;
            }
            if (bestThemeConfig == nullptr) {
                bestThemeConfig = themeConfig;
                result = path;
                continue;
            }
            if (!bestThemeConfig->BestMatch(themeConfig, resConfig)) {
                bestThemeConfig = themeConfig;
                result = path;
            }
        }
    }
    return result;
}

void ThemePackManager::LoadThemeIconsResource(const std::string &bundleName, const std::string &moduleName,
    const std::vector<std::string> &rootDirs)
{
    if (rootDirs.empty()) {
        return;
    }
    for (auto dir : rootDirs) {
        auto pos = dir.rfind('/');
        if (pos == std::string::npos) {
            HILOG_ERROR("invalid dir = %{public}s in LoadThemeIconsResource", dir.c_str());
            continue;
        }
        std::string tempBundleName = dir.substr(pos + 1);
        if (tempBundleName != bundleName) {
            continue;
        }
        const ThemeResource *pThemeResource = ThemeResource::LoadThemeIconResource(dir);
        if (pThemeResource != nullptr) {
            this->iconResource_.emplace_back(const_cast<ThemeResource *>(pThemeResource));
        }
    }
}

const std::string ThemePackManager::FindThemeIconResource(const std::pair<std::string, std::string> &bundleInfo,
    const std::string &iconName)
{
    std::string result;
    for (size_t i = 0; i < iconResource_.size(); i++) {
        ThemeResource *pThemeResource = iconResource_[i];
        if (pThemeResource == nullptr) {
            return std::string("");
        }
        result = pThemeResource->GetThemeAppIcon(bundleInfo, iconName);
    }
    return result;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
