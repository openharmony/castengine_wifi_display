/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#ifndef OHOS_SHARING_WFD_JS_RESULT_H
#define OHOS_SHARING_WFD_JS_RESULT_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "wfd.h"

namespace OHOS {
namespace Sharing {

class WfdJsResult {
public:
    WfdJsResult(int32_t ret) : ret_(ret) {}
    virtual ~WfdJsResult() = default;

    virtual napi_status GetJsResult(napi_env env, napi_value &result);

protected:
    int32_t ret_;
};

class WfdSinkConfigJsResult : public WfdJsResult {
public:
    WfdSinkConfigJsResult(SinkConfig &config) : WfdJsResult(0), config_(config) {}
    ~WfdSinkConfigJsResult() = default;

    napi_status GetJsResult(napi_env env, napi_value &result) override;

private:
    SinkConfig config_;
};

class WfdSinkErrorJsResult : public WfdJsResult {
public:
    WfdSinkErrorJsResult(int32_t code, std::string msg, std::string deviceId)
        : WfdJsResult(0), code_(code), msg_(std::move(msg)), deviceId_(std::move(deviceId)) {}
    ~WfdSinkErrorJsResult() = default;

    napi_status GetJsResult(napi_env env, napi_value &result) override;

private:
    int32_t code_;
    std::string msg_;
    std::string deviceId_;
};

class WfdSinkConnectionJsResult : public WfdJsResult {
public:
    WfdSinkConnectionJsResult(ConnectionInfo info) : WfdJsResult(0), info_(info) {}
    ~WfdSinkConnectionJsResult() = default;

    napi_status GetJsResult(napi_env env, napi_value &result) override;

private:
    ConnectionInfo info_;
};

class WfdSinkAccelerationJsResult : public WfdJsResult {
public:
    WfdSinkAccelerationJsResult(std::string surfaceId) : WfdJsResult(0), surfaceId_(surfaceId) {}
    ~WfdSinkAccelerationJsResult() = default;

    napi_status GetJsResult(napi_env env, napi_value &result) override;

private:
    std::string surfaceId_;
};

} // namespace Sharing
} // namespace OHOS
#endif