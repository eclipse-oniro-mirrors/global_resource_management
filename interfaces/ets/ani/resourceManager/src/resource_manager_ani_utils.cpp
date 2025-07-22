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

static const char* CLASS_NAME_BUSINESSERROR = "L@ohos/base/BusinessError;";

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

void ResourceManagerAniUtils::ThrowAniError(ani_env *env, ani_int code, const std::string &message)
{
    ani_class cls {};
    if (ANI_OK != env->FindClass(CLASS_NAME_BUSINESSERROR, &cls)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "find class %{public}s failed", CLASS_NAME_BUSINESSERROR);
        return;
    }
    ani_method ctor {};
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":V", &ctor)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "find method BusinessError constructor failed");
        return;
    }
    ani_object error {};
    if (ANI_OK != env->Object_New(cls, ctor, &error)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "new object %{public}s failed", CLASS_NAME_BUSINESSERROR);
        return;
    }
    if (ANI_OK != env->Object_SetPropertyByName_Int(error, "code", code)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "set property BusinessError.code failed");
        return;
    }
    ani_string messageRef {};
    if (ANI_OK != env->String_NewUTF8(message.c_str(), message.size(), &messageRef)) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "new message string failed");
        return;
    }
    if (ANI_OK != env->Object_SetPropertyByName_Ref(error, "message", static_cast<ani_ref>(messageRef))) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "set property BusinessError.message failed");
        return;
    }
    if (ANI_OK != env->ThrowError(static_cast<ani_error>(error))) {
        RESMGR_HILOGE(RESMGR_ANI_TAG, "throwError ani_error object failed");
    }
}

void ResourceManagerAniUtils::AniThrow(ani_env *env, int32_t errCode)
{
    std::string errMsg = FindErrMsg(errCode);
    if (errMsg != "") {
        ThrowAniError(env, errCode, errMsg);
    }
}
