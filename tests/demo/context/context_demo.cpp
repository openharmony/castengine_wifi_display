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

#include <iostream>
#include "common/sharing_log.h"
#include "context/context_manager.h"

using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {
class ContextTest {
public:
    virtual ~ContextTest() = default;

    void CreateAgent()
    {
        SharingEvent event;
        auto eventMsg = std::make_shared<AgentEventMsg>();
        event.eventMsg = std::move(eventMsg);
        event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
        ContextManager::GetInstance().HandleEvent(event);
        contextId_ = eventMsg->srcId;
        agentId_ = eventMsg->agentId;
        SHARING_LOGD("create agent info contextId: %{public}d agentId: %{public}d", contextId_, agentId_);
    }

    void DestroyContext()
    {
        SharingEvent event;

        auto eventMsg = std::make_shared<ContextEventMsg>();
        event.eventMsg->type = EVENT_CONTEXTMGR_DESTROY;
        eventMsg->dstId = contextId_;

        event.eventMsg = std::move(eventMsg);
        ContextManager::GetInstance().HandleEvent(event);
    }

    void OnAgentEventNotify()
    {
        auto context = std::make_shared<Context>();
        auto contextEventMsg = std::make_shared<ContextEventMsg>();

        auto agentEventMsg = std::make_shared<AgentEventMsg>();
    }

private:
    uint32_t contextId_ = -1;
    uint32_t agentId_ = -1;
};
} // namespace Sharing
} // namespace OHOS

int main()
{
    SHARING_LOGD("test begin");

    auto contextTest = std::make_shared<ContextTest>();
    contextTest->OnAgentEventNotify();

    return 0;
}