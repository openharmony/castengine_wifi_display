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

#include "udp_client.h"
#include <unistd.h>
#include "common/media_log.h"
#include "network/socket/socket_utils.h"
#include "network/socket/udp_socket.h"
#include "utils/utils.h"
namespace OHOS {
namespace Sharing {

UdpClient::~UdpClient()
{
    SHARING_LOGD("trace.");
    Disconnect();
}

UdpClient::UdpClient()
{
    SHARING_LOGD("trace.");
}

bool UdpClient::Connect(const std::string &peerHost, uint16_t peerPort, const std::string &localIp, uint16_t localPort)
{
    SHARING_LOGD("peerIp:%{public}s, peerPort:%{public}d, thread_id: %{public}llu.", GetAnonyString(peerHost).c_str(),
                 peerPort, GetThreadId());
    std::unique_lock<std::shared_mutex> lk(mutex_);
    int32_t retCode = 0;
    socket_ = std::make_unique<UdpSocket>();
    if (socket_) {
        if (socket_->Connect(peerHost, peerPort, retCode, false, true, localIp, localPort)) {
            SHARING_LOGD("connect success.");
            auto eventRunner = OHOS::AppExecFwk::EventRunner::Create(true);
            eventHandler_ = std::make_shared<UdpClientEventHandler>();
            eventHandler_->SetClient(shared_from_this());
            eventHandler_->SetEventRunner(eventRunner);
            eventRunner->Run();

            eventListener_ = std::make_shared<UdpClientEventListener>();
            eventListener_->SetClient(shared_from_this());

            bool ret = eventListener_->AddFdListener(socket_->GetLocalFd(), eventListener_, eventHandler_);

            auto callback = callback_.lock();
            if (callback) {
                callback->OnClientConnect(true);
            }

            return ret;
        }
    }
    auto callback = callback_.lock();
    if (callback) {
        callback->OnClientConnect(false);
    }
    SHARING_LOGE("connect failed!");
    return false;
}

void UdpClient::Disconnect()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> lk(mutex_);
    if (socket_ != nullptr) {
        eventListener_->RemoveFdListener(socket_->GetLocalFd());
        SocketUtils::ShutDownSocket(socket_->GetLocalFd());
        SocketUtils::CloseSocket(socket_->GetLocalFd());
        socket_.reset();
    }
}

bool UdpClient::Send(const DataBuffer::Ptr &buf, int32_t nSize)
{
    SHARING_LOGD("trace.");
    return Send(buf->Peek(), nSize);
}

bool UdpClient::Send(const char *buf, int32_t nSize)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> lk(mutex_);
    if (socket_ != nullptr) {
        int32_t fd = socket_->GetLocalFd();
        SHARING_LOGD("send fd: %{public}d.", fd);
        if (::write(fd, buf, nSize) != -1) {
            return true;
        } else {
            if (socket_) {
                MEDIA_LOGE("send [%{public}s:%{public}d]Failed, %{public}s.",
                           GetAnonyString(socket_->GetPeerIp()).c_str(), (int32_t)socket_->GetPeerPort(),
                           strerror(errno));
            }

            return false;
        }
    } else {
        return false;
    }
}

bool UdpClient::Send(const std::string &msg)
{
    SHARING_LOGD("trace.");
    return Send(msg.c_str(), msg.size());
}

SocketInfo::Ptr UdpClient::GetSocketInfo()
{
    SHARING_LOGD("trace.");
    return socket_;
}

void UdpClient::OnClientReadable(int32_t fd)
{
    MEDIA_LOGD("fd: %{public}d, thread_id: %{public}llu.", fd, GetThreadId());
    int32_t retCode = 0;
    do {
        DataBuffer::Ptr buf = std::make_shared<DataBuffer>(DEAFULT_READ_BUFFER_SIZE);
        int32_t retCode = read(fd, buf->Data(), DEAFULT_READ_BUFFER_SIZE);
        MEDIA_LOGD("recvSocket len: %{public}d.", retCode);
        if (retCode > 0) {
            buf->UpdateSize(retCode);
            auto callback = callback_.lock();
            if (callback) {
                callback->OnClientReadData(fd, std::move(buf));
            }
        } else if (retCode == 0) {
            SHARING_LOGE("recvSocket failed, error:%{public}s!", strerror(errno));
            Disconnect();
        }
    } while (retCode > 0);
}
} // namespace Sharing
} // namespace OHOS
