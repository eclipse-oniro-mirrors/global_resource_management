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
#ifndef OHOS_THEME_PACK_RESOURCE_H
#define OHOS_THEME_PACK_RESOURCE_H

#include "res_config_impl.h"
#include "hap_resource.h"
#include "res_desc.h"
#include "resource_manager.h"
#include "lock.h"
#include "res_common.h"
#include <unordered_map>
#include "theme_pack_config.h"
#include <tuple>
#include "cJSON.h"

namespace OHOS {
namespace Global {
namespace Resource {
struct ThemeKey {
    std::string bundleName;
    std::string moduleName;
    ResType resType;
    std::string resName;

    ThemeKey(std::string bundle, std::string module, ResType type, std::string name) : bundleName(bundle),
        moduleName(module), resType(type), resName(name) {};
    bool operator == (const ThemeKey& theme) const
    {
        return bundleName == theme.bundleName && moduleName == theme.moduleName
            && resType == theme.resType && resName == theme.resName;
    }
};

struct HashTk {
    std::size_t operator() (const ThemeKey &theme) const
    {
        std::size_t h1 = std::hash<std::string>()(theme.bundleName);
        std::size_t h2 = std::hash<std::string>()(theme.moduleName);
        std::size_t h3 = std::hash<int32_t>()(theme.resType);
        std::size_t h4 = std::hash<std::string>()(theme.resName);
        return h1 ^ h2 ^ h3 ^ h4;
    }
};

class ThemeResource {
public:
    ThemeResource(std::string path);
    ~ThemeResource();
    static const std::shared_ptr<ThemeResource> LoadThemeResource(const std::string& rootDir);
    static const std::shared_ptr<ThemeResource> LoadThemeIconResource(const std::string& rootDir);
    class ThemeQualifierValue {
    public:
        inline const std::string GetResValue() const
        {
            return resValue_;
        }

        inline const std::shared_ptr<ThemeConfig> GetThemeConfig() const
        {
            return themeConfig_;
        }

        inline const ThemeKey GetThemeKey() const
        {
            return themeKey_;
        }
        ThemeQualifierValue(const ThemeKey themeKey, std::shared_ptr<ThemeConfig> themeConfig,
            const std::string &value);
        ~ThemeQualifierValue();
    private:
        ThemeKey themeKey_;
        std::shared_ptr<ThemeConfig> themeConfig_;
        std::string resValue_;
    };

    class ThemeValue {
    public:
        inline void AddThemeLimitPath(std::shared_ptr<ThemeQualifierValue> themeQualifer)
        {
            limitPaths_.push_back(themeQualifer);
        }

        inline const std::vector<std::shared_ptr<ThemeQualifierValue> > &GetThemeLimitPathsConst() const
        {
            return limitPaths_;
        }
        ~ThemeValue();
    private:
        std::vector<std::shared_ptr<ThemeQualifierValue> > limitPaths_;
    };
    /**
     * Get the theme value related bundlename, modulename, resType and resource name.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param resType the resource type
     * @param name the resource name
     * @return the theme value vector
     */
    std::vector<std::shared_ptr<ThemeResource::ThemeValue> >  GetThemeValues(
        const std::pair<std::string, std::string> &bundInfo,
        const ResType &resType, const std::string &name);

    /**
     * Get the theme icon related bundlename, modulename and resource name.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param name the icon name
     * @return the icon path
     */
    const std::string GetThemeAppIcon(const std::pair<std::string, std::string> &bundleInfo, const std::string &name);

    /**
     * Get the theme value related bundlename, modulename, resType and resource name.
     *
     * @param bundleInfo which contains bundlename, modulename
     * @param resType the resource type
     * @param name the resource name
     * @return the theme value vector
     */
    std::string GetThemeResBundleName(const std::string &themePath);

    inline std::string GetThemePath()
    {
        return themePath_;
    }

    std::string themePath_;
private:
    std::vector<std::tuple<ResType, std::string, std::shared_ptr<ThemeValue>> > themeValueVec_;
    std::vector<std::pair<ThemeKey, std::string> > iconValues_;
    void ParseJson(const std::string &bundleName, const std::string &moduleName, const std::string &jsonPath);
    void ParseIcon(const std::string &bundleName, const std::string &moduleName, const std::string &iconPath);
    void InitThemeRes(std::pair<std::string, std::string> bundleInfo, cJSON *root,
        std::shared_ptr<ThemeConfig> themeConfig, const std::string &resTypeStr);
    static ThemeResource *themeRes;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif