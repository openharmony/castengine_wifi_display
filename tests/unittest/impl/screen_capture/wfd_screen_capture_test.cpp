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

#include "wfd_screen_capture_test.h"
#include "mock_media_channel.h"
#include "screen_capture_def.h"
#include "common/const_def.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdScreenCaptureTest::SetUpTestCase(void)
{
    consumer_ = std::make_shared<MockScreenCaptureConsumer>();
    consumer_->Initialize();
}

void WfdScreenCaptureTest::TearDownTestCase(void)
{
    consumer_->DeInitialize();
    consumer_ = nullptr;
}

void WfdScreenCaptureTest::SetUp(void)
{
}

void WfdScreenCaptureTest::TearDown(void)
{
}

HWTEST_F(WfdScreenCaptureTest, HandleSpsFrame_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->dispatcher_ != nullptr);

    Frame::Ptr frame = FrameImpl::Create();
    consumer_->HandleSpsFrame(consumer_->dispatcher_, frame);
}

HWTEST_F(WfdScreenCaptureTest, HandleSpsFrame_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->dispatcher_ != nullptr);

    Frame::Ptr frame = FrameImpl::Create();
    consumer_->HandleSpsFrame(consumer_->dispatcher_, frame);
}

HWTEST_F(WfdScreenCaptureTest, HandlePpsFrame_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->dispatcher_ != nullptr);

    Frame::Ptr frame = FrameImpl::Create();
    consumer_->HandlePpsFrame(consumer_->dispatcher_, frame);
}

HWTEST_F(WfdScreenCaptureTest, HandlePpsFrame_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->dispatcher_ != nullptr);

    Frame::Ptr frame = FrameImpl::Create();
    consumer_->HandlePpsFrame(consumer_->dispatcher_, frame);
}

HWTEST_F(WfdScreenCaptureTest, OnFrame_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);

    consumer_->OnFrame(nullptr, FRAME_TYPE::SPS_FRAME, false);
}

HWTEST_F(WfdScreenCaptureTest, OnFrame_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);

    Frame::Ptr frame = FrameImpl::Create();
    consumer_->OnFrame(frame, FRAME_TYPE::SPS_FRAME, false);
}

HWTEST_F(WfdScreenCaptureTest, OnFrame_003, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);

    Frame::Ptr frame = FrameImpl::Create();
    consumer_->OnFrame(frame, FRAME_TYPE::PPS_FRAME, false);
}

HWTEST_F(WfdScreenCaptureTest, OnFrame_004, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);

    Frame::Ptr frame = FrameImpl::Create();
    frame->SetCapacity(100);
    frame->SetSize(100);
    consumer_->OnFrame(frame, FRAME_TYPE::IDR_FRAME, false);
}

HWTEST_F(WfdScreenCaptureTest, HandleProsumerInitState_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    event.eventMsg = msg;
    consumer_->HandleProsumerInitState(event);
}

HWTEST_F(WfdScreenCaptureTest, HandleEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    msg->type = EVENT_SCREEN_CAPTURE_BASE;
    event.eventMsg = msg;
    int32_t ret = consumer_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdScreenCaptureTest, HandleEvent_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    msg->type = EVENT_SCREEN_CAPTURE_INIT;
    event.eventMsg = msg;
    int32_t ret = consumer_->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdScreenCaptureTest, UpdateOperation_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_INIT;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdScreenCaptureTest, UpdateOperation_002, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_START;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdScreenCaptureTest, UpdateOperation_003, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_PAUSE;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdScreenCaptureTest, UpdateOperation_004, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_RESUME;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdScreenCaptureTest, UpdateOperation_005, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_STOP;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdScreenCaptureTest, UpdateOperation_006, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_DESTROY;
    consumer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdScreenCaptureTest, Capture_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);

    consumer_->Init(0);
    consumer_->StartCapture();
    consumer_->StopCapture();
}

HWTEST_F(WfdScreenCaptureTest, OnInitVideoCaptureError_001, TestSize.Level1)
{
    ASSERT_TRUE(consumer_ != nullptr);
    ASSERT_TRUE(consumer_->mediaChannel_ != nullptr);

    EXPECT_CALL(*consumer_->mediaChannel_, OnConsumerNotify(_)).Times(1);
    consumer_->OnInitVideoCaptureError();
}

} // namespace Sharing
} // namespace OHOS
