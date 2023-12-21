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

#include "event_base.h"
#include "common/common_macro.h"
#include "common/sharing_log.h"
#include "event_manager.h"

namespace OHOS {
namespace Sharing {

int32_t EventEmitter::SendEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    event.emitterType = classType_;
    return EventManager::GetInstance().PushEvent(event);
}

int32_t EventEmitter::SendSyncEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    event.emitterType = classType_;
    return EventManager::GetInstance().PushSyncEvent(event);
}

int32_t EventListener::Register()
{
    SHARING_LOGD("trace.");
    return EventManager::GetInstance().AddListener(shared_from_this());
}

int32_t EventListener::UnRegister()
{
    SHARING_LOGD("trace.");
    return EventManager::GetInstance().DelListener(shared_from_this());
}

int32_t EventListener::AddAcceptEventType(EventType eventType)
{
    SHARING_LOGD("trace.");
    acceptEvents_.emplace_back(eventType);
    return 0;
}

int32_t EventListener::DelAcceptEventType(EventType eventType)
{
    SHARING_LOGD("trace.");
    for (auto it = acceptEvents_.begin(); it != acceptEvents_.end(); it++) {
        if (*it == eventType) {
            acceptEvents_.erase(it);
            break;
        }
    }

    return 0;
}

bool EventListener::IsAcceptType(EventType eventType)
{
    SHARING_LOGD("trace.");
    for (auto &type : acceptEvents_) {
        if (type == eventType) {
            return true;
        }
    }

    return false;
}

std::list<EventType> EventListener::GetAcceptTypes()
{
    SHARING_LOGD("trace.");
    return acceptEvents_;
}

void EventListener::SetListenerClassType(ClassType classType)
{
    SHARING_LOGD("trace.");
    classType_ = classType;
}

ClassType EventListener::GetListenerClassType()
{
    SHARING_LOGD("trace.");
    return classType_;
}

void EventChecker::CheckEvent(EventMsg::Ptr eventMsg)
{
    RETURN_IF_NULL(eventMsg);
    SHARING_LOGD("trace.");

    if (EVENT_CONTEXT_BASE < eventMsg->type && eventMsg->type < EVENT_MEDIA_BASE) {
        if (eventMsg->toMgr != MODULE_CONTEXT) {
            SHARING_LOGE("set to context type: %{public}d to: %{public}d.", eventMsg->type, eventMsg->toMgr);
            eventMsg->toMgr = MODULE_CONTEXT;
        }
    } else if (EVENT_MEDIA_BASE < eventMsg->type && eventMsg->type < EVENT_INTERACTION_BASE) {
        if (eventMsg->toMgr != MODULE_MEDIACHANNEL) {
            SHARING_LOGE("set to mediachannel type: %{public}d to: %{public}d.", eventMsg->type, eventMsg->toMgr);
            eventMsg->toMgr = MODULE_MEDIACHANNEL;
        }
    } else if (EVENT_INTERACTION_BASE < eventMsg->type && eventMsg->type < EVENT_CONFIGURE_BASE) {
        if (eventMsg->toMgr != MODULE_INTERACTION) {
            SHARING_LOGE("set to interaction type: %{public}d to: %{public}d.", eventMsg->type, eventMsg->toMgr);
            eventMsg->toMgr = MODULE_INTERACTION;
        }
    }
}

} // namespace Sharing
} // namespace OHOS