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

#include "tcp_session.h"
#include "common/common_macro.h"
#include "common/media_log.h"
#include "network/socket/socket_utils.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {
TcpSession::TcpSession(SocketInfo::Ptr socket) : BaseNetworkSession(socket)
{
    SHARING_LOGD("trace.");
}

TcpSession::~TcpSession()
{
    SHARING_LOGD("trace.");
    Shutdown();
}

bool TcpSession::Start()
{
    SHARING_LOGD("trace.");
    if (socket_) {
        SHARING_LOGD("tcpSession AddFdListener.");
        auto eventRunner = OHOS::AppExecFwk::EventRunner::Create(true);
        eventHandler_ = std::make_shared<TcpSessionEventHandler>();
        eventHandler_->SetSession(shared_from_this());
        eventHandler_->SetEventRunner(eventRunner);
        eventRunner->Run();

        eventListener_ = std::make_shared<TcpSessionEventListener>();
        eventListener_->SetSession(shared_from_this());

        return eventListener_->AddFdListener(socket_->GetPeerFd(), eventListener_, eventHandler_);
    }

    return false;
}

bool TcpSession::Send(const DataBuffer::Ptr &buf, int32_t nSize)
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(buf);
    return TcpSession::Send(buf->Peek(), nSize);
}

bool TcpSession::Send(const char *buf, int32_t nSize)
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(buf);
    if (socket_) {
        int32_t ret = SocketUtils::SendSocket(socket_->GetPeerFd(), buf, nSize);
        if ((ret != 0)) {
            return true;
        } else {
            SHARING_LOGE("send Failed, Shutdown!");
            if (callback_) {
                callback_->OnSessionClose(socket_->GetPeerFd());
            }

            return false;
        }
    } else {
        return false;
    }
}

void TcpSession::Shutdown()
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

void TcpSession::OnSessionReadble(int32_t fd)
{
    MEDIA_LOGD("fd: %{public}d, thread_id: %{public}llu.", fd, GetThreadId());
    if (socket_ == nullptr) {
        SHARING_LOGE("onReadable nullptr!");
        return;
    }
    if (fd == socket_->GetPeerFd()) {
        int32_t retCode = 0;
        int32_t error = 0;
        DataBuffer::Ptr buf = std::make_shared<DataBuffer>(DEAFULT_READ_BUFFER_SIZE);
        retCode = SocketUtils::RecvSocket(fd, (char *)buf->Data(), DEAFULT_READ_BUFFER_SIZE, 0, error);
        if (retCode > 0) {
            buf->UpdateSize(retCode);
            if (callback_) {
                callback_->OnSessionReadData(fd, std::move(buf));
            }
        } else if (retCode == 0) {
            if (callback_) {
                callback_->OnSessionClose(fd);
            }
            MEDIA_LOGW("recvSocket Shutdown!");
        } else {
            MEDIA_LOGE("recvSocket error!");
        }
    } else {
        MEDIA_LOGD("onReadable receive msg.");
    }
}
} // namespace Sharing
} // namespace OHOS