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

#include <gtest/gtest.h>
#include <iostream>
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "context/context.h"
#include "context/context_manager.h"
#include "agent/srcagent/src_agent.h"
#include "configuration/include/sharing_data.h"
#include "configuration/include/config.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

class ContextTestScene : public BaseSession,
                         public std::enable_shared_from_this<ContextTestScene> {
public:
    ContextTestScene()
    {
        std::cout << "ContextTestScene constructor called." << std::endl;
    }

    void UpdateOperation(SessionStatusMsg::Ptr &statusMsg) override
    {
        (void)statusMsg;
        return;
    }
    void UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg) override
    {
        (void)statusMsg;
        return;
    }
    int32_t HandleEvent(SharingEvent &event) override
    {
        (void)event;
        return 0;
    }
};

class ContextImpl : public Context {
public:
    ContextImpl() = default;
};
REGISTER_CLASS_REFLECTOR(ContextTestScene);
class SharingContextUnitTest : public testing::Test {};

namespace {

HWTEST_F(SharingContextUnitTest, Context_Base_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}


HWTEST_F(SharingContextUnitTest, Context_Base_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_STATE_AGENT_DESTROY;
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}


HWTEST_F(SharingContextUnitTest, Context_Base_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_STATE_AGENT_DESTROY;
    context->Release();
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(SharingContextUnitTest, Context_Base_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret  = 0;
    bool tag = false;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;

    tag = context->IsEmptyAgent();
    EXPECT_EQ(tag, true);
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    
    tag = context->IsEmptyAgent();
    EXPECT_EQ(tag, false);
}

HWTEST_F(SharingContextUnitTest, Context_Base_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = 0;
    bool tag = false;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    uint32_t agentId;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->type = EVENT_CONTEXT_AGENT_CREATE;
    
    tag = context->IsEmptySrcAgent(contextEventMsgSptr->agentId);
    EXPECT_EQ(tag, true);
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    agentId = contextEventMsgSptr->agentId;
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = contextEventMsgSptr;
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    
    tag = context->IsEmptySrcAgent(agentId);
    EXPECT_EQ(tag, false);
}

HWTEST_F(SharingContextUnitTest, Context_Base_06, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->type = EVENT_CONTEXT_AGENT_CREATE;
    
    ret = context->GetSrcAgentSize();
    EXPECT_EQ(ret, 0);
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = contextEventMsgSptr;
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    
    ret = context->GetSrcAgentSize();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(SharingContextUnitTest, Context_Base_07, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    SharingEvent event;
    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    
    ret = context->GetSinkAgentSize();
    EXPECT_EQ(ret, 0);
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    
    ret = context->GetSinkAgentSize();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(SharingContextUnitTest, Context_Base_008, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    
    context->Release();
}

HWTEST_F(SharingContextUnitTest, Context_Base_009, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<AgentStatusMsg> StatusMsg = std::make_shared<AgentStatusMsg>();
    
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    
    StatusMsg->msg = std::make_shared<EventMsg>();
    context->OnAgentNotify(StatusMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_010, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_COMMON_BASE, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_011, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_CONTEXTMGR_AGENT_CREATE, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_012, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_AGENT_DESTROY, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_013, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
   
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_SESSION_BASE, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_014, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_MEDIA_CONSUMER_PAUSE, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_015, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_SCHEDULER_BASE, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_016, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_INTERACTION_MSG_ERROR, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_017, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_CONFIGURE_BASE, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_018, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_VIDEOCHAT, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_019, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_WFD_NOTIFY_RTSP_PLAYED, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_020, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_PLAYER, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_021, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->SendInteractionEvent(EVENT_SCREEN_CAPTURE_BASE, eventMsg);
}

HWTEST_F(SharingContextUnitTest, Context_Base_022, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    context->RemoveAgent(eventMsg->agentId);
}

HWTEST_F(SharingContextUnitTest, Context_Base_023, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    context->DistributeEvent(event);
}

HWTEST_F(SharingContextUnitTest, Context_Base_024, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto agent = context->GetAgentById(contextEventMsgSptr->agentId);
    EXPECT_NE(agent, nullptr);
}

HWTEST_F(SharingContextUnitTest, Context_Base_025, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ret = context->HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto agent = context->GetAgentById(contextEventMsgSptr->agentId);
    EXPECT_NE(agent, nullptr);
    auto srcAgent = std::static_pointer_cast<SrcAgent>(agent);
    uint32_t sinkAgentId = srcAgent->GetSinkAgentId();
    context->CheckNeedDestroySink(sinkAgentId);
}

HWTEST_F(SharingContextUnitTest, Context_Base_026, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->HandleAgentDestroy(event);
}

HWTEST_F(SharingContextUnitTest, Context_Base_027, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    context->HandleStateDestroyAgent(event);
}

HWTEST_F(SharingContextUnitTest, Context_Base_028, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    uint32_t CreatRet = 0;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SRC_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    CreatRet = context->HandleCreateAgent(eventMsg->className, eventMsg->agentType, eventMsg->agentId);
    EXPECT_EQ(CreatRet, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Base_029, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    uint32_t CreatRet = 0;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    
    contextEventMsgSptr->agentType = SINK_AGENT;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    auto eventMsg = ConvertEventMsg<ContextEventMsg>(event);
    std::shared_ptr<Context> context = std::make_shared<Context>();
    CreatRet = context->HandleCreateAgent(eventMsg->className, eventMsg->agentType, eventMsg->agentId);
    EXPECT_EQ(CreatRet, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Base_030, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<Context> context = std::make_shared<Context>();
    EXPECT_NE(context, nullptr);
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ret = context->HandleCreateAgent(contextEventMsgSptr->className,
        contextEventMsgSptr->agentType, contextEventMsgSptr->agentId);
    EXPECT_NE(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXTMGR_CREATE;
    
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
   
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXTMGR_AGENT_CREATE;
    
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXTMGR_STATE_CHANNEL_DESTROY;
    
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_STATE_AGENT_DESTROY;
    
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_06, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_STATE_AGENT_DESTROY;
    
    bool ret = ContextManager::GetInstance().CheckAgentSize(contextEventMsgSptr->agentType);
    EXPECT_EQ(ret, true);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_07, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SRC_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_STATE_AGENT_DESTROY;
    
    bool ret = ContextManager::GetInstance().CheckAgentSize(contextEventMsgSptr->agentType);
    EXPECT_EQ(ret, true);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_08, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    bool tag = false;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    ContextManager::GetInstance().maxSinkAgent_ = 0;
    tag = ContextManager::GetInstance().CheckAgentSize(contextEventMsgSptr->agentType);
    EXPECT_EQ(tag, false);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_09, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    bool tag = false;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SRC_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    ContextManager::GetInstance().maxSrcAgent_ = 0;
    tag = ContextManager::GetInstance().CheckAgentSize(contextEventMsgSptr->agentType);
    EXPECT_EQ(tag, false)
}

HWTEST_F(SharingContextUnitTest, Context_Manager_10, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    ContextManager::GetInstance().HandleAgentCreate(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_11, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    ContextManager::GetInstance().maxSinkAgent_ = 0;
    ContextManager::GetInstance().HandleAgentCreate(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_12, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    uint32_t ret = 0;
    ret = ContextManager::GetInstance().HandleContextCreate();
    EXPECT_NE(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_13, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    uint32_t ret = 0;
    ContextManager::GetInstance().maxContext_ = 0;
    ret = ContextManager::GetInstance().HandleContextCreate();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_14, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    uint32_t ret = 0;
    ret = ContextManager::GetInstance().HandleContextCreate();
    EXPECT_EQ(ret, 0);
    ContextManager::GetInstance().DestroyContext(58);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_15, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().contexts_ = {};
    ContextManager::GetInstance().DestroyContext(58)
}

HWTEST_F(SharingContextUnitTest, Context_Manager_16, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    uint32_t ret = 0;
    Context::Ptr context = nullptr;
    ContextManager::GetInstance().maxContext_ = 20;
    ret = ContextManager::GetInstance().HandleContextCreate();
    EXPECT_NE(ret, 0);
    context = ContextManager::GetInstance().GetContextById(59);
    EXPECT_NE(context, nullptr);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_17, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    Context::Ptr context = nullptr;
    ContextManager::GetInstance().Init();
    context = ContextManager::GetInstance().GetContextById(60);
    EXPECT_EQ(context, nullptr)
}

HWTEST_F(SharingContextUnitTest, Context_Manager_18, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ContextManager::GetInstance().HandleAgentCreate(event);
    ContextManager::GetInstance().HandleContextEvent(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_19, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    ContextManager::GetInstance().HandleContextEvent(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_20, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::string  value("5687868");
    SharingEvent event;
    std::shared_ptr<ConfigEventMsg> ConfigEventMsgSptr;
    ConfigEventMsgSptr = std::make_shared<ConfigEventMsg>(ConfigStatus::CONFIG_STATUS_INVALID, MODULE_CONTEXT);
    ConfigEventMsgSptr->data =  std::make_shared<SharingDataGroupByModule>("ContextTestScene");
    ConfigEventMsgSptr->data->PutSharingValue("agentLimit", "testdescription", std::make_shared<SharingValue>(value));

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = ConfigEventMsgSptr;

    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;

    ContextManager::GetInstance().HandleConfiguration(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_21, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    std::shared_ptr<ConfigEventMsg> ConfigEventMsgSptr;
    ConfigEventMsgSptr= std::make_shared<ConfigEventMsg>(ConfigStatus::CONFIG_STATUS_INVALID, MODULE_CONTEXT);
    ConfigEventMsgSptr->data =  std::make_shared<SharingDataGroupByModule>("ContextTestScene");
    ConfigEventMsgSptr->data->PutSharingValue("networkLimit", "logOn", std::make_shared<SharingValue>(123));

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = ConfigEventMsgSptr;

    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;

    ContextManager::GetInstance().HandleConfiguration(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_22, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    std::shared_ptr<ConfigEventMsg> ConfigEventMsgSptr;
    ConfigEventMsgSptr = std::make_shared<ConfigEventMsg>(ConfigStatus::CONFIG_STATUS_INVALID, MODULE_CONTEXT);

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = ConfigEventMsgSptr;

    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;

    ContextManager::GetInstance().HandleConfiguration(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_23, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ContextManager::GetInstance().HandleAgentCreate(event);
    ContextManager::GetInstance().HandleMediachannelDestroy(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_24, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    ContextManager::GetInstance().HandleMediachannelDestroy(event);
}

HWTEST_F(SharingContextUnitTest, Context_Manager_25, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ContextManager::GetInstance().Init();
    SharingEvent event;
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();

    contextEventMsgSptr->className = "ContextTestScene";
    contextEventMsgSptr->agentType = SINK_AGENT;
    contextEventMsgSptr->agentId = 100;

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;

    std::shared_ptr<Context> context ;
    ContextManager::GetInstance().HandleAgentCreate(event);
    context = ContextManager::GetInstance().GetContextById(event.eventMsg->dstId);
    context->HandleCreateAgent(contextEventMsgSptr->className,
        contextEventMsgSptr->agentType, contextEventMsgSptr->agentId);
    ContextManager::GetInstance().HandleMediachannelDestroy(event);
}
} // namespace
} // namespace Sharing
} // namespace OHOS