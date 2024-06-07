/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <memory>
#include <string>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>


#include "common/sharing_log.h"
#include "media_controller.h"
#include "media_channel.h"
#include "common/event_comm.h"
#include "common/media_log.h"
#include "media_channel_def.h"
#include "securec.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {
class AudioPlayerTestBaseImpl
        : public Singleton<AudioPlayerTestBaseImpl> {
private:
    FILE *audioTestFp_;
    std::string audioTestPath_ = "/data/mediaplayer_audio_test.wav";
    DataBuffer::Ptr audioTestData_;
    size_t audioTestDataLen_;

    bool playbackRun_;
    std::unique_ptr<std::thread> playbackThread_ = nullptr;
public:

    DataBuffer::Ptr GetAudioTestData(void) const
    {
        return audioTestData_;
    }

    size_t GetAudioTestDataLength(void) const
    {
        return audioTestDataLen_;
    }

    bool IsPlaybackRunning(void) const
    {
        return playbackRun_;
    }

    void PlaybackStop(void)
    {
        playbackRun_ = false;
        playbackThread_->join();
        playbackThread_ = nullptr;
    }

    void AudioPlayerPlayback(std::unique_ptr<AudioPlayer>& audioPlayer)
    {
        EXPECT_NE(nullptr, audioPlayer);
        EXPECT_EQ(nullptr, playbackThread_);
        EXPECT_EQ(false, playbackRun_);
        constexpr uint32_t waitForThread = 100;

        playbackRun_ = true;
        playbackThread_ = std::make_unique<std::thread>(
            [this, &audioPlayer]() {
                SHARING_LOGD("AudioPlayerPlayback test start\n");
                while (IsPlaybackRunning()) {
                    SHARING_LOGD("AudioPlayerPlayback Writing\n");
                    audioPlayer->ProcessAudioData(
                        GetAudioTestData()
                    );
                }
                SHARING_LOGD("AudioPlayerPlayback test complete\n");
            }
        );
        pthread_setname_np(playbackThread_->native_handle(), "AuidoPlayerPlayback");
        std::this_thread::sleep_for(std::chrono::milliseconds(waitForThread));
    }

private:
    void AudioTestDataLoad(void)
    {
        audioTestFp_ = ::fopen(audioTestPath_.c_str(), "r");
        EXPECT_NE(nullptr, audioTestFp_);

        bool ret = fseek(audioTestFp_, 0, SEEK_END);
        EXPECT_EQ(false, ret < 0);

        size_t fileLen = ftell(audioTestFp_);
        EXPECT_EQ(false, fileLen < 0);

        audioTestDataLen_ = fileLen + 1;
        rewind(audioTestFp_);
        char *audioTestDataTmp = new char[audioTestDataLen_];
        EXPECT_NE(nullptr, audioTestDataTmp);

        memset_s(audioTestDataTmp, audioTestDataLen_, 0, audioTestDataLen_);

        ret = fread(audioTestDataTmp, 1, fileLen, audioTestFp_);
        EXPECT_EQ(true, ret > 0);

        ::fclose(audioTestFp_);
        audioTestData_ = std::make_shared<DataBuffer>(audioTestDataLen_);
        audioTestData_->Assign(audioTestDataTmp, fileLen);
        audioTestFp_ = nullptr;
        delete[] audioTestDataTmp;
    }

    void AudioTestDataUnLoad(void)
    {
        if (nullptr != audioTestFp_) {
            ::fclose(audioTestFp_);
            audioTestFp_ = nullptr;
        }
        if (nullptr != audioTestData_) {
            audioTestData_ = nullptr;
        }
        audioTestDataLen_ = 0;
    }

public:
    AudioPlayerTestBaseImpl()
    {
        AudioTestDataLoad();
    }

    ~AudioPlayerTestBaseImpl()
    {
        PlaybackStop();
        AudioTestDataUnLoad();
    }

public:
    static constexpr int32_t maxBufferCapacity = ::MAX_BUFFER_CAPACITY;
    static constexpr int32_t maxBufferCapacityIncrement = ::BUFFER_CAPACITY_INCREMENT;
    static constexpr int32_t stabilityLoops = 100;
    static constexpr int32_t defaultChannels = 2;
    static constexpr int32_t defaultSampleRate = 48000;
    static constexpr int32_t defaultBitsPerSample = 16;
    static constexpr float defaultVolume = 1.0;
    static constexpr float testVolume = 0.5;
    static constexpr float testVolumeOverflow = 1.5;
    static constexpr float testVolumeNegative = -0.5;
};

class AudioControllerUnitTest : public testing::Test {};

namespace {
AudioPlayerTestBaseImpl& g_testBase = AudioPlayerTestBaseImpl::GetInstance();

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Create_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<ContextEventMsg> cem = std::make_shared<ContextEventMsg>();
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer =
                                        std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_PCM);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_02,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_DEFAULT);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_03,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_H264);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_04,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_H265);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_05,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_VP8);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_06,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_VP9);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_07,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_AV1);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_08,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_AAC);
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_09,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_G711A);
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_10,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_G711U);
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_11,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_OPUS);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_12,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init(CodecId::CODEC_L16);
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Init_13,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Playback_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    g_testBase.AudioPlayerPlayback(audioPlayer);
    g_testBase.PlaybackStop();
    audioPlayer->Stop();
    audioPlayer->Release();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetParameters_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->SetAudioFormat(
        AudioPlayerTestBaseImpl::defaultChannels,
         AudioPlayerTestBaseImpl::defaultSampleRate);
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetParameters_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->SetAudioFormat(
        AudioPlayerTestBaseImpl::defaultChannels,
        AudioPlayerTestBaseImpl::defaultSampleRate);
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetParameters_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    const int32_t invalidSampleRate = 10000000;
    const int32_t invalidChannels = 10000000;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->SetAudioFormat(AudioPlayerTestBaseImpl::defaultChannels, invalidSampleRate);
    EXPECT_EQ(ret, true);

    ret = audioPlayer->SetAudioFormat(invalidChannels, AudioPlayerTestBaseImpl::defaultSampleRate);
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetParameters_Stability_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);


    for (int i = 0; i < AudioPlayerTestBaseImpl::stabilityLoops; ++i) {
        ret = audioPlayer->SetAudioFormat(
            AudioPlayerTestBaseImpl::defaultChannels,
            AudioPlayerTestBaseImpl::defaultSampleRate);
        EXPECT_EQ(ret, true);
    }
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetVolume_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->SetVolume(AudioPlayerTestBaseImpl::defaultVolume);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetVolume_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->SetVolume(0);
    audioPlayer->SetVolume(AudioPlayerTestBaseImpl::defaultVolume);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetVolume_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->SetVolume(AudioPlayerTestBaseImpl::testVolumeNegative);
    audioPlayer->SetVolume(AudioPlayerTestBaseImpl::testVolumeOverflow);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_SetVolume_Stability_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    g_testBase.AudioPlayerPlayback(audioPlayer);
    for (int i = 0; i < AudioPlayerTestBaseImpl::stabilityLoops; ++i) {
        audioPlayer->SetVolume(0);
        audioPlayer->SetVolume(AudioPlayerTestBaseImpl::defaultVolume);
    }
    g_testBase.PlaybackStop();
    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Start_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Start_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Start_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    audioPlayer->Release();
    ret = audioPlayer->Start();
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Start_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->Stop();
    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_ProcessAudioData_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->ProcessAudioData(g_testBase.GetAudioTestData());

    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_ProcessAudioData_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->ProcessAudioData(g_testBase.GetAudioTestData());

    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Stop_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->ProcessAudioData(g_testBase.GetAudioTestData());

    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Stop_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Stop_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Stop_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Stop_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->Stop();
    ret = audioPlayer->Start();
    EXPECT_EQ(ret, true);

    audioPlayer->Stop();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Release_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    ret = audioPlayer->Init();
    EXPECT_EQ(ret, true);

    audioPlayer->Release();
}

HWTEST_F(AudioControllerUnitTest, Audio_Player_Test_Release_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<AudioPlayer> audioPlayer = std::make_unique<AudioPlayer>();
    EXPECT_NE(nullptr, audioPlayer);

    audioPlayer->Release();
}

} // namespace
} // namespace Sharing
} // namespace OHOS