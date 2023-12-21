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

#ifndef OHOS_SHARING_CONTEXT_MANAGER_H
#define OHOS_SHARING_CONTEXT_MANAGER_H

#include <unordered_map>
#include "context/context.h"
#include "event/handle_event_base.h"
#include "singleton.h"

namespace OHOS {
namespace Sharing {

class ContextManager : public EventEmitter,
                       public Singleton<ContextManager>,
                       public HandleEventBase {
    friend class Singleton<ContextManager>;

public:
    virtual ~ContextManager();

    void Init();
    int32_t HandleEvent(SharingEvent &event) override;

protected:
    uint32_t GetAgentSrcLimit()
    {
        return maxSrcAgent_;
    }

    uint32_t GetAgentSinkLimit()
    {
        return maxSinkAgent_;
    }

protected:
    uint32_t HandleContextCreate();
    void DestroyContext(uint32_t contextId);
    Context::Ptr GetContextById(uint32_t contextId);

private:
    bool CheckAgentSize(AgentType agentType);

    void HandleAgentCreate(SharingEvent &event);
    void HandleContextEvent(SharingEvent &event);
    void HandleConfiguration(SharingEvent &event);
    void HandleMediachannelDestroy(SharingEvent &event);

private:
    uint32_t maxContext_ = MAX_CONTEXT_NUM;
    uint32_t maxSrcAgent_ = MAX_SRC_AGENT_NUM;
    uint32_t maxSinkAgent_ = MAX_SINK_AGENT_NUM;

    std::mutex mutex_;
    std::unordered_map<int32_t, Context::Ptr> contexts_;
};

} // namespace Sharing
} // namespace OHOS
#endif