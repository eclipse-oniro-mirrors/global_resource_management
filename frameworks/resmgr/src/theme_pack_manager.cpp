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
#include <memory>
#include "hilog_wrapper.h"
#include "theme_pack_resource.h"
#include <securec.h>
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
constexpr int FIRST_ELEMENT = 0;
constexpr int SECOND_ELEMENT = 1;
constexpr int THIRED_ELEMENT = 2;
static std::shared_ptr<ThemePackManager> themeMgr = nullptr;
static std::once_flag themeMgrFlag;
constexpr uint32_t SYSTEM_ID_BEGIN = 117440512; // 0x07000000
constexpr uint32_t SYSTEM_ID_END = 134217727; // 0x07FFFFFF
const std::string THEME_FLAG_A = "data/themes/a/app/flag";
const std::string THEME_FLAG_B = "data/themes/b/app/flag";
const std::string THEME_SKIN_A = "/data/themes/a/app/skin";
const std::string THEME_SKIN_B = "/data/themes/b/app/skin";
const std::string THEME_SKIN_A_PATH = "/data/themes/a/skin";
const std::string THEME_SKIN_B_PATH = "/data/themes/b/skin";
const std::string THEME_ICONS_A = "/data/themes/a/app/icons";
const std::string THEME_ICONS_B = "/data/themes/b/app/icons";
const std::string ABSOLUTE_THEME_FLAG_A = "data/service/el1/public/themes/<currentUserId>/a/app/flag";
const std::string ABSOLUTE_THEME_FLAG_B = "data/service/el1/public/themes/<currentUserId>/b/app/flag";
const std::string ABSOLUTE_THEME_SKIN_A = "/data/service/el1/public/themes/<currentUserId>/a/app/skin";
const std::string ABSOLUTE_THEME_SKIN_B = "/data/service/el1/public/themes/<currentUserId>/b/app/skin";
const std::string ABSOLUTE_THEME_SKIN_A_PATH = "/data/service/el1/public/themes/<currentUserId>/a/skin";
const std::string ABSOLUTE_THEME_SKIN_B_PATH = "/data/service/el1/public/themes/<currentUserId>/b/skin";
const std::string ABSOLUTE_THEME_ICONS_A = "/data/service/el1/public/themes/<currentUserId>/a/app/icons";
const std::string ABSOLUTE_THEME_ICONS_B = "/data/service/el1/public/themes/<currentUserId>/b/app/icons";
const std::string ABSOLUTE_THEME_PATH = "/data/service/el1/public/themes/";
const std::string THEME_BASE_PATH_A = "/data/themes/a";
const std::string THEME_BASE_PATH_B = "/data/themes/b";
const std::string ABSOLUTE_THEME_BASE_PATH_A = "/data/service/el1/public/themes/<currentUserId>/a";
const std::string ABSOLUTE_THEME_BASE_PATH_B = "/data/service/el1/public/themes/<currentUserId>/b";

ThemePackManager::ThemePackManager() : isLogFlag_(Utils::IsFileExist(ABSOLUTE_THEME_PATH))
{}

ThemePackManager::~ThemePackManager()
{
    RESMGR_HILOGW_BY_FLAG(isLogFlag_, RESMGR_TAG, "~ThemePackManager");
    skinResource_.clear();
    iconResource_.clear();
    iconMaskValues_.clear();
}

std::shared_ptr<ThemePackManager> ThemePackManager::GetThemePackManager()
{
    std::call_once(themeMgrFlag, [&] {
        themeMgr = std::shared_ptr<ThemePackManager>(new ThemePackManager());
    });
    return themeMgr;
}

std::vector<std::string> ThemePackManager::GetRootDir(const std::string &strCurrentDir, const std::string &basePath)
{
    std::vector<std::string> vDir;
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    char resolvedPath[PATH_MAX] = {0};
    Utils::CanonicalizePath(strCurrentDir.c_str(), resolvedPath, PATH_MAX);
    if (resolvedPath[0] == '\0') {
        return vDir;
    }
    std::unique_ptr<DIR, decltype(&closedir)> dir(opendir(resolvedPath), closedir);
    if (dir == nullptr) {
        return vDir;
    }
    struct dirent *pDir = nullptr;
    std::string maskPath;
    std::string strokePath;
    while ((pDir = readdir(dir.get())) != nullptr) {
        if (strcmp(pDir->d_name, ".") == 0 || strcmp(pDir->d_name, "..") == 0) {
            continue;
        } else if (pDir->d_type == 4) { // 4 means dir
            std::string strNextDir = std::string(resolvedPath) + "/" + pDir->d_name;
            vDir.emplace_back(strNextDir);
        } else if (pDir->d_type == 8) { // 8 means file
            std::string filePath = std::string(resolvedPath) + "/" + pDir->d_name;
            if (filePath.find("icon_mask") != std::string::npos) {
                maskPath = ThemeResource::GetRelativePath(filePath, basePath);
            }
            if (filePath.find("icon_highlightstroke") != std::string::npos) {
                strokePath = ThemeResource::GetRelativePath(filePath, basePath);
            }
        }
    }
    if (!maskPath.empty() || !strokePath.empty()) {
        std::lock_guard<std::mutex> lock(this->lockHighlightIcon_);
        if (!maskPath.empty()) {
            themeMask_ = maskPath;
        }
        if (!strokePath.empty()) {
            themeStroke_ = strokePath;
        }
    }
#endif
    return vDir;
}

std::vector<std::string> ThemePackManager::GetThemeSkinRootDir(const std::string &newPath, const std::string &oldPath,
    const std::string &basePath)
{
    std::vector<std::string> result = GetRootDir(newPath, basePath);
    if (result.empty()) {
        result = GetRootDir(oldPath, basePath);
    }
    return result;
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
    const std::vector<std::string> &rootDirs, int32_t userId, const std::string &basePath)
{
    std::lock_guard<std::mutex> lock(this->lockSkin_);
    ChangeSkinResourceStatus(userId);
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
        auto pThemeResource = ThemeResource::LoadThemeResource(dir, basePath);
        if (pThemeResource != nullptr) {
            this->skinResource_.emplace_back(pThemeResource);
        }
    }
    ClearSkinResource();
}

void ThemePackManager::LoadThemeRes(const std::string &bundleName, const std::string &moduleName, int32_t userId)
{
    UpdateUserId(userId);
    UpdateBasePath(userId);
    ClearHighlightIcon();
    std::vector<std::string> rootDirs;
    std::vector<std::string> iconDirs;
    std::string basePath = GetBasePath();
    if (Utils::IsFileExist(THEME_FLAG_A)) {
        rootDirs = GetThemeSkinRootDir(THEME_SKIN_A_PATH, THEME_SKIN_A, basePath);
        iconDirs = GetRootDir(THEME_ICONS_A, basePath);
    } else if (Utils::IsFileExist(THEME_FLAG_B)) {
        rootDirs = GetThemeSkinRootDir(THEME_SKIN_B_PATH, THEME_SKIN_B, basePath);
        iconDirs = GetRootDir(THEME_ICONS_B, basePath);
    } else {
        LoadSAThemeRes(bundleName, moduleName, userId, rootDirs, iconDirs);
    }
    LoadThemeSkinResource(bundleName, moduleName, rootDirs, userId, basePath);
    LoadThemeIconsResource(bundleName, moduleName, iconDirs, userId, basePath);
    return;
}

void ThemePackManager::LoadThemeIconRes(const std::string &bundleName, const std::string &moduleName, int32_t userId)
{
    UpdateUserId(userId);
    ClearHighlightIcon();
    std::string basePath = GetBasePath();
    std::vector<std::string> iconDirs;
    if (Utils::IsFileExist(THEME_FLAG_A)) {
        iconDirs = GetRootDir(THEME_ICONS_A, basePath);
    } else if (Utils::IsFileExist(THEME_FLAG_B)) {
        iconDirs = GetRootDir(THEME_ICONS_B, basePath);
    } else {
        if (Utils::IsFileExist(ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_A, userId))) {
            iconDirs = GetRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_ICONS_A, userId), basePath);
        } else if (Utils::IsFileExist(ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_B, userId))) {
            iconDirs = GetRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_ICONS_B, userId), basePath);
        } else {
            RESMGR_HILOGE(RESMGR_TAG, "LoadThemesRes failed, userId = %{public}d, bundleName = %{public}s",
                userId, bundleName.c_str());
        }
    }
    LoadThemeIconsResource(bundleName, moduleName, iconDirs, userId, basePath);
}

void ThemePackManager::LoadThemeSkinRes(const std::string &bundleName, const std::string &moduleName, int32_t userId)
{
    UpdateUserId(userId);
    std::string basePath = GetBasePath();
    std::vector<std::string> rootDirs;
    if (Utils::IsFileExist(THEME_FLAG_A)) {
        rootDirs = GetThemeSkinRootDir(THEME_SKIN_A_PATH, THEME_SKIN_A, basePath);
    } else if (Utils::IsFileExist(THEME_FLAG_B)) {
        rootDirs = GetThemeSkinRootDir(THEME_SKIN_B_PATH, THEME_SKIN_B, basePath);
    } else {
        if (Utils::IsFileExist(ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_A, userId))) {
            rootDirs = GetThemeSkinRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_A_PATH, userId),
                ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_A, userId), basePath);
        } else if (Utils::IsFileExist(ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_B, userId))) {
            rootDirs = GetThemeSkinRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_B_PATH, userId),
                ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_B, userId), basePath);
        } else {
            RESMGR_HILOGE(RESMGR_TAG, "LoadThemesRes failed, userId = %{public}d, bundleName = %{public}s",
                userId, bundleName.c_str());
        }
    }
    LoadThemeSkinResource(bundleName, moduleName, rootDirs, userId, basePath);
    return;
}

void ThemePackManager::LoadSAThemeRes(const std::string &bundleName, const std::string &moduleName,
    int32_t userId, std::vector<std::string> &rootDirs, std::vector<std::string> &iconDirs)
{
    std::string basePath = GetBasePath();
    if (Utils::IsFileExist(ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_A, userId))) {
        rootDirs = GetThemeSkinRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_A_PATH, userId),
            ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_A, userId), basePath);
        iconDirs = GetRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_ICONS_A, userId), basePath);
    } else if (Utils::IsFileExist(ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_B, userId))) {
        rootDirs = GetThemeSkinRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_B_PATH, userId),
            ReplaceUserIdInPath(ABSOLUTE_THEME_SKIN_B, userId), basePath);
        iconDirs = GetRootDir(ReplaceUserIdInPath(ABSOLUTE_THEME_ICONS_B, userId), basePath);
    } else {
        RESMGR_HILOGE(RESMGR_TAG, "LoadThemesRes failed, userId = %{public}d, bundleName = %{public}s",
            userId, bundleName.c_str());
    }
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
    const std::vector<std::shared_ptr<IdItem>> &idItems, const ResConfigImpl &resConfig, int32_t userId,
    bool isThemeSystemResEnable)
{
    std::string result;
    for (size_t i = 0; i < idItems.size(); i++) {
        if (idItems[i] == nullptr) {
            continue;
        }
        std::string resName = idItems[i]->name_;
        uint32_t id = idItems[i]->id_;
        ResType resType = idItems[i]->resType_;
        if (id >= SYSTEM_ID_BEGIN && id <= SYSTEM_ID_END) {
            if (resType == ResType::COLOR && !isThemeSystemResEnable) {
                break;
            }
            std::pair<std::string, std::string> tempInfo("systemRes", "entry");
            result = GetThemeResource(tempInfo, resType, resName, resConfig, userId);
        } else {
            result = GetThemeResource(bundleInfo, resType, resName, resConfig, userId);
        }
        if (!result.empty()) {
            break;
        }
    }
    return result;
}

const std::string ThemePackManager::GetThemeResource(const std::pair<std::string, std::string> &bundInfo,
    const ResType &resType, const std::string &resName, const ResConfigImpl &resConfig, int32_t userId)
{
    auto themeQualifierValue = GetThemeQualifierValue(bundInfo, resType, resName, resConfig, userId);
    if (themeQualifierValue == nullptr) {
        RESMGR_HILOGD(RESMGR_TAG, "themeQualifierValue == nullptr");
        return std::string("");
    }
    std::string resValue = themeQualifierValue->GetResValue();
    if (resType == MEDIA) {
        return BuildFullPath(resValue);
    }
    return resValue;
}

std::vector<std::shared_ptr<ThemeResource::ThemeValue> > ThemePackManager::GetThemeResourceList(
    const std::pair<std::string, std::string> &bundInfo, const ResType &resType, const std::string &resName,
    int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockSkin_);
    std::vector<std::shared_ptr<ThemeResource::ThemeValue> > result;
    for (size_t i = 0; i < skinResource_.size(); ++i) {
        auto pThemeResource = skinResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        if (!IsSameResourceByUserId(pThemeResource->GetThemePath(), userId)) {
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
    const std::string &resName, const ResConfigImpl &resConfig, int32_t userId)
{
    auto candidates = this->GetThemeResourceList(bundInfo, resType, resName, userId);
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
    std::lock_guard<std::mutex> lock(lockIconValue_);
    iconMaskValues_.clear();
}

void ThemePackManager::ClearHighlightIcon()
{
    std::lock_guard<std::mutex> lock(this->lockHighlightIcon_);
    iconHighlightValue_ = { "", nullptr, 0 };
    themeStroke_ = "";
}

void ThemePackManager::LoadThemeIconsResource(const std::string &bundleName, const std::string &moduleName,
    const std::vector<std::string> &rootDirs, int32_t userId, const std::string &basePath)
{
    std::lock_guard<std::mutex> lock(this->lockIcon_);
    ChangeIconResourceStatus(userId);
    if (rootDirs.empty()) {
        RESMGR_HILOGW(RESMGR_TAG, "theme resources will clean, id %{public}d", userId);
        ClearIconResource();
        return;
    }
    for (const auto &dir : rootDirs) {
        auto pos = dir.rfind('/');
        if (pos == std::string::npos) {
            RESMGR_HILOGE(RESMGR_TAG, "invalid dir = %{public}s in LoadThemeIconsResource", dir.c_str());
            continue;
        }
        RESMGR_HILOGW_BY_FLAG(isLogFlag_, RESMGR_TAG, "load img, %{public}s", GetMaskString(dir).c_str());
        auto pThemeResource = ThemeResource::LoadThemeIconResource(dir, basePath, isLogFlag_);
        if (pThemeResource != nullptr) {
            this->iconResource_.emplace_back(pThemeResource);
        }
    }
    ClearIconResource();
    RESMGR_HILOGW_BY_FLAG(isLogFlag_, RESMGR_TAG, "load img end, size is %{public}zu", iconResource_.size());
}

const std::string ThemePackManager::FindThemeIconResource(const std::pair<std::string, std::string> &bundleInfo,
    const std::string &iconName, int32_t userId, const std::string &abilityName)
{
    std::lock_guard<std::mutex> lock(this->lockIcon_);
    std::string result;
    for (size_t i = 0; i < iconResource_.size(); i++) {
        auto pThemeResource = iconResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        if (!IsSameResourceByUserId(pThemeResource->GetThemePath(), userId)) {
            continue;
        }
        result = pThemeResource->GetThemeAppIcon(bundleInfo, iconName, abilityName);
        if (!result.empty()) {
            result = BuildFullPath(result);
            RESMGR_HILOGW_BY_FLAG(isLogFlag_, RESMGR_TAG, "find img, %{public}s", GetMaskString(result).c_str());
            break;
        }
    }
    return result;
}

bool ThemePackManager::UpdateThemeId(uint32_t newThemeId)
{
    std::lock_guard<std::mutex> lock(this->lockThemeId_);
    if (newThemeId != 0 && newThemeId != themeId_) {
        RESMGR_HILOGW(RESMGR_TAG, "update theme, themeId_= %{public}d, newThemeId= %{public}d", themeId_, newThemeId);
        themeId_ = newThemeId;
        UpdateBasePath(GetCurrentUserId());
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

bool ThemePackManager::HasIconInTheme(const std::string &bundleName, int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockIcon_);
    bool result = false;
    for (size_t i = 0; i < iconResource_.size(); i++) {
        auto pThemeResource = iconResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        if (!IsSameResourceByUserId(pThemeResource->GetThemePath(), userId)) {
            continue;
        }
        result = pThemeResource->HasIconInTheme(bundleName);
        if (result) {
            break;
        }
    }
    return result;
}

RState ThemePackManager::GetOtherIconsInfo(const std::string &iconName,
    std::unique_ptr<uint8_t[]> &outValue, size_t &len, bool isGlobalMask, int32_t userId)
{
    std::string iconPath;
    std::string iconTag;
    if (iconName.find("icon_mask") != std::string::npos && isGlobalMask) {
        std::lock_guard<std::mutex> lock(this->lockHighlightIcon_);
        iconPath = BuildFullPath(themeMask_);
        iconTag = "global_" + iconName;
    } else {
        std::pair<std::string, std::string> bundleInfo;
        bundleInfo.first = "other_icons";
        iconPath = FindThemeIconResource(bundleInfo, iconName, userId);
        iconTag = "other_icons_" + iconName;
    }

    if (iconPath.empty()) {
        RESMGR_HILOGD(RESMGR_TAG, "no found, iconTag = %{public}s", iconTag.c_str());
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }

    outValue = Utils::LoadResourceFile(iconPath, len);
    if (outValue != nullptr && len != 0) {
        auto tmpInfo = std::make_unique<uint8_t[]>(len);
        errno_t ret = memcpy_s(tmpInfo.get(), len, outValue.get(), len);
        if (ret != 0) {
            RESMGR_HILOGE(RESMGR_TAG, "save fail, iconName = %{public}s, ret = %{public}d", iconName.c_str(), ret);
            return SUCCESS;
        }
        std::lock_guard<std::mutex> lock(this->lockIconValue_);
        iconMaskValues_.emplace_back(std::make_tuple(iconTag, std::move(tmpInfo), len));
        return SUCCESS;
    }
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ThemePackManager::GetHighlightIconInfo(const std::string &iconName, std::unique_ptr<uint8_t[]> &outValue,
    size_t &len, bool isGlobalMask)
{
    std::lock_guard<std::mutex> lock(this->lockHighlightIcon_);
    std::string iconTag;
    std::string iconPath;
    if (iconName.find("icon_highlightstroke") != std::string::npos && isGlobalMask) {
        iconTag = "global_" + iconName;
    }
    std::string tag = std::get<FIRST_ELEMENT>(iconHighlightValue_);
    if (iconTag == tag) {
        size_t length = std::get<THIRED_ELEMENT>(iconHighlightValue_);
        auto iconInfo = std::make_unique<uint8_t[]>(length);
        auto tmpInfo = std::get<SECOND_ELEMENT>(iconHighlightValue_).get();
        errno_t ret = memcpy_s(iconInfo.get(), length, tmpInfo, length);
        if (ret != 0) {
            RESMGR_HILOGE(RESMGR_TAG, "get %{public}s info fail, ret = %{public}d", iconTag.c_str(), ret);
            return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
        }
        len = length;
        outValue = std::move(iconInfo);
        return SUCCESS;
    }
    iconPath = BuildFullPath(themeStroke_);
    if (iconPath.empty()) {
        RESMGR_HILOGD(RESMGR_TAG, "no found, iconTag = %{public}s", iconTag.c_str());
        return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
    }
    outValue = Utils::LoadResourceFile(iconPath, len);
    if (outValue != nullptr && len != 0) {
        auto tmpInfo = std::make_unique<uint8_t[]>(len);
        errno_t ret = memcpy_s(tmpInfo.get(), len, outValue.get(), len);
        if (ret != 0) {
            RESMGR_HILOGE(RESMGR_TAG, "save fail, iconName = %{public}s, ret = %{public}d", iconName.c_str(), ret);
            return SUCCESS;
        }
        iconHighlightValue_ = std::make_tuple(iconTag, std::move(tmpInfo), len);
        return SUCCESS;
    }
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ThemePackManager::GetThemeIconFromCache(
    const std::string &iconTag, std::unique_ptr<uint8_t[]> &outValue, size_t &len)
{
    std::lock_guard<std::mutex> lock(this->lockIconValue_);
    if (iconMaskValues_.empty()) {
        return NOT_FOUND;
    }

    for (const auto &iconValue : iconMaskValues_) {
        std::string tag = std::get<FIRST_ELEMENT>(iconValue);
        if (iconTag != tag) {
            continue;
        }
        size_t length = std::get<THIRED_ELEMENT>(iconValue);
        auto iconInfo = std::make_unique<uint8_t[]>(length);
        auto tmpInfo = std::get<SECOND_ELEMENT>(iconValue).get();
        errno_t ret = memcpy_s(iconInfo.get(), length, tmpInfo, length);
        if (ret != 0) {
            RESMGR_HILOGE(RESMGR_TAG, "get icon info fail, ret = %{public}d", ret);
            continue;
        }
        len = length;
        outValue = std::move(iconInfo);
        return SUCCESS;
    }
    return NOT_FOUND;
}

bool ThemePackManager::IsUpdateByUserId(int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockUserId_);
    return userId != 0 && currentUserId_ != userId;
}

void ThemePackManager::UpdateUserId(int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockUserId_);
    if (userId != 0 && currentUserId_ != userId) {
        RESMGR_HILOGW(RESMGR_TAG, "update userId, %{public}d -> %{public}d", currentUserId_, userId);
        currentUserId_ = userId;
        UpdateBasePath(userId);
    }
}

int32_t ThemePackManager::GetCurrentUserId()
{
    std::lock_guard<std::mutex> lock(this->lockUserId_);
    return currentUserId_;
}

void ThemePackManager::UpdateBasePath(int32_t userId)
{
    std::string basePath;
    if (Utils::IsFileExist(THEME_FLAG_A)) {
        basePath = GetCanonicalBasePath(THEME_BASE_PATH_A);
    } else if (Utils::IsFileExist(THEME_FLAG_B)) {
        basePath = GetCanonicalBasePath(THEME_BASE_PATH_B);
    } else {
        std::string flagA = ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_A, userId);
        if (Utils::IsFileExist(flagA)) {
            basePath = GetCanonicalBasePath(ReplaceUserIdInPath(ABSOLUTE_THEME_BASE_PATH_A, userId));
        } else {
            std::string flagB = ReplaceUserIdInPath(ABSOLUTE_THEME_FLAG_B, userId);
            if (Utils::IsFileExist(flagB)) {
                basePath = GetCanonicalBasePath(ReplaceUserIdInPath(ABSOLUTE_THEME_BASE_PATH_B, userId));
            }
        }
    }
    std::lock_guard<std::mutex> lock(this->lockBasePath_);
    basePath_ = basePath;
}

bool ThemePackManager::IsSameResourceByUserId(const std::string &path, int32_t userId)
{
    if (path.empty() || path.find(ABSOLUTE_THEME_PATH) == std::string::npos) {
        return true;
    }
    auto pos = path.find("/", ABSOLUTE_THEME_PATH.length());
    if (pos == std::string::npos) {
        return true;
    }
    auto subStr = path.substr(ABSOLUTE_THEME_PATH.length(), pos - ABSOLUTE_THEME_PATH.length());
    int tmpId = -1;
    if (!Utils::convertToInteger(subStr, tmpId)) {
        return true;
    }
    return tmpId == userId;
}

void ThemePackManager::ChangeSkinResourceStatus(int32_t userId)
{
    for (size_t i = 0; i < skinResource_.size(); ++i) {
        auto pThemeResource = skinResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        if (IsSameResourceByUserId(pThemeResource->GetThemePath(), userId)) {
            pThemeResource->SetNewResource(false);
        }
    }
}

void ThemePackManager::ChangeIconResourceStatus(int32_t userId)
{
    for (size_t i = 0; i < iconResource_.size(); ++i) {
        auto pThemeResource = iconResource_[i];
        if (pThemeResource == nullptr) {
            continue;
        }
        if (IsSameResourceByUserId(pThemeResource->GetThemePath(), userId)) {
            pThemeResource->SetNewResource(false);
        }
    }
}

const std::string ThemePackManager::GetMaskString(const std::string &path)
{
    if (path.empty() || path.find(ABSOLUTE_THEME_PATH) == std::string::npos) {
        return path;
    }
    return path.substr(ABSOLUTE_THEME_PATH.length(), path.length() - ABSOLUTE_THEME_PATH.length());
}

std::string ThemePackManager::GetCanonicalBasePath(const std::string &basePath)
{
    std::string canonicalBasePath;
    if (!basePath.empty()) {
        char resolvedBasePath[PATH_MAX] = {0};
        Utils::CanonicalizePath(basePath.c_str(), resolvedBasePath, PATH_MAX);
        if (resolvedBasePath[0] != '\0') {
            canonicalBasePath = std::string(resolvedBasePath);
        }
    }
    return canonicalBasePath;
}

const std::string ThemePackManager::GetLogPath(const std::string &path)
{
    if (path.empty()) {
        return path;
    }
    std::string basePath = GetBasePath();
    if (basePath.empty() || path.find(basePath) != 0) {
        return path;
    }
    size_t lastSlash = basePath.rfind('/');
    if (lastSlash == std::string::npos) {
        return path;
    }
    return path.substr(lastSlash + 1);
}

std::string ThemePackManager::BuildFullPath(const std::string &relativePath) const
{
    if (!relativePath.empty() && relativePath.front() != '/') {
        std::string basePath = GetBasePath();
        if (!basePath.empty()) {
            return basePath + "/" + relativePath;
        }
    }
    return relativePath;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
