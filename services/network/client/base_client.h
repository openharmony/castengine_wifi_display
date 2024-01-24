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

#ifndef OHOS_SHARING_BASE_CLIENT_H
#define OHOS_SHARING_BASE_CLIENT_H

#include <cstdint>
#include "event_handler.h"
#include "network/data/socket_info.h"
#include "network/eventhandler/event_descriptor_listener.h"
#include "network/interfaces/iclient.h"

namespace OHOS {
namespace Sharing {
using namespace OHOS::AppExecFwk;

class BaseClient;
class BaseClientEventListener : public EventDescriptorListener {
public:
    void OnReadable(int32_t fd) override;
    void OnWritable(int32_t fd) override;
    void OnShutdown(int32_t fd) override;
    void OnException(int32_t fd) override;

    virtual std::weak_ptr<BaseClient> &GetClient()
    {
        SHARING_LOGD("trace.");
        return client_;
    }

    virtual void SetClient(const std::shared_ptr<BaseClient> client)
    {
        SHARING_LOGD("trace.");
        client_ = client;
    }

protected:
    std::weak_ptr<BaseClient> client_;
};

class BaseClientEventHandler : public OHOS::AppExecFwk::EventHandler {
public:
    virtual void OnClientReadable(int32_t fd) {}
    virtual void OnClientWritable(int32_t fd) {}
    virtual void OnClientShutdown(int32_t fd) {}
    virtual void OnClientException(int32_t fd) {}

    virtual std::weak_ptr<BaseClient> &GetClient()
    {
        SHARING_LOGD("trace.");
        return client_;
    }

    virtual void SetClient(const std::shared_ptr<BaseClient> client)
    {
        SHARING_LOGD("trace.");
        client_ = client;
    }

    void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer &event) override {}

protected:
    std::weak_ptr<BaseClient> client_;
};

class BaseClient : public IClient,
                   public std::enable_shared_from_this<BaseClient> {
public:
    BaseClient();
    ~BaseClient() override;

    virtual void OnClientReadable(int32_t fd) {}
    virtual std::weak_ptr<IClientCallback> &GetCallback();

    void SetRecvOption(int32_t flags) override;
    void RegisterCallback(std::weak_ptr<IClientCallback> callback) override;

protected:
    int32_t flags_ = 0;

    std::weak_ptr<IClientCallback> callback_;
    std::shared_ptr<BaseClientEventHandler> eventHandler_ = nullptr;
    std::shared_ptr<BaseClientEventListener> eventListener_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif