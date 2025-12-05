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

#include <mutex>
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
class ThemePackManager {
public:
    static std::shared_ptr<ThemePackManager> GetThemePackManager();

    ~ThemePackManager();

    /**
     * Load the theme resource.
     *
     * @param bundleName the hap bundleName
     * @param moduleName the hap moduleName
     * @param userId the uesr id
     */
    void LoadThemeRes(const std::string &bundleName, const std::string &moduleName, int32_t userId);

    /**
     * Load the theme resource.
     *
     * @param bundleName the hap bundleName
     * @param moduleName the hap moduleName
     * @param userId the uesr id
     */
    void LoadSAThemeRes(const std::string &bundleName, const std::string &moduleName,
        int32_t userId, std::vector<std::string> &rootDirs, std::vector<std::string> &iconDirs);

    /**
     * Load the skin dir resource in theme pack.
     *
     * @param bundleName the hap bundleName
     * @param moduleName the hap moduleName
     * @param rootDirs the theme skins dirs
     */
    void LoadThemeSkinResource(const std::string &bundleName, const std::string &moduleName,
        const std::vector<std::string> &rootDirs, int32_t userId);

    /**
     * Load the icons dir resource int theme pack.
     *
     * @param bundleName the bundleName
     * @param moduleName the moduleName
     * @param rootDirs the theme icons dirs
     */
    void LoadThemeIconsResource(const std::string &bundleName, const std::string &moduleName,
        const std::vector<std::string> &rootDirs, int32_t userId);

    /**
     * Get the theme resource related to bundlename, modulename, resType, resName and resConfig.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param resType the resoucre type
     * @param resName the resource name
     * @param resConfig the resource config
     * @param userId the user id
     */
    const std::string GetThemeResource(const std::pair<std::string, std::string> &bundleInfo,
        const ResType &resType, const std::string &resName, const ResConfigImpl &resConfig, int32_t userId);

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

    inline const std::string GetMask() const
    {
        return themeMask;
    }

    const std::string ReplaceUserIdInPath(const std::string &originalPath, int32_t userId);

    bool UpdateThemeId(uint32_t newThemeId);

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
    RState GetOtherIconsInfo(const std::string &iconName,
        std::unique_ptr<uint8_t[]> &outValue, size_t &len, bool isGlobalMask, int32_t userId);

    /**
     * Get the theme icon from cache
     *
     * @param iconTag the tag of icon info
     * @param outValue the obtain resource wirte to
     * @param len the data len wirte to
     * @return SUCCESS if the theme icon get success, else failed
     */
    RState GetThemeIconFromCache(const std::string &iconTag, std::unique_ptr<uint8_t[]> &outValue, size_t &len);

    /**
     * Whether to update theme by the user id
     *
     * @param userId the current user id
     * @return true if update theme by the user id, else not update
     */
    bool IsUpdateByUserId(int32_t userId);

    void LoadThemeIconRes(const std::string &bundleName, const std::string &moduleName, int32_t userId);

private:
    ThemePackManager();
    std::string themeMask;
    std::string themeStroke;
    void ChangeSkinResourceStatus(int32_t userId);
    void ChangeIconResourceStatus(int32_t userId);
    void ClearSkinResource();
    void ClearIconResource();
    const std::string GetMaskString(const std::string &path);
    std::vector<std::shared_ptr<ThemeResource>> skinResource_;
    std::vector<std::shared_ptr<ThemeResource>> iconResource_;
    std::vector<std::tuple<std::string, std::unique_ptr<uint8_t[]>, size_t>> iconMaskValues_;
    std::vector<std::shared_ptr<ThemeResource::ThemeValue> > GetThemeResourceList(
        const std::pair<std::string, std::string> &bundInfo, const ResType &resType, const std::string &resName,
        int32_t userId);

    const std::shared_ptr<ThemeResource::ThemeQualifierValue> GetThemeQualifierValue(
        const std::pair<std::string, std::string> &bundInfo, const ResType &resType,
        const std::string &resName, const ResConfigImpl &resConfig, int32_t userId);

    const std::shared_ptr<ThemeResource::ThemeQualifierValue> GetBestMatchThemeResource(
        const std::vector<std::shared_ptr<ThemeResource::ThemeValue> > &candidates,
        const ResConfigImpl &resConfig);

    std::vector<std::string> GetRootDir(const std::string &strCurrentDir);
    std::vector<std::string> GetThemeSkinRootDir(const std::string &newPath, const std::string &oldPath);
    bool IsSameResourceByUserId(const std::string &path, int32_t userId);
    void UpdateUserId(int32_t userId);
    std::mutex lockSkin_;
    std::mutex lockIcon_;
    std::mutex lockThemeId_;
    std::mutex lockIconValue_;
    std::mutex lockUserId_;
    uint32_t themeId_{0};
    bool isFirstCreate = true;
    int32_t currentUserId_ = 0;
    bool isLogFlag_ = false;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif