/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#include "resmgr_enum.h"

namespace OHOS {
namespace Global {
namespace Resource {
int EnumConvertUtils::Direction_ConvertSts2Native(const int index)
{
    return index;
}

int EnumConvertUtils::Direction_ConvertNative2Sts(const int nativeValue)
{
    return nativeValue;
}

int EnumConvertUtils::DeviceType_ConvertSts2Native(const int index)
{
    if (index < 0 || index >= DeviceTypeArray.size()) {
        return 0;
    }
    return DeviceTypeArray[index];
}

int EnumConvertUtils::DeviceType_ConvertNative2Sts(const int nativeValue)
{
    for (int index = 0; index < DeviceTypeArray.size(); index++) {
        if (nativeValue == DeviceTypeArray[index]) {
            return index;
        }
    }
    return 0;
}

int EnumConvertUtils::ScreenDensity_ConvertSts2Native(const int index)
{
    if (index < 0 || index >= ScreenDensityArray.size()) {
        return 0;
    }
    return ScreenDensityArray[index];
}

int EnumConvertUtils::ScreenDensity_ConvertNative2Sts(const int nativeValue)
{
    for (int index = 0; index < ScreenDensityArray.size(); index++) {
        if (nativeValue == ScreenDensityArray[index]) {
            return index;
        }
    }
    return 0;
}

int EnumConvertUtils::ColorMode_ConvertSts2Native(const int index)
{
    return index;
}

int EnumConvertUtils::ColorMode_ConvertNative2Sts(const int nativeValue)
{
    return nativeValue;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS