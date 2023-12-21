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

#ifndef OHOS_SHARING_MANAGER_H
#define OHOS_SHARING_MANAGER_H

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include "event_base.h"
#include "nocopyable.h"
#include "singleton.h"
#include "taskpool.h"

namespace OHOS {
namespace Sharing {

class EventManager : public Singleton<EventManager>,
                     public TaskPool {
public:
    EventManager();
    ~EventManager();

    int32_t Init();
    int32_t StartEventLoop();
    int32_t DrainAllListeners();
    void StopEventLoop();

    int32_t PushEvent(const SharingEvent &event);
    int32_t PushSyncEvent(const SharingEvent &event);

    int32_t AddListener(std::shared_ptr<EventListener> listener);
    int32_t DelListener(std::shared_ptr<EventListener> listener);

private:
    void ProcessEvent();

private:
    std::mutex mutex_;
    std::queue<SharingEvent> events_;
    std::condition_variable hasEvent_;
    std::unique_ptr<std::thread> eventLoop_ = nullptr;
    std::unique_ptr<std::thread> eventThread_ = nullptr;
    std::unordered_map<ClassType, std::list<std::shared_ptr<EventListener>>> listeners_;
};

} // namespace Sharing
} // namespace OHOS
#endif
