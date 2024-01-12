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

#include "udp_socket.h"
#include "common/media_log.h"
#include "network/socket/socket_utils.h"

namespace OHOS {
namespace Sharing {
UdpSocket::UdpSocket()
{
    SHARING_LOGD("trace.");
}

UdpSocket::~UdpSocket()
{
    SHARING_LOGD("trace.");
}

bool UdpSocket::Bind(const uint16_t port, const std::string &host, bool enableReuse, uint32_t backlog)
{
    SHARING_LOGD("trace.");
    int32_t fd = -1;
    if (!SocketUtils::CreateSocket(SOCK_DGRAM, fd)) {
        SHARING_LOGE("bind CreateSocket Failed!");
        return false;
    }
    socketLocalFd_ = fd;

    if (enableReuse) {
        SocketUtils::SetReuseAddr(fd, enableReuse);
        SocketUtils::SetReusePort(fd, enableReuse);
    }

    SocketUtils::SetNonBlocking(fd);
    SocketUtils::SetSendBuf(fd);
    SocketUtils::SetRecvBuf(fd);
    SocketUtils::SetCloseWait(fd);
    SocketUtils::SetCloExec(fd, true);

    if (!SocketUtils::BindSocket(fd, host, port)) {
        SocketUtils::ShutDownSocket(fd);
        SHARING_LOGE("bind BindSocket Failed!");
        return false;
    }

    localIp_ = host;
    localPort_ = port;
    socketType_ = SOCKET_TYPE_UDP;
    socketState_ = SOCKET_STATE_INIT;
    return true;
}

bool UdpSocket::Connect(const std::string &peerIp, uint16_t peerPort, int32_t &retCode, bool isAsync, bool enableReuse,
                        const std::string &localIp, uint16_t localPort)
{
    SHARING_LOGD("trace.");
    int32_t fd = -1;
    if (!SocketUtils::CreateSocket(SOCK_DGRAM, fd)) {
        return false;
    }
    socketLocalFd_ = fd;

    if (enableReuse) {
        SocketUtils::SetReuseAddr(fd, enableReuse);
        SocketUtils::SetReusePort(fd, enableReuse);
    }

    SocketUtils::SetNonBlocking(fd, isAsync);
    SocketUtils::SetSendBuf(fd);
    SocketUtils::SetRecvBuf(fd);
    SocketUtils::SetCloseWait(fd);
    SocketUtils::SetCloExec(fd, true);

    if (SocketUtils::ConnectSocket(fd, isAsync, peerIp, peerPort, retCode)) {
        SHARING_LOGD("connect success .fd: %{public}d.", fd);
        localIp_ = localIp;
        peerIp_ = peerIp;
        localPort_ = localPort;
        peerPort_ = peerPort;
        socketType_ = SOCKET_TYPE_UDP;
        socketState_ = SOCKET_STATE_CONNECTED;
        return true;
    }
    SHARING_LOGE("connect failed!");
    return false;
}
} // namespace Sharing
} // namespace OHOS
