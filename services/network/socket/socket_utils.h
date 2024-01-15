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

#ifndef OHOS_SHARING_SOCKET_UTILS_H
#define OHOS_SHARING_SOCKET_UTILS_H

#include "base_socket.h"
#include "utils/data_buffer.h"
namespace OHOS {
namespace Sharing {
constexpr int32_t DEAFULT_READ_BUFFER_SIZE = 1500;
constexpr uint32_t SOCKET_DEFAULT_BUF_SIZE = 256 * 1024;
constexpr uint32_t READ_BUF_SIZE = 2048;
constexpr uint32_t MAX_READ_BUF_SIZE = 102400000;
class SocketUtils {
public:
    static bool CreateUdpSession(unsigned port, int32_t &fd);
    static bool CreateSocket(int32_t socketType, int32_t &fd);
    static bool CreateTcpServer(const char *ip, unsigned port, int32_t &fd);
    static bool CreateTcpClient(const char *ip, unsigned port, int32_t &fd, int32_t &ret);
    
    static bool CheckAsyncConnect(int32_t fd);
    static bool ListenSocket(int32_t fd, uint32_t backlog = MAX_BACKLOG_NUM);
    static bool BindSocket(int32_t fd, const std::string &host, uint16_t port);
    static bool ConnectSocket(int32_t fd, bool isAsync, const std::string &ip, uint16_t port, int32_t &ret);

    static void SetKeepAlive(int32_t sockfd);
    static bool SetReuseAddr(int32_t fd, bool isOn);
    static bool SetReusePort(int32_t fd, bool isOn);
    static bool SetCloExec(int32_t fd, bool isOn = true);
    static bool SetNoDelay(int32_t fd, bool isOn = true);
    static bool SetCloseWait(int32_t fd, int32_t second = 0);
    static bool SetSendBuf(int32_t fd, int32_t size = SOCKET_DEFAULT_BUF_SIZE);
    static bool SetRecvBuf(int32_t fd, int32_t size = SOCKET_DEFAULT_BUF_SIZE);
    static bool SetNonBlocking(int32_t fd, bool isNonBlock = true, uint32_t write_timeout = 0);

    static int32_t ReadSocket(int32_t fd, DataBuffer::Ptr buf, int32_t &error);
    static int32_t ReadSocket(int32_t fd, char *buf, uint32_t len, int32_t &error);

    static void CloseSocket(int32_t fd);
    static void ShutDownSocket(int32_t fd);
    static bool IsUdpPortAvailable(uint16_t port);

    static int32_t SendSocket(int32_t fd, const char *buf, int32_t len);
    static int32_t AcceptSocket(int32_t fd, struct sockaddr_in *clientAddr, socklen_t *addrLen);
    static int32_t RecvSocket(int32_t fd, char *buf, uint32_t len, int32_t flags, int32_t &error);
    static int32_t Sendto(int32_t fd, const char *buf, size_t len, const char *ip, int32_t nPort);

    static uint16_t GetAvailableUdpPortPair();
    static uint16_t GetAvailableUdpPortPair(uint16_t minPort, uint16_t maxPort);
    static bool GetIpPortInfo(int32_t fd, std::string &strLocalAddr, std::string &strRemoteAddr, uint16_t &localPort,
                              uint16_t &remotePort);

public:
    static uint16_t minPort_;
    static uint16_t maxPort_;
};
} // namespace Sharing
} // namespace OHOS
#endif