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

/**
 * @addtogroup resourcemanager
 *
 * @brief The ability to provide users with access to resources.
 *
 * @since 12
 */

/**
 * @file resmgr_common.h
 *
 * @brief Provides the structure required by the interface.
 *
 * @library libohresmgr.so librawfile.so
 * @since 12
 */
#ifndef GLOBAL_RESMGR_COMMON_H
#define GLOBAL_RESMGR_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The error code of resource manager.
 *
 * @since 12
 */
enum ResourceManager_ErrorCode {
    SUCCESS = 0,
    ERROR_CODE_INVALID_INPUT_PARAMETER = 401,
    ERROR_CODE_RES_ID_NOT_FOUND        = 9001001,
    ERROR_CODE_RES_NOT_FOUND_BY_ID     = 9001002,
    ERROR_CODE_RES_NAME_NOT_FOUND      = 9001003,
    ERROR_CODE_RES_NOT_FOUND_BY_NAME   = 9001004,
    ERROR_CODE_RES_PATH_INVALID        = 9001005,
    ERROR_CODE_RES_REF_TOO_MUCH        = 9001006,
    ERROR_CODE_RES_ID_FORMAT_ERROR     = 9001007,
    ERROR_CODE_RES_NAME_FORMAT_ERROR   = 9001008,
    ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED = 9001009,
    ERROR_CODE_OVERLAY_RES_PATH_INVALID = 9001010,
    ERROE_CODE_OUT_OF_MEMORY = 9001100,
};

/**
 * @brief Enumerates screen density types.
 *
 * @since 12
 */
enum ScreenDensity {
    SCREEN_SDPI = 120,
    SCREEN_MDPI = 160,
    SCREEN_LDPI = 240,
    SCREEN_XLDPI  = 320,
    SCREEN_XXLDPI = 480,
    SCREEN_XXXLDPI = 640,
};
#ifdef __cplusplus
};
#endif

/** @} */
#endif // GLOBAL_RESMGR_COMMON_H
