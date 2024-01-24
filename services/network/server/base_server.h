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

#ifndef OHOS_SHARING_BASE_SERVER_H
#define OHOS_SHARING_BASE_SERVER_H

#include "event_handler.h"
#include "network/eventhandler/event_descriptor_listener.h"
#include "network/interfaces/iserver.h"
#include "network/socket/base_socket.h"

namespace OHOS {
namespace Sharing {
using namespace OHOS::AppExecFwk;

class IServerCallback;
class BaseServer;

class BaseServerEventListener : public EventDescriptorListener {
public:
    void OnReadable(int32_t fd) override;
    void OnWritable(int32_t fd) override;
    void OnShutdown(int32_t fd) override;
    void OnException(int32_t fd) override;

    virtual std::weak_ptr<BaseServer> &GetServer()
    {
        SHARING_LOGD("trace.");
        return server_;
    }

    virtual void SetServer(const std::shared_ptr<BaseServer> server)
    {
        SHARING_LOGD("trace.");
        server_ = server;
    }

protected:
    std::weak_ptr<BaseServer> server_;
};

class BaseServerEventHandler : public OHOS::AppExecFwk::EventHandler {
public:
    virtual void OnServerReadable(int32_t fd) {}
    virtual void OnServerWritable(int32_t fd) {}
    virtual void OnServerShutdown(int32_t fd) {}
    virtual void OnServerException(int32_t fd) {}

    virtual std::weak_ptr<BaseServer> &GetServer()
    {
        SHARING_LOGD("trace.");
        return server_;
    }

    virtual void SetServer(const std::shared_ptr<BaseServer> server)
    {
        SHARING_LOGD("trace.");
        server_ = server;
    }

    void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer &event) override {}

protected:
    std::weak_ptr<BaseServer> server_;
};

class BaseServer : public IServer,
                   public EventDescriptorListener,
                   public std::enable_shared_from_this<BaseServer> {
public:
    BaseServer();
    ~BaseServer() override;

    std::weak_ptr<IServerCallback> &GetCallback() override;
    void RegisterCallback(std::weak_ptr<IServerCallback> callback) override;

    virtual void OnServerReadable(int32_t fd) {}

protected:
    std::weak_ptr<IServerCallback> callback_;
    std::shared_ptr<BaseServerEventHandler> eventHandler_ = nullptr;
    std::shared_ptr<BaseServerEventListener> eventListener_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif