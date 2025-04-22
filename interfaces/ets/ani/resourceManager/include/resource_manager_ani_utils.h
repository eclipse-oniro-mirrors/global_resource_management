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

#include <string>
#include <unordered_map>

#include "ani.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResourceManagerAniUtils {
public:
    static void AniThrow(ani_env *env, int32_t errCode);
private:
    static const std::unordered_map<int32_t, std::string> ErrorCodeToMsg;

    static std::string FindErrMsg(int32_t errCode);
    static ani_object WrapStsError(ani_env *env, const std::string &msg);
    static ani_object CreateError(ani_env *env, ani_int code, const std::string &msg);
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif