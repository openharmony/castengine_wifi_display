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

#include "scheduler.h"
#include "common/common_macro.h"
#include "common/sharing_log.h"
#include "context/context_manager.h"
#include "event/event_manager.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "interaction/interaction_manager.h"
#include "mediachannel/channel_manager.h"

namespace OHOS {
namespace Sharing {

Scheduler::Scheduler()
{
    SHARING_LOGD("trace.");
    EventManager::GetInstance().AddListener(std::make_shared<SchedulerEventListener>());
}

int32_t Scheduler::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    int32_t ret = -1;
    switch (event.eventMsg->toMgr) {
        case MODULE_CONTEXT:
            SHARING_LOGD("handleEvent to context %{public}s.",
                         std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
            ret = ContextManager::GetInstance().HandleEvent(event);
            SHARING_LOGD("handleEvent to context return %{public}s.",
                         std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
            break;
        case MODULE_MEDIACHANNEL:
            SHARING_LOGD("handleEvent to channel %{public}s.",
                         std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
            ret = ChannelManager::GetInstance().HandleEvent(event);
            SHARING_LOGD("handleEvent to channel return %{public}s.",
                         std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
            break;
        case MODULE_INTERACTION:
            SHARING_LOGD("handleEvent to interaction %{public}s.",
                         std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
            ret = InteractionManager::GetInstance().HandleEvent(event);
            SHARING_LOGD("handleEvent to interaction return %{public}s.",
                         std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
            break;
        default:
            break;
    }

    return ret;
}

int32_t SchedulerEventListener::OnEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    return Scheduler::GetInstance().HandleEvent(event);
}

} // namespace Sharing
} // namespace OHOS