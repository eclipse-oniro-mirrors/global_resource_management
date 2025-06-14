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

#ifndef OHOS_RESOURCE_MANAGER_HAPPARSERV1_H
#define OHOS_RESOURCE_MANAGER_HAPPARSERV1_H

#include "hap_parser.h"

namespace OHOS {
namespace Global {
namespace Resource {
class HapParserV1 : public HapParser {
public:
    HapParserV1();

    HapParserV1(std::shared_ptr<ResConfigImpl> &defaultConfig, uint32_t selectedTypes,
                bool loadAll, bool isUpdate = false);

    virtual ~HapParserV1();

    virtual bool Init(const char *path);

    virtual int32_t ParseResHex();

    virtual std::shared_ptr<HapResource> GetHapResource(const char *path, bool isSystem, bool isOverlay);

    void SetResDesc(std::shared_ptr<ResDesc> &resDesc);

    inline const std::shared_ptr<ResDesc> GetResDesc()
    {
        return resDesc_;
    }
private:
    int32_t ParseKey(uint32_t &offset, std::shared_ptr<ResKey> key, bool &match, std::vector<bool> &keyTypes);

    bool getKeyParams(uint32_t &offset, uint32_t paramsCount, bool &match, std::vector<bool> &keyTypes,
        std::vector<std::shared_ptr<KeyParam>> &keyParams);

    int32_t ParseKeyParam(uint32_t &offset, bool &match, std::shared_ptr<KeyParam> &kp);

    void GetLimitKeyValue(KeyType type, std::vector<bool> &keyTypes);

    bool SkipParseItem(const std::shared_ptr<ResKey> &key, bool &match);

    int32_t ParseId(uint32_t &offset, std::shared_ptr<ResId> id);

    int32_t ParseIdItem(uint32_t &offset, std::shared_ptr<IdItem> idItem);

    int32_t ParseStringArray(uint32_t &offset, std::vector<std::string> &values);

    int32_t ParseString(uint32_t &offset, std::string &id, bool includeTemi = true);

    std::unique_ptr<uint8_t[]> buffer_{nullptr};
    size_t bufLen_{0};
    std::shared_ptr<ResDesc> resDesc_{std::make_shared<ResDesc>()};
    std::shared_ptr<ResConfigImpl> defaultConfig_{nullptr};
    uint32_t selectedTypes_{SELECT_ALL};
    std::string deviceType_;
    std::vector<std::string> deviceTypes_;
    bool loadAll_{false};
    bool isUpdate_{false};
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
