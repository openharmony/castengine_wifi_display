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

#include "wfd_rtp_consumer_test.h"
#include "wfd_media_def.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdRtpConsumerTest::SetUpTestCase(void)
{
    mockRtpServer_ = std::make_shared<MockRtpServer>();
    mockRtpUnpack_ = std::make_shared<MockRtpUnpack>();
    consumer_ = std::make_shared<MockWfdRtpConsumer>();
    listener_ = std::make_shared<MockConsumerListener>();
}

void WfdRtpConsumerTest::TearDownTestCase(void)
{
    if (mockRtpServer_ != nullptr) {
        EXPECT_CALL(*mockRtpServer_, Stop());
    }
    if (mockRtpUnpack_ != nullptr) {
        EXPECT_CALL(*mockRtpUnpack_, Release());
    }
    mockRtpServer_ = nullptr;
    mockRtpUnpack_ = nullptr;
    consumer_ = nullptr;
    listener_ = nullptr;
}

void WfdRtpConsumerTest::SetUp(void)
{
    consumer_->SetConsumerListener(listener_);
    if (mockRtpServer_ == nullptr) {
        mockRtpServer_ = std::make_shared<MockRtpServer>();
    }
    if (mockRtpUnpack_ == nullptr) {
        mockRtpUnpack_ = std::make_shared<MockRtpUnpack>();
    }
    consumer_->rtpUnpacker_ = mockRtpUnpack_;
    consumer_->rtpServer_ = std::make_pair(consumer_->rtpServer_.first, mockRtpServer_);
}

void WfdRtpConsumerTest::TearDown(void)
{
}

HWTEST_F(WfdRtpConsumerTest, HandleEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdConsumerEventMsg>();
    msg->type = EVENT_WFD_BASE;
    event.eventMsg = msg;
    int32_t ret = consumer_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdRtpConsumerTest, HandleEvent_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    SharingEvent event;
    auto msg = std::make_shared<WfdConsumerEventMsg>();
    msg->type = EVENT_WFD_MEDIA_INIT;
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    event.eventMsg = msg;
    int32_t ret = consumer_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdRtpConsumerTest, HandleProsumerInitState_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    auto listener = std::make_shared<MockConsumerListener>();
    consumer_->SetConsumerListener(listener);
    EXPECT_CALL(*listener, OnConsumerNotify(_));

    SharingEvent event;
    auto msg = std::make_shared<WfdConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    event.eventMsg = msg;
    consumer_->HandleProsumerInitState(event);
}

HWTEST_F(WfdRtpConsumerTest, UpdateOperation_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_INIT;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpConsumerTest, UpdateOperation_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_START;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpConsumerTest, UpdateOperation_003, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_PAUSE;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpConsumerTest, UpdateOperation_004, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_RESUME;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpConsumerTest, UpdateOperation_005, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(consumer_->rtpUnpacker_ != nullptr);

    EXPECT_CALL(*listener_, OnConsumerNotify(_));
    EXPECT_CALL(*mockRtpUnpack_, Release());
    EXPECT_CALL(*mockRtpServer_, Stop());

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_STOP;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpConsumerTest, UpdateOperation_006, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);
    ASSERT_TRUE(consumer_->rtpUnpacker_ != nullptr);

    EXPECT_CALL(*listener_, OnConsumerNotify(_));
    EXPECT_CALL(*mockRtpUnpack_, Release());
    EXPECT_CALL(*mockRtpServer_, Stop());

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_DESTROY;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpConsumerTest, OnRtpUnpackCallback_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    auto dispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_CALL(*listener_, GetDispatcher()).WillOnce(Return(dispatcher));
    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_PAUSE;
    consumer_->UpdateOperation(statusMsg);

    FrameImpl::Ptr frame = std::make_shared<FrameImpl>();
    frame->codecId_ = CODEC_AAC;
    consumer_->OnRtpUnpackCallback(0, frame);
}

HWTEST_F(WfdRtpConsumerTest, OnRtpUnpackCallback_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    auto dispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_CALL(*listener_, GetDispatcher()).WillOnce(Return(dispatcher));
    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_PAUSE;
    consumer_->UpdateOperation(statusMsg);

    FrameImpl::Ptr frame = std::make_shared<FrameImpl>();
    frame->codecId_ = CODEC_H264;
    consumer_->OnRtpUnpackCallback(0, frame);
}

HWTEST_F(WfdRtpConsumerTest, OnRtpUnpackCallback_003, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    auto dispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_CALL(*listener_, GetDispatcher()).WillOnce(Return(dispatcher));
    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_RESUME;
    consumer_->UpdateOperation(statusMsg);

    FrameImpl::Ptr frame = std::make_shared<FrameImpl>();
    frame->codecId_ = CODEC_AAC;
    consumer_->OnRtpUnpackCallback(0, frame);
}

HWTEST_F(WfdRtpConsumerTest, OnRtpUnpackCallback_004, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    auto dispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_CALL(*listener_, GetDispatcher()).WillOnce(Return(dispatcher));
    EXPECT_CALL(*listener_, OnConsumerNotify(_));

    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_RESUME;
    consumer_->UpdateOperation(statusMsg);

    FrameImpl::Ptr frame = std::make_shared<FrameImpl>();
    frame->codecId_ = CODEC_H264;
    char data[100];
    frame->Assign(data, 100);
    consumer_->OnRtpUnpackCallback(0, frame);
}

HWTEST_F(WfdRtpConsumerTest, OnServerReadData_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->rtpUnpacker_ != nullptr);

    EXPECT_CALL(*mockRtpUnpack_, ParseRtp(_, _));

    DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);
    consumer_->SetRunningState(true);
    consumer_->OnServerReadData(0, buf, nullptr);
}

} // namespace Sharing
} // namespace OHOS
