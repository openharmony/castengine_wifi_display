/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include <gtest/gtest.h>
#include "audio_player.h"

namespace OHOS {
namespace Sharing {

class AudioPlayerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    void SetUp() override
    {
        audioPlayer_ = std::make_shared<AudioPlayer>();
    }

    void TearDown() override
    {
        if (audioPlayer_) {
            audioPlayer_->Release();
        }
    }

protected:
    std::shared_ptr<AudioPlayer> audioPlayer_; // 音频播放器
};

HWTEST_F(AudioPlayerTest, Init_WithAACCodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayer_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, Init_WithMP3CodecId_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_MP3; // MP3编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayer_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, Init_WithNoneCodecId_ReturnFalse, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_NONE; // 无编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayer_->Init(audioTrack);

    EXPECT_FALSE(result);
}

HWTEST_F(AudioPlayerTest, Init_WithOneChannel_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 1; // 单声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayer_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, Init_WithTwoChannels_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayer_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, Init_With44100SampleRate_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量

    bool result = audioPlayer_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, Init_With48000SampleRate_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 48000; // 采样率常量

    bool result = audioPlayer_->Init(audioTrack);

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, Start_WithoutInit_ReturnFalse, TestSize.Level1)
{
    bool result = audioPlayer_->Start();

    EXPECT_FALSE(result);
}

HWTEST_F(AudioPlayerTest, Start_WithInit_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    bool result = audioPlayer_->Start();

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, Start_WithRunning_ReturnTrue, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    bool result = audioPlayer_->Start();

    EXPECT_TRUE(result);
}

HWTEST_F(AudioPlayerTest, SetVolume_WithoutInit_NoCrash, TestSize.Level1)
{
    float volume = 0.5; // 中等音量

    audioPlayer_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, SetVolume_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    float volume = 0.5; // 中等音量

    audioPlayer_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, SetVolume_WithMaxVolume_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    float volume = 1.0; // 最大音量

    audioPlayer_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, SetVolume_WithMinVolume_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    float volume = 0.0; // 最小音量

    audioPlayer_->SetVolume(volume);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, Stop_WithoutInit_NoCrash, TestSize.Level1)
{
    audioPlayer_->Stop();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, Stop_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    audioPlayer_->Stop();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, Stop_WithStartAndStop_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    audioPlayer_->Stop();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, Release_WithoutInit_NoCrash, TestSize.Level1)
{
    audioPlayer_->Release();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, Release_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    audioPlayer_->Release();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, Release_WithStartAndStop_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();
    audioPlayer_->Stop();

    audioPlayer_->Release();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, ProcessAudioData_WithoutInit_NoProcess, TestSize.Level1)
{
    int32_t dataSize = 4096; // 数据大小常量
    std::vector<uint8_t> data(dataSize);
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    buffer->ReplaceData(reinterpret_cast<char*>(data.data()), dataSize);
    uint64_t pts = 0; // 演示时间戳

    audioPlayer_->ProcessAudioData(buffer, pts);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, ProcessAudioData_WithInitButNotRunning_NoProcess, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    int32_t dataSize = 4096; // 数据大小常量
    std::vector<uint8_t> data(dataSize);
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    buffer->ReplaceData(reinterpret_cast<char*>(data.data()), dataSize);
    uint64_t pts = 0; // 演示时间戳

    audioPlayer_->ProcessAudioData(buffer, pts);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, ProcessAudioData_WithNullData_NoProcess, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    DataBuffer::Ptr buffer = nullptr; // 空数据指针
    uint64_t pts = 0; // 演示时间戳

    audioPlayer_->ProcessAudioData(buffer, pts);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, ProcessAudioData_WithValidData_ProcessSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    int32_t dataSize = 4096; // 数据大小常量
    std::vector<uint8_t> data(dataSize);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = static_cast<uint8_t>(i % 256); // 模256生成测试数据
    }
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    buffer->ReplaceData(reinterpret_cast<char*>(data.data()), dataSize);
    uint64_t pts = 0; // 演示时间戳

    audioPlayer_->ProcessAudioData(buffer, pts);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, ProcessAudioData_WithLargeData_ProcessSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    int32_t dataSize = 8192; // 数据大小常量
    std::vector<uint8_t> data(dataSize);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = static_cast<uint8_tari>(i % 256); // 模256生成测试数据
    }
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    buffer->ReplaceData(reinterpret_cast<char*>(data.data()), dataSize);
    uint64_t pts = 0; // 演示时间戳

    audioPlayer_->ProcessAudioData(buffer, pts);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, ProcessAudioData_WithNonZeroPts_ProcessSuccessfully, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    int32_t dataSize = 4096; // 数据大小常量
    std::vector<uint8_t> data(dataSize);
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    buffer->ReplaceData(reinterpret_cast<char*>(data.data()), dataSize);
    uint64_t pts = 1000; // 演示时间戳

    audioPlayer_->ProcessAudioData(buffer, pts);

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, GetDecoderTimestamp_WithoutInit_ReturnZero, TestSize.Level1)
{
    int64_t timestamp = audioPlayer_->GetDecoderTimestamp();

    EXPECT_EQ(timestamp, 0); // 时间戳初始值
}

HWTEST_F(AudioPlayerTest, GetDecoderTimestamp_WithInit_ReturnZero, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    int64_t timestamp = audioPlayer_->GetDecoderTimestamp();

    EXPECT_EQ(timestamp, 0); // 时间戳初始值
}

HWTEST_F(AudioPlayerTest, GetDecoderTimestamp_WithStart_ReturnZero, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    int64_t timestamp = audioPlayer_->GetDecoderTimestamp();

    EXPECT_EQ(timestamp, 0); // 时间戳初始值
}

HWTEST_F(AudioPlayerTest, DropOneFrame_WithoutInit_NoCrash, TestSize.Level1)
{
    audioPlayer_->DropOneFrame();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, DropOneFrame_WithInit_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    audioPlayer_->DropOneFrame();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, DropOneFrame_WithStart_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    audioPlayer_->DropOneFrame();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, FullLifecycle_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);

    audioPlayer_->Start();

    audioPlayer_->SetVolume(0.5); // 设置中等音量

    int32_t dataSize = 4096; // 数据大小常量
    std::vector<uint8_t> data(dataSize);
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    buffer->ReplaceData(reinterpret_cast<char*>(data.data()), dataSize);
    uint64_t pts = 0; // 演示时间戳
    audioPlayer_->ProcessAudioData(buffer, pts);

    int64_t timestamp = audioPlayer_->GetDecoderTimestamp();
    audioPlayer_->DropOneFrame();

    audioPlayer_->Stop();
    audioPlayer_->Release();

    SUCCEED();
}

HWTEST_F(AudioPlayerTest, MultipleProcessAudioData_NoCrash, TestSize.Level1)
{
    AudioTrack audioTrack;
    audioTrack.codecId = CODEC_AAC; // AAC编解码器
    audioTrack.channels = 2; // 双声道
    audioTrack.sampleRate = 44100; // 采样率常量
    audioPlayer_->Init(audioTrack);
    audioPlayer_->Start();

    for (int i = 0; i < 10; i++) { // 循环10次
        int32_t dataSize = 4096; // 数据大小常量
        std::vector<uint8_t> data(dataSize);
        DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
        buffer->ReplaceData(reinterpret_cast<char*>(data.data()), dataSize);
        uint64_t pts = i * 100; // 演示时间戳
        audioPlayer_->ProcessAudioData(buffer, pts);
    }

    audioPlayer_->Stop();
    audioPlayer_->Release();

    SUCCEED();
}

} // namespace Sharing
} // namespace OHOS
