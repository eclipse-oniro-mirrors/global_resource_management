/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_RESOURCE_MANAGER_RESCONFIG_IMPL_H
#define OHOS_RESOURCE_MANAGER_RESCONFIG_IMPL_H

#include <stdint.h>
#include "res_locale.h"
#include "res_common.h"
#include "res_config.h"

#ifdef SUPPORT_GRAPHICS
using icu::Locale;
#endif
namespace OHOS {
namespace Global {
namespace Resource {
class ResConfigImpl : public ResConfig {
public:
    ResConfigImpl();

    /**
     * Whether this resConfig more match request resConfig
     * @param other the other resConfig
     * @param request  the request resConfig
     * @param density the input screen density
     * @return true if this resConfig more match request resConfig than other resConfig, else false
     */
    bool IsMoreSuitable(const std::shared_ptr<ResConfigImpl> other, const std::shared_ptr<ResConfigImpl> request,
        uint32_t density = 0) const;

    /**
     * Set locale information
     * @param language the locale language
     * @param script  the locale script
     * @param region  the locale region
     * @return SUCCESS if set locale information success, else false
     */
    RState SetLocaleInfo(const char *language, const char *script, const char *region);

    /**
     * Set locale information
     * @param localeStr the locale info, for example, zh-Hans-CN.
     * @return SUCCESS if set locale information success, else false
     */
    RState SetLocaleInfo(const char *localeStr);

#ifdef SUPPORT_GRAPHICS
    RState SetPreferredLocaleInfo(Locale &preferredLocaleInfo);

    RState SetLocaleInfo(Locale &localeInfo);
#endif
    /**
     * Set resConfig device type
     * @param deviceType the device type
     */
    void SetDeviceType(DeviceType deviceType);

    /**
     * Set resConfig direction
     * @param direction the resConfig direction
     */
    void SetDirection(Direction direction);

    /**
     * Set resConfig colorMode
     * @param colorMode the resConfig colorMode
     */
    void SetColorMode(ColorMode colorMode);

    /**
     * Set resConfig mcc
     * @param mcc the resConfig mcc
     */
    void SetMcc(uint32_t mcc);

    /**
     * Set resConfig mnc
     * @param mnc the resConfig mnc
     */
    void SetMnc(uint32_t mnc);

    /**
     * Set resConfig screenDensity
     * @param screenDensity the resConfig screenDensity
     */
    void SetScreenDensity(float screenDensity);

    /**
     * Set resConfig screenDensityDpi
     * @param screenDensityDpi the resConfig screenDensityDpi
     */
    void SetScreenDensityDpi(ScreenDensity screenDensityDpi);

#ifdef SUPPORT_GRAPHICS
    const ResLocale *GetResPreferredLocale() const;

    const Locale *GetPreferredLocaleInfo() const;

    const Locale *GetLocaleInfo() const;
#endif

    const ResLocale *GetResLocale() const;

    Direction GetDirection() const;

    float GetScreenDensity() const;

    ScreenDensity GetScreenDensityDpi() const;

    bool CopyLocaleAndPreferredLocale(ResConfig &other);

    bool isLocaleInfoSet();

    ColorMode GetColorMode() const;

    uint32_t GetMcc() const;

    uint32_t GetMnc() const;

    DeviceType GetDeviceType() const;

    /**
     * Whether this resConfig match other resConfig
     * @param other the other resConfig
     * @param isCheckDarkAdaptation whether check the color mode match the dark mode
     * @return true if this resConfig match other resConfig, else false
     */
    bool Match(const std::shared_ptr<ResConfigImpl> other, bool isCheckDarkAdaptation = true) const;

    /**
     * Copy other resConfig to this resConfig
     * @param other the other resConfig
     * @param isRead the isAppDarkRes_ is read-only
     * @return true if copy other resConfig to this resConfig success, else false
     */
    bool Copy(ResConfig &other, bool isRead = false);

    /**
     * Complete the local script
     */
    void CompleteScript();

    /**
     * Whether this resLocal script completed
     * @return true if resLocal script completed, else false
     */
    bool IsCompletedScript() const;

    /**
     * Set resConfig input device
     * @param inputDevice the resConfig input device
     */
    void SetInputDevice(InputDevice inputDevice);

    /**
     * Get resConfig input device
     * @return the resConfig input device
     */
    InputDevice GetInputDevice() const;

    /**
     * Matching screen density by screen density ratio
     * @return ScreenDensity
     */
    ScreenDensity ConvertDensity(float density);

    virtual ~ResConfigImpl();

    /**
     * Set colorMode is setted by app
     * @param isAppColorMode whether colorMode is setted by app
     */
    void SetAppColorMode(bool isAppColorMode);

    /**
     * Get whether colorMode is setted by app
     * @return True if colorMode is setted by app
     */
    bool GetAppColorMode() const;

    /**
     * Set whether the app has dark resources
     * @param isAppDarkRes whether the app has dark resources
     */
    void SetAppDarkRes(bool isAppDarkRes);

    /**
     * Get whether the app has dark resources
     * @return True if the app has dark resources
     */
    bool GetAppDarkRes() const;

private:
    bool IsMoreSpecificThan(const std::shared_ptr<ResConfigImpl> other, uint32_t density = 0) const;

    bool CopyLocale(ResConfig &other);

#ifdef SUPPORT_GRAPHICS
    bool CopyPreferredLocale(ResConfig &other);

    bool CopyLocale(Locale **currentLocaleInfo, ResLocale **currentResLocale,
        const Locale *otherLocaleInfo);
#endif

    bool IsMccMncMatch(uint32_t mcc,  uint32_t mnc) const;

    bool IsDirectionMatch(Direction direction) const;

    bool IsDeviceTypeMatch(DeviceType deviceType) const;

    bool IsColorModeMatch(ColorMode colorMode, bool isCheckDarkAdaptation) const;

    bool IsInputDeviceMatch(InputDevice inputDevice) const;

    int IsMccMncMoreSuitable(uint32_t otherMcc, uint32_t otherMnc, uint32_t requestMcc, uint32_t requestMnc) const;

    int IsDensityMoreSuitable(ScreenDensity otherDensity, ScreenDensity requestDensity, uint32_t density = 0) const;

    bool IsDensityMoreSuitable(int thisDistance, int otherDistance) const;

    int IsDensityMoreSpecificThan(ScreenDensity otherDensity, uint32_t density = 0) const;

#ifdef SUPPORT_GRAPHICS
    RState BuildResLocale(const char *language, const char *script, const char *region, ResLocale **resLocale);

    RState BuildLocaleInfo(const ResLocale *resLocale, Locale **localeInfo);
#endif
private:
    ResLocale *resLocale_;
    Direction direction_;
    float density_;
    ScreenDensity screenDensityDpi_;
    ColorMode colorMode_;
    uint32_t mcc_;
    uint32_t mnc_;
    DeviceType deviceType_;
    InputDevice inputDevice_;
#ifdef SUPPORT_GRAPHICS
    ResLocale *resPreferredLocale_;
    Locale *preferredLocaleInfo_;
    Locale *localeInfo_;
#endif
    bool isCompletedScript_;
    bool isAppColorMode_;
    bool isAppDarkRes_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif