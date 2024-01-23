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

#ifndef OHOS_SHARING_BASE_NETWORK_SESSION_H
#define OHOS_SHARING_BASE_NETWORK_SESSION_H
#include "event_handler.h"
#include "network/eventhandler/event_descriptor_listener.h"
#include "network/interfaces/inetwork_session.h"

namespace OHOS {
namespace Sharing {
using namespace OHOS::AppExecFwk;

class BaseNetworkSession;

class BaseSessionEventListener : public EventDescriptorListener {
public:
    void OnReadable(int32_t fd) override;
    void OnWritable(int32_t fd) override;
    void OnShutdown(int32_t fd) override;
    void OnException(int32_t fd) override;

    virtual std::weak_ptr<BaseNetworkSession> &GetSession()
    {
        MEDIA_LOGD("trace.");
        return session_;
    }

    virtual void SetSession(const std::shared_ptr<BaseNetworkSession> session)
    {
        MEDIA_LOGD("trace.");
        session_ = session;
    }

protected:
    std::weak_ptr<BaseNetworkSession> session_;
};

class BaseSessionEventHandler : public OHOS::AppExecFwk::EventHandler {
public:
    virtual void OnServerReadable(int32_t fd) {}
    virtual void OnServerWritable(int32_t fd) {}
    virtual void OnServerShutdown(int32_t fd) {}
    virtual void OnServerException(int32_t fd) {}

    virtual std::weak_ptr<BaseNetworkSession> &GetSession()
    {
        MEDIA_LOGD("trace.");
        return session_;
    }

    virtual void SetSession(const std::shared_ptr<BaseNetworkSession> session)
    {
        MEDIA_LOGD("trace.");
        session_ = session;
    }

    void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer &event) override {}

protected:
    std::weak_ptr<BaseNetworkSession> session_;
};

class BaseNetworkSession : public INetworkSession,
                           public EventDescriptorListener,
                           public std::enable_shared_from_this<BaseNetworkSession> {
public:
    using Ptr = std::shared_ptr<BaseNetworkSession>;

    virtual ~BaseNetworkSession();
    explicit BaseNetworkSession(SocketInfo::Ptr socket);

    bool Start() override;
    void Shutdown() override;
    bool Send(const char *buf, int32_t nSize) override;
    bool Send(const DataBuffer::Ptr &buf, int32_t nSize) override;

    SocketInfo::Ptr GetSocketInfo() override;
    std::shared_ptr<INetworkSessionCallback> &GetCallback() override;
    void RegisterCallback(std::shared_ptr<INetworkSessionCallback> callback) override;

    virtual void OnSessionReadble(int32_t fd) {}

protected:
    SocketInfo::Ptr socket_ = nullptr;
    std::shared_ptr<INetworkSessionCallback> callback_ = nullptr;
    std::shared_ptr<BaseSessionEventHandler> eventHandler_ = nullptr;
    std::shared_ptr<BaseSessionEventListener> eventListener_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif