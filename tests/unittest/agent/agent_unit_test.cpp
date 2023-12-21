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
#include "agent/sinkagent/sink_agent.h"
#include "agent/srcagent/src_agent.h"
#include "common/sharing_log.h"
using namespace testing::ext;

namespace OHOS {
namespace Sharing {
class AgentUnitTest : public testing::Test {};

namespace {
HWTEST_F(AgentUnitTest, SrcAgent01, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
}

HWTEST_F(AgentUnitTest, SrcAgent02_1, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_2, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_START;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_3, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_STOP;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_4, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_PAUSE;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_5, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_FORWARD_MIX_START;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_6, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_FORWARD_MIX_STOP;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_7, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_TRANSPORT_START;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_8, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_TRANSPORT_STOP;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent02_10, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_MEDIA_PUBLISH;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SrcAgent03_1, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SrcAgent03_2, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_START;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SrcAgent03_3, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_STOP;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SrcAgent03_4, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_PAUSE;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SrcAgent03_5, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_DESTROY;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SrcAgent03_6, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::STATE_SESSION_IDLE;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SrcAgent04, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->UpdateMixPlayerID(1);
    ASSERT_TRUE(ret == SharingErrorCode::ERR_OK);
    auto ret1 = srcAgent->GetMixPlayerID();
    ASSERT_TRUE(ret1 == 1);
}

HWTEST_F(AgentUnitTest, SrcAgent05, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    srcAgent->SetSinkAgentId(1);
}

HWTEST_F(AgentUnitTest, SinkAgent01, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
}

HWTEST_F(AgentUnitTest, SinkAgent02_1, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SinkAgent02_3, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_MEDIA_CHANNEL_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SinkAgent02_4, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_START;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SinkAgent02_5, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_STOP;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SinkAgent02_6, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_PAUSE;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SinkAgent02_9, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(AgentUnitTest, SinkAgent03_1, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SinkAgent03_2, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_START;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SinkAgent03_3, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_STOP;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SinkAgent03_4, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_PAUSE;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SinkAgent03_5, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_DESTROY;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(AgentUnitTest, SinkAgent03_6, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::STATE_SESSION_IDLE;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}
} // namespace
} // namespace Sharing
} // namespace OHOS