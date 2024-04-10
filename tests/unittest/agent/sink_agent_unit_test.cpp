/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "sink_agent_unit_test.h"
#include "agent/sinkagent/sink_agent.h"
#include "common/sharing_log.h"
using namespace testing::ext;

namespace OHOS {
namespace Sharing {
void SinkAgentUnitTest::SetUpTestCase() {}
void SinkAgentUnitTest::TearDownTestCase() {}
void SinkAgentUnitTest::SetUp() {}
void SinkAgentUnitTest::TearDown() {}

class AgentListener : public IAgentListener {
public:
    void OnAgentNotify(AgentStatusMsg::Ptr &statusMsg) override {}
};

namespace {
HWTEST_F(SinkAgentUnitTest, SinkAgent_001, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_002, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    sinkAgent->SetDestroy();
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_003, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    sinkAgent->SetDestroy();
    auto ret = sinkAgent->GetDestroy();
    ASSERT_TRUE(ret);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_004, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    sinkAgent->UpdateMediaChannelId(1);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_005, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    ASSERT_TRUE(listener != nullptr);
    sinkAgent->SetAgentListener(listener);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_006, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    uint32_t mediaChannelId = 1;
    sinkAgent->UpdateMediaChannelId(mediaChannelId);
    auto ret = sinkAgent->GetMediaChannelId();
    ASSERT_TRUE(ret == mediaChannelId);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_007, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetAgentType();
    ASSERT_TRUE(ret == AgentType::SINK_AGENT);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_008, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_009, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_MEDIA_CHANNEL_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_010, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_START;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_011, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_STOP;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_012, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_PAUSE;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_013, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;
    event.eventMsg->dstId = 0;
    auto ret = sinkAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_014, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_015, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "";
    ASSERT_TRUE(sinkAgent->CreateSession(className) != ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_016, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sinkAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_017, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_STOP;
    sinkAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_018, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sinkAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_019, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PLAY_START;
    sinkAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_020, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PLAY_STOP;
    sinkAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_021, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE;
    sinkAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_022, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE;
    sinkAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_023, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_SESSION_START;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_024, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_CONNECTION_FAILURE;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_025, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_INVALID_URL;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_026, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_FORMAT;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_027, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_UNAUTHORIZED;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_028, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_INTERACTION_FAILURE;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_029, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROTOCOL_INTERACTION_TIMEOUT;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_030, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_NETWORK_ERROR;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_031, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_INTAKE_TIMEOUT;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_032, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_DISABLE_ACCELERATION;
    sinkAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_033, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_DISABLE_ACCELERATION;
    sinkAgent->UpdateSessionStatus(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_034, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_DISABLE_ACCELERATION;
    sinkAgent->UpdateSessionStatus(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_035, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    sinkAgent->PopNextStep(AGENT_STEP_IDLE, AGENT_STATUS_IDLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_036, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    sinkAgent->PopNextStep(AGENT_STEP_RESUME, AGENT_STATUS_IDLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_037, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_038, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_START;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_039, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_STOP;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_040, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_PAUSE;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_041, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_DESTROY;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_042, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::STATE_SESSION_IDLE;
    sessionMsg->className = "";
    sinkAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_043, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    sinkAgent->SetRunningStatus(AGENT_STEP_RESUME, AGENT_STATUS_IDLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_044, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGENT_START);
    ASSERT_TRUE(ret == AGENT_STEP_START);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_045, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGENT_CHANNEL_APPENDSURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_APPENDSURFACE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_046, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGENT_CHANNEL_REMOVESURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_REMOVESURFACE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_047, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGENT_PLAY_START);
    ASSERT_TRUE(ret == AGENT_STEP_PLAY);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_048, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGENT_PLAY_STOP);
    ASSERT_TRUE(ret == AGENT_STEP_PLAYSTOP);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_049, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGENT_PROSUMER_ERROR);
    ASSERT_TRUE(ret == AGENT_STEP_DESTROY);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_050, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGENT_DESTROY);
    ASSERT_TRUE(ret == AGENT_STEP_DESTROY);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_051, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStep(EVENT_AGEINT_ACCELERATION_DONE);
    ASSERT_TRUE(ret == AGENT_STEP_IDLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_052, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStepWeight(AGENT_STEP_START);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_START);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_053, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStepWeight(AGENT_STEP_APPENDSURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_054, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStepWeight(AGENT_STEP_REMOVESURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_055, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStepWeight(AGENT_STEP_PLAY);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_056, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStepWeight(AGENT_STEP_PLAYSTOP);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_057, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStepWeight(AGENT_STEP_DESTROY);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_DESTROY);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_058, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto ret = sinkAgent->GetRunStepWeight(AGENT_STEP_IDLE);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_IDLE);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_059, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGEINT_ACCELERATION_DONE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_060, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_DONE;
    sinkAgent->runStep_ = AGENT_STEP_DESTROY;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_061, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_DONE;
    sinkAgent->runStep_ = AGENT_STEP_IDLE;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_062, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_DONE;
    sinkAgent->runStep_ = AGENT_STEP_PLAY;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_063, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_RUNNING;
    sinkAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_064, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_RUNNING;
    sinkAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_065, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_ERROR;
    sinkAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_066, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_ERROR;
    sinkAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_067, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    sinkAgent->runningStatus_ = AGENT_STATUS_INTERRUPT;
    sinkAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = sinkAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_068, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
    auto ret = sinkAgent->NotifyPrivateEvent(sessionMsg);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_069, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_INTERACTION;
    auto ret = sinkAgent->NotifyPrivateEvent(sessionMsg);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_070, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    auto ret = sinkAgent->NotifyPrivateEvent(sessionMsg);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_071, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_072, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_CREATE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_073, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_074, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_STOP;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_075, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_PAUSE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_076, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_RESUME;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_077, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_078, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_DECODER_DIED;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_079, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_INIT;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_080, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_CREATE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_081, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_082, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_083, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_TIMEOUT;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_084, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_STOP;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_085, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_086, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    const std::string className = "WfdSinkSession";
    ASSERT_TRUE(sinkAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_087, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendContextEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_088, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendContextEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_089, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendChannelEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_090, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendInteractionEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_091, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendChannelSetVolumeEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_092, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendChannelSceneTypeEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_093, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendChannelKeyRedirectEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_094, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendChannelAppendSurfaceEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_095, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->SendChannelRemoveSurfaceEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SinkAgentUnitTest, SinkAgent_096, Function | SmallTest | Level2)
{
    auto sinkAgent = std::make_shared<SinkAgent>();
    ASSERT_TRUE(sinkAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    sinkAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    sinkAgent->NotifyConsumer(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}
} // namespace
} // namespace Sharing
} // namespace OHOS