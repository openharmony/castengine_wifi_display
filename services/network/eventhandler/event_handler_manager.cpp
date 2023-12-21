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

#include "event_handler_manager.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
EventHandlerManager::EventHandlerManager()
{
    SHARING_LOGD("trace.");
    Init();
}

EventHandlerManager::~EventHandlerManager()
{
    SHARING_LOGD("~EventHandlerManager.");
    std::unique_lock<std::mutex> lock(mutex);
    eventRunner_->Stop();
    listenerMap_.clear();
    handlerMap_.clear();
}

void EventHandlerManager::Init()
{
    SHARING_LOGD("eventHandlerManager Init.");
    std::unique_lock<std::mutex> lock(mutex);
    eventRunner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    eventRunner_->Run();
}

std::shared_ptr<OHOS::AppExecFwk::EventRunner> EventHandlerManager::GetEventRunner()
{
    SHARING_LOGD("trace.");
    return eventRunner_;
}

std::shared_ptr<OHOS::AppExecFwk::EventHandler> EventHandlerManager::GetEventHandler(int32_t fd)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex);
    if (handlerMap_.find(fd) != handlerMap_.end()) {
        auto handler = handlerMap_[fd].lock();
        if (handler != nullptr) {
            return handler;
        }
    }

    return nullptr;
}

int32_t EventHandlerManager::AddFdListener(int32_t fd, const std::shared_ptr<FileDescriptorListener> &listener)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex);
    if (listenerMap_.find(fd) != listenerMap_.end()) {
        auto listener = listenerMap_[fd].lock();
        if (listener != nullptr) {
            return 0;
        }
    }
    listenerMap_.insert(std::make_pair(fd, listener));
    return 0;
}

int32_t EventHandlerManager::AddEventHandler(int32_t fd, const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &handler)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex);
    if (handlerMap_.find(fd) != handlerMap_.end()) {
        auto handler = handlerMap_[fd].lock();
        if (handler != nullptr) {
            return 0;
        }
    }
    handlerMap_.insert(std::make_pair(fd, handler));
    return 0;
}

int32_t EventHandlerManager::DelFdListener(int32_t fd)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex);
    if (listenerMap_.find(fd) != listenerMap_.end()) {
        listenerMap_.erase(fd);
    }

    return 0;
}

int32_t EventHandlerManager::DelEventHandler(int32_t fd)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex);
    if (handlerMap_.find(fd) != handlerMap_.end()) {
        handlerMap_.erase(fd);
    }

    return 0;
}

} // namespace Sharing
} // namespace OHOS