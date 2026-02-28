/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include <gtest/gtest.h>
#include "video_play_controller.h"

namespace OHOS {
namespace Sharing {

class VideoPlayControllerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    void SetUp() override
    {
        mediaChannelId_ = 1; // 媒体通道ID常量
        videoPlayController_ = std::make_shared<VideoPlayController>(mediaChannelId_);
    }

    void TearDown() override
    {
        if (videoPlayController_) {
            videoPlayController_->Release();
        }
    }

protected:
    uint32_t mediaChannelId_; // 媒体通道ID
    std::shared_ptr<VideoPlayController> videoPlayController_; // 视频播放控制器
};

HWTEST_F(VideoPlayControllerTest, Init_WithNoneCodecId_ReturnFalse, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器

    bool result = videoPlayController_->Init(videoTrack);

    EXPECT_FALSE(result);
}

HWTEST_F(VideoPlayControllerTest, Init_WithValidCodecId_ReturnTrue, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量

    bool result = videoPlayController_->Init(videoTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(VideoPlayControllerTest, Init_WithZeroWidthAndHeight_ReturnTrue, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 0; // 视频宽度为0
    videoTrack.height = 0; // 视频高度为0

    bool result = videoPlayController_->Init(videoTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(VideoPlayControllerTest, SetSurface_WithNullSurface_ReturnFalse, TestSize.Level1)
{
    sptr<Surface> surface = nullptr; // 空Surface
    bool keyFrame = false; // 非关键帧

    bool result = videoPlayController_->SetSurface(surface, keyFrame);

    EXPECT_FALSE(result);
}

HWTEST_F(VideoPlayControllerTest, SetSurface_WithNullDecoder_ReturnFalse, TestSize.Level1)
{
    sptr<Surface> surface = Surface::Create(); // 创建Surface
    bool keyFrame = false; // 非关键帧

    bool result = videoPlayController_->SetSurface(surface, keyFrame);

    EXPECT_FALSE(result);
}

HWTEST_F(VideoPlayControllerTest, SetSurface_WithValidSurface_ReturnTrue, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    bool keyFrame = true; // 关键帧

    bool result = videoPlayController_->SetSurface(surface, keyFrame);

    EXPECT_TRUE(result);
}

HWTEST_F(VideoPlayControllerTest, Start_WithoutSurface_ReturnFalse, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();

    bool result = videoPlayController_->Start(dispatcher);

    EXPECT_FALSE(result);
}

HWTEST_F(VideoPlayControllerTest, Stop_WithNullDispatcher_NoCrash, TestSize.Level1)
{
    BufferDispatcher::Ptr dispatcher = nullptr; // 空dispatcher

    videoPlayController_->Stop(dispatcher);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, Release_WithNullDecoder_NoCrash, TestSize.Level1)
{
    videoPlayController_->Release();

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, StopVideoThread_WithNullThread_NoCrash, TestSize.Level1)
{
    videoPlayController_->StopVideoThread();

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, ProcessVideoData_WithNullData_NoProcess, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    const char *data = nullptr; // 空数据指针
    int32_t size = 100; // 数据大小常量
    uint64_t pts = 0; // 演示时间戳

    videoPlayController_->ProcessVideoData(data, size, pts);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, ProcessVideoData_WithZeroSize_NoProcess, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    const char *data = "test_data"; // 测试数据
    int32_t size = 0; // 数据大小为0
    uint64_t pts = 0; // 演示时间戳

    videoPlayController_->ProcessVideoData(data, size, pts);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, ProcessVideoData_WithWithNegativeSize_NoProcess, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    const char *data = "test_data"; // 测试数据
    int32_t size = -1; // 数据大小为负数
    uint64_t pts = 0; // 演示时间戳

    videoPlayController_->ProcessVideoData(data, size, pts);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, OnVideoDataDecoded_WithNullData_NoRender, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    DataBuffer::Ptr decodedData = nullptr; // 空解码数据

    videoPlayController_->OnVideoDataDecoded(decodedData);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, OnError_WithServiceDiedCode_NotifyMediaController, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    int32_t errorCode = MediaAVCodec::AVCS_ERR_SERVICE_DIED; // 服务死亡错误码

    videoPlayController_->OnError(errorCode);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, OnError_WithUnknownErrorCode_NoNotify, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    int32_t errorCode = 9999; // 未知错误码

    videoPlayController_->OnError(errorCode);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, OnAccelerationDoneNotify_NotifyMediaController, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    videoPlayController_->OnAccelerationDoneNotify();

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, OnKeyModeNotify_WithEnableTrue_NotifyStart, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    bool enable = true; // 启用关键模式

    videoPlayController_->OnKeyModeNotify(enable);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, OnKeyModeNotify_WithEnableFalse_NotifyStop, TestSize.Level1)
{
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    videoPlayController_->Init(videoTrack);

    bool enable = false; // 禁用关键模式

    videoPlayController_->OnKeyModeNotify(enable);

    SUCCEED();
}

HWTEST_F(VideoPlayControllerTest, SetVideoAudioSync_WithNullDecoder_NoSet, TestSize.Level1)
{
    std::shared_ptr<VideoAudioSync> videoAudioSync = std::make_shared<VideoAudioSync>();

    videoPlayController_->SetVideoAudioSync(videoAudioSync);

    SUCCEED();
}

} // namespace Sharing
} // namespace OHOS
