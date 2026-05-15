/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_RESOURCE_MANAGER_DFX_API_METRICS_REPORTER_H
#define OHOS_RESOURCE_MANAGER_DFX_API_METRICS_REPORTER_H

#if defined(SUPPORT_API_METRICS) && !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#define ENABLE_API_METRICS
#endif

#ifdef ENABLE_API_METRICS
#include <chrono>
#include <string>

namespace OHOS {
namespace Global {
namespace Resource {

enum MetricsType {
    API_CALL = 1 << 0,
    TIME     = 1 << 1,
    ALL      = API_CALL | TIME
};

class ApiMetricsReporter {
public:
    explicit ApiMetricsReporter(const std::string &name, MetricsType metricsType = MetricsType::ALL);
    ~ApiMetricsReporter();

    ApiMetricsReporter(const ApiMetricsReporter &other) = delete;
    ApiMetricsReporter &operator=(const ApiMetricsReporter &other) = delete;

private:
    std::string name_;
    int metricsType_;
    std::chrono::time_point<std::chrono::steady_clock> startTime_;
};

#define API_METRICS(name) ApiMetricsReporter metrics_reporter_##__LINE__(name)

} // namespace Resource
} // namespace Global
} // namespace OHOS

#endif // ENABLE_API_METRICS

#endif // OHOS_RESOURCE_MANAGER_DFX_API_METRICS_REPORTER_H