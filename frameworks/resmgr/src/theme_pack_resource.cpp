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
#include "theme_pack_resource.h"

#include "hilog_wrapper.h"
#include "utils/utils.h"
#include <dirent.h>
#include <tuple>
namespace OHOS {
namespace Global {
namespace Resource {
constexpr int FIRST_ELEMENT = 0;
constexpr int SECOND_ELEMENT = 1;
constexpr int THIRED_ELEMENT = 2;
const std::string DYNAMIC_ICON = "dynamic_icons";
ThemeResource *ThemeResource::themeRes = nullptr;
ThemeResource::ThemeResource(std::string path) : themePath_(path)
{
    themeRes = this;
}

ThemeResource::~ThemeResource()
{
    themeValueVec_.clear();
    iconValues_.clear();
}

ThemeResource::ThemeQualifierValue::~ThemeQualifierValue()
{}

ThemeResource::ThemeValue::~ThemeValue()
{
    limitPaths_.clear();
}

ThemeResource::ThemeQualifierValue::ThemeQualifierValue(const ThemeKey themeKey,
    std::shared_ptr<ThemeConfig> themeConfig, const std::string &value) : themeKey_(themeKey),
    themeConfig_(themeConfig), resValue_(value)
{}

std::unordered_map<std::string, ResType> themeResTypeMap {
    {"color", COLOR},
    {"float", FLOAT},
    {"media", MEDIA},
};

std::string GetResKey(const std::string &jsonPath)
{
    auto lastIndex = jsonPath.rfind('/');
    if (lastIndex < 1) {
        return std::string("");
    }
    auto secondLastIndex = jsonPath.rfind('/', lastIndex - 1);
    if (secondLastIndex < 1) {
        return std::string("");
    }
    auto thirdLastIndex = jsonPath.rfind('/', secondLastIndex - 1);
    if (lastIndex == std::string::npos || secondLastIndex == std::string::npos
        || thirdLastIndex == std::string::npos) {
        return std::string("");
    }
    if (secondLastIndex < thirdLastIndex + 1) {
        return std::string("");
    }
    std::string res = jsonPath.substr(thirdLastIndex + 1, secondLastIndex - thirdLastIndex - 1);
    return res;
}

std::shared_ptr<ThemeConfig> GetThemeConfig(const std::string &iconPath)
{
    auto themeConfig = std::make_shared<ThemeConfig>();
    std::string resKey = GetResKey(iconPath);
    if (resKey == "dark") {
        themeConfig->SetThemeColorMode(ColorMode::DARK);
    } else if (resKey == "horizontal") {
        themeConfig->SetThemeDirection(Direction::DIRECTION_HORIZONTAL);
    } else if (resKey == "horizontal-dark") {
        themeConfig->SetThemeDirection(Direction::DIRECTION_HORIZONTAL);
        themeConfig->SetThemeColorMode(ColorMode::DARK);
    } else {
        // default
    }
    return themeConfig;
}

ResType GetResType(const std::string &resTypeStr)
{
    ResType resType = ResType::VALUES;
    if (themeResTypeMap.find(resTypeStr) != themeResTypeMap.end()) {
        resType = themeResTypeMap[resTypeStr];
        return resType;
    }
    return ResType::VALUES;
}

void ThemeResource::InitThemeRes(std::pair<std::string, std::string> bundleInfo, cJSON *root,
    std::shared_ptr<ThemeConfig> themeConfig, const std::string &resTypeStr)
{
    if (root == nullptr) {
        HILOG_WARN("The json file has not resType = %{public}s", resTypeStr.c_str());
        return;
    }

    ResType resType = GetResType(resTypeStr);
    if (root->type == cJSON_Array) {
        cJSON *childValue = root->child;
        while (childValue != nullptr) {
            cJSON *name = cJSON_GetObjectItem(childValue, "name");
            if (name == nullptr || !cJSON_IsString(name)) {
                HILOG_WARN("The resource name is not exist in childValue");
                return;
            }
            cJSON *value = cJSON_GetObjectItem(childValue, "value");
            if (value == nullptr || !cJSON_IsString(value)) {
                HILOG_WARN("The resource value is not exist in childValue");
                return;
            }
            auto themeValue = std::make_shared<ThemeValue>();
            ThemeKey themeKey = ThemeKey(bundleInfo.first, bundleInfo.second, resType, name->valuestring);
            auto themeQualifierValue = std::make_shared<ThemeQualifierValue>(themeKey, themeConfig,
                value->valuestring);
            themeValue->AddThemeLimitPath(themeQualifierValue);
            themeValueVec_.emplace_back(std::make_tuple(resType, name->valuestring, themeValue));
            childValue = childValue->next;
        }
    }
    return;
}

void ThemeResource::ParseJson(const std::string &bundleName, const std::string &moduleName,
    const std::string &jsonPath)
{
    auto len = 0;
    FILE* pf = std::fopen(jsonPath.c_str(), "r");
    if (pf == nullptr) {
        HILOG_ERROR("fopen failed in ParseJson");
        return;
    }
    std::fseek(pf, 0, SEEK_END);
    len = ftell(pf);
    std::fseek(pf, 0, SEEK_SET);
    char *jsonData = (char *)malloc(len + 1);
    if (jsonData == nullptr) {
        HILOG_ERROR("failed malloc in ParseJson");
        if (pf != nullptr) {
            fclose(pf);
            pf = nullptr;
        }
        return;
    }
    std::fread(jsonData, len, 1, pf);
    jsonData[len] = '\0';
    auto themeConfig = GetThemeConfig(jsonPath);
    cJSON *jsonValue = cJSON_Parse(jsonData);
    std::pair<std::string, std::string> bundleInfo(bundleName, moduleName);
    cJSON *floatRoot = cJSON_GetObjectItem(jsonValue, "float");
    InitThemeRes(bundleInfo, floatRoot, themeConfig, "float");

    cJSON *colorRoot = cJSON_GetObjectItem(jsonValue, "color");
    InitThemeRes(bundleInfo, colorRoot, themeConfig, "color");
    free(jsonData);
    jsonData = nullptr;
    if (pf != nullptr) {
        fclose(pf);
        pf = nullptr;
    }
    cJSON_Delete(jsonValue);
    return;
}

void ThemeResource::ParseIcon(const std::string &bundleName, const std::string &moduleName,
    const std::string &iconPath)
{
    auto themeConfig = GetThemeConfig(iconPath);
    auto pos1 = iconPath.rfind('.');
    auto pos2 = iconPath.rfind('/');
    if (pos1 == std::string::npos || pos2 == std::string::npos) {
        HILOG_ERROR("invalid iconPath = %{public}s in ParseIcon", iconPath.c_str());
        return;
    }
    if (pos1 < pos2 + 1) {
        return;
    }
    std::string iconName = iconPath.substr(pos2 + 1, pos1 - pos2 - 1);
    auto themeValue = std::make_shared<ThemeValue>();
    ThemeKey themeKey = ThemeKey(bundleName, moduleName, ResType::MEDIA, iconName);
    auto themeQualifierValue = std::make_shared<ThemeQualifierValue>(themeKey, themeConfig, iconPath);
    themeValue->AddThemeLimitPath(themeQualifierValue);
    themeValueVec_.emplace_back(std::make_tuple(ResType::MEDIA, iconName, themeValue));
    return;
}

std::vector<std::shared_ptr<ThemeResource::ThemeValue> > ThemeResource::GetThemeValues(
    const std::pair<std::string, std::string> &bundInfo, const ResType &resType, const std::string &name)
{
    std::vector<std::shared_ptr<ThemeResource::ThemeValue> > result;
    if (themeValueVec_.empty()) {
        return result;
    }

    for (const auto &themeValue : themeValueVec_) {
        ResType type = std::get<FIRST_ELEMENT>(themeValue);
        std::string resName = std::get<SECOND_ELEMENT>(themeValue);
        if (type == resType && resName == name) {
            result.emplace_back(std::get<THIRED_ELEMENT>(themeValue));
        }
    }
    return result;
}

std::vector<std::string> GetFiles(const std::string &strCurrentDir)
{
    std::vector<std::string> vFiles;
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    DIR *dir;
    struct dirent *pDir;
    if ((dir = opendir(strCurrentDir.c_str())) == nullptr) {
        HILOG_ERROR("opendir failed strCurrentDir = %{public}s", strCurrentDir.c_str());
        return vFiles;
    }
    while ((pDir = readdir(dir)) != nullptr) {
        if (strcmp(pDir->d_name, ".") == 0 || strcmp(pDir->d_name, "..") == 0) {
            continue;
        } else if (pDir->d_type == 8) { // 8 means the file
            vFiles.emplace_back(strCurrentDir + "/" + pDir->d_name);
        } else if (pDir->d_type == 4) { // 4 means the dir
            std::string strNextDir = strCurrentDir + "/" + pDir->d_name;
            std::vector<std::string> temp = GetFiles(strNextDir);
            vFiles.insert(vFiles.end(), temp.begin(), temp.end());
        } else {
            continue;
        }
    }
    closedir(dir);
#endif
    return vFiles;
}

std::tuple<std::string, std::string> GetBundleInfo(const std::string& rootDir, const std::string& path)
{
    if (rootDir.empty() || path.empty()) {
        return std::tuple<std::string, std::string>("", "");
    }
    size_t len = rootDir.size();
    auto pos = rootDir.rfind('/');
    if (pos == std::string::npos) {
        HILOG_ERROR("invalid rootDir = %{public}s in GetBundleInfo", rootDir.c_str());
        return std::tuple<std::string, std::string>("", "");
    }
    std::string bundleName = rootDir.substr(pos + 1);
    auto pos2 = path.find('/', len + 1);
    if (pos2 == std::string::npos) {
        HILOG_ERROR("invalid path = %{public}s in GetBundleInfo", path.c_str());
        return std::tuple<std::string, std::string>("", "");
    }
    if (pos2 < len + 1) {
        return std::tuple<std::string, std::string>("", "");
    }
    std::string moduleName = path.substr(len + 1, pos2 - len -1);
    std::tuple<std::string, std::string> bundleInfoTuple(bundleName, moduleName);
    return bundleInfoTuple;
}

const std::shared_ptr<ThemeResource> ThemeResource::LoadThemeResource(const std::string& rootDir)
{
    if (rootDir.empty()) {
        HILOG_ERROR("Invalid rootDir in LoadThemeResource = %{public}s", rootDir.c_str());
        return nullptr;
    }
    auto themeResource = std::make_shared<ThemeResource>(rootDir);
    std::vector<std::string> resPaths = GetFiles(rootDir);
    for (const auto &path : resPaths) {
        auto bundleInfo = GetBundleInfo(rootDir, path);
        auto pos = path.rfind('.');
        if (pos == std::string::npos) {
            HILOG_ERROR("invalid resPath = %{public}s", path.c_str());
            continue;
        }
        std::string tail = path.substr(pos + 1);
        if (tail == "json") {
            themeRes->ParseJson(std::get<FIRST_ELEMENT>(bundleInfo), std::get<SECOND_ELEMENT>(bundleInfo), path);
        } else {
            themeRes->ParseIcon(std::get<FIRST_ELEMENT>(bundleInfo), std::get<SECOND_ELEMENT>(bundleInfo), path);
        }
    }
    return themeResource;
}

std::string ThemeResource::GetThemeResBundleName(const std::string &themePath)
{
    auto pos = themePath.rfind('/');
    if (pos == std::string::npos) {
        HILOG_ERROR("invalid themePath = %{public}s", themePath.c_str());
        return std::string("");
    }
    std::string bundleName = themePath.substr(pos + 1);
    return bundleName;
}

std::string GetIconsBundleName(const std::string& iconPath)
{
    if (iconPath.empty()) {
        return std::string("");
    }
    auto pos = iconPath.rfind('/');
    if (pos == std::string::npos) {
        HILOG_ERROR("invalid iconPath = %{public}s in GetIconsBundleName", iconPath.c_str());
        return std::string("");
    }
    return iconPath.substr(pos + 1);
}

const std::shared_ptr<ThemeResource> ThemeResource::LoadThemeIconResource(const std::string& iconPath)
{
    if (iconPath.empty()) {
        return nullptr;
    }
    auto themeResource = std::make_shared<ThemeResource>(iconPath);
    std::string bundleName = GetIconsBundleName(iconPath);
    std::vector<std::string> resPaths = GetFiles(iconPath);
    for (const auto &path : resPaths) {
        auto pos1 = path.rfind('.');
        auto pos2 = path.rfind('/');
        if (pos1 == std::string::npos || pos2 == std::string::npos || pos1 < pos2 + 1) {
            HILOG_ERROR("invalid path = %{public}s in LoadThemeIconResource", path.c_str());
            continue;
        }
        std::string iconName = path.substr(pos2 + 1, pos1 - pos2 - 1);
        if (path.find(DYNAMIC_ICON) != std::string::npos) {
            auto pos3 = path.find('/', iconPath.length() + 1);
            if (pos3 == std::string::npos || pos3 < iconPath.length() + 1) {
                continue;
            }
            std::string dynamicBundle = path.substr(iconPath.length() + 1, pos3 - iconPath.length() - 1);
            ThemeKey themeKey = ThemeKey(bundleName, dynamicBundle, ResType::MEDIA, iconName);
            themeRes->iconValues_.emplace_back(std::make_pair(themeKey, path));
        } else {
            ThemeKey themeKey = ThemeKey(bundleName, "", ResType::MEDIA, iconName);
            themeRes->iconValues_.emplace_back(std::make_pair(themeKey, path));
        }
    }
    return themeResource;
}

const std::string ThemeResource::GetThemeAppIcon(const std::pair<std::string, std::string> &bundleInfo,
    const std::string &iconName)
{
    for (size_t i = 0; i < iconValues_.size(); i++) {
        if (iconValues_[i].first.bundleName == DYNAMIC_ICON && iconValues_[i].first.moduleName != bundleInfo.first) {
            continue;
        }
        if (iconValues_[i].first.bundleName != DYNAMIC_ICON && iconValues_[i].first.bundleName != bundleInfo.first) {
            continue;
        }
        if (iconName == iconValues_[i].first.resName) {
            return iconValues_[i].second;
        }
    }
    return std::string("");
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
