/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RESOURCE_MANAGER_ANI_UTILS_H
#define RESOURCE_MANAGER_ANI_UTILS_H

#include <memory>
#include <string>
#include <vector>

#include "ani.h"

#include "resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
using FormatArgs = std::vector<std::tuple<ResourceManager::NapiValueType, std::string>>;
class AniUtils {
public:
    static void AniThrow(ani_env *env, int32_t errCode);

    static std::shared_ptr<ResourceManager> GetResourceManager(ani_env* env, ani_object jsResMgr);
    static bool InitAniParameters(ani_env *env, ani_object args, FormatArgs &params);
    static std::string AniStrToString(ani_env *env, ani_string aniStr);
    static RState ConfigurationToResConfig(ani_env *env,
        ani_object configuration, std::shared_ptr<ResConfig> &resConfig);
    static bool GetOptionalInt(ani_env *env, ani_object optionalInt, int& result);

    static ani_string CreateAniString(ani_env *env, const std::string &str);
    static ani_object CreateConfig(ani_env* env, std::unique_ptr<ResConfig> &cfg);
    static ani_object CreateAniArray(ani_env *env, const std::vector<std::string> &strs);
    static ani_object CreateAniUint8Array(ani_env* env, std::unique_ptr<uint8_t[]> &mediaData, size_t len);
    static ani_object CreateDeviceCapability(ani_env *env, std::unique_ptr<ResConfig> &cfg);
    static ani_object CreateAniRawFileDescriptor(ani_env *env, const ResourceManager::RawFileDescriptor& descriptor);

private:
    static std::string FindErrMsg(int32_t errCode);
    static void ThrowAniError(ani_env *env, ani_int code, const std::string &message);
    static bool GetEnumParamOfConfig(ani_env* env, std::shared_ptr<ResConfig> configPtr, ani_object configuration);
    static bool GetEnumMember(ani_env *env, ani_object options, const std::string name, int& member);
    static bool GetLocaleOfConfig(ani_env* env, std::shared_ptr<ResConfig> configPtr, ani_object configuration);
    static bool SetEnumMember(ani_env *env, ani_object obj, const char* memberName, const char* enumName,
        const int index);
    static std::string GetLocale(std::unique_ptr<ResConfig> &cfg);
    static int GetScreenDensityIndex(ScreenDensity value);
    static bool GetNumberMember(ani_env *env, ani_object options, const std::string name, int& value);
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
