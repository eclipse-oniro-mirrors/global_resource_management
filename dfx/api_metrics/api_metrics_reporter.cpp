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

#include "api_metrics_reporter.h"

#ifdef ENABLE_API_METRICS
#include "hilog_wrapper.h"
#include "histogram_plugin_macros.h"

namespace OHOS {
namespace Global {
namespace Resource {

ApiMetricsReporter::ApiMetricsReporter(const std::string &name, MetricsType metricsType) : name_(name),
    metricsType_(static_cast<unsigned int>(metricsType)), startTime_(std::chrono::steady_clock::now())
{}

ApiMetricsReporter::~ApiMetricsReporter()
{
    if (name_.empty() || metricsType_ == 0) {
        return;
    }
    if (metricsType_ & static_cast<unsigned int>(MetricsType::TIME)) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        HISTOGRAM_TIMES(std::string(name_ + ".Time").c_str(), static_cast<int32_t>(duration.count()));
    }
    if (metricsType_ & static_cast<unsigned int>(MetricsType::API_CALL)) {
        HISTOGRAM_BOOLEAN(std::string(name_ + ".ApiCall").c_str(), true);
    }
}

} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif // ENABLE_API_METRICS