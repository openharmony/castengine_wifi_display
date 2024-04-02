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

#include "src_agent_unit_test.h"
#include "agent/srcagent/src_agent.h"
#include "common/sharing_log.h"
using namespace testing::ext;

namespace OHOS {
namespace Sharing {
void SrcAgentUnitTest::SetUpTestCase() {}
void SrcAgentUnitTest::TearDownTestCase() {}
void SrcAgentUnitTest::SetUp() {}
void SrcAgentUnitTest::TearDown() {}

class AgentListener : public IAgentListener {
public:
    void OnAgentNotify(AgentStatusMsg::Ptr &statusMsg) override {}
};

namespace {
HWTEST_F(SrcAgentUnitTest, SrcAgent_001, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_002, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    srcAgent->SetDestroy();
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_003, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    srcAgent->SetDestroy();
    auto ret = srcAgent->GetDestroy();
    ASSERT_TRUE(ret);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_004, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    srcAgent->UpdateMediaChannelId(1);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_005, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    ASSERT_TRUE(listener != nullptr);
    srcAgent->SetAgentListener(listener);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_006, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    uint32_t mediaChannelId = 1;
    srcAgent->UpdateMediaChannelId(mediaChannelId);
    auto ret = srcAgent->GetMediaChannelId();
    ASSERT_TRUE(ret == mediaChannelId);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_007, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetAgentType();
    ASSERT_TRUE(ret == AgentType::SRC_AGENT);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_008, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(eventMsg != nullptr);
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_009, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_MEDIA_CHANNEL_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_010, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_START;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_011, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_STOP;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_012, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_PROSUMER_PAUSE;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_013, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;
    event.eventMsg->dstId = 0;
    auto ret = srcAgent->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_014, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_015, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "";
    ASSERT_TRUE(srcAgent->CreateSession(className) != ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_016, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    srcAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_017, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_STOP;
    srcAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_018, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    srcAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_019, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PLAY_START;
    srcAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_020, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PLAY_STOP;
    srcAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_021, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE;
    srcAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_022, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE;
    srcAgent->PushNextStep(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_023, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_SESSION_START;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_024, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_CONNECTION_FAILURE;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_025, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_INVALID_URL;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_026, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_FORMAT;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_027, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_UNAUTHORIZED;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_028, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_INTERACTION_FAILURE;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_029, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROTOCOL_INTERACTION_TIMEOUT;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_030, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_NETWORK_ERROR;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_031, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_INTAKE_TIMEOUT;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_032, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_DISABLE_ACCELERATION;
    srcAgent->HandleSessionError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_033, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_DISABLE_ACCELERATION;
    srcAgent->UpdateSessionStatus(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_034, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_DECODE_DISABLE_ACCELERATION;
    srcAgent->UpdateSessionStatus(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_035, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    srcAgent->PopNextStep(AGENT_STEP_IDLE, AGENT_STATUS_IDLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_036, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    srcAgent->PopNextStep(AGENT_STEP_RESUME, AGENT_STATUS_IDLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_037, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_038, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_START;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_039, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_STOP;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_040, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_PAUSE;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_041, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_DESTROY;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_042, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->status = SessionNotifyStatus::STATE_SESSION_IDLE;
    sessionMsg->className = "";
    srcAgent->OnSessionNotify(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_043, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    srcAgent->SetRunningStatus(AGENT_STEP_RESUME, AGENT_STATUS_IDLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_044, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGENT_START);
    ASSERT_TRUE(ret == AGENT_STEP_START);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_045, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGENT_CHANNEL_APPENDSURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_APPENDSURFACE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_046, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGENT_CHANNEL_REMOVESURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_REMOVESURFACE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_047, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGENT_PLAY_START);
    ASSERT_TRUE(ret == AGENT_STEP_PLAY);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_048, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGENT_PLAY_STOP);
    ASSERT_TRUE(ret == AGENT_STEP_PLAYSTOP);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_049, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGENT_PROSUMER_ERROR);
    ASSERT_TRUE(ret == AGENT_STEP_DESTROY);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_050, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGENT_DESTROY);
    ASSERT_TRUE(ret == AGENT_STEP_DESTROY);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_051, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStep(EVENT_AGEINT_ACCELERATION_DONE);
    ASSERT_TRUE(ret == AGENT_STEP_IDLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_052, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStepWeight(AGENT_STEP_START);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_START);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_053, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStepWeight(AGENT_STEP_APPENDSURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_054, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStepWeight(AGENT_STEP_REMOVESURFACE);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_055, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStepWeight(AGENT_STEP_PLAY);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_056, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStepWeight(AGENT_STEP_PLAYSTOP);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_057, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStepWeight(AGENT_STEP_DESTROY);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_DESTROY);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_058, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto ret = srcAgent->GetRunStepWeight(AGENT_STEP_IDLE);
    ASSERT_TRUE(ret == AGENT_STEP_WEIGHT_IDLE);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_059, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGEINT_ACCELERATION_DONE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_060, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_DONE;
    srcAgent->runStep_ = AGENT_STEP_DESTROY;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_061, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_DONE;
    srcAgent->runStep_ = AGENT_STEP_IDLE;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_062, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_DONE;
    srcAgent->runStep_ = AGENT_STEP_PLAY;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_063, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_RUNNING;
    srcAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_064, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_RUNNING;
    srcAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_065, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_ERROR;
    srcAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_066, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_ERROR;
    srcAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_067, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->dstId = 0;
    bool isCached = false;
    srcAgent->runningStatus_ = AGENT_STATUS_INTERRUPT;
    srcAgent->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = srcAgent->CheckRunStep(event, isCached);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_068, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
    auto ret = srcAgent->NotifyPrivateEvent(sessionMsg);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_069, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_INTERACTION;
    auto ret = srcAgent->NotifyPrivateEvent(sessionMsg);
    ASSERT_TRUE(ret == ERR_OK);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_070, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    auto ret = srcAgent->NotifyPrivateEvent(sessionMsg);
    ASSERT_TRUE(ret == ERR_GENERAL_ERROR);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_071, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_072, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_CREATE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_073, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_074, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_STOP;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_075, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_PAUSE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_076, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_RESUME;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_077, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_078, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->type = EVENT_AGENT_DECODER_DIED;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerState(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_079, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_INIT;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_080, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_CREATE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_081, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_START;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_082, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_083, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_TIMEOUT;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_084, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_STOP;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_085, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_086, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    const std::string className = "WfdSourceSession";
    ASSERT_TRUE(srcAgent->CreateSession(className) == ERR_OK);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->HandleProsumerError(sessionMsg);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_087, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendContextEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_088, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendContextEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_089, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendChannelEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_090, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendInteractionEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_091, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendChannelSetVolumeEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_092, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendChannelSceneTypeEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_093, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendChannelKeyRedirectEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_094, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendChannelAppendSurfaceEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_095, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->SendChannelRemoveSurfaceEvent(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_096, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    auto listener = std::make_shared<AgentListener>();
    srcAgent->SetAgentListener(listener);
    SessionStatusMsg::Ptr sessionMsg = std::make_shared<SessionStatusMsg>();
    ASSERT_TRUE(sessionMsg != nullptr);
    sessionMsg->msg = std::make_shared<AgentEventMsg>();
    ASSERT_TRUE(sessionMsg->msg != nullptr);
    sessionMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    sessionMsg->className = "";
    sessionMsg->msg->errorCode = ERR_PLAY_STOP;
    sessionMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    sessionMsg->msg->toMgr = ModuleType::MODULE_WINDOW;
    srcAgent->NotifyProducer(sessionMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(SrcAgentUnitTest, SrcAgent_097, Function | SmallTest | Level2)
{
    auto srcAgent = std::make_shared<SrcAgent>();
    ASSERT_TRUE(srcAgent != nullptr);
    uint32_t sinkAgentId = 1;
    srcAgent->SetSinkAgentId(sinkAgentId);
    auto ret = srcAgent->GetSinkAgentId();
    ASSERT_TRUE(ret == sinkAgentId);
}
} // namespace
} // namespace Sharing
} // namespace OHOS