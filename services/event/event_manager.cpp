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

#include "event_manager.h"
#include "common/common_macro.h"
#include "common/sharing_log.h"
#include "event_base.h"
#include "magic_enum.hpp"

namespace OHOS {
namespace Sharing {

EventManager::EventManager()
{
    SHARING_LOGD("trace.");
}

EventManager::~EventManager()
{
    SHARING_LOGD("trace.");
}

int32_t EventManager::Init()
{
    SHARING_LOGD("trace.");
    SetMaxTaskNum(30); // 30: thread numbers
    SetTimeoutInterval(2000); // 2000: timeout
    return 0;
}

int32_t EventManager::StartEventLoop()
{
    SHARING_LOGD("trace.");
    int32_t ret = Start(GetMaxTaskNum());
    if (ret != 0) {
        return ret;
    }

    eventThread_ = std::make_unique<std::thread>(&EventManager::ProcessEvent, this);
    std::string name = "eventmgr";
    pthread_setname_np(eventThread_->native_handle(), name.c_str());
    return 0;
}

void EventManager::StopEventLoop()
{
    SHARING_LOGD("trace.");
    Stop();
    eventThread_->join();
    eventThread_.reset();
    eventThread_ = nullptr;
}

int32_t EventManager::AddListener(std::shared_ptr<EventListener> listener)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(listener);
    SHARING_LOGD("classtype %{public}d.", listener->GetListenerClassType());
    std::unique_lock<std::mutex> locker(mutex_);
    auto it = listeners_.find(listener->GetListenerClassType());
    if (it != listeners_.end()) {
        it->second.emplace_back(listener);
    } else {
        auto newClassList = std::pair<ClassType, std::list<std::shared_ptr<EventListener>>>();
        newClassList.first = listener->GetListenerClassType();
        newClassList.second.emplace_back(listener);
        listeners_.emplace(newClassList);
    }

    SHARING_LOGD("listeners type count %{public}zu.", listeners_.size());
    return 0;
}

int32_t EventManager::DelListener(std::shared_ptr<EventListener> listener)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t EventManager::DrainAllListeners()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> locker(mutex_);
    listeners_.clear();
    return 0;
}

int32_t EventManager::PushEvent(const SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("push a async event, type: %{public}u %{public}s.", event.eventMsg->type,
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    std::unique_lock<std::mutex> locker(mutex_);
    events_.emplace(event);
    hasEvent_.notify_one();
    return 0;
}

int32_t EventManager::PushSyncEvent(const SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("push a sync event, type: %{public}u %{public}s.", event.eventMsg->type,
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());

    for (auto listenMap : listeners_) {
        if (listenMap.first == event.listenerType) {
            SHARING_LOGD("find Listener type %{public}d %{public}s.", event.eventMsg->type,
                         std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
            auto listener = listenMap.second.front();
            std::packaged_task<BindedTask> task(std::bind(&EventListener::OnEvent, listener, event));
            auto future = task.get_future();
            PushTask(task);
            if (future.wait_for(timeoutInterval_) == std::future_status::ready) {
                SHARING_LOGD("task dispatched success %{public}s.",
                             std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
                return future.get();
            } else {
                SHARING_LOGW("task timeout %{public}s.",
                             std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
                return -1;
            }
        }
    }

    return 0;
}

void EventManager::ProcessEvent()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> locker(mutex_);
    while (isRunning_) {
        if (events_.empty()) {
            hasEvent_.wait(locker);
        } else {
            auto event = events_.front();
            events_.pop();
            for (auto &listenMap : listeners_) {
                if (listenMap.first == event.listenerType) {
                    for (auto &listener : listenMap.second) {
                        SHARING_LOGD("task dispatched success.");
                        std::packaged_task<BindedTask> task(std::bind(&EventListener::OnEvent, listener, event));
                        PushTask(task);
                    }
                    break;
                }
            }
        }
    }
}

} // namespace Sharing
} // namespace OHOS