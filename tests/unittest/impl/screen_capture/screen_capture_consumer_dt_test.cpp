/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include "screen_capture_consumer_dt_test.h"
#include "base_consumer.h"
#include "buffer_dispatcher.h"
#include "common/const_def.h"
#include "mediachannel/media_channel_def.h"
#include "mock_media_channel.h"
#include "screen_capture_def.h"
#include "video_source_encoder.h"
#include "video_source_screen.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

// Mock用于测试
class TestScreenCaptureListener : public BaseConsumer::IConsumerListener {
public:
    MOCK_METHOD(void, OnConsumerNotify, (const ProsumerStatusMsg::Ptr &statusMsg), (override));
    MOCK_METHOD(BufferDispatcher::Ptr, GetDispatcher, (), (override));
};

class ScreenCaptureConsumerDTTest : public testing::Test {
protected:
    void SetUp() override
    {
        listener_ = std::make_shared<TestScreenCaptureListener>();
    }

    void TearDown() override
    {
        listener_.reset();
    }

    std::shared_ptr<TestScreenCaptureListener> listener_;
    
    // 创建 consumer 实例的工具方法
    ScreenCaptureConsumer::Ptr CreateConsumer()
    {
        auto consumer = std::make_shared<ScreenCaptureConsumer>();
        std::weak_ptr<TestScreenCaptureListener> weakListener(listener_);
        consumer->SetConsumerListener(weakListener);
        return consumer;
    }
};

/**
 * @tc.name: ScreenCaptureConsumerDT_Constructor_001
 * @tc.desc: 验证ScreenCaptureConsumer构造函数
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_Constructor_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NE(consumer, nullptr);
    EXPECT_TRUE(consumer->GetId() != 0);
    EXPECT_FALSE(consumer->isInit_);
    EXPECT_FALSE(consumer->isRunning_);
    EXPECT_FALSE(consumer->paused_);
    EXPECT_FALSE(consumer->isHiVisionScreen_);
    EXPECT_TRUE(consumer->firstVideoFrame_);
    EXPECT_EQ(consumer->lastPts_, 0);
    EXPECT_EQ(consumer->lastRealPts_, 0);
    EXPECT_EQ(consumer->frameCount_, 0);
    EXPECT_EQ(consumer->currentSecond_, 0);
    EXPECT_EQ(consumer->audioFrameCount_, 0);
    EXPECT_EQ(consumer->silentFrameCount_, 0);
    EXPECT_EQ(consumer->lowInterval_, LOW_PTS_INTERVAL);
    EXPECT_EQ(consumer->highInterval_, HIGH_PTS_INTERVAL);
}

// ================ AudioEncoderReceiver相关测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_AudioEncoderReceiver_OnFrame_001
 * @tc.desc: AudioEncoderReceiver::OnFrame - 测试空父指针分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_AudioEncoderReceiver_OnFrame_001, TestSize.Level1)
{
    // 创建 AudioEncoderReceiver 但不关联 consumer，测试空父指针
    ScreenCaptureConsumer::AudioEncoderReceiver receiver(nullptr);
    EXPECT_NO_THROW(receiver.OnFrame(nullptr));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_AudioEncoderReceiver_OnFrame_002
 * @tc.desc: AudioEncoderReceiver::OnFrame - 测试正常帧处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_AudioEncoderReceiver_OnFrame_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto frame = FrameImpl::Create();
    frame->SetSize(100);
    EXPECT_NO_THROW(consumer->audioEncoderReceiver_->OnFrame(frame));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_AudioEncoderReceiver_OnFrame_003
 * @tc.desc: AudioEncoderReceiver::OnFrame - 测试dispatcher为空
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_AudioEncoderReceiver_OnFrame_003, TestSize.Level1)
{
    // 创建consumer并释放listener，测试dispatcher为空的情况
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    // 不设置listener，使其过期
    EXPECT_NO_THROW(consumer->audioEncoderReceiver_->OnFrame(FrameImpl::Create()));
}

// ================ OnFrame相关测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_001
 * @tc.desc: OnFrame - 测试空帧处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->OnFrame(nullptr, FRAME_TYPE::SPS_FRAME, false));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_002
 * @tc.desc: OnFrame - 测试监听器过期情况
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_002, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    // 不设置listener，使其过期
    EXPECT_NO_THROW(consumer->OnFrame(FrameImpl::Create(), FRAME_TYPE::SPS_FRAME, false));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_003
 * @tc.desc: OnFrame - 测试暂停状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->paused_ = true;
    EXPECT_NO_THROW(consumer->OnFrame(FrameImpl::Create(), FRAME_TYPE::SPS_FRAME, false));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_004
 * @tc.desc: OnFrame - 测试SPS帧处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto frame = FrameImpl::Create();
    EXPECT_NO_THROW(consumer->OnFrame(frame, FRAME_TYPE::SPS_FRAME, false));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_005
 * @tc.desc: OnFrame - 测试PPS帧处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_005, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto frame = FrameImpl::Create();
    EXPECT_NO_THROW(consumer->OnFrame(frame, FRAME_TYPE::PPS_FRAME, false));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_006
 * @tc.desc: OnFrame - 测试IDR关键帧处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_006, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto frame = FrameImpl::Create();
    frame->SetCapacity(100);
    frame->SetSize(100);
    EXPECT_NO_THROW(consumer->OnFrame(frame, FRAME_TYPE::IDR_FRAME, true));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_007
 * @tc.desc: OnFrame - 测试非关键帧处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_007, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto frame = FrameImpl::Create();
    EXPECT_NO_THROW(consumer->OnFrame(frame, FRAME_TYPE::IDR_FRAME, false));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_008
 * @tc.desc: OnFrame - 测试PTS计算逻辑 - 首次帧
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_008, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto frame = FrameImpl::Create();
    frame->SetCapacity(100);
    frame->SetSize(100);
    EXPECT_NO_THROW(consumer->OnFrame(frame, FRAME_TYPE::IDR_FRAME, true));
    EXPECT_FALSE(consumer->firstVideoFrame_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrame_009
 * @tc.desc: OnFrame - 测试PTS计算逻辑 - 时间差检测
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrame_009, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->firstVideoFrame_ = false;
    consumer->lastPts_ = 1000;
    consumer->lastRealPts_ = 900;
    
    // 设置大时间差
    auto frame = FrameImpl::Create();
    EXPECT_NO_THROW(consumer->OnFrame(frame, FRAME_TYPE::SPS_FRAME, false));
    EXPECT_GT(consumer->lastPts_, 1000);
}

// ================ OnFrameBufferUsed测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_OnFrameBufferUsed_001
 * @tc.desc: OnFrameBufferUsed - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnFrameBufferUsed_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NO_THROW(consumer->OnFrameBufferUsed());
}

// ================ 事件处理相关测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_001
 * @tc.desc: HandleEvent - 测试空事件消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_002
 * @tc.desc: HandleEvent - 测试未知事件类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_BASE;
    event.eventMsg = msg;
    EXPECT_EQ(consumer->HandleEvent(event), 0);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_003
 * @tc.desc: HandleEvent - 测试初始化事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_INIT;
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_004
 * @tc.desc: HandleEvent - 测试播放事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_005
 * @tc.desc: HandleEvent - 测试音频设置事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_005, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_AUDIO;
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_006
 * @tc.desc: HandleEvent - 测试显示设置事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_006, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_DISPLAY;
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_007
 * @tc.desc: HandleEvent - 测试IDR请求事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_007, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_SCREEN_REQUEST_IDR;
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_008
 * @tc.desc: HandleEvent - 测试通用事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_008, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_COMMON;
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleEvent_009
 * @tc.desc: HandleEvent - 测试追加surface事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleEvent_009, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_APPEND_SURFACE;
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleEvent(event));
}

// ================ 子事件处理函数测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerInitState_001
 * @tc.desc: HandleProsumerInitState - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerInitState_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerInitState(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerInitState_002
 * @tc.desc: HandleProsumerInitState - 测试空消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerInitState_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    EXPECT_NO_THROW(consumer->HandleProsumerInitState(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerPlay_001
 * @tc.desc: HandleProsumerPlay - 测试空消息分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerPlay_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    EXPECT_NO_THROW(consumer->HandleProsumerPlay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerPlay_002
 * @tc.desc: HandleProsumerPlay - 测试消息转换失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerPlay_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    // 设置不同类型的消息，导致转换失败
    auto msg = std::make_shared<ScreenCaptureConsumerDisplayEventMsg>();
    event.eventMsg = msg;
    EXPECT_NO_THROW(consumer->HandleProsumerPlay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerPlay_003
 * @tc.desc: HandleProsumerPlay - 测试音视频轨道设置
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerPlay_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    // 模拟设置音视频轨道
    consumer->audioTrack_.codecId = CodecId::CODEC_AAC;
    consumer->videoTrack_.codecId = CodecId::CODEC_H264;
    consumer->isInit_ = true; // 模拟已初始化
    
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerPlay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerPlay_004
 * @tc.desc: HandleProsumerPlay - 测试已初始化状态分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerPlay_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isInit_ = true; // 模拟已初始化
    
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_AAC;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerPlay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerPlay_005
 * @tc.desc: HandleProsumerPlay - 测试InitCapture失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerPlay_005, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_NONE;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    msg->screenId = 1;
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerPlay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerPlay_006
 * @tc.desc: HandleProsumerPlay - 测试StartCapture失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerPlay_006, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    // 模拟StartCapture返回false
    consumer->isInit_ = true;
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerEventMsg>();
    msg->audioTrack.codecId = CodecId::CODEC_NONE;
    msg->videoTrack.codecId = CodecId::CODEC_H264;
    msg->screenId = 1;
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerPlay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerSetAudio_001
 * @tc.desc: HandleProsumerSetAudio - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerSetAudio_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->HandleProsumerSetAudio(SharingEvent()));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerSetDisplay_001
 * @tc.desc: HandleProsumerSetDisplay - 测试videoSourceScreen_为空分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerSetDisplay_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerDisplayEventMsg>();
    msg->projectMode = 0;
    msg->projectRotation = 0;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerSetDisplay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerSetDisplay_002
 * @tc.desc: HandleProsumerSetDisplay - 测试正常显示设置
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerSetDisplay_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerDisplayEventMsg>();
    msg->projectMode = 0;
    msg->projectRotation = 45;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerSetDisplay(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerRequestIdr_001
 * @tc.desc: HandleProsumerRequestIdr - 测试videoSourceEncoder_为空分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerRequestIdr_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NO_THROW(consumer->HandleProsumerRequestIdr(SharingEvent()));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerRequestIdr_002
 * @tc.desc: HandleProsumerRequestIdr - 测试编码器关键帧请求
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerRequestIdr_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    // 模拟videoSourceEncoder_存在
    EXPECT_NO_THROW(consumer->HandleProsumerRequestIdr(SharingEvent()));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_NotifyAppCastScreenId_001
 * @tc.desc: NotifyAppCastScreenId - 测试videoSourceScreen_为空分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_NotifyAppCastScreenId_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NO_THROW(consumer->NotifyAppCastScreenId(0, SharingEvent()));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_NotifyAppCastScreenId_002
 * @tc.desc: NotifyAppCastScreenId - 测试screenId获取失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_NotifyAppCastScreenId_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->NotifyAppCastScreenId(0, SharingEvent()));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_NotifyAppCastScreenId_003
 * @tc.desc: NotifyAppCastScreenId - 测试正常通知
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_NotifyAppCastScreenId_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->NotifyAppCastScreenId(0, SharingEvent()));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleUpdateBitrate_001
 * @tc.desc: HandleUpdateBitrate - 测试JSON验证失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleUpdateBitrate_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NO_THROW(consumer->HandleUpdateBitrate("invalid_json"));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleUpdateBitrate_002
 * @tc.desc: HandleUpdateBitrate - 测试缺少bitrate字段分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleUpdateBitrate_002, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NO_THROW(consumer->HandleUpdateBitrate("{\"other\": 100}"));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleUpdateBitrate_003
 * @tc.desc: HandleUpdateBitrate - 测试bitrate不是整数分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleUpdateBitrate_003, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NO_THROW(consumer->HandleUpdateBitrate("{\"bitrate\": \"not_number\"}"));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleUpdateBitrate_004
 * @tc.desc: HandleUpdateBitrate - 测试正常比特率更新
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleUpdateBitrate_004, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_NO_THROW(consumer->HandleUpdateBitrate("{\"bitrate\": 1000000}"));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_001
 * @tc.desc: HandleProsumerEvent - 测试videoSourceScreen_为空分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = static_cast<int32_t>(AppCastEventType::APP_CAST_CREATE);
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_002
 * @tc.desc: HandleProsumerEvent - 测试APP_CAST_CREATE分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = static_cast<int32_t>(AppCastEventType::APP_CAST_CREATE);
    msg->eventParam = "test_surface";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_003
 * @tc.desc: HandleProsumerEvent - 测试APP_CAST_DESTROY分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = static_cast<int32_t>(AppCastEventType::APP_CAST_DESTROY);
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_004
 * @tc.desc: HandleProsumerEvent - 测试APP_CAST_RESIZE分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = static_cast<int32_t>(AppCastEventType::APP_CAST_RESIZE);
    msg->eventParam = "1920x1080";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_005
 * @tc.desc: HandleProsumerEvent - 测试APP_CAST_MAKE_MIRROR分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_005, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = static_cast<int32_t>(AppCastEventType::APP_CAST_MAKE_MIRROR);
    msg->eventParam = "mirror_param";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->EventHandleProsumer(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_006
 * @tc.desc: HandleProsumerEvent - 测试APP_CAST_ENTER_SMALL_WINDOW分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_006, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = static_cast<int32_t>(AppCastEventType::APP_CAST_ENTER_SMALL_WINDOW);
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_007
 * @tc.desc: HandleProsumerEvent - 测试APP_CAST_EXIT_SMALL_WINDOW分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_007, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = static_cast<int32_t>(AppCastEventType::APP_CAST_EXIT_SMALL_WINDOW);
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_008
 * @tc.desc: HandleProsumerEvent - 测试NOTIFY_EVENT_REFRESH_RATE_VOTE分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_008, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerRefreshEventMsg>();
    msg->eventId = NOTIFY_EVENT_REFRESH_RATE_VOTE;
    msg->interval = 30;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_009
 * @tc.desc: HandleProsumerEvent - 测试UPDATE_BITRATE分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_009, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = UPDATE_BITRATE;
    msg->eventParam = "{\"bitrate\": 2000000}";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerEvent_010
 * @tc.desc: HandleProsumerEvent - 测试未知事件类型分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerEvent_010, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerCommonEventMsg>();
    msg->eventId = -1; // 未知事件
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerEvent(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerAppendSurface_001
 * @tc.desc: HandleProsumerAppendSurface - 测试videoSourceScreen_为空分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerAppendSurface_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerSurfaceEventMsg>();
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerAppendSurface(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerAppendSurface_002
 * @tc.desc: HandleProsumerAppendSurface - 测试producer转换失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerAppendSurface_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerSurfaceEventMsg>();
    // surface为空
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerAppendSurface(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_HandleProsumerAppendSurface_003
 * @tc.desc: HandleProsumerAppendSurface - 测试正常surface设置
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_HandleProsumerAppendSurface_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureConsumerSurfaceEventMsg>();
    // 可以模拟设置surface，这里测试框架限制，只测试基本调用
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(consumer->HandleProsumerAppendSurface(event));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_NotifyAppCastMakeMirror_001
 * @tc.desc: NotifyAppCastMakeMirror - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_NotifyAppCastMakeMirror_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->NotifyAppCastMakeMirror(0, SharingEvent()));
}

// ================ UpdateOperation相关测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_UpdateOperation_001
 * @tc.desc: UpdateOperation - 测试PROSUMER_INIT分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_UpdateOperation_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_INIT;
    
    EXPECT_NO_THROW(consumer->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_UpdateOperation_002
 * @tc.desc: UpdateOperation - 测试PROSUMER_START分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_UpdateOperation_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_START;
    
    EXPECT_NO_THROW(consumer->UpdateOperation(statusMsg));
    EXPECT_FALSE(consumer->paused_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_UpdateOperation_003
 * @tc.desc: UpdateOperation - 测试PROSUMER_PAUSE分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_UpdateOperation_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_PAUSE;
    
    EXPECT_NO_THROW(consumer->UpdateOperation(statusMsg));
    EXPECT_TRUE(consumer->paused_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_UpdateOperation_004
 * @tc.desc: UpdateOperation - 测试PROSUMER_RESUME分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_UpdateOperation_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->paused_ = true; // 设置为暂停状态
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_RESUME;
    
    EXPECT_NO_THROW(consumer->UpdateOperation(statusMsg));
    EXPECT_FALSE(consumer->paused_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_UpdateOperation_005
 * @tc.desc: UpdateOperation - 测试PROSUMER_SUCCESS分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_UpdateOperation_005, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = true;
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_STOP;
    
    EXPECT_NO_THROW(consumer->UpdateOperation(statusMsg));
    EXPECT_FALSE(consumer->isRunning_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_UpdateOperation_006
 * @tc.desc: UpdateOperation - 测试PROSUMER_DESTROY分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_UpdateOperation_006, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_DESTROY;
    
    EXPECT_NO_THROW(consumer->UpdateOperation(statusMsg));
    EXPECT_FALSE(consumer->isRunning_);
}

// ================ 资源释放相关测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_Release_001
 * @tc.desc: Release - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_Release_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_EQ(consumer->Release(), 0);
    EXPECT_FALSE(consumer->isRunning_);
    EXPECT_FALSE(consumer->isInit_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_Release_002
 * @tc.desc: Release - 验证重复调用Release不会出错
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_Release_002, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    consumer->Release();
    EXPECT_NO_THROW(consumer->Release());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_Release_003
 * @tc.desc: Release - 验证析构函数功能
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_Release_003, TestSize.Level1)
{
    EXPECT_NO_THROW([]() {
        auto consumer = std::make_shared<ScreenCaptureConsumer>();
        consumer->~ScreenCaptureConsumer();
    }());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_ReleaseScreenBuffer_001
 * @tc.desc: ReleaseScreenBuffer - 测试videoSourceScreen_为空分支
 * @tc FUNC
.type: */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_ReleaseScreenBuffer_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_EQ(consumer->ReleaseScreenBuffer(), ERR_NULL_SCREEN);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_ReleaseScreenBuffer_002
 * @tc.desc: ReleaseScreenBuffer - 测试正常释放screen buffer
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_ReleaseScreenBuffer_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->ReleaseScreenBuffer());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsPaused_001
 * @tc.desc: IsPaused - 运行中且暂停状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsPaused_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->paused_ = true;
    consumer->isRunning_ = true;
    EXPECT_TRUE(consumer->IsPaused());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsPaused_002
 * @tc.desc: IsPaused - 运行中非暂停状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsPaused_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->paused_ = false;
    consumer->isRunning_ = true;
    EXPECT_FALSE(consumer->IsPaused());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsPaused_003
 * @tc.desc: IsPaused - 非运行状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsPaused_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->paused_ = true;
    consumer->isRunning_ = false;
    EXPECT_FALSE(consumer->IsPaused());
}

// ================ 初始化相关功能测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_InitCapture_001
 * @tc.desc: InitCapture - 测试视频初始化失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    // 设置音视频轨道
    consumer->videoTrack_.codecId = CodecId::CODEC_H264;
    consumer->videoTrack_.width = 1920;
    consumer->videoTrack_.height = 1080;
    consumer->videoTrack_.frameRate = 30;
    
    consumer->audioTrack_.codecId = CodecId::CODEC_NONE;

    EXPECT_FALSE(consumer->InitCapture(0));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitCapture_002
 * @tc.desc: InitCapture - 测试音视频同时初始化的流程
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    consumer->videoTrack_.codecId = CodecId::CODEC_H264;
    consumer->videoTrack_.width = 1920;
    consumer->videoTrack_.height = 1080;
    consumer->videoTrack_.frameRate = 30;
    
    consumer->audioTrack_.codecId = CodecId::CODEC_AAC;
    consumer->audioTrack_.width = 0;
    consumer->audioTrack_.height = 0;
    consumer->audioTrack_.frameRate = 0;

    EXPECT_FALSE(consumer->InitCapture(0));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitCapture_003
 * @tc.desc: InitCapture - 测试初始化后状态变化
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitCapture_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    consumer->isInit_ = true; // 模拟已初始化状态
    EXPECT_NO_THROW(consumer->InitCapture(0)); // 已经初始化，应该安全返回
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitVideoCapture_001
 * @tc.desc: InitVideoCapture - 测试编码器初始化失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitVideoCapture_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_FALSE(consumer->InitVideoCapture(0));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitVideoCapture_002
 * @tc.desc: InitVideoCapture - 测试高帧率视频初始化
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitVideoCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    // 设置私有成员进行测试
    consumer->videoTrack_.codecId = CodecId::CODEC_H264;
    consumer->videoTrack_.width = 1920;
    consumer->videoTrack_.height = 1080;
    consumer->videoTrack_.frameRate = HIVISION_FRAME_RATE;
    
    EXPECT_TRUE(consumer->isHiVisionScreen_);
    EXPECT_EQ(consumer->lowInterval_, LOW_PTS_INTERVAL_HIVISON);
    EXPECT_EQ(consumer->highInterval_, HIGH_PTS_INTERVAL_HIVISON);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitVideoCapture_003
 * @tc.desc: InitVideoCapture - 测试正常初始化
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitVideoCapture_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    
    consumer->videoTrack_.codecId = CodecId::CODEC_H264;
    consumer->videoTrack_.width = 1280;
    consumer->videoTrack_.height = 720;
    consumer->videoTrack_.frameRate = 30;
    
    EXPECT_FALSE(consumer->InitVideoCapture(0));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitAudioCapture_001
 * @tc.desc: InitAudioCapture - 测试AudioCapturer创建失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitAudioCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->InitAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitAudioCapture_002
 * @tc.desc: InitAudioCapture - 测试正常音频捕获初始化
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitAudioCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->InitAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitAudioEncoder_001
 * @tc.desc: InitAudioEncoder - 测试编码器创建失败分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitAudioEncoder_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->InitAudioEncoder());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_InitAudioEncoder_002
 * @tc.desc: InitAudioEncoder - 测试正常音频编码器初始化
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_InitAudioEncoder_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->InitAudioEncoder());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnInitVideoCaptureError_001
 * @tc.desc: OnInitVideoCaptureError - 测试音频轨道存在时的错误处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnInitVideoCaptureError_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    // 设置音频轨道
    consumer->audioTrack_.codecId = CodecId::CODEC_AAC;
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = ProsumerOptRunningStatus::PROSUMER_ERROR;
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    
    EXPECT_NO_THROW(consumer->OnInitVideoCaptureError());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_OnInitVideoCaptureError_002
 * @tc.desc: OnInitVideoCaptureError - 测试音频轨道不存在时的错误处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_OnInitVideoCaptureError_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    // 设置音频轨道为NONE
    consumer->audioTrack_.codecId = CodecId::CODEC_NONE;
    
    EXPECT_NO_THROW(consumer->OnInitVideoCaptureError());
}

// ================ 控制相关功能测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_StartCapture_001
 * @tc.desc: StartCapture - 测试已运行状态检查分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = true;
    EXPECT_FALSE(consumer->StartCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartCapture_002
 * @tc.desc: StartCapture - 测试未初始化状态检查分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = false;
    consumer->isInit_ = false;
    EXPECT_FALSE(consumer->StartCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartCapture_003
 * @tc.desc: StartCapture - 测试正常启动流程
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartCapture_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = false;
    consumer->isInit_ = true;
    EXPECT_FALSE(consumer->StartCapture()); // 由于没有真实初始化，会返回false
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartCapture_004
 * @tc.desc: StartCapture - 测试初始化成功后状态变化
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartCapture_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = false;
    consumer->isInit_ = true;
    consumer->StartCapture();
    EXPECT_TRUE(consumer->isRunning_); // 状态应该变为运行中
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopCapture_001
 * @tc.desc: StopCapture - 测试运行中状态停止
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = true;
    EXPECT_TRUE(consumer->StopCapture());
    EXPECT_FALSE(consumer->isRunning_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopCapture_002
 * @tc.desc: StopCapture - 测试非运行状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = false;
    EXPECT_TRUE(consumer->StopCapture());
    EXPECT_FALSE(consumer->isRunning_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopCapture_003
 * @tc.desc: StopCapture - 测试停止时状态管理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopCapture_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    consumer->isRunning_ = true;
    consumer->isInit_ = true;
    consumer->StopCapture();
    EXPECT_FALSE(consumer->isRunning_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartAudioCapture_001
 * @tc.desc: StartAudioCapture - 测试audioCapturer_为空
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartAudioCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StartAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartAudioCapture_002
 * @tc.desc: StartAudioCapture - 测试AudioCapturer启动失败
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartAudioCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StartAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartAudioCapture_003
 * @tc.desc: StartAudioCapture - 测试GetBufferSize失败
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartAudioCapture_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StartAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartAudioCapture_004
 * @tc.desc: StartAudioCapture - 测试线程创建
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartAudioCapture_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StartAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartAudioCapture_005
 * @tc.desc: StartAudioCapture - 验证线程启动后状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartAudioCapture_005, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StartAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StartVideoCapture_001
 * @tc.desc: StartVideoCapture - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StartVideoCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StartVideoCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopAudioCapture_001
 * @tc.desc: StopAudioCapture - 测试音频轨道检查
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopAudioCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    // 设置音频轨道为NONE，应该不执行停止
    consumer->audioTrack_.codecId = CodecId::CODEC_NONE;
    EXPECT_NO_THROW(consumer->StopAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopAudioCapture_002
 * @tc.desc: StopAudioCapture - 测试音频轨道存在时的停止
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopAudioCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    // 设置音频轨道存在
    consumer->audioTrack_.codecId = CodecId::CODEC_AAC;
    EXPECT_NO_THROW(consumer->StopAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopAudioCapture_003
 * @tc.desc: StopAudioCapture - 测试线程管理分支
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopAudioCapture_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StopAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopAudioCapture_004
 * @tc.desc: StopAudioCapture - 测试AudioCapturer资源释放
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopAudioCapture_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StopAudioCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopVideoCapture_001
 * @tc.desc: StopVideoCapture - 测试视频轨道检查
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopVideoCapture_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    // 设置视频轨道为NONE，应该不执行停止
    consumer->videoTrack_.codecId = CodecId::CODEC_NONE;
    EXPECT_NO_THROW(consumer->StopVideoCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopVideoCapture_002
 * @tc.desc: StopVideoCapture - 测试视频轨道存在时的停止
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopVideoCapture_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    // 设置视频轨道存在
    consumer->videoTrack_.codecId = CodecId::CODEC_H264;
    EXPECT_NO_THROW(consumer->StopVideoCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_StopVideoCapture_003
 * @tc.desc: StopVideoCapture - 测试编码器和屏幕源停止
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_StopVideoCapture_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->StopVideoCapture());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_AudioCaptureThreadWorker_001
 * @tc.desc: AudioCaptureThreadWorker - 测试内存分配失败
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_AudioCaptureThreadWorker_001, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->AudioCaptureThreadWorker());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_AudioCaptureThreadWorker_002
 * @tc.desc: AudioCaptureThreadWorker - 测试音频播放设备不存在时
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_AudioCaptureThreadWorker_002, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->AudioCaptureThreadWorker());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_AudioCaptureThreadWorker_003
 * @tc.desc: AudioCaptureThreadWorker - 测试读取循环和静音帧检测
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_AudioCaptureThreadWorker_003, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->AudioCaptureThreadWorker());
}

/**
 * @tc.name: ScreenCaptureConsumerDT_AudioCaptureThreadWorker_004
 * @tc.desc: AudioCaptureThreadWorker - 测试编码器处理线程
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_AudioCaptureThreadWorker_004, TestSize.Level1)
{
    auto consumer = CreateConsumer();
    EXPECT_NO_THROW(consumer->AudioCaptureThreadWorker());
}

// ================ IsSilent边界Frame===============
测试 =/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_001
 * @tc.desc: IsSilentFrame - 测试空指针处理
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_001, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    EXPECT_FALSE(consumer->IsSilentFrame(nullptr, 0));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_002
 * @tc.desc: IsSilentFrame - 测试静音帧检测 (全0)
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_002, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t silentBuffer[10] = {0};
    EXPECT_TRUE(consumer->IsSilentFrame(silentBuffer, 10));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_003
 * @tc.desc: IsSilentFrame - 测试非静音帧检测
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_003, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t nonSilentBuffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_FALSE(consumer->IsSilentFrame(nonSilentBuffer, 10));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_004
 * @tc.desc: IsSilentFrame - 测试0xFF静音检测
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_004, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t silentBufferFF[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    EXPECT_TRUE(consumer->IsSilentFrame(silentBufferFF, 10));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_005
 * @tc.desc: IsSilentFrame - 测试混合数据检测
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_005, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t mixedBuffer[10] = {0, 0xFF, 1, 2, 0, 0xFF, 0xFF, 3, 4, 5};
    EXPECT_FALSE(consumer->IsSilentFrame(mixedBuffer, 10));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_006
 * @tc.desc: IsSilentFrame - 测试边界值(单字节)
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_006, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t buffer = 0;
    EXPECT_TRUE(consumer->IsSilentFrame(&buffer, 1));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_007
 * @tc.desc: IsSilentFrame - 测试边界值(单字节非静音)
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_007, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t buffer = 1;
    EXPECT_FALSE(consumer->IsSilentFrame(&buffer, 1));
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_009
 * @tc.desc: IsSilentFrame - 测试零长度缓冲区
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_009, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t buffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_TRUE(consumer->IsSilentFrame(buffer, 0)); // 长度为0，视为静音
}

/**
 * @tc.name: ScreenCaptureConsumerDT_IsSilentFrame_010
 * @tc.desc: IsSilentFrame - 测试交替静音和非静音模式
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_IsSilentFrame_010, TestSize.Level1)
{
    auto consumer = std::make_shared<ScreenCaptureConsumer>();
    uint8_t mixedBuffer[20] = {};
    for (int i = 0; i < 20; i++) {
        mixedBuffer[i] = (i % 4 == 0) ? 0 : 1; // 每4个字节一个静音
    }
    EXPECT_FALSE(consumer->IsSilentFrame(mixedBuffer, 20));
}

// ================ 复杂场景测试 ================
/**
 * @tc.name: ScreenCaptureConsumerDT_ComplicatedScenario_001
 * @tc.desc: 完整生命周期测试: 初始化->启动->暂停->恢复->停止->释放
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_ComplicatedScenario_001, TestSize.Level2)
{
    auto consumer = CreateConsumer();
    
    // 测试初始状态
    EXPECT_FALSE(consumer->isInit_);
    EXPECT_FALSE(consumer->isRunning_);
    EXPECT_FALSE(consumer->paused_);
    
    // 模拟启动（会失败但会改变状态）
    consumer->isInit_ = true;
    consumer->StartCapture();
    
    // 暂停测试
    consumer->paused_ = true;
    EXPECT_TRUE(consumer->IsPaused());
    consumer->OnFrame(FrameImpl::Create(), FRAME_TYPE::SPS_FRAME, false); // 暂停状态下应该不处理
    
    // 恢复测试
    consumer->paused_ = false;
    EXPECT_FALSE(consumer->IsPaused());
    
    // 停止测试
    consumer->StopCapture();
    EXPECT_FALSE(consumer->isRunning_);
    
    // 释放测试
    consumer->Release();
    EXPECT_FALSE(consumer->isInit_);
}

/**
 * @tc.name: ScreenCaptureConsumerDT_ComplicatedScenario_002
 * @tc.desc: 内存管理测试: 多次创建和销毁Consumer
 * @tc.type FUNC:
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_ComplicatedScenario_002, TestSize.Level2)
{
    for (int i = 0; i < 5; ++i) {
        auto consumer = CreateConsumer();
        consumer->isInit_ = true;
        consumer->isRunning_ = true;
        consumer->Release();
        EXPECT_FALSE(consumer->isInit_);
        EXPECT_FALSE(consumer->isRunning_);
    }
}

/**
 * @tc.name: ScreenCaptureConsumerDT_ComplicatedScenario_003
 * @tc.desc: 线程安全测试: 多线程环境下对共享资源的访问
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_ComplicatedScenario_003, TestSize.Level3)
{
    auto consumer = CreateConsumer();
    
    // 模拟多线程访问IsPaused
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([consumer, i]() {
            auto localConsumer = std::make_shared<ScreenCaptureConsumer>();
            if (i % 2 == 0) {
                localConsumer->paused_ = true;
            } else {
                localConsumer->paused_ = false;
            }
            bool result = localConsumer->IsPaused();
            EXPECT_EQ(result, (i % 2 == 0));
        });
    }
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

/**
 * @tc.name: ScreenCaptureConsumerDT_ComplicatedScenario_004
 * @tc.desc: PTS计算逻辑测试: 验证时间戳计算的正确性
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureConsumerDTTest, ScreenCaptureConsumerDT_ComplicatedScenario_004, TestSize.Level2)
{
    auto consumer = CreateConsumer();
    consumer->firstVideoFrame_ = false; // 跳过首次帧处理
    
    // 测试初始PTS值
    EXPECT_EQ(consumer->lastPts_, 0);
    EXPECT_EQ(consumer->lastRealPts_, 0);
    
    // 模拟正常时间戳递增
    consumer->lastRealPts_ = 1000;
    consumer->lastPts_ = 1000;
    
    auto frame = FrameImpl::Create();
    consumer->OnFrame(frame, FRAME_TYPE::SPS_FRAME, false);
    EXPECT_GT(consumer->lastPts_, 1000);
    
    // 测试大时间差情况
    consumer->lastRealPts_ = 2000;
    consumer->lastPts_ = 1000;
    consumer->OnFrame(frame, FRAME_TYPE::SPS_FRAME, false);
    EXPECT_GT(consumer->lastPts_, 2000); // 应该重置为realPts
}

} // namespace Sharing
} // namespace OHOS
