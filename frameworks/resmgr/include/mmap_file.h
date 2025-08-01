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

#ifndef OHOS_RESOURCE_MANAGER_MMAPFILE_H
#define OHOS_RESOURCE_MANAGER_MMAPFILE_H

#include <cstdint>

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "file_mapper.h"
#include "extractor.h"
#endif

namespace OHOS {
namespace Global {
namespace Resource {
class MmapFile {
public:
    MmapFile();
    
    ~MmapFile();
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    int fd_{0};

    std::shared_ptr<AbilityBase::Extractor> extractor_{nullptr};

    std::unique_ptr<AbilityBase::FileMapper> mapper_{nullptr};
#endif
    size_t mmapLen_{0};

    uint8_t *mmap_{nullptr};
};
}
}
}
#endif