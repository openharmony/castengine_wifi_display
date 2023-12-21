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

#ifndef OHOS_SHARING_EVENT_HANDLER_MANAGER_H
#define OHOS_SHARING_EVENT_HANDLER_MANAGER_H

#include <memory>
#include <mutex>
#include <singleton.h>
#include "event_handler.h"
#include "event_runner.h"
#include "file_descriptor_listener.h"

namespace OHOS {
namespace Sharing {
using namespace OHOS::AppExecFwk;

class EventHandlerManager : public Singleton<EventHandlerManager> {
    friend class Singleton<EventHandlerManager>;

public:
    virtual ~EventHandlerManager();

    int32_t DelFdListener(int32_t fd);
    int32_t AddFdListener(int32_t fd, const std::shared_ptr<FileDescriptorListener> &listener);

    int32_t DelEventHandler(int32_t fd);
    int32_t AddEventHandler(int32_t fd, const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &handler);

    std::shared_ptr<OHOS::AppExecFwk::EventRunner> GetEventRunner();
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetEventHandler(int32_t fd);

private:
    EventHandlerManager();

    void Init();

private:
    std::mutex mutex;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> eventRunner_ = nullptr;
    std::map<int32_t, std::weak_ptr<FileDescriptorListener>> listenerMap_;
    std::map<int32_t, std::weak_ptr<OHOS::AppExecFwk::EventHandler>> handlerMap_;
};

} // namespace Sharing
} // namespace OHOS
#endif