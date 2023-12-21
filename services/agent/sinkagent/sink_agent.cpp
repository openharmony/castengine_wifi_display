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

#include "sink_agent.h"
#include "common/common_macro.h"
#include "common/event_channel.h"
#include "common/sharing_log.h"
#include "magic_enum.hpp"

namespace OHOS {
namespace Sharing {

SinkAgent::SinkAgent() : Agent(AgentType::SINK_AGENT)
{
    SHARING_LOGD("agentId: %{public}u.", GetId());
}

SinkAgent::~SinkAgent()
{
    SHARING_LOGD("agentId: %{public}u.", GetId());
}

void SinkAgent::OnSessionNotify(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    SHARING_LOGI("session notify status: %{public}s, agentId: %{pubilc}u.",
                 std::string(magic_enum::enum_name((SessionNotifyStatus)statusMsg->status)).c_str(), GetId());
    statusMsg->prosumerId = prosumerId_;
    switch (statusMsg->status) {
        case SessionNotifyStatus::NOTIFY_PROSUMER_CREATE:
            NotifyConsumer(statusMsg, EventType::EVENT_MEDIA_CONSUMER_CREATE);
            break;
        case SessionNotifyStatus::STATE_SESSION_STARTED:
            NotifyConsumer(statusMsg, EventType::EVENT_MEDIA_CONSUMER_START);
            break;
        case SessionNotifyStatus::STATE_SESSION_STOPED:
            NotifyConsumer(statusMsg, EventType::EVENT_MEDIA_CONSUMER_STOP);
            break;
        case SessionNotifyStatus::STATE_SESSION_PAUSED:
            NotifyConsumer(statusMsg, EventType::EVENT_MEDIA_CONSUMER_PAUSE);
            break;
        case SessionNotifyStatus::STATE_SESSION_RESUMED:
            NotifyConsumer(statusMsg, EventType::EVENT_MEDIA_CONSUMER_RESUME);
            break;
        case SessionNotifyStatus::STATE_SESSION_DESTROYED:
            NotifyConsumer(statusMsg, EventType::EVENT_MEDIA_CONSUMER_DESTROY);
            break;
        case SessionNotifyStatus::STATE_SESSION_INTERRUPTED:
            PopNextStep(runStep_, AGENT_STATUS_DONE);
            break;
        default:
            Agent::OnSessionNotify(statusMsg);
            break;
    }
}

void SinkAgent::NotifyConsumer(SessionStatusMsg::Ptr &statusMsg, EventType type)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    SHARING_LOGI("agentId: %{public}u, notify status, type: %{public}s.", GetId(),
                 std::string(magic_enum::enum_name(type)).c_str());

    auto listener = agentListener_.lock();
    if (listener) {
        auto channelMsg = std::make_shared<ChannelEventMsg>();
        channelMsg->agentId = GetId();
        channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
        channelMsg->dstId = mediaChannelId_;
        channelMsg->prosumerId = statusMsg->prosumerId;
        channelMsg->type = type;
        channelMsg->className = statusMsg->className;
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;
        channelMsg->mediaType = statusMsg->mediaType;
        statusMsg->msg = std::move(channelMsg);

        auto agentMsg = std::static_pointer_cast<AgentStatusMsg>(statusMsg);
        agentMsg->agentId = GetId();
        SHARING_LOGI("id: %{public}u, and notify to send event: %{public}s, mediaChannelId: %{public}u.", GetId(),
                     std::string(magic_enum::enum_name(type)).c_str(), mediaChannelId_);
        listener->OnAgentNotify(agentMsg);
    }
}

} // namespace Sharing
} // namespace OHOS