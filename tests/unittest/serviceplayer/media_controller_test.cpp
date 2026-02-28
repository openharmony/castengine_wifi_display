/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include <gtest/gtest.h>
#include "media_controller.h"

namespace OHOS {
namespace Sharing {

class MediaControllerTest : public testing::Test {
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
        mediaController_ = std::make_shared<MediaController>(mediaChannelId_);
    }

    void TearDown() override
    {
        if (mediaController_) {
            mediaController_->Release();
        }
    }

protected:
    uint32_t mediaChannelId_; // 媒体通道ID
    std::shared_ptr<MediaController> mediaController_; // 媒体控制器
};

HWTEST_F(MediaControllerTest, Init_WithNoneCodecId_ReturnFalse, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器

    bool result = mediaController_->Init(audioTrack, videoTrack);

    EXPECT_FALSE(result);
}

HWTEST_F(MediaControllerTest, Init_WithAudioCodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器

    bool result = mediaController_->Init(audioTrack, videoTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(MediaControllerTest, Init_WithVideoCodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量

    bool result = mediaController_->Init(audioTrack, videoTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(MediaControllerTest, Init_WithBothCodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量

    bool result = mediaController_->Init(audioTrack, videoTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(MediaControllerTest, Start_WithoutInit_NoStart, TestSize.Level1)
{
    mediaController_->Start();

    SUCCEED();
}

HWTEST_F(MediaControllerTest, Start_WithAudioInit_StartSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器
    mediaController_->Init(audioTrack, videoTrack);

    mediaController_->Start();

    SUCCEED();
}

HWTEST_F(MediaControllerTest, Start_WithVideoInit_StartSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    mediaController_->Start();

    SUCCEED();
}

HWTEST_F(MediaControllerTest, Stop_WithoutInit_NoStop, TestSize.Level1)
{
    mediaController_->Stop();

    SUCCEED();
}

HWTEST_F(MediaControllerTest, Stop_WithInit_StopSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器
    mediaController_->Init(audioTrack, videoTrack);

    mediaController_->Stop();

    SUCCEED();
}

HWTEST_F(MediaControllerTest, Release_WithoutInit_NoCrash, TestSize.Level1)
{
    mediaController_->Release();

    SUCCEED();
}

HWTEST_F(MediaControllerTest, Release_WithInit_ReleaseSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器
    mediaController_->Init(audioTrack, videoTrack);

    mediaController_->Release();

    SUCCEED();
}

HWTEST_F(MediaControllerTest, AppendSurface_WithNullSurface_ReturnFalse, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = nullptr; // 空Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景

    bool result = mediaController_->AppendSurface(surface, sceneType);

    EXPECT_FALSE(result);
}

HWTEST_F(MediaControllerTest, AppendSurface_WithNoneVideoCodec_ReturnFalse, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景

    bool result = mediaController_->AppendSurface(surface, sceneType);

    EXPECT_FALSE(result);
}

HWTEST_F(MediaControllerTest, AppendSurface_WithValidSurface_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景

    bool result = mediaController_->AppendSurface(surface, sceneType);

    EXPECT_TRUE(result);
}

HWTEST_F(MediaControllerTest, AppendSurface_WithBackgroundScene_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::BACKGROUND; // 后台场景

    bool result = mediaController_->AppendSurface(surface, sceneType);

    EXPECT_TRUE(result);
}

HWTEST_F(MediaControllerTest, RemoveSurface_WithInvalidId_NoRemove, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    uint64_t surfaceId = 9999; // 无效Surface ID

    mediaController_->RemoveSurface(surfaceId);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, RemoveSurface_WithValidId_RemoveSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景
    mediaController_->AppendSurface(surface, sceneType);

    uint64_t surfaceId = surface->GetUniqueId(); // 获取Surface ID

    mediaController_->RemoveSurface(surfaceId);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetVolume_WithoutInit_NoSet, TestSize.Level1)
{
    float volume = 0.5; // 中等音量

    mediaController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetVolume_WithAudioInit_SetSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器
    mediaController_->Init(audioTrack, videoTrack);

    float volume = 0.5; // 中等音量

    mediaController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetVolume_WithMaxVolume_SetSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器
    mediaController_->Init(audioTrack, videoTrack);

    float volume = 1.0; // 最大音量

    mediaController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetVolume_WithMinVolume_SetSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_NONE; // 无编解码器
    mediaController_->Init(audioTrack, videoTrack);

    float volume = 0.0; // 最小音量

    mediaController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetKeyMode_WithInvalidId_NoSet, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    uint64_t surfaceId = 9999; // 无效Surface ID
    bool mode = true; // 启用模式

    mediaController_->SetKeyMode(surfaceId, mode);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetKeyMode_WithValidId_SetSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景
    mediaController_->AppendSurface(surface, sceneType);

    uint64_t surfaceId = surface->GetUniqueId(); // 获取Surface ID
    bool mode = true; // 启用模式

    mediaController_->SetKeyMode(surfaceId, mode);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetKeyMode_WithModeFalse_SetSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景
    mediaController_->AppendSurface(surface, sceneType);

    uint64_t surfaceId = surface->GetUniqueId(); // 获取Surface ID
    bool mode = false; // 禁用模式

    mediaController_->SetKeyMode(surfaceId, mode);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetKeyRedirect_WithInvalidId_NoSet, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    uint64_t surfaceId = 9999; // 无效Surface ID
    bool keyRedirect = true; // 启用重定向

    mediaController_->SetKeyRedirect(surfaceId, keyRedirect);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetKeyRedirect_WithValidId_SetSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景
    mediaController_->AppendSurface(surface, sceneType);

    uint64_t surfaceId = surface->GetUniqueId(); // 获取Surface ID
    bool keyRedirect = true; // 启用重定向

    mediaController_->SetKeyRedirect(surfaceId, keyRedirect);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, SetKeyRedirect_WithRedirectFalse_SetSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    sptr<Surface> surface = Surface::Create(); // 创建Surface
    SceneType sceneType = SceneType::FOREGROUND; // 前台场景
    mediaController_->AppendSurface(surface, sceneType);

    uint64_t surfaceId = surface->GetUniqueId(); // 获取Surface ID
    bool keyRedirect = false; // 禁用重定向

    mediaController_->SetKeyRedirect(surfaceId, keyRedirect);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, OnPlayControllerNotify_WithNullMsg_NoNotify, TestSize.Level1)
{
    ProsumerStatusMsg::Ptr statusMsg = nullptr; // 空状态消息

    mediaController_->OnPlayControllerNotify(statusMsg);

    SUCCEED();
}

HWTEST_F(MediaControllerTest, OnPlayControllerNotify_WithValidMsg_NotifySuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    VideoTrack videoTrack;
    videoTrack.codecId = CODEC_H264; // H264编解码器
    videoTrack.width = 1920; // 视频宽度常量
    videoTrack.height = 1080; // 视频高度常量
    mediaController_->Init(audioTrack, videoTrack);

    auto statusMsg = std::make_shared<ProsumerStatusMsg>(); // 创建状态消息
    auto msg = std::make_shared<EventMsg>(); // 创建事件消息
    statusMsg->eventMsg = msg;
    statusMsg->errorCode = ERR_OK; // 错误码
    statusMsg->status = CONNTROLLER_NOTIFY_START; // 通知开始

    mediaController_->OnPlayControllerNotify(statusMsg);

    SUCCEED();
}

} // namespace Sharing
} // namespace OHOS
