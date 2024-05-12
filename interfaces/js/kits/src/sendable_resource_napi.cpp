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
static thread_local napi_ref g_resource_constructor = nullptr;
static thread_local napi_ref g_sendable_resource_constructor = nullptr;
#define GET_PARAMS(env, info, num)    \
    size_t argc = num;                \
    napi_value argv[num] = {nullptr}; \
    napi_value thisVar = nullptr;     \
    void *data = nullptr;             \
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data)

static napi_value Constructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, 0, nullptr, &thisVar, nullptr);
    return thisVar;
}

static bool GetResourceProp(napi_env env, napi_callback_info info, std::vector<napi_value> &props)
{
    GET_PARAMS(env, info, 1);
    napi_value bundleName = nullptr;
    napi_valuetype bundleNameType;
    if (napi_get_named_property(env, argv[0], "bundleName", &bundleName) != napi_ok ||
        napi_typeof(env, bundleName, &bundleNameType) != napi_ok || bundleNameType != napi_string) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource bundleName property");
        return false;
    }

    napi_value moduleName = nullptr;
    napi_valuetype moduleNameType;
    if (napi_get_named_property(env, argv[0], "moduleName", &moduleName) != napi_ok ||
        napi_typeof(env, moduleName, &moduleNameType) != napi_ok || moduleNameType != napi_string) {
        HILOG_ERROR(LOG_CORE, "Failed to get resource moduleName property");
        return false;
    }

    napi_value id = nullptr;
    napi_valuetype idType;
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

static bool GetParams(napi_env env, napi_value params, napi_value *newParams, uint32_t len, bool isSendable)
{
    napi_status status;
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
    napi_value constructor = nullptr;
    napi_status status;
    if (isSendable) {
        status = napi_get_reference_value(env, g_sendable_resource_constructor, &constructor);
    } else {
        status = napi_get_reference_value(env, g_resource_constructor, &constructor);
    }
    
    if (status != napi_ok || constructor == nullptr) {
        HILOG_ERROR(LOG_CORE, "Failed to get reference value in Create, status = %{public}d", status);
        return nullptr;
    }

    napi_value result = nullptr;
    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok || result == nullptr) {
        HILOG_ERROR(LOG_CORE, "Failed to new instance in Create, status = %{public}d", status);
        return nullptr;
    }

    napi_set_named_property(env, result, "bundleName", props[0]);
    napi_set_named_property(env, result, "moduleName", props[1]);
    napi_set_named_property(env, result, "id", props[2]); // index 2

    napi_value type = props[4]; // index 4
    napi_valuetype valueType;
    if (napi_typeof(env, type, &valueType) == napi_ok && valueType == napi_number) {
        napi_set_named_property(env, result, "type", type);
    }

    bool isArray = false;
    uint32_t len = 0;
    napi_value length = nullptr;
    napi_value params = props[3]; // index 3
    if (napi_is_array(env, params, &isArray) != napi_ok || !isArray ||
        napi_get_named_property(env, params, "length", &length) != napi_ok ||
        napi_get_value_uint32(env, length, &len) != napi_ok) {
        return nullptr;
    }
    napi_value newParams = nullptr;
    if (!GetParams(env, params, &newParams, len, isSendable)) {
        return nullptr;
    }
    napi_set_named_property(env, result, "params", newParams);
    return result;
}

static napi_value Init(napi_env env, bool isSendable)
{
    napi_value result = nullptr;
    napi_status status;
    if (isSendable) {
        napi_value bundleName;
        NAPI_CALL(env, napi_create_string_utf8(env, "", 0, &bundleName));
        napi_value moduleName;
        NAPI_CALL(env, napi_create_string_utf8(env, "", 0, &moduleName));
        napi_value id;
        NAPI_CALL(env, napi_create_uint32(env, 0, &id));
        napi_value params = nullptr;
        NAPI_CALL(env, napi_create_sendable_array(env, &params));
        napi_value type;
        NAPI_CALL(env, napi_create_uint32(env, 0, &type));
        napi_property_descriptor creatorProp[] = {
            DECLARE_NAPI_INSTANCE_PROPERTY("bundleName", bundleName),
            DECLARE_NAPI_INSTANCE_PROPERTY("moduleName", moduleName),
            DECLARE_NAPI_INSTANCE_PROPERTY("id", id),
            DECLARE_NAPI_INSTANCE_PROPERTY("params", params),
            DECLARE_NAPI_INSTANCE_PROPERTY("type", type),
        };
        napi_define_sendable_class(env, "SendableResource", NAPI_AUTO_LENGTH, Constructor, nullptr,
            sizeof(creatorProp) / sizeof(creatorProp[0]), creatorProp, nullptr, &result);
        status = napi_create_reference(env, result, 1, &g_sendable_resource_constructor);
    } else {
        napi_define_class(env, "Resource", NAPI_AUTO_LENGTH, Constructor, nullptr,
            0, nullptr, &result);
        status = napi_create_reference(env, result, 1, &g_resource_constructor);
    }
    if (status != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to create Resource reference at init");
        return result;
    }
    return result;
}

static napi_value FromResource(napi_env env, napi_callback_info info)
{
    return InstanceResource(env, info, true);
}

static napi_value ToResource(napi_env env, napi_callback_info info)
{
    return InstanceResource(env, info, false);
}

static napi_value InitResource(napi_env env)
{
    return Init(env, false);
}

static napi_value InitSendableResource(napi_env env)
{
    return Init(env, true);
}
static napi_value SendableResourceInit(napi_env env, napi_value exports)
{
    napi_property_descriptor prop[] = {
        DECLARE_NAPI_PROPERTY("Resource", InitResource(env)),
        DECLARE_NAPI_PROPERTY("SendableResource", InitSendableResource(env)),
    };
    napi_status status = napi_define_properties(env, exports, sizeof(prop) / sizeof(prop[0]), prop);
    if (status != napi_ok) {
        HILOG_ERROR(LOG_CORE, "failed to define properties for exports");
        return nullptr;
    }

    napi_property_descriptor functionProp[] = {
        DECLARE_NAPI_FUNCTION("fromResource", FromResource),
        DECLARE_NAPI_FUNCTION("toResource", ToResource),
    };

    status = napi_define_properties(env, exports, sizeof(functionProp) / sizeof(functionProp[0]), functionProp);
    if (status != napi_ok) {
        HILOG_ERROR(LOG_CORE, "Failed to set function at init");
        return nullptr;
    }

    return exports;
}

static napi_module g_sendableResourceModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = SendableResourceInit,
    .nm_modname = "sendableResource",
    .nm_priv = ((void*)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void ResMgrRegister()
{
    napi_module_register(&g_sendableResourceModule);
}
} // namespace Resource
} // namespace Global
} // namespace OHOS