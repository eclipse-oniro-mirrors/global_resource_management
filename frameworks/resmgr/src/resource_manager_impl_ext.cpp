/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "resource_manager_impl_ext.h"

#include "accesstoken_kit.h"

#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "utils/utils.h"
#include "res_config.h"
#include "securec.h"
#include "tokenid_kit.h"
#include "utils/common.h"
#include "utils/string_utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
constexpr uint32_t DEFAULT_RETURN_VALUE = 0;
#if defined(RESMGR_SUPPORT_LINUX)
constexpr int LINUX_TYPE = 2;
#endif

bool VerifyCallingPermission(const std::string &permissionName)
{
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    RESMGR_HILOGI(RESMGR_TAG, "VerifyCallingPermission permission %{public}s, callerToken : %{public}u",
        permissionName.c_str(), callerToken);
    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName) ==
        Security::AccessToken::PermissionState::PERMISSION_DENIED) {
        RESMGR_HILOGE(RESMGR_TAG, "permission %{public}s denied, callerToken : %{public}u",
            permissionName.c_str(), callerToken);
        return false;
    }
    return true;
}

ResourceManagerImplExt::ResourceManagerImplExt()
{}

ResourceManagerImplExt::~ResourceManagerImplExt()
{}

bool ResourceManagerImplExt::Init(int32_t appType, std::string bundleName)
{
    if (!VerifyCallingPermission("ohos.permission.GET_BUNDLE_INFO_PRIVILEGED")) {
        RESMGR_HILOGE(RESMGR_TAG, "check ohos.permission.GET_BUNDLE_INFO_PRIVILEGED permission fail");
        return false;
    }
    client_ = std::make_shared<ANCO::ResourceManagerBrokerClient>();
    if (client_ == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "get broker client failed in ResourceManagerImplExt::Init");
        return false;
    }
    client_->init(appType, bundleName);
    return true;
}

RState ResourceManagerImplExt::GetStringById(uint32_t id, std::string &outValue)
{
    if (client_ != nullptr) {
        return static_cast<RState>(client_->GetString(id, outValue));
    }
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetStringFormatById(uint32_t id, std::string &outValue,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    RState state = GetStringById(id, outValue);
    if (state != SUCCESS) {
        return state;
    }
    ResConfigImpl resConfig;
    GetResConfig(resConfig);
    if (!ReplacePlaceholderWithParams(outValue, resConfig, jsParams)) {
        return ERROR_CODE_RES_NAME_FORMAT_ERROR;
    }
    return SUCCESS;
}

RState ResourceManagerImplExt::GetMediaDataById(uint32_t id, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    if (client_ != nullptr) {
        return static_cast<RState>(client_->GetMedia(id, len, outValue, density));
    }
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetMediaBase64DataById(uint32_t id, std::string &outValue, uint32_t density)
{
    size_t len;
    std::unique_ptr<uint8_t[]> tempData;
    RState state = GetMediaDataById(id, len, tempData, density);
    if (state != SUCCESS) {
        return state;
    }
    std::string type = "png";
    return Utils::EncodeBase64(tempData, len, type, outValue);
}

RState ResourceManagerImplExt::GetDrawableInfoById(uint32_t id, std::string &type, size_t &len,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    type = "png";
    return GetMediaDataById(id, len, outValue, density);
}

bool ResourceManagerImplExt::AddResource(const char *path, const uint32_t &selectedTypes)
{
    return false;
}

bool ResourceManagerImplExt::AddResource(const std::string &path, const std::vector<std::string> &overlayPaths)
{
    return false;
}

bool ResourceManagerImplExt::RemoveResource(const std::string &path, const std::vector<std::string> &overlayPaths)
{
    return false;
}

bool ResourceManagerImplExt::AddAppOverlay(const std::string &path)
{
    return false;
}

bool ResourceManagerImplExt::RemoveAppOverlay(const std::string &path)
{
    return false;
}

RState ResourceManagerImplExt::UpdateResConfig(ResConfig &resConfig, bool isUpdateTheme)
{
    return ERROR;
}

void ResourceManagerImplExt::GetResConfig(ResConfig &resConfig)
{}

RState ResourceManagerImplExt::GetStringByName(const char *name, std::string &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetStringFormatById(std::string &outValue, uint32_t id, ...)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetStringFormatByName(std::string &outValue, const char *name, ...)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetStringArrayById(uint32_t id, std::vector<std::string> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetStringArrayByName(const char *name, std::vector<std::string> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetPatternById(uint32_t id, std::map<std::string, std::string> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetPatternByName(const char *name, std::map<std::string, std::string> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetPluralStringById(uint32_t id, int quantity, std::string &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetPluralStringByName(const char *name, int quantity, std::string &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetPluralStringByNameFormat(std::string &outValue, const char *name, int quantity, ...)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetThemeById(uint32_t id, std::map<std::string, std::string> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetThemeByName(const char *name, std::map<std::string, std::string> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetBooleanById(uint32_t id, bool &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetBooleanByName(const char *name, bool &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetIntegerById(uint32_t id, int &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetIntegerByName(const char *name, int &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetFloatById(uint32_t id, float &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetFloatById(uint32_t id, float &outValue, std::string &unit)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetFloatByName(const char *name, float &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetFloatByName(const char *name, float &outValue, std::string &unit)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetIntArrayById(uint32_t id, std::vector<int> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetIntArrayByName(const char *name, std::vector<int> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetColorById(uint32_t id, uint32_t &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetColorByName(const char *name, uint32_t &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetProfileById(uint32_t id, std::string &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetProfileByName(const char *name, std::string &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetMediaById(uint32_t id, std::string &outValue, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetMediaByName(const char *name, std::string &outValue, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetRawFilePathByName(const std::string &name, std::string &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetRawFileDescriptor(const std::string &name, RawFileDescriptor &descriptor)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

RState ResourceManagerImplExt::CloseRawFileDescriptor(const std::string &name)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

RState ResourceManagerImplExt::GetMediaDataByName(
    const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetMediaBase64DataByName(const char *name, std::string &outValue, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetProfileDataById(uint32_t id, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetProfileDataByName(
    const char *name, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetRawFileFromHap(
    const std::string &rawFileName, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

RState ResourceManagerImplExt::GetRawFileDescriptorFromHap(
    const std::string &rawFileName, RawFileDescriptor &descriptor)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

RState ResourceManagerImplExt::IsLoadHap(std::string &hapPath)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

RState ResourceManagerImplExt::GetRawFileList(const std::string &rawDirPath, std::vector<std::string> &rawfileList)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

RState ResourceManagerImplExt::GetDrawableInfoByName(
    const char *name, std::string &type, size_t &len, std::unique_ptr<uint8_t[]> &outValue, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetStringFormatByName(const char *name, std::string &outValue,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

uint32_t ResourceManagerImplExt::GetResourceLimitKeys()
{
    return DEFAULT_RETURN_VALUE;
}

RState ResourceManagerImplExt::GetRawFdNdkFromHap(const std::string &name, RawFileDescriptor &descriptor)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

RState ResourceManagerImplExt::GetResId(const std::string &resTypeName, uint32_t &resId)
{
    return NOT_FOUND;
}

void ResourceManagerImplExt::GetLocales(std::vector<std::string> &outValue, bool includeSystem) {}

RState ResourceManagerImplExt::GetDrawableInfoById(uint32_t id,
    std::tuple<std::string, size_t, std::string> &drawableInfo,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density)
{
    size_t len = 0;
    RState state = GetMediaDataById(id, len, outValue, density);
    std::string type = "png";
    std::string themeMask = "";
    drawableInfo = std::make_tuple(type, len, themeMask);
    return state;
}

RState ResourceManagerImplExt::GetDrawableInfoByName(const char *name,
    std::tuple<std::string, size_t, std::string> &drawableInfo,
    std::unique_ptr<uint8_t[]> &outValue, uint32_t iconType, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetSymbolById(uint32_t id, uint32_t &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetSymbolByName(const char *name, uint32_t &outValue)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetThemeIcons(uint32_t resId,
    std::pair<std::unique_ptr<uint8_t[]>, size_t> &foregroundInfo,
    std::pair<std::unique_ptr<uint8_t[]>, size_t> &backgroundInfo, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

std::string ResourceManagerImplExt::GetThemeMask()
{
    return "";
}

RState ResourceManagerImplExt::IsRawDirFromHap(const std::string &pathName, bool &outValue)
{
    return ERROR_CODE_RES_PATH_INVALID;
}

std::shared_ptr<ResourceManager> ResourceManagerImplExt::GetOverrideResourceManager(
    std::shared_ptr<ResConfig> overrideResConfig)
{
    return nullptr;
}

RState ResourceManagerImplExt::UpdateOverrideResConfig(ResConfig &resConfig)
{
    return ERROR;
}

void ResourceManagerImplExt::GetOverrideResConfig(ResConfig &resConfig) {}

RState ResourceManagerImplExt::GetDynamicIcon(const std::string &resName,
    std::pair<std::unique_ptr<uint8_t[]>, size_t> &iconInfo, uint32_t density)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetStringFormatById(std::string &outValue, uint32_t id, va_list args)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetStringFormatByName(std::string &outValue, const char *name, va_list args)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

RState ResourceManagerImplExt::GetFormatPluralStringById(std::string &outValue, uint32_t id, int quantity,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_ID;
}

RState ResourceManagerImplExt::GetFormatPluralStringByName(std::string &outValue, const char *name, int quantity,
    std::vector<std::tuple<ResourceManager::NapiValueType, std::string>> &jsParams)
{
    return ERROR_CODE_RES_NOT_FOUND_BY_NAME;
}

bool ResourceManagerImplExt::AddPatchResource(const char *path, const char *patchPath)
{
    return false;
}
}  // namespace Resource
}  // namespace Global
}  // namespace OHOS
