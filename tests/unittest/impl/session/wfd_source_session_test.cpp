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

#include "wfd_source_session_test.h"
#include "wfd_message.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdSourceSessionTest::SetUpTestCase(void)
{
    server_ = std::make_shared<MockServer>();
    session_ = std::make_shared<MockWfdSourceSession>();
    listener_ = std::make_shared<MockSessionListener>();
    networkSession_ = std::make_shared<MockNetworkSession>();
    networkSessionCb_ = std::make_shared<WfdSourceSession::WfdSourceNetworkSession>(networkSession_, session_);
    session_->InitSession();
    session_->SetServer(server_);
    session_->SetSessionListener(listener_);
    networkSession_->RegisterCallback(networkSessionCb_);
}

void WfdSourceSessionTest::TearDownTestCase(void)
{
    EXPECT_CALL(*server_, Stop());

    server_ = nullptr;
    session_ = nullptr;
    listener_ = nullptr;
    networkSession_ = nullptr;
    networkSessionCb_ = nullptr;
}

void WfdSourceSessionTest::SetUp(void)
{
}

void WfdSourceSessionTest::TearDown(void)
{
}

HWTEST_F(WfdSourceSessionTest, SetKeepAliveTimer_001, TestSize.Level1)
{
    ASSERT_TRUE(networkSessionCb_ != nullptr);
    networkSessionCb_->SetKeepAliveTimer();
}

HWTEST_F(WfdSourceSessionTest, UnsetKeepAliveTimer_001, TestSize.Level1)
{
    ASSERT_TRUE(networkSessionCb_ != nullptr);
    networkSessionCb_->UnsetKeepAliveTimer();
}

HWTEST_F(WfdSourceSessionTest, OnSessionWriteable_001, TestSize.Level1)
{
    ASSERT_TRUE(networkSessionCb_ != nullptr);
    networkSessionCb_->OnSessionWriteable(0);
}

HWTEST_F(WfdSourceSessionTest, OnSessionClose_001, TestSize.Level1)
{
    ASSERT_TRUE(networkSessionCb_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    networkSessionCb_->OnSessionClose(0);
}

HWTEST_F(WfdSourceSessionTest, OnSessionException_001, TestSize.Level1)
{
    ASSERT_TRUE(networkSessionCb_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    networkSessionCb_->OnSessionException(0);
}

HWTEST_F(WfdSourceSessionTest, OnSendKeepAlive_001, TestSize.Level1)
{
    ASSERT_TRUE(networkSessionCb_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));
    
    networkSessionCb_->OnSendKeepAlive();
}

HWTEST_F(WfdSourceSessionTest, NotifyServiceError_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    session_->NotifyServiceError();
}

HWTEST_F(WfdSourceSessionTest, UpdateOperation_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_SESSION_STOPED;
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSourceSessionTest, UpdateOperation_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_SESSION_PAUSED;
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSourceSessionTest, UpdateOperation_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_SESSION_RESUMED;
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSourceSessionTest, UpdateOperation_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_SESSION_DESTROYED;
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    session_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdSourceSessionTest, NotifyProsumerInit_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_SESSION_DESTROYED;
    auto msg = std::make_shared<EventMsg>();
    statusMsg->msg = msg;
    session_->NotifyProsumerInit(statusMsg);
}

HWTEST_F(WfdSourceSessionTest, HandleEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    
    SharingEvent event;
    auto msg = std::make_shared<WfdSourceSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_INIT;
    event.eventMsg = msg;
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSessionTest, HandleEvent_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    SharingEvent event;
    auto msg = std::make_shared<WfdSourceSessionEventMsg>();
    msg->type = EventType::EVENT_WFD_STATE_MEDIA_INIT;
    event.eventMsg = msg;
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSessionTest, HandleEvent_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSourceSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_KEYMODE_STOP;
    event.eventMsg = msg;
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSessionTest, HandleEvent_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSourceSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_TEARDOWN;
    event.eventMsg = msg;
    int32_t ret = session_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSessionTest, OnServerReadData_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    RtspResponse response;
    std::string rspMsg(response.Stringify());
    auto buf = std::make_shared<DataBuffer>();
    buf->Assign(rspMsg.c_str(), rspMsg.size());

    session_->OnServerReadData(1, buf, networkSession_);
}

HWTEST_F(WfdSourceSessionTest, OnServerReadData_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(true));

    RtspRequest request;
    request.SetMethod(RTSP_METHOD_OPTIONS);
    std::string reqMsg(request.Stringify());
    auto buf = std::make_shared<DataBuffer>();
    buf->Assign(reqMsg.c_str(), reqMsg.size());

    session_->OnServerReadData(1, buf, networkSession_);
}

HWTEST_F(WfdSourceSessionTest, OnServerReadData_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    RtspRequest request;
    std::string reqMsg(request.Stringify());
    auto buf = std::make_shared<DataBuffer>();
    buf->Assign(reqMsg.c_str(), reqMsg.size());

    session_->OnServerReadData(1, buf, networkSession_);
}

HWTEST_F(WfdSourceSessionTest, HandleRequest_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    RtspRequest request;
    request.SetMethod(RTSP_METHOD_OPTIONS);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleRequest(request, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleRequest_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    RtspRequest request;
    request.SetMethod(RTSP_METHOD_SETUP);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleRequest(request, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleRequest_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);
    ASSERT_TRUE(networkSessionCb_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));
    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    RtspRequest request;
    request.SetMethod(RTSP_METHOD_PLAY);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleRequest(request, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleRequest_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));
    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    RtspRequest request;
    request.SetMethod(RTSP_METHOD_PAUSE);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleRequest(request, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleRequest_005, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));
    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    RtspRequest request;
    request.SetMethod(RTSP_METHOD_TEARDOWN);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleRequest(request, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleRequest_006, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    RtspRequestParameter request;
    request.SetSession(session_->sessionID_);
    request.SetMethod(RTSP_METHOD_SET_PARAMETER);
    request.AddBodyItem(WFD_PARAM_IDR_REQUEST);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleRequest(request, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    RtspResponse response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    std::string message = "";
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    RtspResponse response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    std::string message = "";
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M1);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_003, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    WfdRtspM3Response response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    response.SetVideoFormats(VIDEO_1920X1080_30);
    std::string message(response.Stringify());
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M3);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_004, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    WfdRtspM4Response response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    std::string message(response.Stringify());
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M4);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_005, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));
    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(true));

    WfdRtspM4Response response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    std::string message(response.Stringify());
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M4);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_006, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    WfdRtspM5Response response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    std::string message(response.Stringify());
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M5);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_007, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    RtspResponse response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    std::string message(response.Stringify());
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M7_WAIT);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_008, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    RtspResponse response(session_->cseq_, RTSP_STATUS_BAD_REQUEST);
    response.SetSession(session_->sessionID_);
    std::string message(response.Stringify());
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M7_WAIT);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, HandleResponse_009, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnSessionNotify(_));

    RtspResponse response(session_->cseq_, RTSP_STATUS_OK);
    response.SetSession(session_->sessionID_);
    std::string message(response.Stringify());
    session_->SetWfdState(MockWfdSourceSession::WfdSessionState::M8);
    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->HandleResponse(response, message, session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, SendM2Response_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM2Response(session_->cseq_, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, SendM2Response_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(true));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM2Response(session_->cseq_, session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, SendM6Response_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM6Response(session, session_->cseq_);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, SendM6Response_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(true));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM6Response(session, session_->cseq_);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, SendM1Request_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM1Request(session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, SendM1Request_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(true));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM1Request(session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, SendM3Request_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM3Request(session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, SendM3Request_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(true));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM3Request(session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, SendM4Request_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM4Request(session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, SendM4Request_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(true));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM4Request(session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, SendM5Request_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM5Request(session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, SendM5Request_002, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(true));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM5Request(session);
    EXPECT_EQ(ret, true);
}

HWTEST_F(WfdSourceSessionTest, SendCommonResponse_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendCommonResponse(session_->cseq_, session);
    EXPECT_EQ(ret, false);
}

HWTEST_F(WfdSourceSessionTest, SendM16Request_001, TestSize.Level1)
{
    ASSERT_TRUE(session_ != nullptr);
    ASSERT_TRUE(networkSession_ != nullptr);

    EXPECT_CALL(*networkSession_, Send(_, _)).WillOnce(Return(false));

    auto session = static_cast<INetworkSession::Ptr>(networkSession_);
    bool ret = session_->SendM16Request(session);
    EXPECT_EQ(ret, false);
}

} // namespace Sharing
} // namespace OHOS
