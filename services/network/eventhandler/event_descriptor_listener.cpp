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

#include "event_descriptor_listener.h"
#include "common/media_log.h"
#include "common/sharing_log.h"
#include "event_handler_manager.h"
#include "network/socket/socket_utils.h"
#include "utils/data_buffer.h"
#include "utils/utils.h"
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace Sharing {

EventDescriptorListener ::~EventDescriptorListener()
{
    SHARING_LOGD("trace.");
}

EventDescriptorListener::EventDescriptorListener()
{
    SHARING_LOGD("trace.");
}

EventDescriptorListener::EventDescriptorListener(int32_t fd) : socketLocalFd_(fd)
{
    SHARING_LOGD("trace.");
}

int32_t EventDescriptorListener::GetSocketFd()
{
    SHARING_LOGD("trace.");
    return socketLocalFd_;
}

bool EventDescriptorListener::AddFdListener(int32_t fd, const std::shared_ptr<FileDescriptorListener> &listener,
                                            const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &handler,
                                            const std::string &taskName)
{
    SHARING_LOGD("fd: %{public}d.", fd);
    auto existHandler = EventHandlerManager::GetInstance().GetEventHandler(fd);
    if (existHandler == nullptr && handler == nullptr) {
        MEDIA_LOGE("eventHandler is nullptr!");
        return false;
    }

    auto eventHandler = handler;
    if (eventHandler == nullptr) {
        eventHandler = existHandler;
    } else {
        EventHandlerManager::GetInstance().AddEventHandler(fd, eventHandler);
        EventHandlerManager::GetInstance().AddFdListener(fd, listener);
    }
    return ERR_OK == eventHandler->AddFileDescriptorListener(fd,
        FILE_DESCRIPTOR_EVENTS_MASK | FILE_DESCRIPTOR_SHUTDOWN_EVENT | FILE_DESCRIPTOR_EXCEPTION_EVENT,
        listener, taskName);
}

void EventDescriptorListener::RemoveFdListener(int32_t fd,
                                               const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &handler)
{
    SHARING_LOGD("fd: %{public}d.", fd);
    auto existHandler = EventHandlerManager::GetInstance().GetEventHandler(fd);
    if (existHandler == nullptr && handler == nullptr) {
        MEDIA_LOGE("eventHandler is nullptr!");
        return;
    }

    auto eventHandler = handler;
    if (eventHandler == nullptr) {
        eventHandler = existHandler;
    }

    if (eventHandler) {
        eventHandler->RemoveFileDescriptorListener(fd);
    }

    socketLocalFd_ = -1;
    EventHandlerManager::GetInstance().DelFdListener(fd);
    EventHandlerManager::GetInstance().DelEventHandler(fd);
}

void EventDescriptorListener::OnReadable(int32_t fd)
{
    MEDIA_LOGD("trace.");
    if (fd != socketLocalFd_) {
        MEDIA_LOGW("onReadable socketLocalFd_: %{public}d.", socketLocalFd_);
    }
    auto spBuf = std::make_shared<DataBuffer>();
    int32_t error = 0;
    int32_t readSize = SocketUtils::ReadSocket(fd, spBuf, error);
    if (readSize == 0) {
        SHARING_LOGE("onReadable error close socket!");
        if (error == ECONNRESET) {
            OnClose(fd);
        }
    } else {
        OnReadData(fd, spBuf);
    }
}

void EventDescriptorListener::OnWritable(int32_t fd)
{
    MEDIA_LOGD("fd: %{public}d.", fd);
}

void EventDescriptorListener::OnShutdown(int32_t fd)
{
    SHARING_LOGD("fd: %{public}d.", fd);
    OnClose(fd);
}

void EventDescriptorListener::OnException(int32_t fd)
{
    SHARING_LOGD("fd: %{public}d.", fd);
}

void EventDescriptorListener::OnReadData(int32_t fd, DataBuffer::Ptr &buf)
{
    MEDIA_LOGD("trace.");
}

void EventDescriptorListener::OnClose(int32_t fd)
{
    SHARING_LOGD("trace.");
}

} // namespace Sharing
} // namespace OHOS
