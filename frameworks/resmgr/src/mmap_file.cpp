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

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include <sys/mman.h>
#include <sys/stat.h>
#endif
#include <unistd.h>

#include "mmap_file.h"

namespace OHOS {
namespace Global {
namespace Resource {
MmapFile::MmapFile()
{}

MmapFile::~MmapFile()
{
    if (mmap_ != nullptr) {
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
        munmap(mmap_, mmapLen_);
#else
        delete[] mmap_;
#endif
        mmap_ = nullptr;
    }
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    if (fd_ > 0) {
        close(fd_);
        fd_ = 0;
    }
#endif
}
}
}
}