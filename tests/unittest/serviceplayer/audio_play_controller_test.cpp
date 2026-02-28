/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include <gtest/gtest.h>
#include "audio_play_controller.h"

namespace OHOS {
namespace Sharing {

class AudioPlayControllerTest : public testing::Test {
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
        audioPlayController_ = std::make_shared<AudioPlayController>(mediaChannelId_);
    }

    void TearDown() override
    {
        if (audioPlayController_) {
            audioPlayController_->Release();
        }
    }

protected:
    uint32_t mediaChannelId_; // 媒体通道ID
    std::shared_ptr<AudioPlayController> audioPlayController_; // 音频播放控制器
};

HWTEST_F(AudioPlayControllerTest, Init_WithNoneCodecId_ReturnFalse, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器器

    bool result = audioPlayController_->Init(audioTrack);

    EXPECT_FALSE(result);
}

HWTEST_F(AudioPlayControllerTest, Init_WithValidCodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayController_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayControllerTest, Init_WithAACCodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayController_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayControllerTest, Init_WithMPMP3CodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_MP3; // MP3编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayController_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayControllerTest, Start_WithoutInit_ReturnFalse, TestSize.Level1)
{
    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();

    bool result = audioPlayController_->Start(dispatcher);

    EXPECT_FALSE(result);
}

HWTEST_F(AudioPlayControllerTest, Start_WithNullDispatcher_ReturnFalse, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = nullptr; // 空dispatcher

    bool result = audioPlayController_->Start(dispatcher);

    EXPECT_FALSE(result);
}

HWTEST_F(AudioPlayControllerTest, Start_WithInit_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();

    bool result = audioPlayController_->Start(dispatcher);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayControllerTest, Start_WithRunning_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();
    audioPlayController_->Start(dispatcher);

    bool result = audioPlayController_->Start(dispatcher);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayControllerTest, Stop_WithoutInit_NoCrash, TestSize.Level1)
{
    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();

    audioPlayController_->Stop(dispatcher);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, Stop_WithNullDispatcher_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = nullptr; // 空dispatcher

    audioPlayController_->Stop(dispatcher);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, Stop_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();

    audioPlayController_->Stop(dispatcher);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, Stop_WithStartAndStop_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();
    audioPlayController_->Start(dispatcher);

    audioPlayController_->Stop(dispatcher);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, Release_WithoutInit_NoCrash, TestSize.Level1)
{
    (void)audioPlayController_->Release();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, Release_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    audioPlayController_->Release();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, Release_WithStartAndStop_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();
    audioPlayController_->Start(dispatcher);
    audioPlayController_->Stop(dispatcher);

    audioPlayController_->Release();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, StartAudioThread_WithoutInit_NoCrash, TestSize.Level1)
{
    audioPlayController_->StartAudioThread();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, StartAudioThread_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    audioPlayController_->StartAudioThread();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, StopAudioThread_WithoutInit_NoCrash, TestSize.Level1)
{
    audioPlayController_->StopAudioThread();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, StopAudioThread_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    audioPlayController_->StopAudioThread();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, StopAudioThread_WithStartAndStop_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();
    audioPlayController_->Start(dispatcher);
    audioPlayController_->Stop(dispatcher);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, SetVolume_WithoutInit_NoCrash, TestSize.Level1)
{
    float volume = 0.5; // 中等音量

    audioPlayController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, SetVolume_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    float volume = 0.5; // 中等音量

    audioPlayController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, SetVolume_WithMaxVolume_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    float volume = 1.0; // 最大音量

    audioPlayController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, SetVolume_WithMinVolume_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    float volume = 0.0; // 最小音量

    audioPlayController_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, GetAudioDecoderTimestamp_WithoutInit_ReturnZero, TestSize.Level1)
{
    int64_t timestamp = audioPlayController_->GetAudioDecoderTimestamp();

    EXPECT_EQ(timestamp, 0); // 时间戳初始值
}

HWTEST_F(AudioPlayControllerTest, GetAudioDecoderTimestamp_WithInit_ReturnZero, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    int64_t timestamp = audioPlayController_->GetAudioDecoderTimestamp();

    EXPECT_EQ(timestamp, 0); // 时间戳初始值
}

HWTEST_F(AudioPlayControllerTest, DropOneFrame_WithoutInit_NoCrash, TestSize.Level1)
{
    audioPlayController_->DropOneFrame();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, DropOneFrame_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    audioPlayController_->DropOneFrame();

    SUCCEED();
}

HWTEST_F(AudioPlayControllerTest, FullLifecycle_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayController_->Init(audioTrack);

    BufferDispatcher::Ptr dispatcher = std::make_shared<BufferDispatcher>();
    audioPlayController_->Start(dispatcher);

    audioPlayController_->SetVolume(0.5); // 设置中等音量
    int64_t timestamp = audioPlayController_->GetAudioDecoderTimestamp();
    audioPlayController_->DropOneFrame();

    audioPlayController_->Stop(dispatcher);
    audioPlayController_->Release();

    SUCCEED();
}

} // namespace Sharing
} // namespace OHOS
