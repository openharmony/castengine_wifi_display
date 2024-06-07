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
#include "audio_play_controller.h"
#include "buffer_dispatcher.h"
#include "securec.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {
class AudioPlayControllerTestBaseImpl
        :public Singleton<AudioPlayControllerTestBaseImpl>,
         public IdentifierInfo {
private:
    FILE *audioTestFp_;
    std::string audioTestPath_ = "/data/mediaplayer_audio_test.wav";
    DataBuffer::Ptr audioTestData_;
    MediaData::Ptr mediaData_;
    size_t audioTestDataLen_;

    AudioTrack audioTrack_;
    BufferDispatcher::Ptr dispatcher_ = nullptr;

    bool playbackRun_;
    std::unique_ptr<std::thread> playbackThread_ = nullptr;
public:
    AudioPlayControllerTestBaseImpl()
    {
        dispatcher_ = std::make_shared<BufferDispatcher>(maxBufferCapacity, maxBufferCapacityIncrement);
        EXPECT_NE(nullptr, dispatcher_);

        mediaData_ = std::make_shared<MediaData>();
        EXPECT_NE(nullptr, mediaData_);
        mediaData_->isRaw = false;
        mediaData_->keyFrame = false;
        mediaData_->ssrc = 0;
        mediaData_->pts = 0;
        mediaData_->mediaType = MediaType::MEDIA_TYPE_AUDIO;
        AudioTestDataLoad();
        AudioTrackReset();
    }

    ~AudioPlayControllerTestBaseImpl() override
    {
        PlaybackStop();
        AudioTestDataUnLoad();
        BufferDispatcherReset();
    }
public:
    DataBuffer::Ptr& GetAudioTestData(void)
    {
        return audioTestData_;
    }

    size_t GetAudioTestDataLength(void) const
    {
        return audioTestDataLen_;
    }

    void AudioTrackReset(void)
    {
        audioTrack_.codecId = CodecId::CODEC_AAC;
        audioTrack_.sampleRate = defaultSampleRate;
        audioTrack_.sampleBit = DEFAULT_SAMPLE_BIT;
        audioTrack_.channels = defaultChannels;
    }

    AudioTrack& GetAudioTrack(void)
    {
        return audioTrack_;
    }

    void BufferDispatcherReset(void)
    {
        if (dispatcher_ != nullptr) {
            dispatcher_->StopDispatch();
            dispatcher_->FlushBuffer();
            dispatcher_->ReleaseAllReceiver();
        }
    }

    BufferDispatcher::Ptr& GetBufferDispatcher(void)
    {
        return dispatcher_;
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

    void AudioPlayerPlaybackStart(void)
    {
        EXPECT_EQ(nullptr, playbackThread_);
        EXPECT_EQ(false, playbackRun_);
        constexpr uint32_t waitForThread = 100;

        playbackRun_ = true;
        playbackThread_ = std::make_unique<std::thread>(
            [this]() {
                constexpr uint32_t interval = 20;
                SHARING_LOGD("AudioPlayerPlayback test start\n");
                while (IsPlaybackRunning()) {
                    SHARING_LOGD("AudioPlayerPlayback Writing\n");
                    dispatcher_->InputData(
                        mediaData_
                    );
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                }
                SHARING_LOGD("AudioPlayerPlayback test complete\n");
            }
        );
        pthread_setname_np(playbackThread_->native_handle(), "AudioPlayerPlayback");
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
        mediaData_->buff = audioTestData_;
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

class AudioPlayControllerUnitTest : public testing::Test {};

namespace {
AudioPlayControllerTestBaseImpl& g_testBase =
    AudioPlayControllerTestBaseImpl::GetInstance();

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Create_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<ContextEventMsg> cem = std::make_shared<ContextEventMsg>();
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_PCM;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_02,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_DEFAULT;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_03,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_H264;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_04,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_H265;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_05,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_VP8;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_06,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_VP9;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_07,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_AV1;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_08,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_AAC;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_09,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_G711A;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_10,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_G711U;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_11,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_OPUS;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_12,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_L16;
    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Init_13,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    g_testBase.AudioTrackReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Playback_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    g_testBase.AudioPlayerPlaybackStart();
    g_testBase.PlaybackStop();

    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
    g_testBase.AudioTrackReset();
    g_testBase.BufferDispatcherReset();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_SetVolume_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    audioPlayController->SetVolume(AudioPlayControllerTestBaseImpl::defaultVolume);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_SetVolume_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    audioPlayController->SetVolume(0);
    audioPlayController->SetVolume(AudioPlayControllerTestBaseImpl::defaultVolume);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_SetVolume_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    audioPlayController->SetVolume(AudioPlayControllerTestBaseImpl::testVolumeNegative);
    audioPlayController->SetVolume(AudioPlayControllerTestBaseImpl::testVolumeOverflow);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_SetVolume_Stability_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    g_testBase.AudioPlayerPlaybackStart();
    for (int i = 0; i < AudioPlayControllerTestBaseImpl::stabilityLoops; ++i) {
        audioPlayController->SetVolume(0);
        audioPlayController->SetVolume(AudioPlayControllerTestBaseImpl::defaultVolume);
    }
    g_testBase.PlaybackStop();
    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Start_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Start_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Start_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    audioPlayController->Release();
    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, false);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Start_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Start_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Stop_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);
    g_testBase.AudioPlayerPlaybackStart();

    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
    g_testBase.PlaybackStop();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Stop_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Stop_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Stop_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    audioPlayController->Release();
    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Stop_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);


    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
    ret = audioPlayController->Start(g_testBase.GetBufferDispatcher());
    EXPECT_EQ(ret, true);

    audioPlayController->Stop(g_testBase.GetBufferDispatcher());
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Release_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    ret = audioPlayController->Init(g_testBase.GetAudioTrack());
    EXPECT_EQ(ret, true);

    audioPlayController->Release();
}

HWTEST_F(AudioPlayControllerUnitTest, Audio_Play_Controller_Test_Release_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<AudioPlayController> audioPlayController =
        std::make_unique<AudioPlayController>(g_testBase.GetId());
    EXPECT_NE(nullptr, audioPlayController);

    audioPlayController->Release();
}

} // namespace
} // namespace Sharing
} // namespace OHOS