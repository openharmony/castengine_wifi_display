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
#include <sstream>
#include <thread>
#include <unistd.h>
#include "common/sharing_log.h"
#include "network/interfaces/iclient_callback.h"
#include "network/network_factory.h"

using namespace std;
using namespace OHOS::Sharing;

class ClientAgent final : public IClientCallback,
                          public std::enable_shared_from_this<ClientAgent> {
public:
    ~ClientAgent()
    {
        SHARING_LOGD("===[ ~ClientAgent()]  ");
        clientPtr_->Disconnect();
    }
    ClientAgent()
    {
        SHARING_LOGD("===[ ClientAgent()] ");
    }

    void UdpConnect()
    {
        bool ret = NetworkFactory::CreateUdpClient("127.0.0.1", 9999, "127.0.0.1", 0, shared_from_this(), clientPtr_);
        if (ret) {
            SHARING_LOGD("===[ClientAgent] CreateUdpClient success");
        } else {
            SHARING_LOGE("===[ClientAgent] CreateUdpClient failed");
        }
    }

    void TcpConnect()
    {
        bool ret = NetworkFactory::CreateTcpClient("127.0.0.1", 8888, shared_from_this(),
                                                   clientPtr_); //(const IServerCallback::Ptr &)
        if (ret) {
            SHARING_LOGD("===[ClientAgent] CreateTcpClient success");
        } else {
            SHARING_LOGE("===[ClientAgent] CreateTcpClient failed");
        }
    }

    virtual void OnClientConnectResult(bool isSuccess) override
    {
        if (isSuccess) {
            SHARING_LOGD("===[ClientAgent] OnClientConnectResult success");
        } else {
            SHARING_LOGE("===[ClientAgent] OnClientConnectResult failed");
        }
    }

    virtual void OnClientReadData(int32_t fd, DataBuffer::Ptr buf) override
    {
        SHARING_LOGD("===[ClientAgent] OnClientReadData");
    }

    virtual void OnClientWriteable(int32_t fd) override
    {
        SHARING_LOGD("===[ClientAgent] OnClientWriteable");
        static int index = 0;
        string msg = "tcp/udp client OnClientConnectResult message.index=" + std::to_string(index++);
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        bufSend->PushData(msg.c_str(), msg.size());
        clientPtr_->Send(bufSend, bufSend->Size());
    }

    virtual void OnClientClose(int32_t fd) override
    {
        SHARING_LOGD("===[ClientAgent] OnClientClose");
        clientPtr_->Disconnect();
    }

    virtual void OnClientException(int32_t fd) override
    {
        SHARING_LOGD("===[ClientAgent] OnClientException");
        clientPtr_->Disconnect();
    }

    NetworkFactory::ClientPtr clientPtr_{nullptr};
};

int main()
{
    SHARING_LOGD("trace");
    std::thread run_client([]() {
        std::string inputCmd;
        auto clientAgent = std::make_shared<ClientAgent>();

        SHARING_LOGD("===[CLIENT DEMO] Please input command.tcp-1,udp-2:");
        getline(std::cin, inputCmd);
        SHARING_LOGD("===[CLIENT DEMO] Get command: %{public}s", inputCmd.c_str());
        if (inputCmd == "quit") {
        } else if (inputCmd == "1") {
            clientAgent->TcpConnect();
        } else if (inputCmd == "2") {
            clientAgent->UdpConnect();
        } else if (inputCmd == "3") {
            SHARING_LOGD("===[CLIENT DEMO] ===send message:");
        }
        while (1) {
            sleep(1);
        }
    });
    run_client.join();
    return 0;
}