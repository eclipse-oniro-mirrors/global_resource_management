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

#ifndef OHOS_RESOURCE_MANAGER_HAPPARSERV2_H
#define OHOS_RESOURCE_MANAGER_HAPPARSERV2_H

#include "hap_parser.h"

#include "hap_resource_v2.h"
#include "res_config_impl.h"

namespace OHOS {
namespace Global {
namespace Resource {
class HapParserV2 : public HapParser {
public:
    HapParserV2();

    virtual ~HapParserV2();

    virtual bool Init(const char *path);

    virtual int32_t ParseResHex();

    virtual std::shared_ptr<HapResource> GetHapResource(const char *path, bool isSystem, bool isOverlay);

    static uint32_t ParseResInfo(uint32_t &offset, ResInfo &resInfo, const size_t bufLen, const uint8_t *buf);

    static uint32_t ParseConfigItem(uint32_t &offset, ConfigItem &resInfo, const size_t bufLen, const uint8_t *buf);

    static int32_t ParseStringArray(uint32_t &offset, std::vector<std::string> &values, size_t bufLen, uint8_t *buf);

    static int32_t ParseString(uint32_t &offset, std::string &id, size_t bufLen, uint8_t *buf);
private:
    bool GetIndexMmap(const char *path);

    bool GetIndexMmapFromHap(const char *path);

    bool GetIndexMmapFromIndex(const char *path);

    uint32_t ParseHeader(uint32_t &offset, const size_t bufLen, const uint8_t *buf);

    uint32_t ParseKeys(uint32_t &offset, const size_t bufLen, const uint8_t *buf);

    uint32_t ParseKey(uint32_t &offset, std::shared_ptr<KeyInfo> key, const size_t bufLen, const uint8_t *buf);

    uint32_t ParseKeyParam(uint32_t &offset, std::shared_ptr<KeyParam> keyParam,
        const size_t bufLen, const uint8_t *buf);

    void GetLimitKeyValue(KeyType type);

    uint32_t ParseIds(uint32_t &offset, const size_t bufLen, const uint8_t *buf);

    uint32_t ParseType(uint32_t &offset, const size_t bufLen, const uint8_t *buf);

    uint32_t ParseItem(uint32_t &offset, const size_t bufLen, const uint8_t *buf, const TypeInfo &typeInfo);
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    std::shared_ptr<AbilityBase::Extractor> extractor_{nullptr};

    std::unique_ptr<AbilityBase::FileMapper> mapper_{nullptr};
#endif
    size_t mmapLen_{0};

    uint8_t *mmap_{nullptr};

    ResIndexHeader resHeader_;

    IdsHeader idsHeader_;

    // <resconfig id, resconfig>
    std::unordered_map<uint32_t, std::shared_ptr<ResConfigImpl>> keys_;

    // <resource id, resource>
    std::unordered_map<uint32_t, std::shared_ptr<IdValuesV2>> idMap_;

    // <type, <resource name, resource>>
    std::unordered_map<uint32_t, std::unordered_map<std::string, std::shared_ptr<IdValuesV2>>> typeNameMap_;

    // judge the resource is adapt dark mode or not.
    bool hasDarkRes_{false};
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
