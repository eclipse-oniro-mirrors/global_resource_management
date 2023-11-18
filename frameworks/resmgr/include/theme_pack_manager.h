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

#include "res_config_impl.h"
#include "hap_resource.h"
#include "res_desc.h"
#include "resource_manager.h"
#include "lock.h"
#include "res_common.h"
#include <unordered_map>
#include "theme_pack_resource.h"
#include "theme_pack_config.h"

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
     * @param themeMask the theme icon mask
     */
    void LoadThemeRes(const std::string &bundleName, const std::string &moduleName, std::string& themeMask);

    /**
     * Load the skin dir resource in theme pack.
     *
     * @param bundleName the hap bundleName
     * @param moduleName the hap moduleName
     * @param rootDirs the theme skins dirs
     */
    void LoadThemeSkinResource(const std::string &bundleName, const std::string &moduleName,
        const std::vector<std::string> &rootDirs);

    /**
     * Load the icons dir resource int theme pack.
     *
     * @param bundleName the bundleName
     * @param moduleName the moduleName
     * @param rootDirs the theme icons dirs
     */
    void LoadThemeIconsResource(const std::string &bundleName, const std::string &moduleName,
        const std::vector<std::string> &rootDirs);
    
    /**
     * Get the theme resource related to bundlename, modulename, resType, resName and resConfig.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param resType the resoucre type
     * @param resName the resource name
     * @param resConfig the resource config
     */
    const std::string GetThemeResource(const std::pair<std::string, std::string> &bundleInfo,
        const ResType &resType, const std::string &resName, const ResConfigImpl &resConfig);

    /**
     * Find the best theme resource related to bundlename, modulename, idItems and resConfig.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param idItems which used to process the reference resource
     * @param resConfig the resource config
     * @return the best resource or empty
     */
    const std::string FindThemeResource(const std::pair<std::string, std::string> &bundleInfo,
        std::vector<const IdItem *> idItems, const ResConfigImpl &resConfig);

    /**
     * Find the best icon resource related to bundlename, modulename, resource name.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param iconName the icon resource name
     * @return the best resource or empty
     */
    const std::string FindThemeIconResource(const std::pair<std::string, std::string> &bundleInfo,
        const std::string &iconName);

private:
    ThemePackManager();
    std::vector<ThemeResource *> skinResource_;
    std::vector<ThemeResource *> iconResource_;
    std::vector<const ThemeResource::ThemeValue *> GetThemeResourceList(
        const std::pair<std::string, std::string> &bundInfo, const ResType &resType, const std::string &resName) const;

    const ThemeResource::ThemeQualifierValue *GetThemeQualifierValue(
        const std::pair<std::string, std::string> &bundInfo, const ResType &resType,
        const std::string &resName, const ResConfigImpl &resConfig);

    const ThemeResource::ThemeQualifierValue *GetBestMatchThemeResource(
        const std::vector<const ThemeResource::ThemeValue *> candidates, const ResConfigImpl &resConfig);
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif