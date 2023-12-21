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

#ifndef OHOS_SHARING_AGENT_DEF_H
#define OHOS_SHARING_AGENT_DEF_H

namespace OHOS {
namespace Sharing {

enum AgentType { NONE_AGENT, SRC_AGENT, SINK_AGENT };

enum AgentRunStep {
    AGENT_STEP_IDLE,
    AGENT_STEP_CREATE,
    AGENT_STEP_START,
    AGENT_STEP_PAUSE,
    AGENT_STEP_RESUME,
    AGENT_STEP_APPENDSURFACE,
    AGENT_STEP_REMOVESURFACE,
    AGENT_STEP_PLAY,
    AGENT_STEP_PLAYSTOP,
    AGENT_STEP_DESTROY
};

enum AgentRunStepWeight {
    AGENT_STEP_WEIGHT_IDLE,
    AGENT_STEP_WEIGHT_REUSABLE,
    AGENT_STEP_WEIGHT_START,
    AGENT_STEP_WEIGHT_DESTROY
};

enum AgentRunningStatus {
    AGENT_STATUS_IDLE,
    AGENT_STATUS_RUNNING,
    AGENT_STATUS_DONE,
    AGENT_STATUS_ERROR,
    AGENT_STATUS_INTERRUPT
};

struct AgentRunStepKey {
    bool operator<(AgentRunStepKey const &agentRunStepKey) const
    {
        if (weight == agentRunStepKey.weight) {
            if (stepType == agentRunStepKey.stepType) {
                return requestId < agentRunStepKey.requestId;
            }
            return stepType < agentRunStepKey.stepType;
        } else {
            return weight > agentRunStepKey.weight;
        }
    }

    uint32_t requestId = 0;
    AgentRunStep stepType = AGENT_STEP_IDLE;
    AgentRunStepWeight weight = AGENT_STEP_WEIGHT_IDLE;
};

} // namespace Sharing
} // namespace OHOS
#endif