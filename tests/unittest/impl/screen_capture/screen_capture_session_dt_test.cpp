/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include "screen_capture_session_dt_test.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {


ScreenCaptureSessionDTTest::ScreenCaptureSessionDTTest() : session_(nullptr) {}

void ScreenCaptureSessionDTTest::SetUp()
{
    session_ = std::make_shared<ScreenCaptureSession>();
}

void ScreenCaptureSessionDTTest::TearDown()
{
    session_.reset();
}

// ================ 构造和销毁测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_Constructor_001
 * @tc.desc: 验证ScreenCaptureSession构造函数
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_Constructor_001, TestSize.Level1)
{
    EXPECT_NE(session_, nullptr);
    EXPECT_TRUE(session_->GetId() != 0);
    EXPECT_EQ(session_->screenId_, 0);
    EXPECT_EQ(session_->captureType_, MEDIA_TYPE_AV);
    EXPECT_EQ(session_->audioFormat_, AUDIO_48000_16_2);
    EXPECT_EQ(session_->videoFormat_, VIDEO_1920X1080_30);
}

/**
 * @tc.name: ScreenCaptureSessionDT_Destructor_001
 * @tc.desc: 验证ScreenCaptureSession析构函数
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_Destructor_001, TestSize.Level1)
{
    EXPECT_NO_THROW([]() {
        auto testSession = std::make_shared<ScreenCaptureSession>();
        testSession->~ScreenCaptureSession();
    }());
}

// ================ HandleEvent主函数测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_001
 * @tc.desc: HandleEvent - 测试空事件消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_001, TestSize.Level1)
{
    SharingEvent event;
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_002
 * @tc.desc: HandleEvent - 测试handleRtspPlayed事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDT_HandleEvent_002, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    msg->mediaType = MEDIA_TYPE_AV;
    msg->videoFormat = VIDEO_1920X1080_30;
    msg->audioFormat = AUDIO_48000_16_2;
    msg->codecId = CodecId::CODEC_H264;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_003
 * @tc.desc: HandleEvent - 测试初始化事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_003, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_INIT;
    msg->mediaType = MEDIA_TYPE_VIDEO;
    msg->videoFormat = VIDEO_1280X720_30;
    msg->screenId = 12345;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
    EXPECT_EQ(session_->captureType_, MEDIA_TYPE_VIDEO);
    EXPECT_EQ(session_->videoFormat_, VIDEO_1280X720_30);
    EXPECT_EQ(session_->screenId_, 12345);
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_004
 * @tc.desc: HandleEvent - 测试consumer初始化状态事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_004, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_STATE_PROSUMER_INIT;
    msg->errorCode = ERR_OK;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_005
 * @tc.desc: HandleEvent - 测试consumer初始化失败事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_005, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_STATE_PROSUMER_INIT;
    msg->errorCode = ERR_SESSION_START;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_006
 * @tc.desc: HandleEvent - 测试音频设置事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_006, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SetAudioSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_AUDIO;
    msg->isAudioOn = false;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_007
 * @tc.desc: HandleEvent - 测试IDR请求事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_007, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_REQUEST_IDR;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_008
 * @tc.desc: HandleEvent - 测试显示设置事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_008, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SetDisplaySessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_DISPLAY;
    msg->projectMode = 1;
    msg->projectRotation = 90;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_009
 * @tc.desc: HandleEvent - 测试通用事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_009, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<CommonSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_COMMON;
    msg->eventId = 1001;
    msg->eventParam = "test_param";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_010
 * @tc.desc: HandleEvent - 测试追加surface事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_010, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<AppendSurfaceSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_APPEND_SURFACE;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_011
 * @tc.desc: HandleEvent - 测试prosumer通用事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_011, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<CommonSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_PROSUMER_COMMON;
    msg->eventId = 2001;
    msg->eventParam = "prosumer_param";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleEvent_012
 * @tc.desc: HandleEvent - 测试未知事件类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleEvent_012, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SessionEventMsg>();
    msg->type = static_cast<EventType>(999); // 未知事件类型
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ HandleRtspPlay事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleRtspPlay_001
 * @tc.desc: HandleRtspPlay - 测试AV媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleRtspPlay_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    msg->mediaType = MEDIA_TYPE_AV;
    msg->videoFormat = VIDEO_1920X1080_30;
    msg->audioFormat = AUDIO_48000_16_2;
    msg->codecId = CodecId::CODEC_AAC;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleRtspPlay_002
 * @tc.desc: HandleRtspPlay - 测试VIDEO媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleRtspPlay_002, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    msg->mediaType = MEDIA_TYPE_VIDEO;
    msg->videoFormat = VIDEO_1280X720_30;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleRtspPlay_003
 * @tc.desc: HandleRtspPlay - 测试AUDIO媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleRtspPlay_003, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    msg->mediaType = MEDIA_TYPE_AUDIO;
    msg->audioFormat = AUDIO_44100_16_2;
    msg->codecId = CodecId::CODEC_G711U;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleRtspPlay_004
 * @tc.desc: HandleRtspPlay - 测试未知媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleRtspPlay_004, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    msg->mediaType = static_cast<MediaType>(999); // 未知媒体类型
    msg->videoFormat = VIDEO_1920X1080_30;
    msg->audioFormat = AUDIO_48000_16_2;
    msg->codecId = CodecId::CODEC_H264;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ HandleSessionInit事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionInit_001
 * @tc.desc: HandleSessionInit - 测试有效消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionInit_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_INIT;
    msg->mediaType = MEDIA_TYPE_VIDEO;
    msg->videoFormat = VIDEO_1920X1080_60;
    msg->audioFormat = AUDIO_44100_16_2;
    msg->screenId = 98765;
    event.eventMsg = msg;
    
    session_->HandleSessionInit(event);
    
    EXPECT_EQ(session_->captureType_, MEDIA_TYPE_VIDEO);
    EXPECT_EQ(session_->videoFormat_, VIDEO_1920X1080_60);
    EXPECT_EQ(session_->audioFormat_, AUDIO_44100_16_2);
    EXPECT_EQ(session_->screenId_, 98765);
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionInit_002
 * @tc:.desc HandleSessionInit - 测试空消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionInit_002, TestSize.Level1)
{
    SharingEvent event;
    event.eventMsg = nullptr;
    
    EXPECT_NO_THROW(session_->HandleSessionInit(event));
}

// ================ HandleProsumerInitState事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleProsumerInitState_001
 * @tc.desc: HandleProsumerInitState - 测试初始化成功
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleProsumerInitState_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_STATE_PROSUMER_INIT;
    msg->errorCode = ERR_OK;
    msg->requestId = 3001;
    msg->prosumerId = 4001;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleProsumerInitState_002
 * @tc.desc: HandleProsumerInitState - 测试初始化失败
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleProsumerInitState_002, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_STATE_PROSUMER_INIT;
    msg->errorCode = ERR_SESSION_START;
    msg->requestId = 3002;
    msg->prosumerId = 4002;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleProsumerInitState_003
 * @tc.desc: HandleProsumerInitState - 测试空消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleProsumerInitState_003, TestSize.Level1)
{
    SharingEvent event;
    event.eventMsg = nullptr;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleProsumerInitState_004
 * @tc.desc: HandleProsumerInitState - 测试未知消息类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleProsumerInitState_004, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SetAudioSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_AUDIO;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ HandleSessionSetAudio事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionSetAudio_001
 * @tc.desc: HandleSessionSetAudio - 测试音频开启
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionSetAudio_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SetAudioSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_AUDIO;
    msg->isAudioOn = true;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionSetAudio_002
 * @tc.desc: HandleSessionSetAudio - 测试音频关闭
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionSetAudio_002, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SetAudioSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_AUDIO;
    msg->isAudioOn = false;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ HandleSessionRequestIdr事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionRequestIdr_001
 * @tc.desc: HandleSessionRequestIdr - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionRequestIdr_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_REQUEST_IDR;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ HandleSessionSetDisplay事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionSetDisplay_001
 * @tc.desc: HandleSessionSetDisplay - 测试显示设置
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionSetDisplay_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<SetDisplaySessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_DISPLAY;
    msg->projectMode = 2;
    msg->projectRotation = 180;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ HandleSessionCommon事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionCommon_001
 * @tc.desc: HandleSessionCommon - 测试通用事件
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionCommon_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<CommonSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_COMMON;
    msg->eventId = 5001;
    msg->eventParam = "common_test_param";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ HandleSessionAppendSurface事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_HandleSessionAppendSurface_001
 * @tc.desc: HandleSessionAppendSurface - 测试surface追加
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_HandleSessionAppendSurface_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<AppendSurfaceSessionEventMsg>();
    msg->type = EventType::EVENT_SCREEN_CAPTURE_APPEND_SURFACE;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ NotifyEvent事件处理测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_NotifyEvent_001
 * @tc.desc: NotifyEvent - 测试事件通知
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyEvent_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<CommonSessionEventMsg>();
    msg->type = EventType::EVENT_AGENT_PROSUMER_COMMON;
    msg->eventId = 6001;
    msg->eventParam = "notify_test_param";
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleEvent(event));
}

// ================ UpdateOperation状态更新测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_001
 * @tc.desc: UpdateOperation - 测试SESSION_START状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_001, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionRunningStatus::SESSION_START;
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_002
 * @tc.desc: UpdateOperation - 测试SESSION_STOP状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_002, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionRunningStatus::SESSION_STOP;
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_003
 * @tc.desc: UpdateOperation - 测试SESSION_PAUSE状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_003, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionRunningStatus::SESSION_PAUSE;
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_004
 * @tc.desc: UpdateOperation - 测试SESSION_RESUME状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_004, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionRunningStatus::SESSION_RESUME;
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_005
 * @tc.desc: UpdateOperation - 测试SESSION_DESTROY状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_005, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = SessionRunningStatus::SESSION_DESTROY;
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_006
 * @tc.desc: UpdateOperation - 测试未知状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_006, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    // 设置一个无效的状态值
    statusMsg->status = static_cast<SessionRunningStatus>(999);
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_007
 * @tc.desc: UpdateOperation - 测试空消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_007, TestSize.Level1)
{
    SessionStatusMsg::Ptr statusMsg = nullptr;
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateOperation_008
 * @tc.desc: UpdateOperation - 测试空消息内容
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateOperation_008, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = nullptr;
    
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

// ================ NotifyProsumerInit测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_NotifyProsumerInit_001
 * @tc.desc: NotifyProsumerInit - 测试AV媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyProsumerInit_001, TestSize.Level1)
{
    // 设置会话状态
    session_->captureType_ = MEDIA_TYPE_AV;
    session_->screenId_ = 11111;
    session_->videoFormat_ = VIDEO_1920X1080_30;
    session_->audioFormat_ = AUDIO_48000_16_2;
    
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    
    EXPECT_NO_THROW(session_->NotifyProsumerInit(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_NotifyProsumerInit_002
 * @tc.desc: NotifyProInitsumer - 测试VIDEO媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyProsumerInit_002, TestSize.Level1)
{
    // 设置会话状态
    session_->captureType_ = MEDIA_TYPE_VIDEO;
    session_->screenId_ = 22222;
    session_->videoFormat_ = VIDEO_1280X720_60;
    
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    
    EXPECT_NO_THROW(session_->NotifyProsumerInit(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_NotifyProsumerInit_003
 * @tc.desc: NotifyProsumerInit - 测试AUDIO媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyProsumerInit_003, TestSize.Level1)
{
    // 设置会话状态
    session_->captureType_ = MEDIA_TYPE_AUDIO;
    session_->screenId_ = 33333;
    session_->audioFormat_ = AUDIO_44100_16_2;
    
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    
    EXPECT_NO_THROW(session_->NotifyProsumerInit(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_NotifyProsumerInit_004
 * @tc.desc: NotifyProsumerInit - 测试未知媒体类型
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyProsumerInit_004, TestSize.Level1)
{
    // 设置会话状态
    session_->captureType_ = static_cast<MediaType>(999); // 未知媒体类型
    session_->screenId_ = 44444;
    
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    
    EXPECT_NO_THROW(session_->NotifyProsumerInit(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_NotifyProsumerInit_005
 * @tc.desc: NotifyProsumerInit - 测试空消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyProsumerInit_005, TestSize.Level1)
{
    SessionStatusMsg::Ptr statusMsg = nullptr;
    
    EXPECT_NO_THROW(session_->NotifyProsumerInit(statusMsg));
}

// ================ UpdateMediaStatus媒体状态更新测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_001
 * @tc.desc: UpdateMediaStatus - 测试创建成功状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_001, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_PROSUMER_CREATE_SUCCESS;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_002
 * @tc.desc: UpdateMediaStatus - 测试启动成功状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_002, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_PROSUMER_START_SUCCESS;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_003
 * @tc.desc: UpdateMediaStatus - 测试停止成功状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_003, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_PROSUMER_STOP_SUCCESS;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_004
 * @tc.desc: UpdateMediaStatus - 测试销毁成功状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_004, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_PROSUMER_DESTROY_SUCCESS;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_005
 * @tc.desc: UpdateMediaStatus - 测试启动中成功状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_005, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_PROSUMER_STARTED_SUCCESS;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_006
 * @tc.desc: UpdateMediaStatus - 测试未知状态
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_006, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = static_cast<MediaNotifyStatus>(999); // 未知状态
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_007
 * @tc.desc: UpdateMediaStatus - 测试空消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_007, TestSize.Level1)
{
    SessionStatusMsg::Ptr statusMsg = nullptr;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_UpdateMediaStatus_008
 * @tc.desc: UpdateMediaStatus - 测试空消息内容
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_UpdateMediaStatus_008, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = nullptr;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
}

// ================ NotifyAgentSessionStatus通知测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_NotifyAgentSessionStatus_001
 * @tc.desc: NotifyAgentSessionStatus - 测试创建通知
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyAgentSessionStatus_001, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = NOTIFY_PROSUMER_CREATE;
    
    EXPECT_NO_THROW(session_->NotifyAgentSessionStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_NotifyAgentSessionStatus_002
 * @tc.desc: NotifyAgentSessionStatus - 测试其他状态通知
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyAgentSessionStatus_002, TestSize.Level1)
{
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_SESSION_STARTED;
    
    EXPECT_NO_THROW(session_->NotifyAgentSessionStatus(statusMsg));
}

/**
 * @tc.name: ScreenCaptureSessionDT_NotifyAgentSessionStatus_003
 * @tc.desc: NotifyAgentSessionStatus - 测试空消息
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyAgentSessionStatus_003, TestSize.Level1)
{
    SessionStatusMsg::Ptr statusMsg = nullptr;
    
    EXPECT_NO_THROW(session_->NotifyAgentSessionStatus(statusMsg));
}

// ================ NotifyRtspPlay测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_NotifyRtspPlay_001
 * @tc.desc: NotifyRtspPlay - 基本功能测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_NotifyRtspPlay_001, TestSize.Level1)
{
    EXPECT_NO_THROW(session_->NotifyRtspPlay());
}

// ================ 边界条件测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_Boundary_001
 * @tc.desc: 边界条件 - 测试极大值参数
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_Boundary_001, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_INIT;
    msg->mediaType = static_cast<MediaType>(UINT32_MAX);
    msg->videoFormat = static_cast<VideoFormat>(UINT32_MAX);
    msg->audioFormat = static_cast<AudioFormat>(UINT32_MAX);
    msg->screenId = UINT64_MAX;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleSessionInit(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_Boundary_002
 * @tc.desc: 边界条件 - 测试0参数
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_Boundary_002, TestSize.Level1)
{
    SharingEvent event;
    auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
    msg->type = EventType::EVENT_SESSION_INIT;
    msg->mediaType = MEDIA_TYPE_AV;
    msg->videoFormat = VIDEO_1920X1080_30;
    msg->audioFormat = AUDIO_48000_16_2;
    msg->screenId = 0;
    event.eventMsg = msg;
    
    EXPECT_NO_THROW(session_->HandleSessionInit(event));
}

/**
 * @tc.name: ScreenCaptureSessionDT_Boundary_003
 * @tc.desc: 边界条件 - 测试连续多次调用
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_Boundary_003, TestSize.Level1)
{
    for (int i = 0; i < 100; ++i) {
        SharingEvent event;
        auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
        msg->type = EventType::EVENT_SCREEN_CAPTURE_SET_AUDIO;
        msg->isAudioOn = (i % 2 == 0);
        event.eventMsg = msg;
        
        EXPECT_NO_THROW(session_->HandleEvent(event));
    }
}

// ================ 并发测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_Concurrency_001
 * @tc.desc: 并发测试 - 多线程访问
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_Concurrency_001, TestSize.Level2)
{
    std::vector<std::thread> threads;
    const int threadCount = 10;
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i]() {
            auto localSession = std::make_shared<ScreenCaptureSession>();
            SharingEvent event;
            auto msg = std::make_shared<ScreenCaptureSessionEventMsg>();
            msg->type = EventType::EVENT_SESSION_INIT;
            msg->mediaType = static_cast<MediaType>((i % 3) + 1);
            event.eventMsg = msg;
            
            EXPECT_NO_THROW(localSession->HandleEvent(event));
        });
    }
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

// ================ 组合场景测试 ================
/**
 * @tc.name: ScreenCaptureSessionDT_ComplicatedScenario_001
 * @tc.desc: 组合场景 - 完整生命周期测试
 * @tc.type: FUNC
 */
HWTEST_F(ScreenCaptureSessionDTTest, ScreenCaptureSessionDT_ComplicatedScenario_001, TestSize.Level2)
{
    // 1. 初始化会话
    SharingEvent initEvent;
    auto initMsg = std::make_shared<ScreenCaptureSessionEventMsg>();
    initMsg->type = EventType::EVENT_SESSION_INIT;
    initMsg->mediaType = MEDIA_TYPE_AV;
    initMsg->videoFormat = VIDEO_1920X1080_30;
    initMsg->audioFormat = AUDIO_48000_16_2;
    initMsg->screenId = 99999;
    initEvent.eventMsg = initMsg;
    
    session_->HandleSessionInit(initEvent);
    EXPECT_EQ(session_->captureType_, MEDIA_TYPE_AV);
    EXPECT_EQ(session_->screenId_, 99999);
    
    // 2. 模拟播放通知
    SharingEvent playEvent;
    auto playMsg = std::make_shared<ScreenCaptureSessionEventMsg>();
    playMsg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    playMsg->mediaType = MEDIA_TYPE_AV;
    playMsg->videoFormat = VIDEO_1920X1080_30;
    playMsg->audioFormat = AUDIO_48000_16_2;
    playMsg->codecId = CodecId::CODEC_H264;
    playEvent.eventMsg = playMsg;
    
    EXPECT_NO_THROW(session_->HandleEvent(playEvent));
    
    // 3. 测试状态更新
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_PROSUMER_CREATE_SUCCESS;
    
    EXPECT_NO_THROW(session_->UpdateMediaStatus(statusMsg));
    
    // 4. 测试操作更新
    statusMsg->status = SessionRunningStatus::SESSION_START;
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
    
    // 5. 测试设置音频
    SharingEvent audioEvent;
    auto audioMsg = std::make_shared<SetAudioSessionEventMsg>();
    audioMsg->type = EventType::EVENT_SCREEN_CAPTURE_SET_AUDIO;
    audioMsg->isAudioOn = false;
    audioEvent.eventMsg = audioMsg;
    
    EXPECT_NO_THROW(session_->HandleEvent(audioEvent));
    
    // 6. 测试设置显示
    SharingEvent displayEvent;
    auto displayMsg = std::make_shared<SetDisplaySessionEventMsg>();
    displayMsg->type = EventType::EVENT_SCREEN_CAPTURE_SET_DISPLAY;
    displayMsg->projectMode = 3;
    displayMsg->projectRotation = 270;
    displayEvent.eventMsg = displayMsg;
    
    EXPECT_NO_THROW(session_->HandleEvent(displayEvent));
    
    // 7. 测试通用事件
    SharingEvent commonEvent;
    auto commonMsg = std::make_shared<CommonSessionEventMsg>();
    commonMsg->type = EventType::EVENT_SCREEN_CAPTURE_COMMON;
    commonMsg->eventId = 7001;
    commonMsg->eventParam = "scenario_test";
    commonEvent.eventMsg = commonMsg;
    
    EXPECT_NO_THROW(session_->HandleEvent(commonEvent));
    
    // 8. 测试会话销毁
    statusMsg->status = SessionRunningStatus::SESSION_DESTROY;
    EXPECT_NO_THROW(session_->UpdateOperation(statusMsg));
}

} // namespace Sharing
} // namespace OHOS
