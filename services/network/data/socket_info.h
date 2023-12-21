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

#ifndef OHOS_SHARING_SOCKET_INFO_H
#define OHOS_SHARING_SOCKET_INFO_H

#include <memory>
#include <netinet/in.h>
#include <string>
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
enum SocketType { SOCKET_TYPE_INVALID, SOCKET_TYPE_TCP, SOCKET_TYPE_UDP };

enum SocketState { SOCKET_STATE_INIT, SOCKET_STATE_CONNECTED, SOCKET_STATE_LISTENING, SOCKET_STATE_CLOSED };

class SocketInfo {
public:
    using Ptr = std::shared_ptr<SocketInfo>;
    using UniPtr = std::unique_ptr<SocketInfo>;

    SocketInfo()
    {
        MEDIA_LOGD("trace.");
    }

    explicit SocketInfo(std::string localIp, std::string peerIp, int32_t socketLocalFd, int32_t socketPeerFd,
                        uint16_t localPort, uint16_t peerPort)
        : peerPort_(peerPort), socketPeerFd_(socketPeerFd), peerIp_(peerIp), localPort_(localPort),
          socketLocalFd_(socketLocalFd), localIp_(localIp)
    {
        MEDIA_LOGD("trace.");
    }

    virtual ~SocketInfo()
    {
        MEDIA_LOGD("trace.");
    }

    std::string GetLocalIp() const
    {
        MEDIA_LOGD("trace.");
        return localIp_;
    }

    std::string GetPeerIp() const
    {
        MEDIA_LOGD("trace.");
        return peerIp_;
    }

    uint16_t GetLocalPort()
    {
        MEDIA_LOGD("trace.");
        return localPort_;
    }

    uint16_t GetPeerPort()
    {
        MEDIA_LOGD("trace.");
        return peerPort_;
    }

    SocketType GetSocketType() const
    {
        MEDIA_LOGD("trace.");
        return socketType_;
    }

    int32_t GetLocalFd() const
    {
        MEDIA_LOGD("trace.");
        return socketLocalFd_;
    }

    int32_t GetPeerFd() const
    {
        MEDIA_LOGD("trace.");
        return socketPeerFd_;
    }

    void SetSocketState(SocketState state)
    {
        MEDIA_LOGD("trace.");
        socketState_ = state;
    }

    SocketState GetSocketState() const
    {
        MEDIA_LOGD("trace.");
        return socketState_;
    }

    void SetSocketType(SocketType type)
    {
        MEDIA_LOGD("trace.");
        socketType_ = type;
    }

    SocketType GetSocketType()
    {
        MEDIA_LOGD("trace.");
        return socketType_;
    }

public:
    uint16_t peerPort_ = 0;
    int32_t socketPeerFd_ = -1;

    std::string peerIp_;

    struct sockaddr_in udpClientAddr_;
    socklen_t udpClientLen_ = sizeof(struct sockaddr_in);

protected:
    uint16_t localPort_ = 0;
    int32_t socketLocalFd_ = -1;

    std::string localIp_;

    SocketType socketType_ = SOCKET_TYPE_INVALID;
    SocketState socketState_ = SOCKET_STATE_INIT;
};
} // namespace Sharing
} // namespace OHOS
#endif