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

#ifndef OHOS_SHARING_CONTEXT_H
#define OHOS_SHARING_CONTEXT_H

#include <unordered_map>
#include "agent/agent.h"
#include "common/identifier.h"
#include "event/handle_event_base.h"

namespace OHOS {
namespace Sharing {

class Context : public EventEmitter,
                public IdentifierInfo,
                public HandleEventBase,
                public IAgentListener,
                public std::enable_shared_from_this<Context> {
public:
    using Ptr = std::shared_ptr<Context>;

    Context();
    virtual ~Context();

    bool IsEmptyAgent();
    bool IsEmptySrcAgent(uint32_t sinkAgentId);
    int32_t GetSrcAgentSize();
    int32_t GetSinkAgentSize();
    int32_t HandleEvent(SharingEvent &event) override;

    void Release();
    void OnAgentNotify(AgentStatusMsg::Ptr &statusMsg) override;
    void SendInteractionEvent(EventType eventType, ContextEventMsg::Ptr &eventMsg);

private:
    void RemoveAgent(uint32_t agentId);
    void DistributeEvent(SharingEvent &event);
    void CheckNeedDestroySink(uint32_t sinkAgentId);
    Agent::Ptr GetAgentById(uint32_t agentId);

    void HandleAgentDestroy(SharingEvent &event);
    void HandleStateDestroyAgent(SharingEvent &event);
    uint32_t HandleCreateAgent(const std::string &designation, AgentType agentType, uint32_t sinkAgentId);

private:
    uint32_t interactionId_ = INVALID_ID;

    std::mutex mutex_;
    std::atomic<bool> destroying_ = false;
    std::unordered_map<int32_t, Agent::Ptr> agents_;
};

} // namespace Sharing
} // namespace OHOS
#endif
