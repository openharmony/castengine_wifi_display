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

#include "tcp_server.h"
#include <arpa/inet.h>
#include "common/media_log.h"
#include "network/session/tcp_session.h"
#include "network/socket/socket_utils.h"
#include "network/socket/tcp_socket.h"
#include "utils/utils.h"
namespace OHOS {
namespace Sharing {
TcpServer::~TcpServer()
{
    SHARING_LOGD("trace.");
    Stop();
}

TcpServer::TcpServer()
{
    SHARING_LOGD("trace.");
}

bool TcpServer::Start(uint16_t port, const std::string &host, bool enableReuse, uint32_t backlog)
{
    SHARING_LOGD("server ip:%{public}s, Port:%{public}d, thread_id: %{public}llu.", GetAnonyString(host).c_str(), port,
                 GetThreadId());
    socket_ = std::make_unique<TcpSocket>();
    if (socket_) {
        if (socket_->Bind(port, host, enableReuse, backlog)) {
            SHARING_LOGD("start success fd: %{public}d.", socket_->GetLocalFd());
            socketLocalFd_ = socket_->GetLocalFd();

            auto eventRunner = OHOS::AppExecFwk::EventRunner::Create(true);
            eventHandler_ = std::make_shared<TcpServerEventHandler>();
            eventHandler_->SetServer(shared_from_this());
            eventHandler_->SetEventRunner(eventRunner);
            eventRunner->Run();

            eventListener_ = std::make_shared<TcpServerEventListener>();
            eventListener_->SetServer(shared_from_this());

            return eventListener_->AddFdListener(socket_->GetLocalFd(), eventListener_, eventHandler_);
        }
    }

    SHARING_LOGE("start failed!");
    return false;
}

void TcpServer::Stop()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> lk(mutex_);
    if (socket_ != nullptr) {
        if (eventListener_) {
            eventListener_->RemoveFdListener(socket_->GetLocalFd());
        }
        SocketUtils::ShutDownSocket(socket_->GetLocalFd());
        SocketUtils::CloseSocket(socket_->GetLocalFd());
        for (auto it = sessions_.begin(); it != sessions_.end(); it++) {
            SHARING_LOGD("closeClientSocket:erase fd: %{public}d,size: %{public}zu.", it->first, sessions_.size());
            if (it->second) {
                it->second->Shutdown();
                it->second.reset();
            }
        }
        sessions_.clear();
        socket_.reset();
    }
}

void TcpServer::CloseClientSocket(int32_t fd)
{
    SHARING_LOGD("fd: %{public}d.", fd);
    if (fd > 0) {
        auto itemItr = sessions_.find(fd);
        if (itemItr != sessions_.end()) {
            if (itemItr->second) {
                itemItr->second->Shutdown();
                itemItr->second.reset();
            }
            sessions_.erase(itemItr);
            SHARING_LOGD("erase fd: %{public}d.", fd);
        }
    }
}

SocketInfo::Ptr TcpServer::GetSocketInfo()
{
    SHARING_LOGD("trace.");
    return socket_;
}

void TcpServer::OnServerReadable(int32_t fd)
{
    SHARING_LOGD("fd: %{public}d, socketLocalFd: %{public}d, thread_id: %{public}llu.", fd, socketLocalFd_,
                 GetThreadId());
    std::unique_lock<std::shared_mutex> lk(mutex_);
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(sockaddr_in);
    if (fd == socketLocalFd_) {
        int32_t clientFd = SocketUtils::AcceptSocket(fd, &clientAddr, &addrLen);
        if (clientFd < 0) {
            SHARING_LOGE("onReadable accept client error!");
            return;
        }
        SocketUtils::SetNonBlocking(clientFd);
        SocketUtils::SetNoDelay(clientFd, true);
        SocketUtils::SetSendBuf(clientFd);
        SocketUtils::SetRecvBuf(clientFd);
        SocketUtils::SetCloseWait(clientFd);
        SocketUtils::SetCloExec(clientFd, true);
        SocketUtils::SetKeepAlive(clientFd);
        SHARING_LOGD("onReadable accept client fd: %{public}d.", clientFd);
        if (socket_) {
            socket_->socketPeerFd_ = clientFd;

            SocketInfo::Ptr socketInfo = std::make_shared<SocketInfo>(
                socket_->GetLocalIp(), inet_ntoa(clientAddr.sin_addr), socket_->GetLocalFd(), clientFd,
                socket_->GetLocalPort(), clientAddr.sin_port);
            if (socketInfo) {
                socketInfo->SetSocketType(SOCKET_TYPE_TCP);
                BaseNetworkSession::Ptr session = std::make_shared<TcpSession>(std::move(socketInfo));
                if (session) {
                    MEDIA_LOGE("[TcpServer] OnReadable new session start.");
                    sessions_.insert(make_pair(clientFd, std::move(session)));
                    auto callback = callback_.lock();
                    if (callback) {
                        callback->OnAccept(sessions_[clientFd]);
                    }
                } else {
                    MEDIA_LOGE("onReadable create session failed!");
                }
            } else {
                MEDIA_LOGE("onReadable create SocketInfo failed!");
            }
        }
    } else {
        MEDIA_LOGD("onReadable receive msg!");
    }
}
} // namespace Sharing
} // namespace OHOS