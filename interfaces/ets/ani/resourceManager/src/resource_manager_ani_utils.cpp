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

#include "resource_manager_ani_utils.h"

#include "hilog_wrapper.h"
#include "rstate.h"

using namespace OHOS;
using namespace Global;
using namespace Resource;

const std::unordered_map<int32_t, std::string> ResourceManagerAniUtils::ErrorCodeToMsg {
    {ERROR_CODE_INVALID_INPUT_PARAMETER, "Invalid input parameter"},
    {ERROR_CODE_RES_ID_NOT_FOUND, "Invalid resource ID"},
    {ERROR_CODE_RES_NAME_NOT_FOUND, "Invalid resource name"},
    {ERROR_CODE_RES_NOT_FOUND_BY_ID, "No matching resource is found based on the resource ID"},
    {ERROR_CODE_RES_NOT_FOUND_BY_NAME, "No matching resource is found based on the resource name"},
    {ERROR_CODE_RES_PATH_INVALID, "Invalid relative path"},
    {ERROR_CODE_RES_REF_TOO_MUCH, "The resource is referenced cyclically"},
    {ERROR_CODE_RES_ID_FORMAT_ERROR, "Failed to format the resource obtained based on the resource ID"},
    {ERROR_CODE_RES_NAME_FORMAT_ERROR, "Failed to format the resource obtained based on the resource Name"},
    {ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED, "Failed to access the system resource"},
    {ERROR_CODE_OVERLAY_RES_PATH_INVALID, "Invalid overlay path"},
    {ERROR, "Unknow error"}
};

std::string ResourceManagerAniUtils::FindErrMsg(int32_t errCode)
{
    auto iter = ResourceManagerAniUtils::ErrorCodeToMsg.find(errCode);
    std::string errMsg = iter != ResourceManagerAniUtils::ErrorCodeToMsg.end() ? iter->second : "";
    return errMsg;
}

ani_object ResourceManagerAniUtils::WrapStsError(ani_env *env, const std::string &msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "null env");
        return nullptr;
    }

    ani_string aniMsg = nullptr;
    if ((status = env->String_NewUTF8(msg.c_str(), msg.size(), &aniMsg)) != ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "String_NewUTF8 failed %{public}d", status);
        return nullptr;
    }

    ani_ref undefRef;
    if ((status = env->GetUndefined(&undefRef)) != ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "GetUndefined failed %{public}d", status);
        return nullptr;
    }

    if ((status = env->FindClass("Lescompat/Error;", &cls)) != ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "FindClass failed %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;Lescompat/ErrorOptions;:V", &method)) !=
        ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Class_FindMethod failed %{public}d", status);
        return nullptr;
    }

    if ((status = env->Object_New(cls, method, &obj, aniMsg, undefRef)) != ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}

ani_object ResourceManagerAniUtils::CreateError(ani_env *env, ani_int code, const std::string &msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "null env");
        return nullptr;
    }
    if ((status = env->FindClass("L@ohos/base/BusinessError;", &cls)) != ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "FindClass failed %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "DLescompat/Error;:V", &method)) != ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Class_FindMethod failed %{public}d", status);
        return nullptr;
    }
    ani_object error = WrapStsError(env, msg);
    if (error == nullptr) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "error null");
        return nullptr;
    }
    ani_double dCode(code);
    if ((status = env->Object_New(cls, method, &obj, dCode, error)) != ANI_OK) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}

void ResourceManagerAniUtils::AniThrow(ani_env *env, int32_t errCode)
{
    std::string errMsg = FindErrMsg(errCode);
    if (errMsg != "") {
        ani_object error = CreateError(env, errCode, errMsg);
        env->ThrowError(static_cast<ani_error>(error));
    }
}
