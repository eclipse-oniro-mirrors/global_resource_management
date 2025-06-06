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
 * @{
 *
 * @brief Provides the c interface to obtain resources, and relies on librawfile.z.so when used.
 *
 * @since 12
 */

/**
 * @file resmgr_common.h
 *
 * @brief Provides the structure required by the interface.
 * @syscap SystemCapability.Global.ResourceManager
 * @library libohresmgr.so
 * @since 12
 */
#ifndef GLOBAL_RESMGR_COMMON_H
#define GLOBAL_RESMGR_COMMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The error code of resource manager.
 *
 * @since 12
 */
typedef enum ResourceManager_ErrorCode {
    /** @error Success */
    SUCCESS = 0,
    /** @error Invalid input parameter */
    ERROR_CODE_INVALID_INPUT_PARAMETER = 401,
    /** @error Invalid resource ID */
    ERROR_CODE_RES_ID_NOT_FOUND = 9001001,
    /** @error Invalid resource name */
    ERROR_CODE_RES_NOT_FOUND_BY_ID = 9001002,
    /** @error No matching resource is found based on the resource ID */
    ERROR_CODE_RES_NAME_NOT_FOUND = 9001003,
    /** @error No matching resource is found based on the resource name */
    ERROR_CODE_RES_NOT_FOUND_BY_NAME = 9001004,
    /** @error Invalid relative path */
    ERROR_CODE_RES_PATH_INVALID = 9001005,
    /** @error The resource is referenced cyclically */
    ERROR_CODE_RES_REF_TOO_MUCH = 9001006,
    /** @error Failed to format the resource obtained based on the resource ID */
    ERROR_CODE_RES_ID_FORMAT_ERROR = 9001007,
    /** @error Failed to format the resource obtained based on the resource Name */
    ERROR_CODE_RES_NAME_FORMAT_ERROR = 9001008,
    /** @error Failed to access the system resource */
    ERROR_CODE_SYSTEM_RES_MANAGER_GET_FAILED = 9001009,
    /** @error Invalid overlay path */
    ERROR_CODE_OVERLAY_RES_PATH_INVALID = 9001010,
    /** @error Out of memory */
    ERROR_CODE_OUT_OF_MEMORY = 9001100,
} ResourceManager_ErrorCode;

/**
 * @brief Enumerates screen density types.
 *
 * @since 12
 */
typedef enum ScreenDensity {
    /** Indicates small screen density. */
    SCREEN_SDPI = 120,
    /** Indicates medium screen density. */
    SCREEN_MDPI = 160,
    /** Indicates large screen density. */
    SCREEN_LDPI = 240,
    /** Indicates extra-large screen density. */
    SCREEN_XLDPI = 320,
    /** Indicates extra-extra-large screen density. */
    SCREEN_XXLDPI = 480,
    /** Indicates extra-extra-extra-large screen density. */
    SCREEN_XXXLDPI = 640,
} ScreenDensity;

/**
 * @brief Enumerates screen directions.
 *
 * @since 12
 */
typedef enum ResourceManager_Direction {
    /** Indicates the vertical direction. */
    DIRECTION_VERTICAL = 0,
    /** Indicates the horizontal direction. */
    DIRECTION_HORIZONTAL = 1,
} ResourceManager_Direction;

/**
 * @brief Enumerates color mode types.
 *
 * @since 12
 */
typedef enum ResourceManager_ColorMode {
    /** Indicates dark mode. */
    DARK = 0,
    /** Indicates light mode. */
    LIGHT = 1,
} ResourceManager_ColorMode;

/**
 * @brief Enumerates device types.
 *
 * @since 12
 */
typedef enum ResourceManager_DeviceType {
    /** Indicates a phone. */
    DEVICE_TYPE_PHONE = 0X00,
    /** Indicates a tablet. */
    DEVICE_TYPE_TABLET = 0x01,
    /** Indicates a car. */
    DEVICE_TYPE_CAR = 0x02,
    /** Indicates a PC. */
    DEVICE_TYPE_PC = 0x03,
    /** Indicates a smart TV. */
    DEVICE_TYPE_TV = 0x04,
    /** Indicates a wearable device. */
    DEVICE_TYPE_WEARABLE = 0x06,
    /** Indicates a 2in1 device. */
    DEVICE_TYPE_2IN1 = 0x07,
} ResourceManager_DeviceType;

/**
 * @brief Enumerates device configuration.
 *
 * @since 12
 */
typedef struct ResourceManager_Configuration {
    /** Indicates the screen direction of the current device. */
    ResourceManager_Direction direction;
    /** Indicates the current system language, for example, zh-Hans-CN. */
    char* locale;
    /** Indicates the device type. */
    ResourceManager_DeviceType deviceType;
    /** Indicates the screen density. */
    ScreenDensity screenDensity;
    /** Indicates the color mode. */
    ResourceManager_ColorMode colorMode;
    /** Indicates the mcc. */
    uint32_t mcc;
    /** Indicates the mnc. */
    uint32_t mnc;
    /** Reserved attributes. */
    uint32_t reserved[20];
} ResourceManager_Configuration;
#ifdef __cplusplus
};
#endif

/** @} */
#endif // GLOBAL_RESMGR_COMMON_H
