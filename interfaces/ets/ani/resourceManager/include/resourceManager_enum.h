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

#ifndef RESMGR_ENUM_H
#define RESMGR_ENUM_H

#include <array>
#include <string>

namespace OHOS {
namespace Global {
namespace Resource {
class EnumConvertUtils {
public:
    static int Direction_ConvertSts2Native(const int index);
    static int Direction_ConvertNative2Sts(const int nativeValue);

    static int DeviceType_ConvertSts2Native(const int index);
    static int DeviceType_ConvertNative2Sts(const int nativeValue);

    static int ScreenDensity_ConvertSts2Native(const int index);
    static int ScreenDensity_ConvertNative2Sts(const int nativeValue);

    static int ColorMode_ConvertSts2Native(const int index);
    static int ColorMode_ConvertNative2Sts(const int nativeValue);

private:
    static constexpr std::array<int, 2> DirectionArray = { 0, 1 };
    static constexpr std::array<int, 7> DeviceTypeArray = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07 };
    static constexpr std::array<int, 6> ScreenDensityArray = { 120, 160, 240, 320, 480, 640 };
    static constexpr std::array<int, 2> ColorModeArray = { 0, 1 };
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif