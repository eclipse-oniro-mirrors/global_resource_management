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

#include "hap_parser.h"

#include <cstdlib>
#include <string>
#include <fcntl.h>
#include <fstream>
#include <unzip.h>
#include <unistd.h>
#include <set>
#include <sys/stat.h>
#include <unordered_map>

#include "hap_resource_manager.h"
#include "hap_resource_v1.h"
#include "hilog_wrapper.h"
#include "locale_matcher.h"
#if defined(__WINNT__)
#include <cstring>
#else
#include "securec.h"
#endif
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#endif
#include "utils/errors.h"
#include "utils/string_utils.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
HapParser::HapParser()
{}

HapParser::~HapParser()
{}

bool HapParser::Init(const char *path)
{
    return true;
}

int32_t HapParser::ParseResHex()
{
    return OK;
}

bool HapParser::GetIndexData(const char *path, std::unique_ptr<uint8_t[]> &buf, size_t &bufLen)
{
    if (Utils::ContainsTail(path, Utils::tailSet)) {
        return GetIndexDataFromHap(path, buf, bufLen);
    } else {
        return GetIndexDataFromIndex(path, buf, bufLen);
    }
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string GetModuleName(const char *configStr, size_t len)
{
    if (configStr == nullptr) {
        return std::string();
    }
    std::string config(configStr, len);
    static const char *key = "\"moduleName\"";
    auto idx = config.find(key);
    if (idx == std::string::npos) {
        return std::string();
    }
    auto start = config.find("\"", idx + strlen(key));
    if (start == std::string::npos) {
        return std::string();
    }
    auto end = config.find("\"", start + 1);
    if (end == std::string::npos) {
        return std::string();
    }

    if (end < start + 1) {
        return std::string();
    }
    std::string retStr = std::string(configStr + start + 1, end - start - 1);
    return retStr;
}
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string HapParser::ParseModuleName(std::shared_ptr<AbilityBase::Extractor> &extractor)
{
    if (extractor == nullptr) {
        return std::string();
    }
    std::unique_ptr<uint8_t[]> configBuf;
    size_t len;
    bool ret = extractor->ExtractToBufByName("config.json", configBuf, len);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get config data from ability");
        return std::string();
    }
    // parse config.json
    std::string mName = GetModuleName(reinterpret_cast<char *>(configBuf.get()), len);
    if (mName.size() == 0) {
        RESMGR_HILOGE(RESMGR_TAG, "parse moduleName from config.json error");
        return std::string();
    }
    return mName;
}
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string HapParser::GetIndexFilePath(std::shared_ptr<AbilityBase::Extractor> &extractor)
{
    std::string mName = ParseModuleName(extractor);
    std::string indexFilePath = std::string("assets/");
    indexFilePath.append(mName);
    indexFilePath.append("/resources.index");
    return indexFilePath;
}
#endif

bool HapParser::GetIndexDataFromHap(const char *path, std::unique_ptr<uint8_t[]> &buf, size_t &bufLen)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool isNewExtractor = false;
    std::shared_ptr<AbilityBase::Extractor> extractor = AbilityBase::ExtractorUtil::GetExtractor(path, isNewExtractor);
    if (extractor == nullptr) {
        return false;
    }
    std::string indexFilePath;
    if (extractor->IsStageModel()) {
        indexFilePath = "resources.index";
    } else {
        indexFilePath = GetIndexFilePath(extractor);
    }
    bool ret = extractor->ExtractToBufByName(indexFilePath, buf, bufLen);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get buf data indexFilePath, %{public}s", indexFilePath.c_str());
        return false;
    }
#endif
    return true;
}

bool HapParser::GetIndexDataFromIndex(const char *path, std::unique_ptr<uint8_t[]> &buf, size_t &bufLen)
{
    char outPath[PATH_MAX + 1] = {0};
    Utils::CanonicalizePath(path, outPath, PATH_MAX);
    std::ifstream inFile(outPath, std::ios::binary | std::ios::in);
    if (!inFile.good()) {
        return false;
    }
    inFile.seekg(0, std::ios::end);
    int fileLen = inFile.tellg();
    if (fileLen <= 0) {
        RESMGR_HILOGE(RESMGR_TAG, "file size is zero");
        inFile.close();
        return false;
    }
    bufLen = static_cast<size_t>(fileLen);
    buf = std::make_unique<uint8_t[]>(fileLen);
    if (buf == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "Error allocating memory");
        inFile.close();
        return false;
    }
    inFile.seekg(0, std::ios::beg);
    inFile.read(reinterpret_cast<char*>(buf.get()), fileLen);
    inFile.close();
    RESMGR_HILOGD(RESMGR_TAG, "extract success, bufLen:%d", fileLen);
    return true;
}

std::shared_ptr<HapResource> HapParser::GetHapResource(const char *path, bool isSystem, bool isOverlay)
{
    return nullptr;
}

std::string GetPath(const std::string &filePath, std::string &rawFilePath)
{
    std::string tempName = filePath;
    const std::string rawFileDirName = "rawfile/";
    if (tempName.length() <= rawFileDirName.length() ||
        (tempName.compare(0, rawFileDirName.length(), rawFileDirName) != 0)) {
        tempName = rawFileDirName + tempName;
    }
    rawFilePath.append(tempName);
    return rawFilePath;
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string GetRawFilePathFromFa(std::shared_ptr<AbilityBase::Extractor> &extractor,
    const std::string &filePath)
{
    std::string moduleName = HapParser::ParseModuleName(extractor);
    std::string rawFilePath("assets/");
    rawFilePath.append(moduleName);
    rawFilePath.append("/resources/");
    GetPath(filePath, rawFilePath);
    return rawFilePath;
}
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string GetRawFilePathFromStage(const std::string &filePath)
{
    std::string rawFilePath("resources/");
    GetPath(filePath, rawFilePath);
    return rawFilePath;
}
#endif

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string GetRawFilePath(std::shared_ptr<AbilityBase::Extractor> &extractor,
    const std::string &rawFileName)
{
    std::string rawfilePath;
    if (extractor->IsStageModel()) {
        rawfilePath = GetRawFilePathFromStage(rawFileName);
    } else {
        rawfilePath = GetRawFilePathFromFa(extractor, rawFileName);
    }
    return rawfilePath;
}
#endif

RState HapParser::ReadRawFileFromHap(const std::string &hapPath, const std::string &patchPath,
    const std::string &rawFileName, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool isNewExtractor = false;
    std::string tempPath = patchPath.empty() ? hapPath : patchPath;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(tempPath, isNewExtractor);
    if (extractor == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor hapPath, %{public}s", tempPath.c_str());
        return NOT_FOUND;
    }
    std::string rawfilePath = GetRawFilePath(extractor, rawFileName);
    if (!extractor->HasEntry(rawfilePath) && patchPath.empty()) {
        RESMGR_HILOGD(RESMGR_TAG,
            "the rawfile file %{public}s is not exist in %{public}s", rawfilePath.c_str(), tempPath.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
    if (!extractor->HasEntry(rawfilePath) && !patchPath.empty()) {
        extractor = AbilityBase::ExtractorUtil::GetExtractor(hapPath, isNewExtractor);
        if (extractor == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor hapPath, %{public}s", tempPath.c_str());
            return NOT_FOUND;
        }
        rawfilePath = GetRawFilePath(extractor, rawFileName);
        if (!extractor->HasEntry(rawfilePath)) {
            RESMGR_HILOGD(RESMGR_TAG,
                "the rawfile file %{public}s is not exist in %{public}s", rawfilePath.c_str(), tempPath.c_str());
            return ERROR_CODE_RES_PATH_INVALID;
        }
    }
    bool ret = extractor->ExtractToBufByName(rawfilePath, outValue, len);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get rawfile data rawfilePath, %{public}s, hapPath, %{public}s",
            rawfilePath.c_str(), tempPath.c_str());
        return NOT_FOUND;
    }
#endif
    return SUCCESS;
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
RState GetExtractor(const char *hapPath, const char *patchPath, char *outPath, const std::string &rawFileName,
                    std::string &rawfilePath, std::shared_ptr<AbilityBase::Extractor> &extractor)
{
    bool isNewExtractor = false;
    bool patchPathHasEntry = false;
    if (strlen(patchPath) != 0) {
        Utils::CanonicalizePath(patchPath, outPath, PATH_MAX);
        extractor = AbilityBase::ExtractorUtil::GetExtractor(outPath, isNewExtractor);
        if (extractor == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor in ReadRawFileDescriptor hapPath, %{public}s", outPath);
            return NOT_FOUND;
        }
        rawfilePath = GetRawFilePath(extractor, rawFileName);
        if (extractor->HasEntry(rawfilePath)) {
            patchPathHasEntry = true;
        }
    }
    if (!patchPathHasEntry) {
        Utils::CanonicalizePath(hapPath, outPath, PATH_MAX);
        extractor = AbilityBase::ExtractorUtil::GetExtractor(outPath, isNewExtractor);
        if (extractor == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor in ReadRawFileDescriptor hapPath, %{public}s", outPath);
            return NOT_FOUND;
        }
        rawfilePath = GetRawFilePath(extractor, rawFileName);
        if (!extractor->HasEntry(rawfilePath)) {
            RESMGR_HILOGD(RESMGR_TAG,
                "the rawfile file %{public}s is not exist in %{public}s", rawfilePath.c_str(), outPath);
            return ERROR_CODE_RES_PATH_INVALID;
        }
    }
    return SUCCESS;
}
#endif

RState HapParser::ReadRawFileDescriptor(const char *hapPath, const char *patchPath, const std::string &rawFileName,
    ResourceManager::RawFileDescriptor &descriptor)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    char outPath[PATH_MAX + 1] = {0};
    std::string rawfilePath = "";
    std::shared_ptr<AbilityBase::Extractor> extractor = nullptr;
    RState resoult = GetExtractor(hapPath, patchPath, outPath, rawFileName, rawfilePath, extractor);
    if (resoult != SUCCESS) {
        return resoult;
    }
    AbilityBase::FileInfo fileInfo;
    bool ret = extractor->GetFileInfo(rawfilePath, fileInfo);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get rawFileDescriptor rawfilePath, %{public}s", rawfilePath.c_str());
        return NOT_FOUND;
    }
    int zipFd = open(outPath, O_RDONLY);
    if (zipFd < 0) {
        RESMGR_HILOGE(RESMGR_TAG, "failed open file %{public}s", outPath);
        return NOT_FOUND;
    }
    descriptor.offset = static_cast<long>(fileInfo.offset);
    descriptor.length = static_cast<long>(fileInfo.length);
    descriptor.fd = zipFd;
#endif
    return SUCCESS;
}

RState HapParser::GetRawFileList(const std::string &hapPath, const std::string &rawDirPath,
    std::set<std::string>& fileSet)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool isNewExtractor = false;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(hapPath, isNewExtractor);
    if (extractor == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG,
            "failed to get extractor from ability in GetRawFileList hapPath, %{public}s", hapPath.c_str());
        return NOT_FOUND;
    }
    std::string rawfilePath = GetRawFilePath(extractor, rawDirPath);
    if (!extractor->IsDirExist(rawfilePath)) {
        RESMGR_HILOGD(RESMGR_TAG,
            "the rawfile dir %{public}s is not exist in %{public}s", rawfilePath.c_str(), hapPath.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
    bool ret = extractor->GetFileList(rawfilePath, fileSet);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get fileSet from ability rawfilePath, %{public}s", rawfilePath.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
#endif
    return SUCCESS;
}

RState HapParser::GetRawFileListUnCompressed(const std::string &indexPath, const std::string &rawDirPath,
    std::vector<std::string>& fileList)
{
#ifdef __WINNT__
    char seperator = '\\';
#else
    char seperator = '/';
#endif
    auto pos = indexPath.rfind(seperator);
    if (pos == std::string::npos) {
        return ERROR_CODE_RES_PATH_INVALID;
    }
    std::string rawFilePath = indexPath.substr(0, pos) + "/resources/";
    GetPath(rawDirPath, rawFilePath);
    return Utils::GetFiles(rawFilePath, fileList);
}

std::shared_ptr<ResConfigImpl> HapParser::BuildResConfig(ResConfigKey *configKey)
{
    if (configKey == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "configKey is null");
        return nullptr;
    }
    auto resConfig = std::make_shared<ResConfigImpl>();
    if (resConfig == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResConfigImpl failed when BuildResConfig");
        return nullptr;
    }
    resConfig->SetDeviceType(configKey->deviceType);
    resConfig->SetDirection(configKey->direction);
    resConfig->SetColorMode(configKey->colorMode);
    resConfig->SetMcc(configKey->mcc);
    resConfig->SetMnc(configKey->mnc);
    resConfig->SetInputDevice(configKey->inputDevice);
    resConfig->SetScreenDensity((configKey->screenDensity) / Utils::DPI_BASE);
    RState r = resConfig->SetLocaleInfo(configKey->language, configKey->script, configKey->region);
    if (r != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG,
            "error set locale,lang %s,script %s,region %s", configKey->language, configKey->script, configKey->region);
    }

    return resConfig;
}

void HapParser::GetKeyParamsLocales(std::shared_ptr<KeyParam> kp, std::string &locale, bool &isLocale)
{
    KeyType keyType = kp->type_;
    if (keyType == KeyType::MCC || keyType == KeyType::MNC) {
        return;
    }
    if (keyType == KeyType::LANGUAGES) {
        locale = kp->GetStr();
        isLocale = true;
        return;
    }
    if (keyType == KeyType::SCRIPT) {
        locale.append("-");
        locale.append(kp->GetStr());
        return;
    }
    if (keyType == KeyType::REGION) {
        locale.append("-");
        locale.append(kp->GetStr());
    }
}

DeviceType HapParser::GetDeviceType(uint32_t value)
{
    DeviceType deviceType = DEVICE_NOT_SET;
    if (value == DEVICE_CAR) {
        deviceType = DEVICE_CAR;
    } else if (value == DEVICE_PAD) {
        deviceType = DEVICE_PAD;
    } else if (value == DEVICE_PHONE) {
        deviceType = DEVICE_PHONE;
    } else if (value == DEVICE_TABLET) {
        deviceType = DEVICE_TABLET;
    } else if (value == DEVICE_TV) {
        deviceType = DEVICE_TV;
    } else if (value == DEVICE_WEARABLE) {
        deviceType = DEVICE_WEARABLE;
    } else if (value == DEVICE_TWOINONE) {
        deviceType = DEVICE_TWOINONE;
    }
    return deviceType;
}

uint32_t HapParser::GetMcc(uint32_t value)
{
    return value;
}

uint32_t HapParser::GetMnc(uint32_t value)
{
    return value;
}

ColorMode HapParser::GetColorMode(uint32_t value)
{
    ColorMode colorMode = COLOR_MODE_NOT_SET;
    if (value == DARK) {
        colorMode = DARK;
    } else {
        colorMode = LIGHT;
    }
    return colorMode;
}

InputDevice HapParser::GetInputDevice(uint32_t value)
{
    InputDevice inputDevice = INPUTDEVICE_NOT_SET;
    if (value == INPUTDEVICE_POINTINGDEVICE) {
        inputDevice = INPUTDEVICE_POINTINGDEVICE;
    }
    return inputDevice;
}

ScreenDensity HapParser::GetScreenDensity(uint32_t value)
{
    ScreenDensity screenDensity = SCREEN_DENSITY_NOT_SET;
    if (value == SCREEN_DENSITY_SDPI) {
        screenDensity = SCREEN_DENSITY_SDPI;
    } else if (value == SCREEN_DENSITY_MDPI) {
        screenDensity = SCREEN_DENSITY_MDPI;
    } else if (value == SCREEN_DENSITY_LDPI) {
        screenDensity = SCREEN_DENSITY_LDPI;
    } else if (value == SCREEN_DENSITY_XLDPI) {
        screenDensity = SCREEN_DENSITY_XLDPI;
    } else if (value == SCREEN_DENSITY_XXLDPI) {
        screenDensity = SCREEN_DENSITY_XXLDPI;
    } else if (value == SCREEN_DENSITY_XXXLDPI) {
        screenDensity = SCREEN_DENSITY_XXXLDPI;
    }
    return screenDensity;
}

RState HapParser::IsRawDirFromHap(const char *hapPath, const std::string &pathName, bool &outValue)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    if (pathName.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "the rawfile path is empty");
        return ERROR_CODE_RES_PATH_INVALID;
    }
    bool isNewExtractor = false;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(hapPath, isNewExtractor);
    if (extractor == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor hapPath, %{public}s", hapPath);
        return NOT_FOUND;
    }
    std::string rawPath = GetRawFilePath(extractor, pathName);
    if (extractor->HasEntry(rawPath)) {
        outValue = false;
    } else if (extractor->IsDirExist(rawPath)) {
        outValue = true;
    } else {
        RESMGR_HILOGD(RESMGR_TAG, "the rawfile file %{public}s is not exist in %{public}s", rawPath.c_str(), hapPath);
        return ERROR_CODE_RES_PATH_INVALID;
    }
#endif
    return SUCCESS;
}

RState HapParser::IsRawDirUnCompressed(const std::string &pathName, bool &outValue)
{
    char outPath[PATH_MAX + 1] = {0};
    Utils::CanonicalizePath(pathName.c_str(), outPath, PATH_MAX);
    struct stat fileStat {};
    if (stat(outPath, &fileStat) != 0) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get rawfile file info, %{public}s", outPath);
        return ERROR_CODE_RES_PATH_INVALID;
    }
    if ((fileStat.st_mode & S_IFDIR)) {
        outValue = true;
    } else if ((fileStat.st_mode & S_IFREG)) {
        outValue = false;
    } else {
        RESMGR_HILOGE(RESMGR_TAG, "the rawfile file %{public}s is not exist", outPath);
        return ERROR_CODE_RES_PATH_INVALID;
    }
    return SUCCESS;
}

std::shared_ptr<ResConfigImpl> HapParser::CreateResConfigFromKeyParams(
    const std::vector<std::shared_ptr<KeyParam>> &keyParams)
{
    auto resConfig = std::make_shared<ResConfigImpl>();
    if (resConfig == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResConfigImpl failed when CreateResConfigFromKeyParams");
        return nullptr;
    }
    size_t len = keyParams.size();
    // default path
    if (len == 0) {
        resConfig->SetColorMode(COLOR_MODE_NOT_SET);
        return resConfig;
    }
    size_t i = 0;
    ResConfigKey configKey;
    for (i = 0; i < len; ++i) {
        const std::shared_ptr<KeyParam> kp = keyParams.at(i);
        if (kp->type_ == LANGUAGES) {
            configKey.language = kp->GetStr().c_str();
        } else if (kp->type_ == REGION) {
            configKey.region = kp->GetStr().c_str();
        } else if (kp->type_ == SCRIPT) {
            configKey.script = kp->GetStr().c_str();
        } else if (kp->type_ == SCREEN_DENSITY) {
            configKey.screenDensity = GetScreenDensity(kp->value_);
        } else if (kp->type_ == DEVICETYPE) {
            configKey.deviceType = GetDeviceType(kp->value_);
        } else if (kp->type_ == DIRECTION) {
            if (kp->value_ == 0) {
                configKey.direction = DIRECTION_VERTICAL;
            } else {
                configKey.direction = DIRECTION_HORIZONTAL;
            }
        } else if (kp->type_ == INPUTDEVICE) {
            configKey.inputDevice = GetInputDevice(kp->value_);
        } else if (kp->type_ == COLORMODE) {
            configKey.colorMode = GetColorMode(kp->value_);
        } else if (kp->type_ == MCC) {
            configKey.mcc = GetMcc(kp->value_);
        } else if (kp->type_ == MNC) {
            configKey.mnc = GetMnc(kp->value_);
        }
    }
    return BuildResConfig(&configKey);
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
