/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "res_desc.h"

#include <cstdlib>

#include "hilog_wrapper.h"
#if defined(__WINNT__)
#include <cstring>
#else
#include "securec.h"
#endif
#include "utils/errors.h"
#include "utils/string_utils.h"
#include "utils/utils.h"
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "parameters.h"
#endif

namespace OHOS {
namespace Global {
namespace Resource {
const std::string PROPERTY_DEVICE_TYPE = "const.product.devicetype";
const std::string PROPERTY_DEVICE_TYPE_DEFAULT = "default";
std::string KeyParam::GetScreenDensityStr() const
{
    std::string ret("not_screen_density");
    if (type_ == KeyType::SCREEN_DENSITY) {
        switch (value_) {
            case ScreenDensity::SCREEN_DENSITY_SDPI:
                ret = std::string(RE_120_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_MDPI:
                ret = std::string(RE_160_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_LDPI:
                ret = std::string(RE_240_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_XLDPI:
                ret = std::string(RE_320_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_XXLDPI:
                ret = std::string(RE_480_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_XXXLDPI:
                ret = std::string(RE_640_STR);
                break;
            default:
                break;
        }
    }
    return ret;
}
std::string KeyParam::GetDeviceTypeStr() const
{
    std::string ret("not_device_type");
    if (type_ == KeyType::DEVICETYPE) {
        switch (value_) {
            case DeviceType::DEVICE_PHONE:
                ret = std::string(PHONE_STR);
                break;
            case DeviceType::DEVICE_TABLET:
                ret = std::string(TABLET_STR);
                break;
            case DeviceType::DEVICE_CAR:
                ret = std::string(CAR_STR);
                break;
            case DeviceType::DEVICE_PAD:
                ret = std::string(PAD_STR);
                break;
            case DeviceType::DEVICE_TV:
                ret = std::string(TV_STR);
                break;
            case DeviceType::DEVICE_WEARABLE:
                ret = std::string(WEARABLE_STR);
                break;
            case DeviceType::DEVICE_TWOINONE:
                ret = std::string(TWOINONE_STR);
                break;
            default:
                break;
        }
    }
    return ret;
}

std::string KeyParam::GetColorModeStr() const
{
    std::string ret("not_color_mode");
    if (type_ == KeyType::COLORMODE) {
        switch (value_) {
            case ColorMode::DARK:
                ret = std::string(DARK_STR);
                break;
            case ColorMode::LIGHT:
                ret = std::string(LIGHT_STR);
                break;
            default:
                break;
        }
    }
    return ret;
}

std::string KeyParam::GetInputDeviceStr() const
{
    std::string ret("not_input_device");
    if (type_ == KeyType::INPUTDEVICE) {
        if (value_ == InputDevice::INPUTDEVICE_POINTINGDEVICE) {
            ret = std::string(POINTING_DEVICE_STR);
        }
    }
    return ret;
}

std::string KeyParam::GetMccStr() const
{
    std::string ret("not_mcc");
    if (type_ == KeyType::MCC) {
        ret = std::string("mcc");
    }
    return ret;
}

std::string KeyParam::GetMncStr() const
{
    std::string ret("not_mnc");
    if (type_ == KeyType::MNC) {
        ret = std::string("mnc");
    }
    return ret;
}

const std::string KeyParam::ConvertToStr() const
{
    if ((type_ == KeyType::LANGUAGES) || (type_ == KeyType::REGION) || (type_ == KeyType::SCRIPT)) {
        char tmp[4];
        char tmp2[5];
        errno_t eret = memcpy_s(tmp, sizeof(tmp), &value_, sizeof(value_));
        if (eret != OK) {
            RESMGR_HILOGE(RESMGR_TAG, "memcpy_s error : %d", eret);
            return std::string();
        }
        int j = 0;
        // 4 means langauges/region/script key value max length
        for (int i = 0; i < 4; ++i) {
            if (tmp[3 - i]) { // 3 means reverse temp value to temp2
                tmp2[j++] = tmp[3 - i]; // 3 means reverse temp value to temp2
            }
        }
        tmp2[j] = '\0';
        return std::string(tmp2);
    }
    if (type_ == KeyType::DIRECTION) {
        return std::string((value_ == 0) ? VERTICAL : HORIZONTAL);
    }
    if (type_ == KeyType::DEVICETYPE) {
        return GetDeviceTypeStr();
    }
    if (type_ == KeyType::COLORMODE) {
        return GetColorModeStr();
    }
    if (type_ == KeyType::INPUTDEVICE) {
        return GetInputDeviceStr();
    }
    if (type_ == KeyType::MCC) {
        return GetMccStr();
    }
    if (type_ == KeyType::MNC) {
        return GetMncStr();
    }
    if (type_ == KeyType::SCREEN_DENSITY) {
        return GetScreenDensityStr();
    }
    return std::string();
}

std::string KeyParam::ToString() const
{
    std::string ret = FormatString("[type:%d, value:%u", type_, value_);
    if (str_.length() > 0) {
        ret.append(FormatString(", str:%s", str_.c_str()));
    }
    ret.append("]");
    return ret;
}

// IdItem

std::map<ResType, std::string> IdItem::resTypeStrList;

bool IdItem::sInit = IdItem::Init();

bool IdItem::Init()
{
    resTypeStrList.insert(make_pair(ResType::STRING, std::string("string")));
    resTypeStrList.insert(make_pair(ResType::BOOLEAN, std::string("boolean")));
    resTypeStrList.insert(make_pair(ResType::COLOR, std::string("color")));
    resTypeStrList.insert(make_pair(ResType::FLOAT, std::string("float")));
    resTypeStrList.insert(make_pair(ResType::INTEGER, std::string("integer")));
    resTypeStrList.insert(make_pair(ResType::PATTERN, std::string("pattern")));
    resTypeStrList.insert(make_pair(ResType::THEME, std::string("theme")));
    resTypeStrList.insert(make_pair(ResType::MEDIA, std::string("media")));
    resTypeStrList.insert(make_pair(ResType::SYMBOL, std::string("symbol")));
    resTypeStrList.insert(make_pair(ResType::PLURALS, std::string("plural")));
    return true;
}

bool IdItem::HaveParent() const
{
    if (!(resType_ == THEME || resType_ == PATTERN)) {
        return false;
    }
    return (values_.size() % 2 == 1); // Taking the remainder of 2 to determine the existence of a parent node
}

bool IdItem::IsRef(const std::string &value, ResType &resType, uint32_t &id)
{
    const char *it = value.c_str();
    const char *st = it;
    if (*st != '$') {
        return false;
    }
    auto index = value.find(":");
    if (index == std::string::npos || index < ArrayIndex::INDEX_TWO) {
        return false;
    }
    std::string typeStr;
    std::string idStr;
    typeStr.assign(it + 1, index - 1);
    idStr.assign(it + index + 1, value.size() - index);

    unsigned long tmpId;
    if (!Utils::convertToUnsignedLong(idStr, tmpId) || tmpId > UINT32_MAX) {
        return false;
    }
    uint32_t idd = static_cast<uint32_t>(tmpId);
    for (auto iit = resTypeStrList.begin(); iit != resTypeStrList.end(); ++iit) {
        auto tValue = iit->second;
        auto type = iit->first;
        if (typeStr == tValue) {
            id = idd;
            resType = type;
            return true;
        }
    }

    return false;
}

std::string IdItem::ToString() const
{
    std::string ret = FormatString(
        "[size:%u, resType:%d, id:%u, valueLen:%u, isArray:%d, name:'%s', value:",
        size_, resType_, id_, valueLen_, isArray_, name_.c_str());
    if (isArray_) {
        ret.append("[");
        for (size_t i = 0; i < values_.size(); ++i) {
            ret.append(FormatString("'%s',", values_[i].c_str()));
        }
        ret.append("]");
    } else {
        ret.append(FormatString("'%s'", value_.c_str()));
    }
    ret.append("]");
    return ret;
}

IdParam::~IdParam()
{}

std::string IdParam::ToString() const
{
    return FormatString("[id:%u, offset:%u, data:%s]", id_, offset_,
        idItem_->ToString().c_str());
}

ResId::~ResId()
{}

std::string ResId::ToString() const
{
    std::string ret = FormatString("idcount:%u, ", count_);
    for (size_t i = 0; i < idParams_.size(); ++i) {
        ret.append(idParams_[i]->ToString());
    }
    return ret;
}

ResKey::~ResKey()
{
    RESMGR_HILOGD(RESMGR_TAG, "~ResKey()");
}

std::string ResKey::ToString() const
{
    std::string ret = FormatString("offset:%u, keyParamsCount:%u, folder path:", offset_, keyParamsCount_);
    ret.append(resConfig_->ToString());
    ret.append("\nid: ");
    ret.append(resId_->ToString());
    return ret;
}

ResDesc::ResDesc()
{}

ResDesc::~ResDesc()
{
    RESMGR_HILOGD(RESMGR_TAG, "~ResDesc()");
}

std::string ResDesc::ToString() const
{
    std::string ret = FormatString("keyCount:%u\n", keys_.size());
    for (size_t i = 0; i < keys_.size(); ++i) {
        ret.append(keys_[i]->ToString());
        ret.append("\n");
    }
    return ret;
}

std::string ResDesc::GetCurrentDeviceType()
{
    std::string deviceType;
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    deviceType = system::GetParameter(PROPERTY_DEVICE_TYPE, PROPERTY_DEVICE_TYPE_DEFAULT);
#endif
    return deviceType;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
