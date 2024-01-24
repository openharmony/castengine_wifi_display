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

#ifndef OHOS_SHARING_AGENT_H
#define OHOS_SHARING_AGENT_H

#include <map>
#include "agent/session/base_session.h"
#include "agent_def.h"
#include "common/event_comm.h"

namespace OHOS {
namespace Sharing {

class IAgentListener {
public:
    using Ptr = std::shared_ptr<IAgentListener>;

    virtual ~IAgentListener() = default;

    virtual void OnAgentNotify(AgentStatusMsg::Ptr &statusMsg) = 0;
};

class Agent : public IdentifierInfo,
              public ISessionListener,
              public HandleEventBase,
              public std::enable_shared_from_this<Agent> {
public:
    using Ptr = std::shared_ptr<Agent>;

    explicit Agent(AgentType agentType);

    void SetDestroy()
    {
        destroy_ = true;
    }

    bool GetDestroy()
    {
        return destroy_;
    }

    void UpdateMediaChannelId(uint32_t mediaChannelId)
    {
        mediaChannelId_ = mediaChannelId;
    }

    void SetAgentListener(std::weak_ptr<IAgentListener> agentListener)
    {
        agentListener_ = agentListener;
    }

    uint32_t GetMediaChannelId()
    {
        return mediaChannelId_;
    }

    AgentType GetAgentType()
    {
        return agentType_;
    }

public:
    int32_t HandleEvent(SharingEvent &event) override;
    SharingErrorCode CreateSession(const std::string &className);

protected:
    void PushNextStep(SessionStatusMsg::Ptr &statusMsg);
    void HandleSessionError(SessionStatusMsg::Ptr &statusMsg);
    void UpdateSessionStatus(SessionStatusMsg::Ptr &statusMsg);
    void PopNextStep(AgentRunStep step, AgentRunningStatus status);
    void OnSessionNotify(SessionStatusMsg::Ptr &statusMsg) override;
    void SetRunningStatus(AgentRunStep step, AgentRunningStatus status);

    AgentRunStep GetRunStep(EventType eventType);
    AgentRunStepWeight GetRunStepWeight(AgentRunStep runStep);
    SharingErrorCode CheckRunStep(SharingEvent &event, bool &isCached);

private:
    uint32_t NotifyPrivateEvent(SessionStatusMsg::Ptr &statusMsg);

    void HandleProsumerState(SessionStatusMsg::Ptr &statusMsg);
    void HandleProsumerError(SessionStatusMsg::Ptr &statusMsg);

    void SendContextEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);
    void SendChannelEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);
    void SendInteractionEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);
    void SendChannelSetVolumeEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);
    void SendChannelSceneTypeEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);
    void SendChannelKeyRedirectEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);
    void SendChannelAppendSurfaceEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);
    void SendChannelRemoveSurfaceEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType);

protected:
    bool destroy_ = false;
    uint32_t prosumerId_ = INVALID_ID;
    uint32_t mediaChannelId_ = INVALID_ID;

    std::mutex runStepMutex_;
    std::weak_ptr<IAgentListener> agentListener_;
    std::map<AgentRunStepKey, SharingEvent> runEvents_;

    AgentType agentType_ = SRC_AGENT;
    BaseSession::Ptr session_ = nullptr;
    AgentRunStep runStep_ = AGENT_STEP_IDLE;
    AgentRunningStatus runningStatus_ = AGENT_STATUS_IDLE;
};

} // namespace Sharing
} // namespace OHOS
#endif