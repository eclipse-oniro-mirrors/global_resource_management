/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HILOG_WRAPPER_H
#define HILOG_WRAPPER_H

#undef RESMGR_TAG
#define RESMGR_TAG "ResourceManager"

#undef RESMGR_JS_TAG
#define RESMGR_JS_TAG "ResourceManagerJs"

#undef RESMGR_NATIVE_TAG
#define RESMGR_NATIVE_TAG "ResourceManagerNative"

#undef RESMGR_RAWFILE_TAG
#define RESMGR_RAWFILE_TAG "ResourceManagerRawFile"

#undef RESMGR_ANI_TAG
#define RESMGR_ANI_TAG "ResourceManagerAni"

#ifdef CONFIG_HILOG

#include "hilog/log.h"

namespace OHOS::HiviewDFX {
    #define RESMGR_HILOGE(tag, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_ERROR, 0xD001E00, tag, fmt, ##__VA_ARGS__)
    #define RESMGR_HILOGW(tag, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_WARN, 0xD001E00, tag, fmt, ##__VA_ARGS__)
    #define RESMGR_HILOGI(tag, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_INFO, 0xD001E00, tag, fmt, ##__VA_ARGS__)
    #define RESMGR_HILOGD(tag, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_DEBUG, 0xD001E00, tag, fmt, ##__VA_ARGS__)
    #define RESMGR_HILOGF(tag, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_FATAL, 0xD001E00, tag, fmt, ##__VA_ARGS__)
    #define RESMGR_HILOGI_BY_FLAG(flag, tag, fmt, ...)                              \
        if (flag) {                                                                 \
            HILOG_IMPL(LOG_CORE, LOG_INFO, 0xD001E00, tag, fmt, ##__VA_ARGS__);     \
        }
    #define RESMGR_HILOGW_BY_FLAG(flag, tag, fmt, ...)                              \
        if (flag) {                                                                 \
            HILOG_IMPL(LOG_CORE, LOG_WARN, 0xD001E00, tag, fmt, ##__VA_ARGS__);     \
        }
}

namespace OHOS {
namespace Global {
namespace Resource {
extern LogLevel g_logLevel;
} // namespace Resource
} // namespace Global
} // namespace OHOS

#else

#define RESMGR_HILOGE(tag, fmt, ...)
#define RESMGR_HILOGW(tag, fmt, ...)
#define RESMGR_HILOGI(tag, fmt, ...)
#define RESMGR_HILOGD(tag, fmt, ...)
#define RESMGR_HILOGF(tag, fmt, ...)
#define RESMGR_HILOGI_BY_FLAG(flag, tag, fmt, ...)
#define RESMGR_HILOGW_BY_FLAG(flag, tag, fmt, ...)

#endif  // CONFIG_HILOG

#endif  // HILOG_WRAPPER_H