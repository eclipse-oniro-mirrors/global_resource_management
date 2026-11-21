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
#ifndef OHOS_THEME_PACK_MANAGER_H
#define OHOS_THEME_PACK_MANAGER_H

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "hap_resource.h"
#include "res_common.h"
#include "res_config_impl.h"
#include "res_desc.h"
#include "resource_manager.h"
#include "theme_pack_config.h"
#include "theme_pack_resource.h"

namespace OHOS {
namespace Global {
namespace Resource {
/**
 * Theme skin resources of one bundle.
 *
 * For app bundles the owner is the ResourceManagerImpl (through ThemeSkinRef), and
 * ThemePackManager keeps only a weak index entry, so the resource is released together
 * with the last owner. For system bundles no ResourceManager owns the container:
 * ThemePackManager keeps a strong reference in systemSkinResource_ for the process lifetime.
 */
struct ThemeBundleResource {
    std::shared_ptr<ThemeResource> resource;
};

class ThemePackManager {
public:
    static std::shared_ptr<ThemePackManager> GetThemePackManager();

    ~ThemePackManager();

    /**
     * Load the theme resource.
     *
     * @param bundleName the hap bundleName
     * @param moduleName the hap moduleName
     * @param userId the user id
     * @return the skin resource container of the bundle, null when no skin is loaded
     */
    std::shared_ptr<ThemeBundleResource> LoadThemeRes(const std::string &bundleName, const std::string &moduleName,
        int32_t userId);

    /**
     * Load the theme skin resources of one bundle.
     *
     * Returns the container already held for the bundle, or loads it on first use.
     * The caller holds the returned container; ThemePackManager keeps only a weak
     * reference, so the resources are released when the last holder is destroyed.
     *
     * @param bundleName the hap bundleName
     * @param userId the user id
     * @return the skin resource container, never null
     */
    std::shared_ptr<ThemeBundleResource> LoadThemeBundleSkinResource(const std::string &bundleName, int32_t userId);

    /**
     * Remove the weak index entry of a bundle after its last ResourceManager released it.
     * Called by ThemeSkinRef's destructor; the entry is erased only when the weak_ptr is
     * expired, so a container still shared by another ResourceManager is kept.
     *
     * @param bundleName the hap bundleName
     * @param userId the user id
     */
    void RemoveThemeBundleSkinResource(const std::string &bundleName, int32_t userId);

    /**
     * Load the theme resource.
     *
     * @param bundleName the hap bundleName
     * @param userId the user id
     * @param rootDirs the theme skins dirs output
     * @param iconDirs the theme icons dirs output
     */
    void LoadSAThemeRes(const std::string &bundleName, int32_t userId,
        std::vector<std::string> &rootDirs, std::vector<std::string> &iconDirs);

    /**
     * Load the skin dir resource in theme pack.
     *
     * Clears the old resources of all alive bundles and reloads them from rootDirs.
     * The triggering bundle is always included in the alive list.
     *
     * @param bundleName the hap bundleName
     * @param moduleName the hap moduleName
     * @param rootDirs the theme skins dirs
     * @param userId the user id
     * @param basePath the base path for relative path conversion
     * @return the skin resource container of the triggering bundle, null when rootDirs is empty
     */
    std::shared_ptr<ThemeBundleResource> LoadThemeSkinResource(const std::string &bundleName,
        const std::string &moduleName, const std::vector<std::string> &rootDirs, int32_t userId,
        const std::string &basePath = "");

    /**
     * Load the icons dir resource int theme pack.
     *
     * @param bundleName the bundleName
     * @param moduleName the moduleName
     * @param rootDirs the theme icons dirs
     * @param userId the user id
     * @param basePath the base path for relative path conversion
     */
    void LoadThemeIconsResource(const std::string &bundleName, const std::string &moduleName,
        const std::vector<std::string> &rootDirs, int32_t userId, const std::string &basePath = "");

    /**
     * Get the theme resource related to bundlename, modulename, resType, resName and resConfig.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param resType the resoucre type
     * @param resName the resource name
     * @param resConfig the resource config
     * @param userId the user id
     */
    const std::string GetThemeResource(const std::pair<std::string, std::string> &bundleInfo, const ResType &resType,
        const std::string &resName, const ResConfigImpl &resConfig, int32_t userId);

    /**
     * Find the best theme resource related to bundlename, modulename, idItems and resConfig.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param idItems which used to process the reference resource
     * @param resConfig the resource config
     * @param userId the user id
     * @param isThemeSystemResEnable true is theme system res enable
     * @return the best resource or empty
     */
    const std::string FindThemeResource(const std::pair<std::string, std::string> &bundleInfo,
        const std::vector<std::shared_ptr<IdItem>> &idItems, const ResConfigImpl &resConfig, int32_t userId,
        bool isThemeSystemResEnable = false);

    /**
     * Find the best icon resource related to bundlename, modulename, resource name.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param iconName the icon resource name
     * @param userId the user id
     * @param abilityName the hap abilityName
     * @return the best resource or empty
     */
    const std::string FindThemeIconResource(const std::pair<std::string, std::string> &bundleInfo,
        const std::string &iconName, int32_t userId, const std::string &abilityName = "");

    inline std::string GetBasePath(int32_t userId) const
    {
        std::lock_guard<std::mutex> lock(this->lockBasePath_);
        auto it = basePathMap_.find(userId);
        return it != basePathMap_.end() ? it->second : "";
    }

    inline const std::string GetMask(int32_t userId) const
    {
        std::lock_guard<std::mutex> lock(this->lockHighlightIcon_);
        auto it = themeMaskMap_.find(userId);
        if (it == themeMaskMap_.end()) {
            return "";
        }
        return BuildFullPath(it->second, userId);
    }

    const std::string ReplaceUserIdInPath(const std::string &originalPath, int32_t userId);
    const std::string GetLogPath(const std::string &path, int32_t userId);

    bool UpdateThemeId(uint32_t newThemeId, int32_t userId);

    bool UpdateThemeIcon(uint32_t newThemeIcon, int32_t userId);

    bool UpdateThemeSkin(uint32_t newThemeSkin, int32_t userId);

    bool IsFirstLoadResource();

    /**
     * Whether an icon exists in the theme
     *
     * @param bundleName the hap bundleName
     * @param userId the user id
     * @return true if icon exists, else no exists
     */
    bool HasIconInTheme(const std::string &bundleName, int32_t userId);

    /**
     * Get icons info in other icons by icon name
     *
     * @param iconName the icon name
     * @param outValue the obtain resource wirte to
     * @param len the data len wirte to
     * @param isGlobalMask true if the global mask, else other icons
     * @param userId the user id
     * @return SUCCESS if the theme icon get success, else failed
     */
    RState GetOtherIconsInfo(const std::string &iconName, std::unique_ptr<uint8_t[]> &outValue, size_t &len,
        bool isGlobalMask, int32_t userId);

    /**
     * Get icons info of icon_highlightstroke
     *
     * @param iconName the icon name
     * @param outValue the obtain resource wirte to
     * @param len the data len wirte to
     * @param isGlobalMask true if the global mask, else other icons
     * @param userId the user id
     * @return SUCCESS if the theme icon get success, else failed
     */
    RState GetHighlightIconInfo(const std::string &iconName, std::unique_ptr<uint8_t[]> &outValue, size_t &len,
        bool isGlobalMask, int32_t userId);

    /**
     * Get the theme icon from cache
     *
     * @param iconTag the tag of icon info
     * @param outValue the obtain resource wirte to
     * @param len the data len wirte to
     * @return SUCCESS if the theme icon get success, else failed
     */
    RState GetThemeIconFromCache(const std::string &iconTag, std::unique_ptr<uint8_t[]> &outValue, size_t &len,
        int32_t userId);

    /**
     * Whether to update theme by the user id
     *
     * @param userId the current user id
     * @return true if update theme by the user id, else not update
     */
    bool IsUpdateByUserId(int32_t userId);

    void LoadThemeIconRes(const std::string &bundleName, const std::string &moduleName, int32_t userId);

    void LoadThemeSkinRes(const std::string &bundleName, const std::string &moduleName, int32_t userId);

private:
    ThemePackManager();
    std::unordered_map<int32_t, std::string> themeMaskMap_;
    std::unordered_map<int32_t, std::string> themeStrokeMap_;
    // The following methods require the caller to hold lockSkin_.
    std::vector<std::string> ClearSkinResource(int32_t userId);
    // Scans rootDirs once and writes the resources of the given bundles (plus system resources
    // when they are not loaded yet) into their containers.
    void LoadSkinResourcesLocked(const std::vector<std::string> &bundles, const std::vector<std::string> &rootDirs,
        int32_t userId, const std::string &basePath);
    // Loads the bundle's skin dirs (plus system resources) and writes them into the bundle's
    // container, creating the container when no ResourceManager holds it yet.
    // Returns the container of bundleName.
    std::shared_ptr<ThemeBundleResource> AppendThemeSkinResource(const std::string &bundleName,
        const std::vector<std::string> &rootDirs, int32_t userId, const std::string &basePath);
    bool IsSysResourcesLoaded(int32_t userId);
    std::shared_ptr<ThemeBundleResource> GetThemeSkinResourceLocked(int32_t userId, const std::string &bundleName);
    std::shared_ptr<ThemeBundleResource> GetOrCreateSkinContainerLocked(int32_t userId, const std::string &bundleName);
    void ClearIconResource(int32_t userId);
    void ClearHighlightIcon(int32_t userId);
    const std::string GetMaskString(const std::string &path);
    // owner = ResourceManagerImpl (through ThemeSkinRef); ThemePackManager keeps a weak index.
    std::map<int32_t, std::map<std::string, std::weak_ptr<ThemeBundleResource>>> skinResource_;
    // System skin resources have no ResourceManager owner, so the manager keeps them alive.
    std::map<int32_t, std::map<std::string, std::shared_ptr<ThemeBundleResource>>> systemSkinResource_;
    std::map<int32_t, std::vector<std::shared_ptr<ThemeResource>>> iconResource_;
    std::unordered_map<int32_t, std::vector<std::tuple<std::string, std::unique_ptr<uint8_t[]>, size_t>>>
        iconMaskValuesMap_;
    std::unordered_map<int32_t, std::tuple<std::string, std::unique_ptr<uint8_t[]>, size_t>>
        iconHighlightValueMap_;
    std::vector<std::shared_ptr<ThemeResource::ThemeValue> > GetThemeResourceList(
        const std::pair<std::string, std::string> &bundInfo, const ResType &resType, const std::string &resName,
        int32_t userId);

    const std::shared_ptr<ThemeResource::ThemeQualifierValue> GetThemeQualifierValue(
        const std::pair<std::string, std::string> &bundInfo, const ResType &resType,
        const std::string &resName, const ResConfigImpl &resConfig, int32_t userId);

    const std::shared_ptr<ThemeResource::ThemeQualifierValue> GetBestMatchThemeResource(
        const std::vector<std::shared_ptr<ThemeResource::ThemeValue> > &candidates,
        const ResConfigImpl &resConfig);

    std::vector<std::string> GetRootDir(const std::string &strCurrentDir, const std::string &basePath, int32_t userId);
    std::vector<std::string> GetThemeSkinRootDir(const std::string &newPath, const std::string &oldPath,
        const std::string &basePath, int32_t userId);
    std::vector<std::string> GetSystemResRootDirs(int32_t userId);
    void UpdateSystemResRootDirs(std::vector<std::string> &rootDirs, int32_t userId);
    bool UpdateThemeValue(std::unordered_map<int32_t, uint32_t> &themeMap, uint32_t newValue, int32_t userId,
        const std::string &tag);
    void UpdateBasePath(int32_t userId);
    void UpdateUserId(int32_t userId);
    std::string BuildFullPath(const std::string &relativePath, int32_t userId) const;
    std::string GetCanonicalBasePath(const std::string &basePath);
    std::shared_mutex lockSkin_;
    std::mutex lockIcon_;
    std::mutex lockThemeMaps_;
    std::mutex lockIconValue_;
    mutable std::mutex lockHighlightIcon_;
    mutable std::mutex lockBasePath_;
    std::mutex lockUserId_;
    std::unordered_map<int32_t, uint32_t> themeIdMap_;
    std::unordered_map<int32_t, uint32_t> themeIconMap_;
    std::unordered_map<int32_t, uint32_t> themeSkinMap_;
    std::unordered_map<int32_t, std::string> basePathMap_;
    std::atomic<bool> isFirstCreate_{ true };
    int32_t currentUserId_ = 0;
    const bool isLogFlag_ = false;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif