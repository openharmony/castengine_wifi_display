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

#ifndef OHOS_SHARING_SCHEDULER_H
#define OHOS_SHARING_SCHEDULER_H

#include "event/event_base.h"
#include "event/handle_event_base.h"
#include "singleton.h"

namespace OHOS {
namespace Sharing {

class SchedulerEventListener : public EventListener {
public:
    SchedulerEventListener()
    {
        classType_ = CLASS_TYPE_SCHEDULER;
    }

    virtual ~SchedulerEventListener() = default;

    int32_t OnEvent(SharingEvent &event) final;
};

class Scheduler : public Singleton<Scheduler>,
                  public HandleEventBase,
                  public EventEmitter {
    friend class Singleton<Scheduler>;

public:
    virtual ~Scheduler() = default;

    int32_t HandleEvent(SharingEvent &event) final;

protected:
    Scheduler();
};

} // namespace Sharing
} // namespace OHOS
#endif