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
#include "network/interfaces/inetwork_session_callback.h"
#include "network/interfaces/iserver_callback.h"
#include "network/network_factory.h"
#include "network/server/udp_server.h"
#include "network/client/udp_client.h"

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
                           public INetworkSessionCallback,
                           public std::enable_shared_from_this<UdpTestAgent> {
public:
    UdpTestAgent() {}

    ~UdpTestAgent()
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

    void OnAccept(std::weak_ptr<INetworkSession> session) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerAccept");
    }

    void OnServerReadData(int32_t fd, DataBuffer::Ptr buf, INetworkSession::Ptr session) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerReadData");
        static int index = 0;
        if (session) {
            sessionPtrVec_.push_back(session);
        }
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        string msg = "udp server message.index=" + std::to_string(index++);
        bufSend->PushData(msg.c_str(), msg.size());
        size_t nSize = sessionPtrVec_.size();
        for (size_t i = 0; i < nSize; i++) {
            sleep(2);
        }
    }

    void OnServerWriteable(int32_t fd) override
    {
        SHARING_LOGD("onServerWriteable");
    }

    void OnServerClose(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerClose");
        serverPtr_ = nullptr;
    }

    void OnServerException(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnServerException");
        serverPtr_ = nullptr;
    }

    void OnSessionReadData(int32_t fd, DataBuffer::Ptr buf) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionReadData");
        static int index = 0;
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        string msg = "udp session send message=" + std::to_string(index++);
        bufSend->PushData(msg.c_str(), msg.size());
        size_t nSize = sessionPtrVec_.size();
        for (size_t i = 0; i < nSize; i++) {
            if (sessionPtrVec_[i]->GetSocketInfo()->GetPeerFd() == fd) {
                break;
            }
        }
        sleep(3);
    }

    void OnSessionWriteable(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionWriteable");
    }

    void OnSessionClose(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionClose");
    }

    void OnSessionException(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnSessionException");
    }

    void OnClientConnect(bool isSuccess) override
    {
        if (isSuccess) {
            SHARING_LOGD("===[UdpTestAgent] OnClientConnect success");
            static int index = 0;
            string msg = "udp client OnClientConnect message.index=" + std::to_string(index++);
            DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
            bufSend->PushData(msg.c_str(), msg.size());
            if (clientPtr_ != nullptr) {
                SHARING_LOGD("===[UdpTestAgent] OnClientConnect send");
            } else {
                SHARING_LOGD("===[UdpTestAgent] OnClientConnect nullptr");
            }
        } else {
            if (clientPtr_ != nullptr) {
                SHARING_LOGE("===[UdpTestAgent] OnClientConnect failed");
                clientPtr_->Disconnect();
            }
        }
    }

    void OnClientReadData(int32_t fd, DataBuffer::Ptr buf) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnClientReadData");
        static int index = 0;
        string msg = "udp client message.index=" + std::to_string(index++);
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        bufSend->PushData(msg.c_str(), msg.size());
        sleep(3);
    }

    void OnClientWriteable(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnClientWriteable");
    }

    void OnClientClose(int32_t fd) override
    {
        SHARING_LOGD("===[UdpTestAgent] OnClientClose");
        clientPtr_ = nullptr;
    }

    void OnClientException(int32_t fd) override
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

//测试服务器start
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_001, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
}

// 测试服务器在端口已被占用时的start
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_002, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1", false);
    auto udpServer1 = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer1 != nullptr);
    ret = udpServer1->Start(8888, "127.0.0.1", false);
    ASSERT_FALSE(ret);
}

// 测试server和client对连
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_003, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto udpClient = std::make_shared<UdpClient>();
    ASSERT_TRUE(udpClient != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    ret = udpClient->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
}

// 测试client的disconnect
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_004, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    clientPtr->Disconnect();
}

// 测试server的disconnect
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_005, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    udpServer->CloseClientSocket(udpServer->GetSocketInfo()->GetLocalFd());
}

// 测试端口没有被监听时的连接
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_006, TestSize.Level0)
{
    auto udpClient = std::make_shared<UdpClient>();
    ASSERT_TRUE(udpClient != nullptr);
    auto ret = udpClient->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_FALSE(ret);
}


// 测试ip地址不合法时的连接
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_007, TestSize.Level0)
{
    auto udpClient = std::make_shared<UdpClient>();
    ASSERT_TRUE(udpClient != nullptr);
    auto ret = udpClient->Connect("192.55.1", 28888, "192.55.1.2", 8889);
    ASSERT_FALSE(ret);
}

// 测试没有连接时的disconnect
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_008, TestSize.Level0)
{
    auto udpClient = std::make_shared<UdpClient>();
    ASSERT_TRUE(udpClient != nullptr);
    udpClient->Disconnect();
}

// 测试连接时的Send(const std::string &msg)
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_009, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    ret = clientPtr->Send(std::string("hello world"));
    ASSERT_TRUE(ret);
}

// 测试连接时的Send(const char *buf, int32_t nSize)
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_010, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    char testString[] = "hello world";
    size_t testStringLen = strlen(testString);
    ret = clientPtr->Send(testString, testStringLen);
    ASSERT_TRUE(ret);
}

// 测试连接时的Send(const DataBuffer::Ptr &buf, int32_t nSize)
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_011, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    auto buf = std::make_shared<DataBuffer>();
    ASSERT_TRUE(buf != nullptr);
    char testString[] = "hello world";
    size_t testStringLen = strlen(testString);
    buf->PushData(testString, testStringLen);
    ret = clientPtr->Send(buf, testStringLen);
    ASSERT_TRUE(ret);
}

// 测试尚未连接时的Send(const std::string &msg)
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_012, TestSize.Level0)
{
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    auto ret = clientPtr->Send(std::string("hello world"));
    ASSERT_FALSE(ret);
}

// 测试尚未连接时的Send(const char *buf, int32_t nSize)
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_013, TestSize.Level0)
{
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    char testString[] = "hello world";
    size_t testStringLen = strlen(testString);
    auto ret = clientPtr->Send(testString, testStringLen);
    ASSERT_FALSE(ret);
}

// 测试尚未连接时的Send(const DataBuffer::Ptr &buf, int32_t nSize)
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_014, TestSize.Level0)
{
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    auto buf = std::make_shared<DataBuffer>();
    ASSERT_TRUE(buf != nullptr);
    char testString[] = "hello world";
    size_t testStringLen = strlen(testString);
    buf->PushData(testString, testStringLen);
    auto ret = clientPtr->Send(buf, testStringLen);
    ASSERT_FALSE(ret);
}

// 测试连接后client是否可获取SocketInfo
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_015, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    auto socketInfo = clientPtr->GetSocketInfo();
    ASSERT_TRUE(socketInfo != nullptr);
}

// 测试连接后server是否可获取SocketInfo
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_016, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    auto socketInfo = udpServer->GetSocketInfo();
    ASSERT_TRUE(socketInfo != nullptr);
}

// 测试连接后client获取的SocketInfo是否正确
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_017, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    auto socketInfo = clientPtr->GetSocketInfo();
    ASSERT_TRUE(socketInfo != nullptr);
    EXPECT_EQ(socketInfo->GetLocalIp(), "127.0.0.1");
    EXPECT_EQ(socketInfo->GetLocalPort(), 8889);
    EXPECT_NE(socketInfo->GetLocalFd(), -1);
    EXPECT_EQ(socketInfo->GetPeerIp(), "127.0.0.1");
    EXPECT_EQ(socketInfo->GetPeerPort(), 8888);
}

// 测试连接后Server获取的SocketInfo是否正确
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_018, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto ret = udpServer->Start(8888, "127.0.0.1");
    ASSERT_TRUE(ret);
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
    ASSERT_TRUE(ret);
    auto socketInfo = udpServer->GetSocketInfo();
    ASSERT_TRUE(socketInfo != nullptr);
    EXPECT_EQ(socketInfo->GetLocalIp(), "127.0.0.1");
    EXPECT_EQ(socketInfo->GetLocalPort(), 8888);
    EXPECT_NE(socketInfo->GetLocalFd(), -1);
}

// 测试未连接时client是否能获取SocketInfo
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_019, TestSize.Level0)
{
    auto clientPtr = std::make_shared<UdpClient>();
    ASSERT_TRUE(clientPtr != nullptr);
    auto socketInfo = clientPtr->GetSocketInfo();
    ASSERT_TRUE(socketInfo == nullptr);
}

// 测试未监听时server是否可获取SocketInfo
HWTEST_F(NetworkUdpUnitTest, NetworkUdpUnitTest_020, TestSize.Level0)
{
    auto udpServer = std::make_shared<UdpServer>();
    ASSERT_TRUE(udpServer != nullptr);
    auto socketInfo = udpServer->GetSocketInfo();
    ASSERT_TRUE(socketInfo == nullptr);
}


} // namespace
} // namespace Sharing
} // namespace OHOS