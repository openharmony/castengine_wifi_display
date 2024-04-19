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
#include "network/server/tcp_server.h"
#include "network/client/tcp_client.h"

using namespace std;
using namespace OHOS::Sharing;
using namespace testing::ext;

namespace OHOS {
namespace Sharing {

class NetworkTcpUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class TcpTestAgent final : public IServerCallback,
                           public IClientCallback,
                           public INetworkSessionCallback,
                           public std::enable_shared_from_this<TcpTestAgent> {
public:
    TcpTestAgent() {}

    ~TcpTestAgent()
    {
        if (serverPtr_) {
            serverPtr_->Stop();
        }
    }

    bool StartTcpServer(uint16_t port)
    {
        bool ret = NetworkFactory::CreateTcpServer(port, shared_from_this(), serverPtr_);
        if (!ret) {
            SHARING_LOGE("===[TcpTestAgent] StartTcpServer failed");
        } else {
            SHARING_LOGD("===[TcpTestAgent] server agent started");
        }
        return ret;
    }

    bool StartTcpClient(const std::string &peerIp, uint16_t port)
    {
        bool ret = NetworkFactory::CreateTcpClient(peerIp, port, shared_from_this(), clientPtr_);
        if (!ret) {
            SHARING_LOGE("===[TcpTestAgent] StartTcpClient failed");
        } else {
            SHARING_LOGD("===[TcpTestAgent] client agent started");
        }
        return ret;
    }

    void OnAccept(std::weak_ptr<INetworkSession> sessionPtr) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnServerAccept");
        auto session = sessionPtr.lock();
        if (!session) {
            return;
        }
        session->RegisterCallback(shared_from_this());
        session->Start();
        static int index = 0;
        DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
        string msg = "tcp server accept send message=" + std::to_string(index++);
        buf->PushData(msg.c_str(), msg.size());
        sessionPtrVec_.push_back(session);
    }

    void OnServerReadData(int32_t fd, DataBuffer::Ptr buf, INetworkSession::Ptr session) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnServerReadData");
        static int index = 0;
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        string msg = "tcp server message.index=" + std::to_string(index++);
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
        SHARING_LOGD("===[TcpTestAgent] OnServerClose");
        serverPtr_ = nullptr;
    }

    void OnServerException(int32_t fd) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnServerException");
        serverPtr_ = nullptr;
    }

    void OnSessionReadData(int32_t fd, DataBuffer::Ptr buf) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnSessionReadData");
        static int index = 0;
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        string msg = "tcp session send message=" + std::to_string(index++);
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
        SHARING_LOGD("===[TcpTestAgent] OnSessionWriteable");
    }

    void OnSessionClose(int32_t fd) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnSessionClose");
    }

    void OnSessionException(int32_t fd) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnSessionException");
    }

    void OnClientConnect(bool isSuccess) override
    {
        if (isSuccess) {
            SHARING_LOGD("===[TcpTestAgent] OnClientConnect success");
            static int index = 0;
            string msg = "tcp client OnClientConnect message.index=" + std::to_string(index++);
            DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
            bufSend->PushData(msg.c_str(), msg.size());
            if (clientPtr_ != nullptr) {
                SHARING_LOGD("===[TcpTestAgent] OnClientConnect send");
            } else {
                SHARING_LOGD("===[TcpTestAgent] OnClientConnect nullptr");
            }
        } else {
            if (clientPtr_ != nullptr) {
                SHARING_LOGE("===[TcpTestAgent] OnClientConnect failed");
                clientPtr_->Disconnect();
            }
        }
    }

    void OnClientReadData(int32_t fd, DataBuffer::Ptr buf) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnClientReadData");
        static int index = 0;
        string msg = "tcp client message.index=" + std::to_string(index++);
        DataBuffer::Ptr bufSend = std::make_shared<DataBuffer>();
        bufSend->PushData(msg.c_str(), msg.size());
        sleep(3);
    }

    void OnClientWriteable(int32_t fd) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnClientWriteable");
    }

    void OnClientClose(int32_t fd) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnClientClose");
        clientPtr_ = nullptr;
    }

    void OnClientException(int32_t fd) override
    {
        SHARING_LOGD("===[TcpTestAgent] OnClientException");
        clientPtr_ = nullptr;
    }

private:
    NetworkFactory::ServerPtr serverPtr_{nullptr};
    NetworkFactory::ClientPtr clientPtr_{nullptr};
    std::vector<INetworkSession::Ptr> sessionPtrVec_;
};

namespace {

    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_001, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
    }

    // 测试服务器在端口已被占用时的start
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_002, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1", false);
        auto tcpServer1 = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer1 != nullptr);
        ret = tcpServer1->Start(8888, "127.0.0.1", false);
        ASSERT_FALSE(ret);
    }

    // 测试server和client对连
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_003, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto tcpClient = std::make_shared<TcpClient>();
        ASSERT_TRUE(tcpClient != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        ret = tcpClient->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
    }

    // 测试client的disconnect
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_004, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
        clientPtr->Disconnect();
    }

    // 测试server的disconnect
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_005, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
        tcpServer->CloseClientSocket(tcpServer->GetSocketInfo()->GetLocalFd());
    }

    // 测试ip地址不合法时的连接
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_006, TestSize.Level0)
    {
        auto tcpClient = std::make_shared<TcpClient>();
        ASSERT_TRUE(tcpClient != nullptr);
        auto ret = tcpClient->Connect("192.55.1", 8888, "192.55.1", 8889);
        ASSERT_FALSE(ret);
    }

    // 测试端口未被监听时的连接
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_007, TestSize.Level0)
    {
        auto tcpClient = std::make_shared<TcpClient>();
        ASSERT_TRUE(tcpClient != nullptr);
        auto ret = tcpClient->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_FALSE(ret);
    }


    // 测试没有连接时的disconnect
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_008, TestSize.Level0)
    {
        auto tcpClient = std::make_shared<TcpClient>();
        ASSERT_TRUE(tcpClient != nullptr);
        tcpClient->Disconnect();
    }

    // 测试连接时的Send(const std::string &msg)
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_009, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
        ret = clientPtr->Send(std::string("hello world"));
        ASSERT_TRUE(ret);
    }

    // 测试连接时的Send(const char *buf, int32_t nSize)
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_010, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
        char testString[] = "hello world";
        size_t testStringLen = strlen(testString);
        ret = clientPtr->Send(testString, testStringLen);
        ASSERT_TRUE(ret);
    }

    // 测试连接时的Send(const DataBuffer::Ptr &buf, int32_t nSize)
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_011, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
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
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_012, TestSize.Level0)
    {
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        auto ret = clientPtr->Send(std::string("hello world"));
        ASSERT_FALSE(ret);
    }

    // 测试尚未连接时的Send(const char *buf, int32_t nSize)
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_013, TestSize.Level0)
    {
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        char testString[] = "hello world";
        size_t testStringLen = strlen(testString);
        auto ret = clientPtr->Send(testString, testStringLen);
        ASSERT_FALSE(ret);
    }

    // 测试尚未连接时的Send(const DataBuffer::Ptr &buf, int32_t nSize)
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_014, TestSize.Level0)
    {
        auto clientPtr = std::make_shared<TcpClient>();
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
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_015, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
        auto socketInfo = clientPtr->GetSocketInfo();
        ASSERT_TRUE(socketInfo != nullptr);
    }

    // 测试连接后server是否可获取SocketInfo
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_016, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
        auto socketInfo = tcpServer->GetSocketInfo();
        ASSERT_TRUE(socketInfo != nullptr);
    }

    // 测试连接后client获取的SocketInfo是否正确
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_017, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
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
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_018, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto ret = tcpServer->Start(8888, "127.0.0.1");
        ASSERT_TRUE(ret);
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        ret = clientPtr->Connect("127.0.0.1", 8888, "127.0.0.1", 8889);
        ASSERT_TRUE(ret);
        auto socketInfo = tcpServer->GetSocketInfo();
        ASSERT_TRUE(socketInfo != nullptr);
        EXPECT_EQ(socketInfo->GetLocalIp(), "127.0.0.1");
        EXPECT_EQ(socketInfo->GetLocalPort(), 8888);
        EXPECT_NE(socketInfo->GetLocalFd(), -1);
    }

    // 测试未连接时client是否能获取SocketInfo
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_019, TestSize.Level0)
    {
        auto clientPtr = std::make_shared<TcpClient>();
        ASSERT_TRUE(clientPtr != nullptr);
        auto socketInfo = clientPtr->GetSocketInfo();
        ASSERT_TRUE(socketInfo == nullptr);
    }

    // 测试未监听时server是否可获取SocketInfo
    HWTEST_F(NetworkTcpUnitTest, NetworkTcpUnitTest_020, TestSize.Level0)
    {
        auto tcpServer = std::make_shared<TcpServer>();
        ASSERT_TRUE(tcpServer != nullptr);
        auto socketInfo = tcpServer->GetSocketInfo();
        ASSERT_TRUE(socketInfo == nullptr);
    }

} // namespace
} // namespace Sharing
} // namespace OHOS