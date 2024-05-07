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

#include "tcp_socket.h"
#include "common/media_log.h"
#include "network/socket/socket_utils.h"
#include "utils.h"

namespace OHOS {
namespace Sharing {
TcpSocket::TcpSocket()
{
    SHARING_LOGD("trace.");
}

TcpSocket::~TcpSocket()
{
    SHARING_LOGD("trace.");
}

bool TcpSocket::Bind(const uint16_t port, const std::string &host, bool enableReuse, uint32_t backlog)
{
    SHARING_LOGD("trace.");
    int32_t fd = -1;
    if (!SocketUtils::CreateSocket(SOCK_STREAM, fd)) {
        SHARING_LOGE("bind CreateSocket Failed!");
        return false;
    }
    socketLocalFd_ = fd;

    if (enableReuse) {
        SocketUtils::SetReuseAddr(fd, enableReuse);
        SocketUtils::SetReusePort(fd, enableReuse);
    }

    SocketUtils::SetCloseWait(fd);
    SocketUtils::SetCloExec(fd, true);

    if (!SocketUtils::BindSocket(fd, host, port)) {
        SHARING_LOGE("bind BindSocket Failed!");
        SocketUtils::ShutDownSocket(fd);
        return false;
    }

    if (!SocketUtils::ListenSocket(fd, backlog)) {
        SHARING_LOGE("bind ListenSocket Failed!");
        SocketUtils::ShutDownSocket(fd);
        return false;
    }

    localIp_ = host;
    localPort_ = port;
    socketType_ = SOCKET_TYPE_TCP;
    SetSocketState(SOCKET_STATE_LISTENING);
    return true;
}

bool TcpSocket::Connect(const std::string &peerIp, uint16_t peerPort, int32_t &retCode, bool isAsync, bool enableReuse,
                        const std::string &localIp, uint16_t localPort)
{
    SHARING_LOGD("trace.");
    int32_t fd = -1;
    if (!SocketUtils::CreateSocket(SOCK_STREAM, fd)) {
        SHARING_LOGE("create socket failed!");
        return false;
    }

    socketLocalFd_ = fd;
    if (enableReuse) {
        SocketUtils::SetReuseAddr(fd, enableReuse);
        SocketUtils::SetReusePort(fd, enableReuse);
    }
    SocketUtils::SetNoDelay(fd, true);
    SocketUtils::SetSendBuf(fd);
    SocketUtils::SetRecvBuf(fd);
    SocketUtils::SetCloseWait(fd);
    SocketUtils::SetCloExec(fd, true);
    SocketUtils::SetKeepAlive(fd);
    SocketUtils::SetNonBlocking(fd, isAsync);

    int32_t value = 0xD0;
    setsockopt(fd, IPPROTO_IP, IP_TOS, &value, sizeof(value));

    if (!SocketUtils::ConnectSocket(fd, isAsync, peerIp, peerPort, retCode)) {
        SHARING_LOGE("connect socket failed, ip: %{public}s, port: %{public}d.", GetAnonyString(peerIp).c_str(),
                     peerPort);
        return false;
    } else {
        SHARING_LOGI("connect socket success, ip: %{public}s, port: %{public}d.", GetAnonyString(peerIp).c_str(),
                     peerPort);
    }

    localIp_ = localIp;
    localPort_ = localPort;
    peerIp_ = peerIp;
    peerPort_ = peerPort;
    socketType_ = SOCKET_TYPE_TCP;
    socketState_ = SOCKET_STATE_CONNECTED;
    return true;
}
} // namespace Sharing
} // namespace OHOS