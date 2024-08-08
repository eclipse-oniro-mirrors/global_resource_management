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

#include "auto_mutex.h"
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
const std::string themeSkinA = "/data/themes/a/app/skin";
const std::string themeSkinB = "/data/themes/b/app/skin";
const std::string themeIconsA = "/data/themes/a/app/icons";
const std::string themeIconsB = "/data/themes/b/app/icons";
const std::string absoluteThemeFlagA = "data/service/el1/public/themes/<currentUserId>/a/app/flag";
const std::string absoluteThemeFlagB = "data/service/el1/public/themes/<currentUserId>/b/app/flag";
const std::string absoluteThemeSkinA = "/data/service/el1/public/themes/<currentUserId>/a/app/skin";
const std::string absoluteThemeSkinB = "/data/service/el1/public/themes/<currentUserId>/b/app/skin";
const std::string absoluteThemeIconsA = "/data/service/el1/public/themes/<currentUserId>/a/app/icons";
const std::string absoluteThemeIconsB = "/data/service/el1/public/themes/<currentUserId>/b/app/icons";
ThemePackManager::ThemePackManager()
{}

ThemePackManager::~ThemePackManager()
{
    skinResource_.clear();
    iconResource_.clear();
}

std::shared_ptr<ThemePackManager> ThemePackManager::GetThemePackManager()
{
    std::call_once(themeMgrFlag, [&] {
        themeMgr = std::shared_ptr<ThemePackManager>(new ThemePackManager());
    });
    return themeMgr;
}

std::vector<std::string> ThemePackManager::GetRootDir(const std::string &strCurrentDir)
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

void ThemePackManager::ClearSkinResource()
{
    for (auto it = skinResource_.begin(); it != skinResource_.end();) {
        if ((*it) == nullptr) {
            continue;
        }
        // 1 means get the enable theme
        if (!(*it)->IsNewResource()) {
            it = skinResource_.erase(it);
        } else {
            ++it;
        }
    }
}

void ThemePackManager::LoadThemeSkinResource(const std::string &bundleName, const std::string &moduleName,
    const std::vector<std::string> &rootDirs, int32_t userId)
{
    AutoMutex mutex(this->lockSkin_);
    for (size_t i = 0; i < skinResource_.size(); ++i) {
        auto pThemeResource = skinResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        pThemeResource->SetNewResource(false);
    }
    if (rootDirs.empty()) {
        ClearSkinResource();
        return;
    }
    for (const auto &dir : rootDirs) {
        auto pos = dir.rfind('/');
        if (pos == std::string::npos) {
            RESMGR_HILOGE(RESMGR_TAG, "invalid dir = %{public}s in LoadThemeSkinResource", dir.c_str());
            continue;
        }
        std::string tempBundleName = dir.substr(pos + 1);
        if (tempBundleName != bundleName && tempBundleName != "systemRes") {
            continue;
        }
        auto pThemeResource = ThemeResource::LoadThemeResource(dir);
        if (pThemeResource != nullptr) {
            this->skinResource_.emplace_back(pThemeResource);
        }
    }
    ClearSkinResource();
}

void ThemePackManager::LoadThemeRes(const std::string &bundleName, const std::string &moduleName, int32_t userId)
{
    std::vector<std::string> rootDirs;
    std::vector<std::string> iconDirs;
    if (Utils::IsFileExist(themeFlagA)) {
        rootDirs = GetRootDir(themeSkinA);
        iconDirs = GetRootDir(themeIconsA);
    } else if (Utils::IsFileExist(themeFlagB)) {
        rootDirs = GetRootDir(themeSkinB);
        iconDirs = GetRootDir(themeIconsB);
    } else {
        LoadSAThemeRes(bundleName, moduleName, userId, rootDirs, iconDirs);
    }
    LoadThemeSkinResource(bundleName, moduleName, rootDirs, userId);
    LoadThemeIconsResource(bundleName, moduleName, iconDirs, userId);
    return;
}

void ThemePackManager::LoadSAThemeRes(const std::string &bundleName, const std::string &moduleName,
    int32_t userId, std::vector<std::string> &rootDirs, std::vector<std::string> &iconDirs)
{
    if (Utils::IsFileExist(ReplaceUserIdInPath(absoluteThemeFlagA, userId))) {
        rootDirs = GetRootDir(ReplaceUserIdInPath(absoluteThemeSkinA, userId));
        iconDirs = GetRootDir(ReplaceUserIdInPath(absoluteThemeIconsA, userId));
    } else if (Utils::IsFileExist(ReplaceUserIdInPath(absoluteThemeFlagB, userId))) {
        rootDirs = GetRootDir(ReplaceUserIdInPath(absoluteThemeSkinB, userId));
        iconDirs = GetRootDir(ReplaceUserIdInPath(absoluteThemeIconsB, userId));
    }
    return;
}

const std::string ThemePackManager::ReplaceUserIdInPath(const std::string &originalPath, int32_t userId)
{
    std::string result = originalPath;
    auto found = result.find("<currentUserId>");
    if (found != std::string::npos) {
        result.replace(found, 15, std::to_string(userId)); // 15 is the length of "<currentUserId>"
    }
    return result;
}

const std::string ThemePackManager::FindThemeResource(const std::pair<std::string, std::string> &bundleInfo,
    std::vector<std::shared_ptr<IdItem>> idItems, const ResConfigImpl &resConfig, bool isThemeSystemResEnable)
{
    std::string result;
    for (size_t i = 0; i < idItems.size(); i++) {
        std::string resName = idItems[i]->GetItemResName();
        uint32_t id = idItems[i]->GetItemResId();
        ResType resType = idItems[i]->GetItemResType();
        if (Utils::IsPrefix(sysResIdPreFix, std::to_string(id))) {
            if (resType == ResType::COLOR && !isThemeSystemResEnable) {
                break;
            }
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
        RESMGR_HILOGD(RESMGR_TAG, "themeQualifierValue == nullptr");
        return std::string("");
    }
    return themeQualifierValue->GetResValue();
}

std::vector<std::shared_ptr<ThemeResource::ThemeValue> > ThemePackManager::GetThemeResourceList(
    const std::pair<std::string, std::string> &bundInfo, const ResType &resType, const std::string &resName)
{
    AutoMutex mutex(this->lockSkin_);
    std::vector<std::shared_ptr<ThemeResource::ThemeValue> > result;
    for (size_t i = 0; i < skinResource_.size(); ++i) {
        auto pThemeResource = skinResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        std::string bundleName = pThemeResource->GetThemeResBundleName(pThemeResource->themePath_);
        if (bundleName != bundInfo.first) {
            continue;
        }
        result = pThemeResource->GetThemeValues(bundInfo, resType, resName);
    }
    return result;
}

const std::shared_ptr<ThemeResource::ThemeQualifierValue> ThemePackManager::GetThemeQualifierValue(
    const std::pair<std::string, std::string> &bundInfo, const ResType &resType,
    const std::string &resName, const ResConfigImpl &resConfig)
{
    auto candidates = this->GetThemeResourceList(bundInfo, resType, resName);
    if (candidates.size() == 0) {
        return nullptr;
    }
    return GetBestMatchThemeResource(candidates, resConfig);
}

const std::shared_ptr<ThemeResource::ThemeQualifierValue> ThemePackManager::GetBestMatchThemeResource(
    const std::vector<std::shared_ptr<ThemeResource::ThemeValue> > &candidates, const ResConfigImpl &resConfig)
{
    std::shared_ptr<ThemeResource::ThemeQualifierValue> result = nullptr;
    std::shared_ptr<ThemeConfig> bestThemeConfig = nullptr;
    for (auto iter = candidates.rbegin(); iter != candidates.rend(); iter++) {
        const std::vector<std::shared_ptr<ThemeResource::ThemeQualifierValue> > ThemePaths =
            (*iter)->GetThemeLimitPathsConst();
        size_t len = ThemePaths.size();
        for (size_t i = 0; i < len; i++) {
            std::shared_ptr<ThemeResource::ThemeQualifierValue> path = ThemePaths[i];
            auto themeConfig = path->GetThemeConfig();
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

void ThemePackManager::ClearIconResource()
{
    for (auto it = iconResource_.begin(); it != iconResource_.end();) {
        if ((*it) == nullptr) {
            continue;
        }
        // 1 means get the enable theme
        if (!(*it)->IsNewResource()) {
            it = iconResource_.erase(it);
        } else {
            ++it;
        }
    }
}

void ThemePackManager::LoadThemeIconsResource(const std::string &bundleName, const std::string &moduleName,
    const std::vector<std::string> &rootDirs, int32_t userId)
{
    AutoMutex mutex(this->lockIcon_);
    for (size_t i = 0; i < iconResource_.size(); ++i) {
        auto pThemeResource = iconResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        pThemeResource->SetNewResource(false);
    }
    if (rootDirs.empty()) {
        ClearIconResource();
        return;
    }
    for (const auto &dir : rootDirs) {
        auto pos = dir.rfind('/');
        if (pos == std::string::npos) {
            RESMGR_HILOGE(RESMGR_TAG, "invalid dir = %{public}s in LoadThemeIconsResource", dir.c_str());
            continue;
        }
        auto pThemeResource = ThemeResource::LoadThemeIconResource(dir);
        if (pThemeResource != nullptr) {
            this->iconResource_.emplace_back(pThemeResource);
        }
    }
    ClearIconResource();
}

const std::string ThemePackManager::FindThemeIconResource(const std::pair<std::string, std::string> &bundleInfo,
    const std::string &iconName)
{
    AutoMutex mutex(this->lockIcon_);
    std::string result;
    for (size_t i = 0; i < iconResource_.size(); i++) {
        auto pThemeResource = iconResource_[i];
        if (pThemeResource == nullptr) {
            return std::string("");
        }
        result = pThemeResource->GetThemeAppIcon(bundleInfo, iconName);
        if (!result.empty()) {
            break;
        }
    }
    return result;
}

bool ThemePackManager::UpdateThemeId(uint32_t newThemeId)
{
    AutoMutex mutex(this->lockThemeId_);
    if (newThemeId != 0 && newThemeId != themeId_) {
        RESMGR_HILOGI(RESMGR_TAG, "update theme, themeId_= %{public}d, newThemeId= %{public}d", themeId_, newThemeId);
        themeId_ = newThemeId;
        return true;
    }
    return false;
}

bool ThemePackManager::IsFirstLoadResource()
{
    if (isFirstCreate) {
        isFirstCreate = false;
        return true;
    }
    return false;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
