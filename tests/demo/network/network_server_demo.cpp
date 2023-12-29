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

#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include "common/sharing_log.h"
#include "network/interfaces/inetwork_session_callback.h"
#include "network/interfaces/iserver_callback.h"
#include "network/network_factory.h"
#include "safe_map.h"
#include "utils/utils.h"

using namespace OHOS::Sharing;
using namespace std;

class SessionAgent : public INetworkSessionCallback,
                     public std::enable_shared_from_this<SessionAgent> {
public:
    SessionAgent(std::weak_ptr<INetworkSession> sessionPtr, std::weak_ptr<IServer> serverPtr)
        : sessionPtr_(sessionPtr), serverPtr_(serverPtr)
    {
        SHARING_LOGD("trace.");
    }
    ~SessionAgent()
    {
        SHARING_LOGD("trace.");
    }
    void OnSessionReadData(int32_t fd, DataBuffer::Ptr buf) override
    {
        SHARING_LOGD("OnSessionReadData fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
    }

    void OnSessionWriteable(int32_t fd) override
    {
        SHARING_LOGD("OnSessionWriteable fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
        static int index = 0;
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        string msg = "tcp session send message=" + std::to_string(index++);
        bufSend->PushData(msg.c_str(), msg.size());
        auto session = sessionPtr_.lock();
        if (session) {
            session->Send(bufSend, bufSend->Size());
        }
    }

    void OnSessionClose(int32_t fd) override
    {
        SHARING_LOGD("OnSessionClose fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
        auto server = serverPtr_.lock();
        if (server) {
            server->CloseClientSocket(fd);
        }
    }

    void OnSessionException(int32_t fd) override
    {
        SHARING_LOGD("OnSessionException fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
        auto server = serverPtr_.lock();
        if (server) {
            server->CloseClientSocket(fd);
        }
    }

private:
    std::weak_ptr<INetworkSession> sessionPtr_;
    std::weak_ptr<IServer> serverPtr_;
};

class ServerAgent final : public IServerCallback,
                          public std::enable_shared_from_this<ServerAgent> {
public:
    explicit ServerAgent(bool tcpOrUdp) : tcpOrUdp_(tcpOrUdp)
    {
        SHARING_LOGD("trace.");
    }

    ~ServerAgent()
    {
        SHARING_LOGD("trace.");
        if (serverPtr_) {
            serverPtr_->Stop();
        }
    }

    void OnAccept(std::weak_ptr<INetworkSession> session) override
    {
        SHARING_LOGD("trace.");
        auto sessionStr = session.lock();
        if (sessionStr) {
            auto sa = std::make_shared<SessionAgent>(session, serverPtr_);
            sessionStr->RegisterCallback(std::move(sa));
            sessionStr->Start();
            static int index = 0;
            DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
            string msg = "tcp server accept send message=" + std::to_string(index++);
            buf->PushData(msg.c_str(), msg.size());
            int sendSize = buf->Size();
            sessionStr->Send(std::move(buf), sendSize);
        }
    }

    void Start()
    {
        if (tcpOrUdp_) {
            bool ret = NetworkFactory::CreateTcpServer(8888, shared_from_this(), serverPtr_, "");
            if (!ret) {
                SHARING_LOGE("TCP server start failed");
            }
        } else {
            bool ret = NetworkFactory::CreateUdpServer(9999, "", shared_from_this(), serverPtr_);
            if (!ret) {
                SHARING_LOGE("UDP server start failed");
            }
        }
        SHARING_LOGD("server agent started");
    }
    void OnServerReadData(int32_t fd, DataBuffer::Ptr buf, INetworkSession::Ptr session) override
    {
        SHARING_LOGD("OnServerReadData fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
    }

    void OnServerWriteable(int32_t fd) override
    {
        SHARING_LOGD("OnServerWriteable fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
    }

    void OnServerClose(int32_t fd) override
    {
        SHARING_LOGD("OnServerClose fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
        serverPtr_->Stop();
    }

    void OnServerException(int32_t fd) override
    {
        SHARING_LOGD("OnServerException fd: %{public}d,thread_id: %{public}llu.", fd, GetThreadId());
        serverPtr_->Stop();
    }

    NetworkFactory::ServerPtr serverPtr_{nullptr};
    INetworkSession::Ptr sessionPtr_ = nullptr;
    bool tcpOrUdp_ = true;
    std::vector<INetworkSession::Ptr> sessionPtrVec_;
};

class ServerManager {
public:
    ~ServerManager()
    {
        SHARING_LOGD("trace.");
        serverAgents_.Clear();
    }

    ServerManager()
    {
        SHARING_LOGD("trace.");
        serverAgents_.Clear();
    }

    void CreateTcpServer()
    {
        SHARING_LOGD("CreateTcpServer");
        serverAgent_ = std::make_shared<ServerAgent>(true);
        serverAgent_->Start();
    }

    void CreateUdpServer()
    {
        SHARING_LOGD("CreateUdpServer");
        serverAgent_ = std::make_shared<ServerAgent>(false);
        serverAgent_->Start();
    }

    void StopServer()
    {
        if (serverAgent_) {
            if (serverAgent_->serverPtr_) {
                serverAgent_->serverPtr_->Stop();
            }
        }
    }
    std::shared_ptr<ServerAgent> serverAgent_;
    OHOS::SafeMap<uint32_t, std::shared_ptr<ServerAgent>> serverAgents_;
    uint32_t serverIdNum = 0;
};

int main()
{
    SHARING_LOGD("trace");
    std::thread run_server([]() {
        ServerManager serverMgr;
        std::stringstream ss;
        std::string inputCmd;

        SHARING_LOGD("Please input command.tcp-1,udp-2:");
        getline(std::cin, inputCmd);
        SHARING_LOGD("Get command: %{public}s.", inputCmd.c_str());
        if (inputCmd == "quit") {
        } else if (inputCmd == "1") {
            serverMgr.CreateTcpServer();

        } else if (inputCmd == "2") {
            serverMgr.CreateUdpServer();

        } else if (inputCmd == "3") {
        }
        while (1) {
            sleep(1);
        }
    });
    run_server.join();
    return 0;
}