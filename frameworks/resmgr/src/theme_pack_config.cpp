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
#include "theme_pack_config.h"

#include "hap_manager.h"
#include "hilog_wrapper.h"
namespace OHOS {
namespace Global {
namespace Resource {
ThemeConfig::ThemeConfig() : themeDirection_(DIRECTION_NOT_SET), themeColorMode_(COLOR_MODE_NOT_SET) {}

void ThemeConfig::SetThemeDirection(Direction direction)
{
    this->themeDirection_ = direction;
}

void ThemeConfig::SetThemeColorMode(ColorMode colorMode)
{
    this->themeColorMode_ = colorMode;
}

bool ThemeConfig::Match(const std::shared_ptr<ThemeConfig> &themeConfig, const ResConfigImpl &resConfig)
{
    ColorMode colorMode = resConfig.GetColorMode();
    Direction direction = resConfig.GetDirection();
    ColorMode themeColorMode_ = themeConfig->GetThemeColorMode();
    Direction themeDirection_ = themeConfig->GetThemeDirection();
    if (direction != DIRECTION_NOT_SET && themeDirection_ != DIRECTION_NOT_SET) {
        if (direction != themeDirection_) {
            return false;
        }
    }
    if (colorMode == DARK && !resConfig.GetAppColorMode() && !resConfig.GetAppDarkRes()) {
        return themeColorMode_ == COLOR_MODE_NOT_SET;
    }
    if (colorMode != COLOR_MODE_NOT_SET && themeColorMode_ != COLOR_MODE_NOT_SET) {
        if (colorMode != themeColorMode_) {
            return false;
        }
    }
    return true;
}

bool ThemeConfig::BestMatch(const std::shared_ptr<ThemeConfig> &themeConfig, const ResConfigImpl &resConfig) const
{
    ColorMode colorMode = resConfig.GetColorMode();
    Direction direction = resConfig.GetDirection();
    ColorMode themeColorMode_ = themeConfig->GetThemeColorMode();
    Direction themeDirection_ = themeConfig->GetThemeDirection();
    if (this->themeDirection_ != themeDirection_ && direction != DIRECTION_NOT_SET) {
        return this->themeDirection_ != DIRECTION_NOT_SET;
    }
  
    if (this->themeColorMode_ != themeColorMode_ && colorMode != COLOR_MODE_NOT_SET) {
        return this->themeColorMode_ != COLOR_MODE_NOT_SET;
    }
    return this->IsMoreMatchThan(themeConfig);
}

bool ThemeConfig::IsMoreMatchThan(const std::shared_ptr<ThemeConfig> &themeConfig) const
{
    if (this->themeDirection_ != themeConfig->themeDirection_) {
        return this->themeDirection_ != DIRECTION_NOT_SET;
    }

    if (this->themeColorMode_ != themeConfig->themeColorMode_) {
        return this->themeColorMode_ != COLOR_MODE_NOT_SET;
    }
    
    return true;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
