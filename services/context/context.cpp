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

#include "context.h"
#include "agent/sinkagent/sink_agent.h"
#include "agent/srcagent/src_agent.h"
#include "common/common_macro.h"
#include "common/event_channel.h"
#include "common/sharing_log.h"
#include "magic_enum.hpp"

namespace OHOS {
namespace Sharing {
Context::Context()
{
    SHARING_LOGD("contextId: %{public}u.", GetId());
}

Context::~Context()
{
    SHARING_LOGD("contextId: %{public}u.", GetId());
}

void Context::Release()
{
    SHARING_LOGD("contextId: %{public}u.", GetId());
    destroying_ = true;
    std::lock_guard<std::mutex> lock(mutex_);
    if (agents_.size()) {
        SHARING_LOGW("exception to enter! contextId: %{public}u.", GetId());
    }
    agents_.clear();
}

int32_t Context::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI(
        "contextId: %{public}u, fromMgr: %{public}u, srcId: %{public}u, "
        "toMgr: %{public}u, toId: %{public}u, event: %{public}s.",
        GetId(), event.eventMsg->fromMgr, event.eventMsg->srcId, event.eventMsg->toMgr, event.eventMsg->dstId,
        std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    if (destroying_) {
        SHARING_LOGW("destroying contextId: %{public}u.", GetId());
        return -1;
    }

    switch (event.eventMsg->type) {
        case EventType::EVENT_CONTEXT_AGENT_CREATE: {
            auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
            if (eventMsg) {
                interactionId_ = eventMsg->srcId;
                eventMsg->agentId = HandleCreateAgent(eventMsg->className, eventMsg->agentType, eventMsg->agentId);
            }
            break;
        }
        case EventType::EVENT_CONTEXT_AGENT_DESTROY:
            HandleAgentDestroy(event);
            break;
        case EventType::EVENT_CONTEXT_STATE_AGENT_DESTROY:
            HandleStateDestroyAgent(event);
            break;
        default:
            DistributeEvent(event);
            break;
    }

    return 0;
}

void Context::HandleAgentDestroy(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);
    auto eventMsg = ConvertEventMsg<AgentEventMsg>(event);
    if (eventMsg == nullptr) {
        SHARING_LOGE("msg null.");
        return;
    }

    SHARING_LOGI("contextId: %{public}u, destroy agent agentId: %{public}u.", GetId(), eventMsg->agentId);
    interactionId_ = eventMsg->srcId;
    auto agent = GetAgentById(eventMsg->agentId);
    if (agent == nullptr) {
        SHARING_LOGE("agent null agentId: %{public}d.", eventMsg->agentId);
        return;
    }

    if (agent->GetAgentType() == SRC_AGENT) {
        event.eventMsg->type = EVENT_AGENT_DESTROY;
        agent->HandleEvent(event);
    } else {
        agent->SetDestroy();
        if (IsEmptySrcAgent(eventMsg->agentId)) {
            event.eventMsg->type = EVENT_AGENT_DESTROY;
            agent->HandleEvent(event);
        } else {
            SHARING_LOGI("contextId: %{public}u, destroy agent need wait src agent destroy. agentId: %{public}u.",
                         GetId(), eventMsg->agentId);
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto item : agents_) {
                if (item.second->GetAgentType() == SRC_AGENT) {
                    event.eventMsg->type = EVENT_AGENT_DESTROY;
                    item.second->HandleEvent(event);
                }
            }
        }
    }
}

void Context::DistributeEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto eventMsg = ConvertEventMsg<AgentEventMsg>(event);
    if (eventMsg) {
        auto agent = GetAgentById(eventMsg->agentId);
        if (agent) {
            agent->HandleEvent(event);
        } else {
            SHARING_LOGE("agent null, contextId: %{pulbic}u, agentId: %{public}u, event: %{public}s.", GetId(),
                         eventMsg->agentId, std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
        }
    }
}

bool Context::IsEmptySrcAgent(uint32_t sinkAgentId)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &item : agents_) {
        if (item.second->GetAgentType() == SRC_AGENT) {
            auto srcAgent = std::static_pointer_cast<SrcAgent>(item.second);
            if (srcAgent && srcAgent->GetSinkAgentId() == sinkAgentId) {
                return false;
            }
        }
    }

    return true;
}

int32_t Context::GetSinkAgentSize()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t agentSize = 0;
    for (auto &item : agents_) {
        if (item.second->GetAgentType() == SINK_AGENT) {
            SHARING_LOGD("agent id still has %{public}d.", item.second->GetId());
            ++agentSize;
        }
    }

    return agentSize;
}

int32_t Context::GetSrcAgentSize()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t agentSize = 0;
    for (auto &item : agents_) {
        if (item.second->GetAgentType() == SRC_AGENT) {
            ++agentSize;
        }
    }

    return agentSize;
}

void Context::SendInteractionEvent(EventType eventType, ContextEventMsg::Ptr &eventMsg)
{
    SHARING_LOGD("trace.");
    auto interactionMsg = std::make_shared<InteractionEventMsg>();
    interactionMsg->fromMgr = ModuleType::MODULE_CONTEXT;
    interactionMsg->srcId = GetId();
    interactionMsg->toMgr = ModuleType::MODULE_INTERACTION;
    interactionMsg->dstId = interactionId_;

    interactionMsg->type = eventType;
    interactionMsg->errorCode = eventMsg->errorCode;
    interactionMsg->agentId = eventMsg->agentId;
    interactionMsg->agentType = eventMsg->agentType;
    interactionMsg->contextId = GetId();
    interactionMsg->surfaceId = eventMsg->surfaceId;

    SharingEvent event;
    event.eventMsg = std::move(interactionMsg);
    SendEvent(event);
    SHARING_LOGI("event: %{public}u is sended by contextId: %{public}u.", event.eventMsg->type, GetId());
}

bool Context::IsEmptyAgent()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    return agents_.size() == 0;
}

void Context::OnAgentNotify(AgentStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    SHARING_LOGI("contextId: %{public}u, agentId: %{public}u, toMgr: %{public}u, dstId: %{public}u, event: %{public}s.",
                 GetId(), statusMsg->agentId, statusMsg->msg->toMgr, statusMsg->msg->dstId,
                 std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());

    if (statusMsg->msg) {
        if (statusMsg->msg->toMgr == ModuleType::MODULE_INTERACTION) {
            statusMsg->msg->dstId = interactionId_;
            auto interactionMsg = std::static_pointer_cast<InteractionEventMsg>(statusMsg->msg);
            RETURN_IF_NULL(interactionMsg);
            interactionMsg->contextId = GetId();
        } else if (statusMsg->msg->toMgr == ModuleType::MODULE_CONTEXT) {
            statusMsg->msg->dstId = GetId();
        }

        SharingEvent event;
        event.eventMsg = std::move(statusMsg->msg);
        event.eventMsg->fromMgr = ModuleType::MODULE_CONTEXT;
        event.eventMsg->srcId = GetId();

        SendEvent(event);
        SHARING_LOGI("event: %{public}s is sended by contextId: %{public}u.",
                     std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), GetId());
    }
}

uint32_t Context::HandleCreateAgent(const std::string &className, AgentType agentType, uint32_t sinkAgentId)
{
    SHARING_LOGI("contextId: %{public}u, agentType: %{public}s, sinkAgentId: %{public}u.", GetId(),
                 std::string(magic_enum::enum_name(agentType)).c_str(), sinkAgentId);

    std::shared_ptr<Agent> agent = nullptr;
    if (agentType == AgentType::SINK_AGENT) {
        auto sinkAgent = std::make_shared<SinkAgent>();
        if (sinkAgent->CreateSession(className) == SharingErrorCode::ERR_SESSION_CREATE) {
            SHARING_LOGE("create agent session error.");
            return INVALID_ID;
        } else {
            auto channelMsg = std::make_shared<ChannelEventMsg>();
            channelMsg->agentId = sinkAgent->GetId();
            channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
            channelMsg->type = EVENT_MEDIA_CHANNEL_CREATE;

            SharingEvent event;
            event.eventMsg = channelMsg;
            event.eventMsg->fromMgr = ModuleType::MODULE_CONTEXT;
            event.eventMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
            event.eventMsg->srcId = GetId();
            SendSyncEvent(event);

            SHARING_LOGI("create agent create media channel agentId: %{public}u channelId: %{public}u.",
                         sinkAgent->GetId(), channelMsg->srcId);
            sinkAgent->UpdateMediaChannelId(channelMsg->srcId);

            agent = sinkAgent;
        }
    } else {
        std::lock_guard<std::mutex> lock(mutex_);
        Agent::Ptr sinkAgent = nullptr;
        if (agents_.find(sinkAgentId) != agents_.end()) {
            sinkAgent = agents_.find(sinkAgentId)->second;
        }
        if (sinkAgent != nullptr && !sinkAgent->GetDestroy()) {
            auto srcAgent = std::make_shared<SrcAgent>();
            if (srcAgent->CreateSession(className) == SharingErrorCode::ERR_SESSION_CREATE) {
                SHARING_LOGE("create agent session error.");
                return INVALID_ID;
            } else {
                SHARING_LOGI("channelId: %{public}u is set to src agentId: %{public}u.", sinkAgent->GetMediaChannelId(),
                             srcAgent->GetId());
                srcAgent->UpdateMediaChannelId(sinkAgent->GetMediaChannelId());
                srcAgent->SetSinkAgentId(sinkAgentId);
            }
            agent = srcAgent;
        } else {
            SHARING_LOGE("sinkAgentId invalid.");
            return INVALID_ID;
        }
    }

    agent->SetAgentListener(shared_from_this());

    std::lock_guard<std::mutex> lock(mutex_);
    agents_.emplace(agent->GetId(), agent);
    SHARING_LOGI("contextId: %{public}u, create agent className: %{public}s "
        "agentType: %{public}s agentId: %{public}u size: %{public}zu.",
        GetId(), className.c_str(), std::string(magic_enum::enum_name(agentType)).c_str(), agent->GetId(),
        agents_.size());
    return agent->GetId();
}

Agent::Ptr Context::GetAgentById(uint32_t agentId)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = agents_.find(agentId);
    if (itr != agents_.end()) {
        return itr->second;
    }

    return nullptr;
}

void Context::RemoveAgent(uint32_t agentId)
{
    SHARING_LOGD("contextId: %{public}u, remove agentId: %{public}u.", GetId(), agentId);
    std::lock_guard<std::mutex> lock(mutex_);
    auto itr = agents_.find(agentId);
    if (itr != agents_.end()) {
        agents_.erase(itr);
    }
}

void Context::HandleStateDestroyAgent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    if (eventMsg) {
        auto agent = GetAgentById(eventMsg->agentId);
        if (agent) {
            AgentType agentType = agent->GetAgentType();
            if (agentType == AgentType::SRC_AGENT) {
                auto srcAgent = std::static_pointer_cast<SrcAgent>(agent);
                uint32_t sinkAgentId = srcAgent->GetSinkAgentId();
                RemoveAgent(eventMsg->agentId);
                CheckNeedDestroySink(sinkAgentId);
            } else if (agentType == AgentType::SINK_AGENT) {
                RemoveAgent(eventMsg->agentId);
            }

            eventMsg->agentType = agentType;
            SendInteractionEvent(EVENT_INTERACTION_STATE_AGENT_DESTROYED, eventMsg);
        } else {
            SHARING_LOGE("agent null agentId: %{public}d.", eventMsg->agentId);
        }
    }
}

void Context::CheckNeedDestroySink(uint32_t sinkAgentId)
{
    SHARING_LOGD("trace.");
    auto agent = GetAgentById(sinkAgentId);
    RETURN_IF_NULL(agent);

    if (agent->GetDestroy() && IsEmptySrcAgent(sinkAgentId)) {
        SHARING_LOGI("produer destroyed. src agent is all destroyed. destroy sink agent now. "
            "contextId: %{public}u, sinkAgentId: %{public}u.", GetId(), sinkAgentId);

        SharingEvent event;
        auto agentEvent = std::make_shared<AgentEventMsg>();
        event.eventMsg = std::move(agentEvent);
        event.eventMsg->fromMgr = ModuleType::MODULE_CONTEXT;
        event.eventMsg->srcId = GetId();
        event.eventMsg->toMgr = ModuleType::MODULE_CONTEXT;
        event.eventMsg->dstId = GetId();
        event.eventMsg->type = EVENT_AGENT_DESTROY;
        agent->HandleEvent(event);
    }
}

} // namespace Sharing
} // namespace OHOS