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

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "common/sharing_log.h"
#include "network/interfaces/iclient_callback.h"
#include "network/interfaces/iserver_callback.h"
#include "network/interfaces/isession_callback.h"
#include "network/network_factory.h"

using namespace std;
using namespace OHOS::Sharing;
using namespace testing::ext;

namespace OHOS {
namespace Sharing {
class NetworkUdpUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class UdpTestAgent final : public IServerCallback,
                           public IClientCallback,
                           public ISessionCallback,
                           public std::enable_shared_from_this<UdpTestAgent> {
public:
    UdpTestAgent() {}

    virtual ~UdpTestAgent()
    {
        if (serverPtr_) {
            serverPtr_->Stop();
        }
    }

    bool StartUdpServer(uint16_t port, const std::string &localIp)
    {
        bool ret = NetworkFactory::CreateUdpServer(port, localIp, shared_from_this(), serverPtr_);
        if (!ret) {
            SHARING_LOGE("===[UdpTestAgent] StartUdpServer failed");
        } else {
            SHARING_LOGD("===[UdpTestAgent] server agent started");
        }
        return ret;
    }

    bool StartUdpClient(const std::string &peerIp, uint16_t peerPort, const std::string &localIp, uint16_t localPort)
    {
        bool ret =
            NetworkFactory::CreateUdpClient(peerIp, peerPort, localIp, localPort, shared_from_this(), clientPtr_);
        if (!ret) {
            SHARING_LOGE("===[UdpTestAgent] StartUdpClient failed");
        } else {
            SHARING_LOGD("===[UdpTestAgent] client agent started");
        }
        return ret;
    }

    virtual void OnAccept(INetworkSession::Ptr session) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerAccept");
    }

    virtual void OnServerReadData(int32_t fd, const DataBuffer::Ptr &buf, const INetworkSession::Ptr session) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerReadData");
        static int index = 0;
        buf->PrintBuf();
        if (session) {
            sessionPtrVec_.push_back(session);
        }
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        const DataBuffer::Ptr &bufRefs = bufSend;
        string msg = "udp server message.index=" + std::to_string(index++);
        bufSend->PushData(msg.c_str(), msg.size());
        size_t nSize = sessionPtrVec_.size();
        for (size_t i = 0; i < nSize; i++) {
            sessionPtrVec_[i]->Send(bufRefs, bufRefs->Size());
            sleep(2);
        }
    }

    virtual void OnServerWriteable(int32_t fd) override
    {
        SHARING_LOGD("onServerWriteable");
    }

    virtual void OnServerClose(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerClose");
        serverPtr_ = nullptr;
    }

    virtual void OnServerException(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerException");
        serverPtr_ = nullptr;
    }

    virtual void OnSessionReadData(int32_t fd, const DataBuffer::Ptr &buf) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionReadData");
        buf->PrintBuf();
        static int index = 0;
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        string msg = "udp session send message=" + std::to_string(index++);
        bufSend->PushData(msg.c_str(), msg.size());
        size_t nSize = sessionPtrVec_.size();
        for (size_t i = 0; i < nSize; i++) {
            if (sessionPtrVec_[i]->GetSocketInfo()->GetPeerFd() == fd) {
                sessionPtrVec_[i]->Send(bufSend, bufSend->Size());
                break;
            }
        }
        sleep(3);
    }

    virtual void OnSessionWriteable(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionWriteable");
    }

    virtual void OnSessionClose(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionClose");
    }

    virtual void OnSessionException(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionException");
    }

    virtual void OnClientConnectResult(bool isSuccess) override
    {
        if (isSuccess) {
            SHARING_LOGD("===[UdpTestAgent] OnClientConnectResult success");
            static int index = 0;
            string msg = "udp client OnClientConnectResult message.index=" + std::to_string(index++);
            DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
            bufSend->PushData(msg.c_str(), msg.size());
            if (clientPtr_ != nullptr) {
                SHARING_LOGD("===[UdpTestAgent] OnClientConnectResult send");
                clientPtr_->Send(bufSend, bufSend->Size());
            } else {
                SHARING_LOGD("===[UdpTestAgent] OnClientConnectResult nullptr");
            }
        } else {
            if (clientPtr_ != nullptr) {
                SHARING_LOGE("===[UdpTestAgent] OnClientConnectResult failed");
                clientPtr_->Disconnect();
            }
        }
    }

    virtual void OnClientReadData(int32_t fd, const DataBuffer::Ptr &buf) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnClientReadData");
        buf->PrintBuf();
        static int index = 0;
        string msg = "udp client message.index=" + std::to_string(index++);
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        bufSend->PushData(msg.c_str(), msg.size());
        clientPtr_->Send(bufSend, bufSend->Size());
        sleep(3);
    }

    virtual void OnClientWriteable(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnClientWriteable");
    }

    virtual void OnClientClose(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnClientClose");
        clientPtr_ = nullptr;
    }

    virtual void OnClientException(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnClientException");
        clientPtr_ = nullptr;
    }

private:
    NetworkFactory::ServerPtr serverPtr_{nullptr};
    NetworkFactory::ClientPtr clientPtr_{nullptr};
    std::vector<INetworkSession::Ptr> sessionPtrVec_;
};

namespace {
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_001, TestSize.Level0)
{
    auto udpAgent = std::make_shared<UdpTestAgent>();
    ASSERT_TRUE(udpAgent != nullptr);
}

HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_002, TestSize.Level0)
{
    auto udpAgent = std::make_shared<UdpTestAgent>();
    ASSERT_TRUE(udpAgent != nullptr);
    auto ret = udpAgent->StartUdpServer(8888, "");
    ASSERT_TRUE(ret);
}

HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_003, TestSize.Level0)
{
    auto udpAgent = std::make_shared<UdpTestAgent>();
    ASSERT_TRUE(udpAgent != nullptr);
    auto ret = udpAgent->StartUdpServer(8889, "");
    ASSERT_TRUE(ret);
    ret = udpAgent->StartUdpClient("127.0.0.1", 8889, "127.0.0.1", 0);
    ASSERT_TRUE(ret);
}

HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_004, TestSize.Level0)
{
    auto udpAgent = std::make_shared<UdpTestAgent>();
    ASSERT_TRUE(udpAgent != nullptr);
    auto ret = udpAgent->StartUdpServer(8890, "");
    ASSERT_TRUE(ret);
    ret = udpAgent->StartUdpClient("127.0.0.1", 1888, "127.0.0.1", 0);
    ASSERT_TRUE(ret);
}

HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_005, TestSize.Level0)
{
    auto udpAgent = std::make_shared<UdpTestAgent>();
    ASSERT_TRUE(udpAgent != nullptr);
    auto ret = udpAgent->StartUdpServer(8889, "");
    ASSERT_TRUE(ret);
    ret = udpAgent->StartUdpClient("1212121", 8889, "127.0.0.1", 0);
    ASSERT_FALSE(ret);
}

HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_006, TestSize.Level0)
{
    auto udpAgent = std::make_shared<UdpTestAgent>();
    ASSERT_TRUE(udpAgent != nullptr);
    auto ret = udpAgent->StartUdpServer(8891, "");
    ASSERT_TRUE(ret);
    ret = udpAgent->StartUdpClient("1212121", 8891, "127.0.0.1", 0);
    ASSERT_FALSE(ret);
}
} // namespace
} // namespace Sharing
} // namespace OHOS