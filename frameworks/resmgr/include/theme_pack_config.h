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
#ifndef OHOS_THEME_PACK_CONFIG_H
#define OHOS_THEME_PACK_CONFIG_H

#include "res_config_impl.h"
#include "resource_manager.h"
#include "res_common.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ThemeConfig {
public:
    ThemeConfig();
    /**
     * Set theme resConfig direction
     *
     * @param direction the theme resConfig direction
     */
    void SetThemeDirection(Direction direction);

    /**
     * Set theme resConfig colorMode
     *
     * @param colorMode the theme resConfig colorMode
     */
    void SetThemeColorMode(ColorMode colorMode);

    ColorMode GetThemeColorMode() const
    {
        return themeColorMode_;
    }

    Direction GetThemeDirection() const
    {
        return themeDirection_;
    }

    /**
     * Whether this themeConfig match the device resConfig
     *
     * @param themeConfig the theme resConfig
     * @param resConfig the device resConfig
     * @return true if this themeConfig match the device resConfig, else false
     */
    static bool Match(const std::shared_ptr<ThemeConfig> &themeConfig, const ResConfigImpl &resConfig);

    /**
     * Whether this themeConfig best match the device resConfig
     *
     * @param themeConfig the theme resConfig
     * @param resConfig the device resConfig
     * @return true if this themeConfig best match the device resConfig, else false
     */
    bool BestMatch(const std::shared_ptr<ThemeConfig> &themeConfig, const ResConfigImpl &resConfig) const;
private:
    bool IsMoreMatchThan(const std::shared_ptr<ThemeConfig> &themeConfig) const;

    Direction themeDirection_;
    ColorMode themeColorMode_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif