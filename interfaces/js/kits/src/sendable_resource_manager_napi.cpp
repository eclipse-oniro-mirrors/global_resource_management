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
#include "rstate.h"
#include "hilog/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine.h"

namespace OHOS {
namespace Global {
namespace Resource {
using namespace OHOS::HiviewDFX;
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001E00

#undef LOG_TAG
#define LOG_TAG "ResourceJs"
#define GET_PARAMS(env, info, num)    \
    size_t argc = num;                \
    napi_value argv[num] = {nullptr}; \
    napi_value thisVar = nullptr;     \
    void *data = nullptr;             \
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data)

static bool IsNapiObject(napi_env env, napi_callback_info info)
{
    GET_PARAMS(env, info, 1);
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_object) {
        HILOG_WARN(LOG_CORE, "Parameter type is not napi_object");
        return false;
    }
    return true;
}

static void NapiThrow(napi_env env, int32_t errCode)
{
    napi_value code = nullptr;
    napi_create_string_latin1(env, std::to_string(errCode).c_str(), NAPI_AUTO_LENGTH, &code);

    napi_value message = nullptr;
    std::string errMsg = "Invalid input parameter";
    napi_create_string_latin1(env, errMsg.c_str(), NAPI_AUTO_LENGTH, &message);
    if (errMsg != "") {
        napi_value error = nullptr;
        napi_create_error(env, code, message, &error);
        napi_throw(env, error);
    }
}

static bool GetResourceProp(napi_env env, napi_callback_info info, std::vector<napi_value> &props)
{
    if (!IsNapiObject(env, info)) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource object");
        return false;
    }
    GET_PARAMS(env, info, 1);
    napi_value bundleName = nullptr;
    napi_valuetype bundleNameType = napi_undefined;
    if (napi_get_named_property(env, argv[0], "bundleName", &bundleName) != napi_ok ||
        napi_typeof(env, bundleName, &bundleNameType) != napi_ok || bundleNameType != napi_string) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource bundleName property");
        return false;
    }

    napi_value moduleName = nullptr;
    napi_valuetype moduleNameType = napi_undefined;
    if (napi_get_named_property(env, argv[0], "moduleName", &moduleName) != napi_ok ||
        napi_typeof(env, moduleName, &moduleNameType) != napi_ok || moduleNameType != napi_string) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource moduleName property");
        return false;
    }

    napi_value id = nullptr;
    napi_valuetype idType = napi_undefined;
    if (napi_get_named_property(env, argv[0], "id", &id) != napi_ok ||
        napi_typeof(env, id, &idType) != napi_ok || idType != napi_number) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource id property");
        return false;
    }

    napi_value params = nullptr;
    if (napi_get_named_property(env, argv[0], "params", &params) != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource params property");
        return false;
    }

    napi_value type = nullptr;
    if (napi_get_named_property(env, argv[0], "type", &type) != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource type property");
        return false;
    }
    props.emplace_back(bundleName);
    props.emplace_back(moduleName);
    props.emplace_back(id);
    props.emplace_back(params);
    props.emplace_back(type);
    return true;
}

static bool GetParams(napi_env env, napi_value params, napi_value *newParams, bool isSendable)
{
    napi_status status;
    bool isArray = false;
    uint32_t len = 0;
    napi_value length = nullptr;
    napi_valuetype paramsType = napi_undefined;
    if (napi_typeof(env, params, &paramsType) != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource params property type");
        return false;
    }
    if (paramsType == napi_undefined) {
        return true;
    }
    if (napi_is_array(env, params, &isArray) != napi_ok || !isArray ||
        napi_get_named_property(env, params, "length", &length) != napi_ok ||
        napi_get_value_uint32(env, length, &len) != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Resource params property params is error");
        return false;
    }
    if (isSendable) {
        status = napi_create_sendable_array_with_length(env, len, newParams);
    } else {
        status = napi_create_array_with_length(env, len, newParams);
    }
    if (status != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to create array, status = %{public}d", status);
        return false;
    }
    napi_value jsValue = nullptr;
    for (uint32_t i = 0; i < len; i++) {
        if (napi_get_element(env, params, i, &jsValue) != napi_ok) {
            HILOG_ERROR(LOG_CORE, "Failed to get element");
            return false;
        }
        if (napi_set_element(env, *(newParams), i, jsValue) != napi_ok) {
            HILOG_ERROR(LOG_CORE, "Failed to set element");
            return false;
        }
    }
    return true;
}

static napi_value InstanceResource(napi_env env, napi_callback_info info, bool isSendable)
{
    std::vector<napi_value> props;
    if (!GetResourceProp(env, info, props)) {
        return nullptr;
    }
    napi_property_descriptor creatorProp[5] = { // array len
        DECLARE_NAPI_DEFAULT_PROPERTY("bundleName", props[0]),
        DECLARE_NAPI_DEFAULT_PROPERTY("moduleName", props[1]),
        DECLARE_NAPI_DEFAULT_PROPERTY("id", props[2]),
        DECLARE_NAPI_INSTANCE_GENERIC_PROPERTY("params"),
        DECLARE_NAPI_INSTANCE_GENERIC_PROPERTY("type"),
    };

    napi_value params = props[3]; // index 3
    napi_value newParams = nullptr;
    if (!GetParams(env, params, &newParams, isSendable)) {
        return nullptr;
    }
    if (newParams != nullptr) {
        creatorProp[3] = DECLARE_NAPI_DEFAULT_PROPERTY("params", newParams); // index 3
    }

    napi_value type = props[4]; // index 4
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, type, &valueType) != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Resource type property type is error");
        return nullptr;
    }
    if (valueType != napi_undefined) {
        if (valueType != napi_number) {
            HILOG_ERROR(LOG_CORE, "Resource type property type is not number");
            return nullptr;
        }
        creatorProp[4] = DECLARE_NAPI_DEFAULT_PROPERTY("type", type); // index 4
    }

    napi_value result = nullptr;
    napi_status status;
    if (isSendable) {
        status = napi_create_sendable_object_with_properties(env, sizeof(creatorProp) / sizeof(creatorProp[0]),
            creatorProp, &result);
    } else {
        status =napi_create_object_with_properties(env, &result, sizeof(creatorProp) / sizeof(creatorProp[0]),
            creatorProp);
    }
    if (status != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to create object with properties, status = %{public}d", status);
        return nullptr;
    }
    return result;
}

static napi_value ResourceToSendableResource(napi_env env, napi_callback_info info)
{
    napi_value result = InstanceResource(env, info, true);
    if (result == nullptr) {
        NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    return result;
}

static napi_value SendableResourceToResource(napi_env env, napi_callback_info info)
{
    napi_value result = InstanceResource(env, info, false);
    if (result == nullptr) {
        NapiThrow(env, ERROR_CODE_INVALID_INPUT_PARAMETER);
        return nullptr;
    }
    return result;
}

static napi_value SendableResourceManagerInit(napi_env env, napi_value exports)
{
    napi_property_descriptor functionProp[] = {
        DECLARE_NAPI_FUNCTION("resourceToSendableResource", ResourceToSendableResource),
        DECLARE_NAPI_FUNCTION("sendableResourceToResource", SendableResourceToResource),
    };

    napi_status status = napi_define_properties(env, exports, sizeof(functionProp) / sizeof(functionProp[0]),
        functionProp);
    if (status != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to set function at init");
        return nullptr;
    }

    return exports;
}

static napi_module g_sendableResourceManagerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = SendableResourceManagerInit,
    .nm_modname = "sendableResourceManager",
    .nm_priv = ((void*)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void ResMgrRegister()
{
    napi_module_register(&g_sendableResourceManagerModule);
}
} // namespace Resource
} // namespace Global
} // namespace OHOS