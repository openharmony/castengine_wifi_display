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

#include "base_network_session.h"
#include "common/media_log.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {
BaseNetworkSession::BaseNetworkSession(SocketInfo::Ptr socket) : socket_(std::move(socket))
{
    SHARING_LOGD("trace.");
}

BaseNetworkSession::~BaseNetworkSession()
{
    SHARING_LOGD("trace.");
}

void BaseNetworkSession::RegisterCallback(std::shared_ptr<INetworkSessionCallback> callback)
{
    SHARING_LOGD("trace.");
    callback_ = std::move(callback);
}

std::shared_ptr<INetworkSessionCallback> &BaseNetworkSession::GetCallback()
{
    SHARING_LOGD("trace.");
    return callback_;
}

SocketInfo::Ptr BaseNetworkSession::GetSocketInfo()
{
    SHARING_LOGD("trace.");
    return socket_;
}

bool BaseNetworkSession::Start()
{
    SHARING_LOGD("trace.");
    return true;
}

bool BaseNetworkSession::Send(const DataBuffer::Ptr &buf, int32_t nSize)
{
    SHARING_LOGD("trace.");
    return false;
}

bool BaseNetworkSession::Send(const char *buf, int32_t nSize)
{
    SHARING_LOGD("trace.");
    return false;
}

void BaseNetworkSession::Shutdown()
{
    SHARING_LOGD("trace.");
}

void BaseSessionEventListener::OnReadable(int32_t fd)
{
    SHARING_LOGD("fd: %{public}d, thread_id: %{public}llu.", fd, GetThreadId());
    auto session = session_.lock();
    if (session) {
        session->OnSessionReadble(fd);
    }
}

void BaseSessionEventListener::OnWritable(int32_t fd)
{
    SHARING_LOGD("thread_id: %{public}llu.", GetThreadId());
    auto session = session_.lock();
    if (session) {
        auto callback = session->GetCallback();
        if (callback) {
            callback->OnSessionWriteable(fd);
        }
    }
}

void BaseSessionEventListener::OnShutdown(int32_t fd)
{
    SHARING_LOGD("thread_id: %{public}llu.", GetThreadId());
    auto session = session_.lock();
    if (session) {
        auto callback = session->GetCallback();
        if (callback) {
            callback->OnSessionClose(fd);
        }
    }
}

void BaseSessionEventListener::OnException(int32_t fd)
{
    SHARING_LOGD("thread_id: %{public}llu.", GetThreadId());
    auto session = session_.lock();
    if (session) {
        auto callback = session->GetCallback();
        if (callback) {
            callback->OnSessionException(fd);
        }
    }
}

} // namespace Sharing
} // namespace OHOS