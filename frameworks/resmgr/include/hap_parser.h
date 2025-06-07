/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_RESOURCE_MANAGER_HAPPARSER_H
#define OHOS_RESOURCE_MANAGER_HAPPARSER_H

#include <cstdint>
#include <cstdio>
#include <string>
#include <set>
#include <unzip.h>
#include "hap_resource.h"
#include "res_desc.h"
#include "res_config_impl.h"
#include "resource_manager.h"

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "file_mapper.h"
#include "extractor.h"
#endif

namespace OHOS {
namespace Global {
namespace Resource {
class HapParser {
public:
    HapParser();

    virtual ~HapParser();

    virtual bool Init(const char *path);

    virtual int32_t ParseResHex();

    virtual std::shared_ptr<HapResource> GetHapResource(const char *path, bool isSystem, bool isOverlay);

    static bool GetIndexData(const char *path, std::unique_ptr<uint8_t[]> &buf, size_t &bufLen);

    #if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    static std::string GetIndexFilePath(std::shared_ptr<AbilityBase::Extractor> &extractor);
    #endif

    static bool GetIndexDataFromHap(const char *path, std::unique_ptr<uint8_t[]> &buf, size_t &bufLen);

    static bool GetIndexDataFromIndex(const char *path, std::unique_ptr<uint8_t[]> &buf, size_t &bufLen);

    /**
     * Get the raw file data from hap
     * @param hapPath the hap path
     * @param patchPath the hqf path
     * @param rawFileName the rawFile path
     * @param len the rawFile path
     * @param outValue the rawFile path
     * @return the rawFile path
     */
    static RState ReadRawFileFromHap(const std::string &hapPath, const std::string &patchPath,
        const std::string &rawFileName, size_t &len, std::unique_ptr<uint8_t[]> &outValue);

    /**
     * Get the raw file descriptor
     * @param hapPath the hap path
     * @param patchPath the hqf path
     * @param rawFileName the rawFile path
     * @param descriptor the rawFile path
     * @return the rawFile path
     */
    static RState ReadRawFileDescriptor(const char *hapPath, const char *patchPath,
        const std::string &rawFileName, ResourceManager::RawFileDescriptor &descriptor);
    
    /**
     * Get the raw file list
     * @param hapPath the hap path
     * @param rawDirPath the rawfile directory path
     * @param fileList the rawfile list write to
     * @return SUCCESS if resource exist, else not found
     */
    static RState GetRawFileList(const std::string &hapPath, const std::string &rawDirPath,
        std::set<std::string>& fileList);

    /**
     * Get the raw file list in UnCompressed
     * @param indexPath the hap path
     * @param rawDirPath the rawfile directory path
     * @param fileList the rawfile list write to
     * @return SUCCESS if resource exist, else not found
     */
    static RState GetRawFileListUnCompressed(const std::string &indexPath, const std::string &rawDirPath,
        std::vector<std::string>& fileList);

    /**
     * Get screen density
     * @param value the type of screen density
     * @return the screen density related to the value
     */
    static ScreenDensity GetScreenDensity(uint32_t value);

    /**
     * Get device type
     * @param value the type of device
     * @return the device type related to the value
     */
    static DeviceType GetDeviceType(uint32_t value);

    /**
     * Get color mode
     * @param value the type of device
     * @return the color mode related to the value
     */
    static ColorMode GetColorMode(uint32_t value);

    /**
     * Get mcc
     * @param value the type of mcc
     * @return the mcc related to the value
     */
    static uint32_t GetMcc(uint32_t value);

    /**
     * Get mnc
     * @param value the type of mnc
     * @return the mnc related to the value
     */
    static uint32_t GetMnc(uint32_t value);
	
    /**
     * Get input device
     * @param value the type of input device
     * @return the input device related to the value
     */
    static InputDevice GetInputDevice(uint32_t value);

    /**
     * Whether this raw resource is a directory
     *
     * @param hapPath the hap path
     * @param pathName the raw resource path
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if raw resource is a directory, else not found
     */
    static RState IsRawDirFromHap(const char *hapPath, const std::string &pathName, bool &outValue);

    /**
     * Whether this raw resource is a directory in UnCompressed
     *
     * @param pathName the raw resource path
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if raw resource is a directory, else not found
     */
    static RState IsRawDirUnCompressed(const std::string &pathName, bool &outValue);

    /**
     * Create resource config from KeyParams
     * @param keyParams the keyParams contain type and value
     * @return the resource config related to the keyParams
     */
    static std::shared_ptr<ResConfigImpl> CreateResConfigFromKeyParams(
        const std::vector<std::shared_ptr<KeyParam>> &keyParams);

    struct ResConfigKey {
        const char *language = nullptr;
        const char *script = nullptr;
        const char *region = nullptr;
        ScreenDensity screenDensity = SCREEN_DENSITY_NOT_SET;
        Direction direction = DIRECTION_NOT_SET;
        DeviceType deviceType = DEVICE_NOT_SET;
        ColorMode colorMode = COLOR_MODE_NOT_SET;
        InputDevice inputDevice = INPUTDEVICE_NOT_SET;
        uint32_t mcc = MCC_UNDEFINED;
        uint32_t mnc = MNC_UNDEFINED;
    };

    static std::shared_ptr<ResConfigImpl> BuildResConfig(ResConfigKey *configKey);

    static void GetKeyParamsLocales(std::shared_ptr<KeyParam> kp, std::string &locale, bool &isLocale);

    #if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    static std::string ParseModuleName(std::shared_ptr<AbilityBase::Extractor> &extractor);
    #endif
protected:
    uint32_t limitKeyValue_{0};
    
    std::set<std::string> locales_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
