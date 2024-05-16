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

#include "wfd_sink_session_test.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdSinkSessionTest::SetUpTestCase(void)
{
    client_ = std::make_shared<MockClient>();
    session_ = std::make_shared<MockWfdSinkSession>();
    listener_ = std::make_shared<MockSessionListener>();
    session_->InitSession();
    session_->SetSessionListener(listener_);
}

void WfdSinkSessionTest::TearDownTestCase(void)
{
    EXPECT_CALL(*client_, Disconnect());
    client_ = nullptr;
    session_ = nullptr;
    listener_ = nullptr;
}

void WfdSinkSessionTest::SetUp(void)
{
    session_->SetClient(client_);
}

void WfdSinkSessionTest::TearDown(void)
{
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_INIT;
    event.eventMsg = msg;
    session_->SetInterruptState(true);
    session_->SetRunningState(SESSION_INTERRUPT);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_INIT;
    event.eventMsg = msg;
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_STATE_PROSUMER_INIT;
    event.eventMsg = msg;
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).WillOnce(Return(false));

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_KEYMODE_STOP;
    event.eventMsg = msg;
    session_->rtspSession_ = "session_str";
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    session_->SetWfdState(WfdSinkSession::WfdSessionState::PLAYING);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_005, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).WillOnce(Return(false));

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_WFD_REQUEST_IDR;
    event.eventMsg = msg;
    session_->rtspSession_ = "session_str";
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    session_->SetWfdState(WfdSinkSession::WfdSessionState::PLAYING);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_006, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_TEARDOWN;
    event.eventMsg = msg;
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    session_->SetWfdState(MockWfdSinkSession::WfdSessionState::STOPPING);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_007, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_TEARDOWN;
    event.eventMsg = msg;
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    session_->SetWfdState(MockWfdSinkSession::WfdSessionState::PLAYING);
    session_->SetConnectState(false);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_008, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).WillOnce(Return(false));

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_TEARDOWN;
    event.eventMsg = msg;
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    session_->rtspSession_ = "session_str";
    session_->SetWfdState(MockWfdSinkSession::WfdSessionState::PLAYING);
    session_->SetConnectState(true);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, HandleEvent_009, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    SharingEvent event;
    auto msg = std::make_shared<WfdSinkSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_STATE_WRITE_WARNING;
    event.eventMsg = msg;
    session_->SetInterruptState(false);
    session_->SetRunningState(SESSION_START);
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSessionTest, UpdateOperation_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = SESSION_INTERRUPT;
    session_->SetInterruptState(true);
    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateOperation_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = SESSION_PAUSE;
    session_->SetInterruptState(false);

    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateOperation_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = SESSION_RESUME;
    session_->SetInterruptState(false);

    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateOperation_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = SESSION_INTERRUPT;
    session_->SetInterruptState(false);

    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateOperation_005, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).WillOnce(Return(false));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = SESSION_STOP;
    session_->SetInterruptState(false);

    session_->rtspSession_ = "session_str";
    session_->SetWfdState(MockWfdSinkSession::WfdSessionState::PLAYING);
    session_->SetConnectState(true);
    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateOperation_006, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = SESSION_DESTROY;
    session_->SetInterruptState(false);

    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateMediaStatus_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    session_->SetRunningState(SESSION_INTERRUPT);
    session_->SetInterruptState(true);
    session_->UpdateMediaStatus(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateMediaStatus_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = STATE_PROSUMER_CREATE_SUCCESS;
    session_->SetRunningState(SESSION_INTERRUPT);
    session_->SetInterruptState(false);

    session_->UpdateMediaStatus(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateMediaStatus_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = STATE_PROSUMER_START_SUCCESS;
    session_->SetRunningState(SESSION_START);
    session_->SetInterruptState(false);

    session_->UpdateMediaStatus(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateMediaStatus_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = STATE_PROSUMER_STOP_SUCCESS;
    session_->SetRunningState(SESSION_START);
    session_->SetInterruptState(false);

    session_->UpdateMediaStatus(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateMediaStatus_005, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = STATE_PROSUMER_DESTROY_SUCCESS;
    session_->SetRunningState(SESSION_START);
    session_->SetInterruptState(false);

    session_->UpdateMediaStatus(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, UpdateMediaStatus_00, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).WillOnce(Return(false));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    statusMsg->status = STATE_PROSUMER_RESUME_SUCCESS;
    session_->SetRunningState(SESSION_START);
    session_->SetInterruptState(false);
    session_->SetWfdState(WfdSinkSession::WfdSessionState::PLAYING);
    session_->rtspSession_ = "session_str";

    session_->UpdateMediaStatus(statusMsg);
}

HWTEST_F(WfdSinkSessionTest, OnClientReadData_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);

    session_->SetRunningState(SESSION_INTERRUPT);
    session_->SetInterruptState(true);

    session_->OnClientReadData(0, buf);
}

HWTEST_F(WfdSinkSessionTest, OnClientReadData_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    auto buf = std::make_shared<DataBuffer>();
    char data[100] = {0};
    buf->Assign(data, 100);

    session_->SetRunningState(SESSION_START);
    session_->SetInterruptState(false);

    session_->OnClientReadData(0, buf);
}

HWTEST_F(WfdSinkSessionTest, OnClientClose_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    session_->SetRunningState(SESSION_INTERRUPT);
    session_->SetInterruptState(true);

    session_->OnClientClose(0);
}

HWTEST_F(WfdSinkSessionTest, OnClientClose_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    session_->SetRunningState(SESSION_START);
    session_->SetInterruptState(false);
    session_->SetWfdState(WfdSinkSession::WfdSessionState::PLAYING);

    session_->OnClientClose(0);
}

HWTEST_F(WfdSinkSessionTest, HandleRequest_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(false));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspRequest request;
    std::string message("req msg");
    request.SetCSeq(1);
    request.SetMethod(RTSP_METHOD_OPTIONS);

    session_->HandleRequest(request, message);
}

HWTEST_F(WfdSinkSessionTest, HandleRequest_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(1)
        .WillOnce(Return(false));

    RtspRequest request;
    std::string message("req msg");
    request.SetCSeq(1);
    request.SetMethod(RTSP_METHOD_GET_PARAMETER);

    session_->HandleRequest(request, message);
}

HWTEST_F(WfdSinkSessionTest, HandleRequest_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspRequestParameter request;
    request.AddBodyItem(WFD_PARAM_VIDEO_FORMATS);
    request.AddBodyItem(WFD_PARAM_AUDIO_CODECS);
    request.AddBodyItem(WFD_PARAM_RTP_PORTS);
    request.AddBodyItem(WFD_PARAM_CONTENT_PROTECTION);
    request.AddBodyItem(WFD_PARAM_COUPLED_SINK);
    request.AddBodyItem(WFD_PARAM_UIBC_CAPABILITY);
    request.AddBodyItem(WFD_PARAM_STANDBY_RESUME);
    std::string message("req msg");
    request.SetCSeq(1);
    request.SetMethod(RTSP_METHOD_GET_PARAMETER);

    session_->HandleRequest(request, message);
}

HWTEST_F(WfdSinkSessionTest, HandleRequest_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    RtspRequestParameter request;
    request.AddBodyItem(WFD_PARAM_TRIGGER);
    std::string message("req msg");
    request.SetCSeq(1);
    request.SetMethod(RTSP_METHOD_SET_PARAMETER);

    session_->HandleRequest(request, message);
}

HWTEST_F(WfdSinkSessionTest, HandleRequest_005, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspRequestParameter request;
    request.AddBodyItem(WFD_PARAM_VIDEO_FORMATS);
    std::string message("req msg");
    request.SetCSeq(1);
    request.SetMethod(RTSP_METHOD_SET_PARAMETER);

    session_->HandleRequest(request, message);
}

HWTEST_F(WfdSinkSessionTest, HandleRequest_006, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspRequestParameter request;
    request.AddBodyItem(WFD_PARAM_VIDEO_FORMATS);
    std::string message("req msg");
    request.SetCSeq(1);
    request.SetMethod(RTSP_METHOD_SET_PARAMETER);

    session_->HandleRequest(request, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM2Response_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(1, RTSP_STATUS_BAD_REQUEST);
    std::string message("req msg");
    session_->HandleM2Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM2Response_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(1, RTSP_STATUS_OK);
    std::string message("req msg");
    session_->HandleM2Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM6Response_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(1, RTSP_STATUS_BAD_REQUEST);
    std::string message("req msg");
    session_->HandleM6Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM6Response_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(1, RTSP_STATUS_OK);
    std::string message("req msg");
    session_->rtspSession_ = "session_str";
    session_->HandleM6Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM7Response_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(1, RTSP_STATUS_BAD_REQUEST);
    std::string message("req msg");
    session_->HandleM7Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM7Response_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    RtspResponse response(1, RTSP_STATUS_OK);
    std::string message("req msg");
    session_->HandleM7Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM8Response_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(1, RTSP_STATUS_BAD_REQUEST);
    std::string message("req msg");
    session_->HandleM8Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleM8Response_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));
    RtspResponse response(1, RTSP_STATUS_OK);
    std::string message("req msg");
    session_->HandleM8Response(response, message);
}

HWTEST_F(WfdSinkSessionTest, SendM1Response_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(1)
        .WillOnce(Return(false));

    bool ret = session_->SendM1Response(1);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSinkSessionTest, HandleCommonResponse_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(1, RTSP_STATUS_BAD_REQUEST);
    std::string message("req msg");
    session_->HandleCommonResponse(response, message);
}

HWTEST_F(WfdSinkSessionTest, HandleTriggerMethod_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(false));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    bool ret = session_->HandleTriggerMethod(1, RTSP_METHOD_SETUP);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSinkSessionTest, HandleTriggerMethod_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(false));

    session_->SetConnectState(true);
    session_->rtspSession_ = "session_str";
    bool ret = session_->HandleTriggerMethod(1, RTSP_METHOD_TEARDOWN);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSinkSessionTest, SendM7Request_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(client_ != nullptr);

    EXPECT_CALL(*client_, Send(_, _)).Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(*listener_, OnSessionNotify(_));

    session_->rtspSession_ = "session_str";
    bool ret = session_->SendM7Request();
    EXPECT_EQ(ret, false);
}

} // namespace Sharing
} // namespace OHOS
