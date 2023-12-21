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

#ifndef OHOS_SHARING_EVENT_BASE_H
#define OHOS_SHARING_EVENT_BASE_H

#include <list>
#include <memory>
#include <string>
#include "common/event_comm.h"

namespace OHOS {
namespace Sharing {

class EventEmitter {
public:
    virtual ~EventEmitter() = default;

    virtual int32_t SendEvent(SharingEvent &event);
    virtual int32_t SendSyncEvent(SharingEvent &event);

protected:
    ClassType classType_ = CLASS_TYPE_SCHEDULER;
};

class EventListener : public std::enable_shared_from_this<EventListener> {
public:
    virtual ~EventListener() = default;

    virtual int32_t Register();
    virtual int32_t UnRegister();
    virtual int32_t OnEvent(SharingEvent &event) = 0;

    virtual bool IsAcceptType(EventType eventType);
    virtual void SetListenerClassType(ClassType classType);
    virtual int32_t AddAcceptEventType(EventType eventType);
    virtual int32_t DelAcceptEventType(EventType eventType);

    virtual std::list<EventType> GetAcceptTypes();
    virtual ClassType GetListenerClassType();

protected:
    std::list<EventType> acceptEvents_;
    ClassType classType_ = CLASS_TYPE_SCHEDULER;
};

class EventChecker {
public:
    static void CheckEvent(EventMsg::Ptr eventMsg);
};

} // namespace Sharing
} // namespace OHOS
#endif