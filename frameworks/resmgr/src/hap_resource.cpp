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

#include "hap_resource.h"

using ReadLock = std::shared_lock<std::shared_mutex>;
using WriteLock = std::unique_lock<std::shared_mutex>;

namespace OHOS {
namespace Global {
namespace Resource {
ValueUnderQualifierDir::ValueUnderQualifierDir(const std::pair<std::string, std::string> &resPath,
    std::shared_ptr<ResConfigImpl> resConfig, bool isOverlay, bool isSystemResource) : indexPath_(resPath.first),
    resourcePath_(resPath.second), resConfig_(resConfig), isOverlay_(isOverlay), isSystemResource_(isSystemResource)
{}

ValueUnderQualifierDir::~ValueUnderQualifierDir()
{}

std::shared_ptr<IdItem> ValueUnderQualifierDir::GetIdItem() const
{
    return nullptr;
}

IdValues::~IdValues()
{}

HapResource::HapResource(const std::string path, time_t lastModTime) : indexPath_(path), lastModTime_(lastModTime)
{}

HapResource::~HapResource()
{
    lastModTime_ = 0;
}

bool HapResource::IsSystemResource() const
{
    return false;
}

bool HapResource::IsOverlayResource() const
{
    return false;
}

const std::shared_ptr<IdValues> HapResource::GetIdValues(const uint32_t id)
{
    return nullptr;
}

const std::shared_ptr<IdValues> HapResource::GetIdValuesByName(const std::string name, const ResType resType)
{
    return nullptr;
}

RState HapResource::Update(std::shared_ptr<ResConfigImpl> &defaultConfig)
{
    return SUCCESS;
}

uint32_t HapResource::GetLimitKeysValue()
{
    ReadLock lock(mutex_);
    return limitKeyValue_;
}

std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> HapResource::BuildNameTypeIdMapping()
{
    return std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>>();
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
