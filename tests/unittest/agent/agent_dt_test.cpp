/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include "agent/agent.h"
#include "agent_def.h"
#include "common/event_comm.h"
#include "common/sharing_log.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

class MockAgentListener : public IAgentListener {
public:
    MOCK_METHOD(void, OnAgentNotify, (AgentStatusMsg::Ptr &statusMsg), (override));
};

namespace OHOS {
namespace Sharing {

class AgentDTTest : public testing::Test {
protected:
    void SetUp() override {
        agentSrc_ = std::make_shared<Agent>(SRC_AGENT);
        agentSink_ = std::make_shared<Agent>(SINK_AGENT);
        mockListener_ = std::make_shared<MockAgentListener>();
        agentSrc_->SetAgentListener(mockListener_);
        agentSink_->SetAgentListener(mockListener_);
    }

    void TearDown() override {
        agentSrc_.reset();
        agentSink_.reset();
        mockListener_.reset();
    }

    std::shared_ptr<Agent> agentSrc_;
    std::shared_ptr<Agent> agentSink_;
    std::shared_ptr<MockAgentListener> mockListener_;
};

namespace {
HWTEST_F(AgentDTTest, Agent_CreateSession_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    const std::string className = "TestSession";
    SharingErrorCode ret = agentSrc_->CreateSession(className);
    EXPECT_EQ(ret, ERR_OK);
}

HWTEST_F(AgentDTTest, Agent_CreateSession_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    const std::string className = "";
    SharingErrorCode ret = agentSrc_->CreateSession(className);
    EXPECT_EQ(ret, ERR_SESSION_CREATE);
}

HWTEST_F(AgentDTTest, Agent_CreateSession_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    const std::string className = nullptr;
    SharingErrorCode ret = agentSrc_->CreateSession(className);
    EXPECT_EQ(ret, ERR_SESSION_CREATE);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_NullEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = nullptr;
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_NE(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_AgentStart_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_START;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_START);
            EXPECT_EQ(statusMsg->status, SESSION_START);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_AgentDestroy_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_DESTROY);
            EXPECT_EQ(statusMsg->status, SESSION_STOP);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_AgentPause_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_PAUSE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->mediaType = MEDIA_TYPE_VIDEO;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PAUSE);
            EXPECT_EQ(statusMsg->status, SESSION_PAUSE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_AgentResume_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_RESUME;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->mediaType = MEDIA_TYPE_AUDIO;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_RESUME);
            EXPECT_EQ(statusMsg->status, SESSION_RESUME);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerCreate_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PROSUMER_CREATE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId =200 1;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerStart_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PROSUMER_START;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerStop_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PROSUMER_STOP;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerPause_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PROSUMER_PAUSE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerResume_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PROSUMER_RESUME;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerDestroy_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerStarted_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PROSUMER_STARTED;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ProsumerError_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_PROSUMER_ERROR;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_PROSUMER_INIT;
    std::static_pointer_cast<AgentEventMsg>(event.eventMsg)->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_PlayStart_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_PLAY_START;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_PLAY_START);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_PlayStop_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_PLAY_STOP;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_PLAY_STOP);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_PlayStateStart_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PLAY_START;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_PLAY_START);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_PlayStateStart_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PLAY_START;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_INVALID_URL;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_PLAY_START);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_PlayStateStop_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_PLAY_STOP;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_PLAY_STOP);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelAppendSurface_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_APPENDSURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->surface = std::make_shared<Surface>();
    agentEventMsg->sceneType = SCENE_TYPE_SINGLE;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelRemoveSurface_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->surfaceId = 3001;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_REMOVESURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelStateAppendSurface_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        })
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->status, AGENT_STATUS_DONE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelStateAppendSurface_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_INVALID_URL;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelStateRemoveSurface_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->surfaceId = 3001;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_REMOVESURFACE);
        })
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_STATE_REMOVE_SURFACE);
            EXPECT_EQ(statusMsg->surfaceId, 3001);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelSetSceneType_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_SETSCENETYPE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->surfaceId = 3001;
    agentEventMsg->sceneType = SCENE_TYPE_SPLIT;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_SETSCENETYPE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelSetVolume_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_SETVOLUME;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->volume = 0.8f;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_SETVOLUME);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_ChannelSetKeyRedirect_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_SETKEYREDIRECT;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->surfaceId = 3001;
    agentEventMsg->keyRedirect = true;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_KEY_REDIRECT);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_AccelerationDone_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_ACCELERATION_DONE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->surfaceId = 3001;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_ACCELERATION_DONE);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_DecoderDied_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<AgentEventMsg>();
    event.eventMsg->type = EVENT_AGENT_DECODER_DIED;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    auto agentEventMsg = std::static_pointer_cast<AgentEventMsg>(event.eventMsg);
    agentEventMsg->surfaceId = 3001;
    agentEventMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_DECODER_DIED);
        });
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleEvent_Unknown_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    int32_t ret = agentSrc_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerError_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_PROSUMER_ERROR;
    statusMsg->msg->errorCode = ERR_PROSUMER_INIT;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleProsumerError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerError_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_PROSUMER_ERROR;
    statusMsg->msg->errorCode = ERR_PROSUMER_STOP;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleProsumerError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerError_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSink_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_PROSUMER_ERROR;
    statusMsg->msg->errorCode = ERR_PROSUMER_DESTROY;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_DESTROY);
        });
    
    agentSink_->HandleProsumerError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerError_004, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_PROSUMER_ERROR;
    statusMsg->msg->errorCode = ERR_PROSUMER_DESTROY;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_CONTEXT_STATE_AGENT_DESTROY);
        });
    
    agentSrc_->HandleProsumerError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerError_005, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_PROSUMER_ERROR;
    statusMsg->msg->errorCode = ERR_PLAY_STOP;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerState_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->session_ = std::make_shared<BaseSession>();
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_CREATE;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(A_HandleProsumergentDT, Test, AgentTestState Level_{
002_TRUE(agent Function | Small |1)
    ASSERTSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerState_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_PAUSE;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerState_004, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_RESUME;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerState_005, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_STOP;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerState_006, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerState_007, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_STARTED;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleProsumerState_008, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_DECODER_DIED;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->HandleProsumerState(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_SESSION_START;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_CONNECTION_FAILURE;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_INVALID_URL;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_004, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_DECODE_FORMAT;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_005, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_UNAUTHORIZED;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_006, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_INTERACTION_FAILURE;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_007, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_PROTOCOL_INTERACTION_TIMEOUT;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_008, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_NETWORK_ERROR;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_009, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_INTAKE_TIMEOUT;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_010, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_TCP_KEEPALIVE_TIMEOUT;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_011, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_PROTOCOL_CONNECTION_TIMEOUT;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_012, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->errorCode = ERR_SOCKET_ERROR;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_HandleSessionError_013, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg_shared =Agent std::make<EventMsg>();
    statusMsg->msg->errorCode = ERR_PLAY_STOP;
    
    agentSrc_->HandleSessionError(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_START);
    EXPECT_EQ(ret, AGENT_STEP_START);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_CHANNEL_APPENDSURFACE);
    EXPECT_EQ(ret, AGENT_STEP_APPENDSURFACE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_CHANNEL_REMOVESURFACE);
    EXPECT_EQ(ret, AGENT_STEP_REMOVESURFACE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_004, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_PLAY_START);
    EXPECT_EQ(ret, AGENT_STEP_PLAY);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_005, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_PLAY_STOP);
    EXPECT_EQ(ret, AGENT_STEP_PLAYSTOP);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_006, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_PROSUMER_ERROR);
    EXPECT_EQ(ret, AGENT_STEP_DESTROY);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_007, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_DESTROY);
    EXPECT_EQ(ret, AGENT_STEP_DESTROY);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_008, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_ACCELERATION_DONE);
    EXPECT_EQ(ret, AGENT_STEP_IDLE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStep_009, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStep(EVENT_AGENT_DECODER_DIED);
    EXPECT_EQ(ret, AGENT_STEP_IDLE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight(AGENT_STEP_START);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_START);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight(AGENT_STEP_APPENDSURFACE);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight(AGENT_STEP_REMOVESURFACE);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_004, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight(AGENT_STEP_PLAY);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_005, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight(AGENT_STEP_PLAYSTOP);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_REUSABLE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_006, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight(AGENT_STEP_DESTROY);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_DESTROY);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_007, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight(AGENT_STEP_IDLE);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_IDLE);
}

HWTEST_F(AgentDTTest, Agent_GetRunStepWeight_008, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetRunStepWeight((AgentRunStep)999);
    EXPECT_EQ(ret, AGENT_STEP_WEIGHT_IDLE);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_ACCELERATION_DONE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(isCached);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    agentSrc_->runningStatus_ = AGENT_STATUS_DONE;
    agentSrc_->runStep_ = AGENT_STEP_DESTROY;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_GENERAL_ERROR);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    agentSrc_->runningStatus_ = AGENT_STATUS_DONE;
    agentSrc_->runStep_ = AGENT_STEP_IDLE;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(isCached);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_004, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    agentSrc_->runningStatus_ = AGENT_STATUS_DONE;
    agentSrc_->runStep_ = AGENT_STEP_PLAY;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(isCached);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_005, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    agentSrc_->runningStatus_ = AGENT_STATUS_RUNNING;
    agentSrc_->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_EVENT_REPEAT);
    EXPECT_TRUE(isCached);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_006, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->requestId = 1002;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    agentSrc_->runningStatus_ = AGENT_STATUS_RUNNING;
    agentSrc_->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(isCached);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_007, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_DESTROY;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool is =Cached;
    agent falseSrc_->runningStatus_ = AGENT_STATUS_ERROR;
    agentSrc_->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(isCached);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_008, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    agentSrc_->runningStatus_ = AGENT_STATUS_ERROR;
    agentSrc_->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_GENERAL_ERROR);
}

HWTEST_F(AgentDTTest, Agent_CheckRunStep_009, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SharingEvent event;
    event.eventMsg = std::make_shared<EventMsg>();
    event.eventMsg->type = EVENT_AGENT_CHANNEL_REMOVESURFACE;
    event.eventMsg->requestId = 1001;
    event.eventMsg->errorCode = ERR_OK;
    
    bool isCached = false;
    agentSrc_->runningStatus_ = AGENT_STATUS_INTERRUPT;
    agentSrc_->runStep_ = AGENT_STEP_REMOVESURFACE;
    auto ret = agentSrc_->CheckRunStep(event, isCached);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(isCached);
}

HWTEST_F(AgentDTTest, Agent_SetRunningStatus_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->SetRunningStatus(AGENT_STEP_START, AGENT_STATUS_RUNNING);
    EXPECT_EQ(agentSrc_->runStep_, AGENT_STEP_START);
    EXPECT_EQ(agentSrc_->runningStatus_, AGENT_STATUS_RUNNING);
}

HWTEST_F(AgentDTTest, Agent_SetRunningStatus_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->SetRunningStatus(AGENT_STEP_DESTROY, AGENT_STATUS_DONE);
    EXPECT_EQ(agentSrc_->runStep_, AGENT_STEP_DESTROY);
    EXPECT_EQ(agentSrc_->runningStatus_, AGENT_STATUS_DONE);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_START;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
        });
    
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_STOP;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->status, SESSION_DESTROY);
        });
    
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_CONTEXT_STATE_AGENT_DESTROY);
        });
    
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_004, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSink_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_MEDIA_CHANNEL_DESTROY);
        });
    
    agentSink_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_005, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PLAY_START;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_006, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PLAY_STOP;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_007, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_008, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_STATE_REMOVE_SURFACE);
           
    });
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PushNextStep_009, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_START;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->PushNextStep(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_OnSessionNotify_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionNotifyStatus::NOTIFY_SESSION_PRIVATE_EVENT;
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_START;
    statusMsg->msg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_START);
        });
    
    agentSrc_->OnSessionNotify(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_OnSessionNotify_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionNotifyStatus::STATE_SESSION_ERROR;
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_START;
    statusMsg->msg->errorCode = ERR_SESSION_START;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*mockListener_, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_INTERACTION_MSG_ERROR);
        });
    
    agentSrc_->OnSessionNotify(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_OnSessionNotify_003, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionNotifyStatus::STATE_SESSION_IDLE;
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_START;
    statusMsg->msg->errorCode = ERR_SESSION_START;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->OnSessionNotify(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_PopNextStep_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->PopNextStep(AGENT_STEP_IDLE, AGENT_STATUS_IDLE);
}

HWTEST_F(AgentDTTest, Agent_PopNextStep_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->PopNextStep(AGENT_STEP_RESUME, AGENT_STATUS_IDLE);
}

HWTEST_F(AgentDTTest, Agent_UpdateSessionStatus_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    const std::string className = "TestSession";
    EXPECT_TRUE(agentSrc_->CreateSession(className) == ERR_OK);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    statusMsg->className = "TestSession";
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_START;
    statusMsg->msg->errorCode = ERR_OK;
    
    agentSrc_->UpdateSessionStatus(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_UpdateSessionStatus_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionNotifyStatus::NOTIFY_PROSUMER_CREATE;
    statusMsg->className = "TestSession";
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsgmsg->->type = EVENT_AGENT_START;
    statusMsg->msg->errorCode = ERR_OK;
    
    agentSrc_->UpdateSessionStatus(statusMsg);
}

HWTEST_F(AgentDTTest, Agent_SendContextEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*listener, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_CONTEXT_STATE_AGENT_DESTROY);
        });
    
    agentSrc_->SendContextEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendContextEvent_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->SendContextEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendChannelEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    agentSrc_->mediaChannelId_ = 1001;
    agentSrc_->prosumerId_ = 2001;
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*listener, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PROSUMER_RESUME);
            EXPECT_EQ(statusMsg->msg->dstId, 1001);
            EXPECT_EQ(statusMsg->msg->prosumerId, 2001);
        });
    
    agentSrc_->SendChannelEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendChannelEvent_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->SendChannelEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendInteractionEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    agentSrc_->prosumerId_ = 2001;
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*listener, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PROSUMER_RESUME);
            EXPECT_EQ(statusMsg->msg->dstId, 2001);
            EXPECT_EQ(std::static_pointer_cast<InteractionEventMsg>(statusMsg->msg)->agentType, SRC_AGENT);
        });
    
    agentSrc_->SendInteractionEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendInteractionEvent_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->prosumerId = 2001;
    
    agentSrc_->SendInteractionEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendChannelSetVolumeEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    agentSrc_->mediaChannelId_ = 1001;
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->volume = 0.8f;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*listener, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PROSUMER_RESUME);
            EXPECT_EQ(std::static_pointer_cast<ChannelSetVolumeEventMsg>(statusMsg->msg)->volume, 0.8f);
        });
    
    agentSrc_->SendChannelSetVolumeEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendChannelSceneTypeEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    agentSrc_->mediaChannelId_ = 1001;
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->surfaceId = 3001;
    statusMsg->sceneType = SCENE_TYPE_SINGLE;
    statusMsg->prosumerId = 2001;

    EXPECT_CALL(*listener, OnAgentNotify(testing::_)).WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
        EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PROSUMER_RESUME);
        EXPECT_EQ(std::static_pointer_cast<ChannelSetSceneTypeEventMsg>(statusMsg->msg)->surfaceId, 3001);
        EXPECT_EQ(std::static_pointer_cast<ChannelSetSceneTypeEventMsg>(statusMsg->msg)->sceneType, SCENE_TYPE_SINGLE);
    });

    agentSrc_->SendChannelSceneTypeEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendChannelKeyRedirectEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    agentSrc_->mediaChannelId_ = 1001;
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->surfaceId = 3001;
    statusMsg->keyRedirect = true;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*listener, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PROSUMER_RESUME);
            EXPECT_EQ(std::static_pointer_cast<ChannelSetKeyRedirectEventMsg>(statusMsg->msg)->surfaceId, 3001);
            EXPECT_EQ(std::static_pointer_cast<ChannelSetKeyRedirectEventMsg>(statusMsg->msg)->keyRedirect, true);
        });
    
    agentSrc_->SendChannelKeyRedirectEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendChannelAppendSurfaceEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    agentSrc_->mediaChannelId_ = 1001;
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->surface = std::make_shared<Surface>();
    statusMsg->sceneType = SCENE_TYPE_SINGLE;
    statusMsg->prosumerId = 2001;

    EXPECT_CALL(*listener, OnAgentNotify(testing::_)).WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
        EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PROSUMER_RESUME);
        EXPECT_NE(std::static_pointer_cast<ChannelAppendSurfaceEventMsg>(statusMsg->msg)->surface, nullptr);
        EXPECT_EQ(std::static_pointer_cast<ChannelAppendSurfaceEventMsg>(statusMsg->msg)->sceneType, SCENE_TYPE_SINGLE);
    });

    agentSrc_->SendChannelAppendSurfaceEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_SendChannelRemoveSurfaceEvent_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    agentSrc_->mediaChannelId_ = 1001;
    
    SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<AgentEventMsg>();
    statusMsg->msg->type = EVENT_AGENT_STATE_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_OK;
    statusMsg->surfaceId = 3001;
    statusMsg->prosumerId = 2001;
    
    EXPECT_CALL(*listener, OnAgentNotify(testing::_))
        .WillOnce([](AgentStatusMsg::Ptr &statusMsg) {
            EXPECT_EQ(statusMsg->msg->type, EVENT_AGENT_PROSUMER_RESUME);
            EXPECT_EQ(std::static_pointer_cast<ChannelRemoveSurfaceEventMsg>(statusMsg->msg)->surfaceId, 3001);
        });
    
    agentSrc_->SendChannelRemoveSurfaceEvent(statusMsg, EVENT_AGENT_PROSUMER_RESUME);
}

HWTEST_F(AgentDTTest, Agent_GetAgentType_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto ret = agentSrc_->GetAgentType();
    EXPECT_EQ(ret, SRC_AGENT);
}

HWTEST_F(AgentDTTest, Agent_GetAgentType_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSink_ != nullptr);
    
    auto ret = agentSink_->GetAgentType();
    EXPECT_EQ(ret, SINK_AGENT);
}

HWTEST_F(AgentDTTest, Agent_SetDestroy_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->SetDestroy();
    EXPECT_TRUE(agentSrc_->GetDestroy());
}

HWTEST_F(AgentDTTest, Agent_SetDestroy_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    EXPECT_FALSE(agentSrc_->GetDestroy());
}

HWTEST_F(AgentDTTest, Agent_UpdateMediaChannelId_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->UpdateMediaChannelId(1001);
    EXPECT_EQ(agentSrc_->GetMediaChannelId(), 1001);
}

HWTEST_F(AgentDTTest, Agent_UpdateMediaChannelId_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->UpdateMediaChannelId(0);
    EXPECT_EQ(agentSrc_->GetMediaChannelId(), 0);
}

HWTEST_F(AgentDTTest, Agent_SetAgentListener_001, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    auto listener = std::make_shared<MockAgentListener>();
    agentSrc_->SetAgentListener(listener);
    EXPECT_FALSE(agentSrc_->agentListener_.expired());
}

HWTEST_F(AgentDTTest, Agent_SetAgentListener_002, Function | SmallTest | Level1)
{
    ASSERT_TRUE(agentSrc_ != nullptr);
    
    agentSrc_->SetAgentListener(std::weak_ptr<IAgentListener>());
    EXPECT_TRUE(agentSrc_->agentListener_.expired());
}

} // namespace Sharing
} // namespace OHOS
