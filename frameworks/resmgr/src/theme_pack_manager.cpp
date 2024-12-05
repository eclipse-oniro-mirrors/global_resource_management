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
const std::string absoluteThemePath = "/data/service/el1/public/themes/";
ThemePackManager::ThemePackManager()
{}

ThemePackManager::~ThemePackManager()
{
    RESMGR_HILOGI_BY_FLAG(isLogFlag_, RESMGR_TAG, "~ThemePackManager");
    skinResource_.clear();
    iconResource_.clear();
    iconMaskValues_.clear();
    useCountMap_.clear();
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
        auto pThemeResource = ThemeResource::LoadThemeResource(dir);
        if (pThemeResource != nullptr) {
            this->skinResource_.emplace_back(pThemeResource);
        }
    }
    ClearSkinResource();
}

void ThemePackManager::LoadThemeRes(const std::string &bundleName, const std::string &moduleName, int32_t userId)
{
    UpdateUserId(userId);
    std::vector<std::string> rootDirs;
    std::vector<std::string> iconDirs;
    if (Utils::IsFileExist(themeFlagA)) {
        rootDirs = GetRootDir(themeSkinA);
        iconDirs = GetRootDir(themeIconsA);
    } else if (Utils::IsFileExist(themeFlagB)) {
        rootDirs = GetRootDir(themeSkinB);
        iconDirs = GetRootDir(themeIconsB);
    } else {
        isLogFlag_ = true;
        LoadSAThemeRes(bundleName, moduleName, userId, rootDirs, iconDirs);
    }
    LoadThemeSkinResource(bundleName, moduleName, rootDirs, userId);
    LoadThemeIconsResource(bundleName, moduleName, iconDirs, userId);
    return;
}

void ThemePackManager::LoadThemeIconRes(const std::string &bundleName, const std::string &moduleName, int32_t userId)
{
    UpdateUserId(userId);
    std::vector<std::string> iconDirs;
    if (Utils::IsFileExist(themeFlagA)) {
        iconDirs = GetRootDir(themeIconsA);
    } else if (Utils::IsFileExist(themeFlagB)) {
        iconDirs = GetRootDir(themeIconsB);
    } else {
        if (Utils::IsFileExist(ReplaceUserIdInPath(absoluteThemeFlagA, userId))) {
            iconDirs = GetRootDir(ReplaceUserIdInPath(absoluteThemeIconsA, userId));
        } else if (Utils::IsFileExist(ReplaceUserIdInPath(absoluteThemeFlagB, userId))) {
            iconDirs = GetRootDir(ReplaceUserIdInPath(absoluteThemeIconsB, userId));
        } else {
            RESMGR_HILOGE(RESMGR_TAG, "LoadThemesRes failed, userId = %{public}d, bundleName = %{public}s",
                userId, bundleName.c_str());
        }
    }
    LoadThemeIconsResource(bundleName, moduleName, iconDirs, userId);
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
        std::string resName = idItems[i]->GetItemResName();
        uint32_t id = idItems[i]->GetItemResId();
        ResType resType = idItems[i]->GetItemResType();
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
    return themeQualifierValue->GetResValue();
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
    iconMaskValues_.clear();
}

void ThemePackManager::LoadThemeIconsResource(const std::string &bundleName, const std::string &moduleName,
    const std::vector<std::string> &rootDirs, int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockIcon_);
    ChangeIconResourceStatus(userId);
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
        RESMGR_HILOGI_BY_FLAG(isLogFlag_, RESMGR_TAG, "load img, %{public}s", GetMaskString(dir).c_str());
        auto pThemeResource = ThemeResource::LoadThemeIconResource(dir);
        if (pThemeResource != nullptr) {
            this->iconResource_.emplace_back(pThemeResource);
        }
    }
    ClearIconResource();
    RESMGR_HILOGI_BY_FLAG(isLogFlag_, RESMGR_TAG, "load img end, size is %{public}zu", iconResource_.size());
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
            RESMGR_HILOGI_BY_FLAG(isLogFlag_, RESMGR_TAG, "find img, %{public}s", GetMaskString(result).c_str());
            break;
        }
    }
    return result;
}

bool ThemePackManager::UpdateThemeId(uint32_t newThemeId)
{
    std::lock_guard<std::mutex> lock(this->lockThemeId_);
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
    std::lock_guard<std::mutex> lock(this->lockIconValue_);
    std::string iconPath;
    std::string iconTag;
    if (iconName.find("icon_mask") != std::string::npos && isGlobalMask) {
        iconPath = themeMask;
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
        iconMaskValues_.emplace_back(std::make_tuple(iconTag, std::move(tmpInfo), len));
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
        RESMGR_HILOGI(RESMGR_TAG,
            "update userId, currentUserId_= %{public}d, userId= %{public}d", currentUserId_, userId);
        currentUserId_ = userId;
    }
}

bool ThemePackManager::IsSameResourceByUserId(const std::string &path, int32_t userId)
{
    std::string absolutePath("/data/service/el1/public/themes/");
    if (path.empty() || path.find(absolutePath) == std::string::npos) {
        return true;
    }
    auto pos = path.find("/", absolutePath.length());
    if (pos == std::string::npos) {
        return true;
    }
    auto subStr = path.substr(absolutePath.length(), pos - absolutePath.length());
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

void ThemePackManager::SetFlagByUserId(int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockUseCount_);
    auto iter = useCountMap_.find(userId);
    if (iter != useCountMap_.end()) {
        useCountMap_[userId] = iter->second + 1;
        return;
    }
    useCountMap_[userId] = 1;
}

void ThemePackManager::CheckFlagByUserId(int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockUseCount_);
    auto iter = useCountMap_.find(userId);
    if (iter == useCountMap_.end()) {
        return;
    }
    if (iter->second > 1) {
        useCountMap_[userId] = iter->second - 1;
        return;
    }
    useCountMap_.erase(userId);
    ReleaseSkinResource(userId);
    ReleaseIconResource(userId);
    if (useCountMap_.empty() || !IsUpdateByUserId(userId)) {
        UpdateUserId(-1); // reset user id
    }
}

void ThemePackManager::ReleaseSkinResource(int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockSkin_);
    ChangeSkinResourceStatus(userId);
    ClearSkinResource();
}

void ThemePackManager::ReleaseIconResource(int32_t userId)
{
    std::lock_guard<std::mutex> lock(this->lockIcon_);
    ChangeIconResourceStatus(userId);
    ClearIconResource();
}

const std::string ThemePackManager::GetMaskString(const std::string &path)
{
    if (path.empty() || path.find(absoluteThemePath) == std::string::npos) {
        return path;
    }
    return path.substr(absoluteThemePath.length(), path.length() - absoluteThemePath.length());
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
