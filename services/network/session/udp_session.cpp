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

#include "udp_session.h"
#include <arpa/inet.h>
#include <iostream>
#include "common/common_macro.h"
#include "common/media_log.h"
#include "network/socket/socket_utils.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {
UdpSession::UdpSession(SocketInfo::Ptr socket) : BaseNetworkSession(socket)
{
    SHARING_LOGD("trace.");
}

UdpSession ::~UdpSession()
{
    SHARING_LOGD("trace.");
}

bool UdpSession::Start()
{
    SHARING_LOGD("trace.");
    if (socket_) {
        SHARING_LOGD("udpSession AddFdListener.");

        auto eventRunner = OHOS::AppExecFwk::EventRunner::Create(true);
        eventHandler_ = std::make_shared<UdpSessionEventHandler>();
        eventHandler_->SetSession(shared_from_this());
        eventHandler_->SetEventRunner(eventRunner);
        eventRunner->Run();

        eventListener_ = std::make_shared<UdpSessionEventListener>();
        eventListener_->SetSession(shared_from_this());

        return eventListener_->AddFdListener(socket_->GetPeerFd(), eventListener_, eventHandler_);
    }

    return false;
}

void UdpSession::Shutdown()
{
    SHARING_LOGD("trace.");
    if (socket_) {
        if (eventListener_) {
            eventListener_->RemoveFdListener(socket_->GetPeerFd());
        }
        SocketUtils::ShutDownSocket(socket_->GetPeerFd());
        SocketUtils::CloseSocket(socket_->GetPeerFd());
        socket_.reset();
    }
}

bool UdpSession::Send(const DataBuffer::Ptr &buf, int32_t nSize)
{
    MEDIA_LOGD("trace.");
    return UdpSession::Send(buf->Peek(), nSize);
}

bool UdpSession::Send(const char *buf, int32_t nSize)
{
    MEDIA_LOGD("trace.");
    if (socket_ == nullptr) {
        SHARING_LOGE("socket nullptr!");
        return false;
    }
    MEDIA_LOGD("fd: %{public}d, address: %{public}s, port: %{public}d, len: %{public}d.", socket_->GetLocalFd(),
               GetAnonyString(inet_ntoa(socket_->udpClientAddr_.sin_addr)).c_str(), socket_->udpClientAddr_.sin_port,
               socket_->udpClientLen_);
    RETURN_FALSE_IF_NULL(buf);

    int32_t retCode = ::sendto(socket_->GetLocalFd(), buf, nSize, 0, (struct sockaddr *)&socket_->udpClientAddr_,
                               socket_->udpClientLen_);
    if ((retCode < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
        SHARING_LOGE("send error!");
        return false;
    } else if (retCode > 0) {
        return true;
    } else {
        SHARING_LOGE("sendSocket error: %{public}s!", strerror(errno));
        return false;
    }
}

void UdpSession::OnSessionReadble(int32_t fd)
{
    SHARING_LOGD("fd: %{public}d, local_fd:%{public}d, thread_id: %{public}llu.", fd, socket_->GetLocalFd(),
                 GetThreadId());
    if (socket_ == nullptr) {
        MEDIA_LOGE("socket nullptr!");
        return;
    }
    if (fd == socket_->GetLocalFd()) {
        int32_t retCode = 0;
        do {
            DataBuffer::Ptr buf = std::make_shared<DataBuffer>(DEAFULT_READ_BUFFER_SIZE);
            struct sockaddr_in clientAddr;
            socklen_t len = sizeof(struct sockaddr_in);
            retCode = ::recvfrom(fd, buf->Data(), DEAFULT_READ_BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &len);
            MEDIA_LOGD("recvSocket len: %{public}d, address: %{public}s, port: %{public}d.", retCode,
                       GetAnonyString(inet_ntoa(clientAddr.sin_addr)).c_str(), clientAddr.sin_port);

            if (retCode > 0) {
                buf->UpdateSize(retCode);
                if (callback_) {
                    callback_->OnSessionReadData(fd, std::move(buf));
                }
            } else if (retCode == 0) {
                MEDIA_LOGW("recvSocket RET CODE 0!");
            } else {
                MEDIA_LOGE("recvSocket error!");
            }
        } while (retCode > 0);
    } else {
        MEDIA_LOGD("onReadable receive msg.");
    }
}
} // namespace Sharing
} // namespace OHOS