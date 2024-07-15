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
#include <chrono>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>


#include "common/sharing_log.h"
#include "media_controller.h"
#include "media_channel.h"
#include "common/event_comm.h"
#include "common/media_log.h"
#include "media_channel_def.h"
#include "media_channel.h"
#include "video_play_controller.h"
#include "buffer_dispatcher.h"
#include "protocol/frame/h264_frame.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "surface.h"
#include "surface_utils.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"
#include "utils/utils.h"
#include "securec.h"
#include "window.h"
#include "window_option.h"
#include "ui/rs_surface_node.h"
#include "configuration/include/config.h"
#include "common/reflect_registration.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {
class MediaplayerTestConsumer final: public BaseConsumer {
    void UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg) override
    {
        (void)statusMsg;
    }

    int32_t HandleEvent(SharingEvent &event) override
    {
        (void) event;
        return 0;
    }

    int32_t Release() override
    {
        return 0;
    }
};

REGISTER_CLASS_REFLECTOR(MediaplayerTestConsumer);

class MediaControllerTestBaseImpl
        : public Singleton<MediaControllerTestBaseImpl> {
private:
    FILE *videoTestFp_;
    std::string videoTestPath_ = "/data/mediaplayer_video_test.mp4";
    DataBuffer::Ptr videoTestData_;
    std::vector<DataBuffer::Ptr> videoFrames_;
    MediaData::Ptr mediaData_;
    size_t videoTestDataLen_;

    VideoTrack videoTrack_;
    BufferDispatcher::Ptr dispatcher_ = nullptr;
    MediaChannel::Ptr mediaChannel_ = nullptr;

    bool playbackRun_;
    std::unique_ptr<std::thread> playbackThread_ = nullptr;

    sptr<Rosen::WindowOption> windowOption_ = nullptr;
    sptr<Rosen::Window>  window_ = nullptr;
    std::shared_ptr<Rosen::RSSurfaceNode>  surfaceNode_ = nullptr;
    sptr<Surface>  surface_ = nullptr;
    uint64_t surfaceId_ = 0;

    bool isFirstKeyFrame_ = false;
    size_t frameNums_ = 0;
    std::chrono::steady_clock::time_point gopInterval_;
public:
    DataBuffer::Ptr& GetVideoTestData(void)
    {
        return videoTestData_;
    }

    size_t GetVideoTestDataLength(void) const
    {
        return videoTestDataLen_;
    }

    sptr<Surface>& GetSurface(void)
    {
        return surface_;
    }

    BufferDispatcher::Ptr& GetBufferDispatcher(void)
    {
        return dispatcher_;
    }

    VideoTrack& GetVideoTrack(void)
    {
        return videoTrack_;
    }

    MediaChannel::Ptr& GetMediaChannel(void)
    {
        return mediaChannel_;
    }

    uint32_t GetId(void)
    {
        return mediaChannel_->GetId();
    }

    void VideoTrackReset(void)
    {
        videoTrack_.codecId = CodecId::CODEC_H264;
        videoTrack_.width = defaultWidth;
        videoTrack_.height = defaultHeight;
        videoTrack_.frameRate = defaultFrameRate;
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

    void BufferDispatcherReset(void)
    {
        if (dispatcher_ != nullptr) {
            dispatcher_->StopDispatch();
            dispatcher_->FlushBuffer();
            dispatcher_->ReleaseAllReceiver();
        }
    }
    void VideoPlayerPlaybackStart(void)
    {
        EXPECT_EQ(nullptr, playbackThread_);
        EXPECT_EQ(false, playbackRun_);
        constexpr uint32_t waitForThread = 100;

        playbackRun_ = true;
        playbackThread_ = std::make_unique<std::thread>(
            [this]() {
                constexpr uint32_t interval = 20;
                SHARING_LOGD("VideoPlayerPlayback test start\n");
                while (IsPlaybackRunning()) {
                    SHARING_LOGD("VideoPlayerPlayback Writing\n");
                    VideoDataPlayOnce();
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                }
                SHARING_LOGD("VideoPlayerPlayback test complete\n");
            }
        );
        pthread_setname_np(playbackThread_->native_handle(), "VideoPlayerPlayback");
        std::this_thread::sleep_for(std::chrono::milliseconds(waitForThread));
    }

    void SpsDataSetup(const char *buf, size_t len)
    {
        auto sps = std::make_shared<MediaData>();
        sps->buff = std::make_shared<DataBuffer>();
        sps->mediaType = MEDIA_TYPE_VIDEO;
        sps->buff->Assign(const_cast<char *>(buf), len);
        dispatcher_->SetSpsNalu(sps);
    }

    void PpsDataSetup(const char *buf, size_t len)
    {
        auto pps = std::make_shared<MediaData>();
        pps->buff = std::make_shared<DataBuffer>();
        pps->mediaType = MEDIA_TYPE_VIDEO;
        pps->buff->Assign(const_cast<char *>(buf), len);
        dispatcher_->SetPpsNalu(pps);
    }

    void MediaDataSetup(const char *buf, size_t len, size_t prefix)
    {
        auto mediaData = std::make_shared<MediaData>();
        mediaData->buff = std::make_shared<DataBuffer>();
        mediaData->mediaType = MEDIA_TYPE_VIDEO;
        mediaData->isRaw = false;
        mediaData->keyFrame = (*(buf + prefix) & 0x1f) == 0x05 ? true : false;
        if (mediaData->keyFrame) {
            if (isFirstKeyFrame_) {
                MEDIA_LOGD("TEST STATISTICS Miracast:first, agent ID:%{public}d, get video frame.", GetId());
                isFirstKeyFrame_ = false;
            } else {
                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<double, std::milli> diff = end - gopInterval_;
                MEDIA_LOGD("TEST STATISTIC Miracast:interval:%{public}.0f ms, "
                    "agent ID:%{public}d, get video frame, gop:%{public}d, "
                    "average receiving frames time:%{public}.0f ms.",
                    diff.count(), GetId(), frameNums_, diff.count() / frameNums_);
            }
            frameNums_ = 0;
            gopInterval_ = std::chrono::steady_clock::now();
        }
        ++frameNums_;
        mediaData->buff->ReplaceData((char *)buf, len);
        mediaData->pts = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        dispatcher_->InputData(mediaData);
    }

    void VideoDataPlayOnce(void)
    {
        SplitH264(reinterpret_cast<char *>(videoTestData_->Data()), videoTestData_->Size(), 0,
            [&](const char *buf, size_t len, size_t prefix) {
                if ((*(buf + prefix) & 0x1f) == 0x06) {
                    return;
                }
                if ((*(buf + prefix) & 0x1f) == 0x07) {
                    auto spsOld = dispatcher_->GetSPS();
                    if (spsOld != nullptr && spsOld->buff != nullptr) {
                        return;
                    }
                    SpsDataSetup(buf, len);
                    return;
                }
                if ((*(buf + prefix) & 0x1f) == 0x08) {
                    auto ppsOld = dispatcher_->GetPPS();
                    if (ppsOld != nullptr && ppsOld->buff != nullptr) {
                        return;
                    }
                    PpsDataSetup(buf, len);
                    return;
                }
                MediaDataSetup(buf, len, prefix);
            });
    }

private:
    void VideoTestDataLoad(void)
    {
        videoTestFp_ = ::fopen(videoTestPath_.c_str(), "r");
        EXPECT_NE(nullptr, videoTestFp_);

        bool ret = fseek(videoTestFp_, 0, SEEK_END);
        EXPECT_EQ(false, ret < 0);

        size_t fileLen = ftell(videoTestFp_);
        EXPECT_EQ(false, fileLen < 0);

        videoTestDataLen_ = fileLen + 1;
        rewind(videoTestFp_);
        char *videoTestDataTmp = new char[videoTestDataLen_];
        EXPECT_NE(nullptr, videoTestDataTmp);

        memset_s(videoTestDataTmp, videoTestDataLen_, 0, videoTestDataLen_);

        ret = fread(videoTestDataTmp, 1, fileLen, videoTestFp_);
        EXPECT_EQ(true, ret > 0);

        ::fclose(videoTestFp_);
        videoTestData_ = std::make_shared<DataBuffer>(videoTestDataLen_);
        videoTestData_->Assign(videoTestDataTmp, fileLen);
        videoTestFp_ = nullptr;
        delete[] videoTestDataTmp;
    }

    void VideoTestDataUnLoad(void)
    {
        if (nullptr != videoTestFp_) {
            ::fclose(videoTestFp_);
            videoTestFp_ = nullptr;
        }
        if (nullptr != videoTestData_) {
            videoTestData_ = nullptr;
        }
        videoTestDataLen_ = 0;
    }

    void InitWindow(void)
    {
        EXPECT_EQ(nullptr, windowOption_);
        EXPECT_EQ(nullptr, window_);
        EXPECT_EQ(nullptr, surfaceNode_);
        EXPECT_EQ(nullptr, surface_);

        windowOption_ = new Rosen::WindowOption();
        windowOption_->SetWindowRect({
            defaultPositionX,
            defaultPositionY,
            defaultWidth,
            defaultHeight});
        windowOption_->SetWindowType(
            Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
        windowOption_->SetWindowMode(
            Rosen::WindowMode::WINDOW_MODE_FULLSCREEN);
        window_ = Rosen::Window::Create(
            "Video Controller Test Window",
            windowOption_);

        surfaceNode_ = window_->GetSurfaceNode();
        surfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE);
        Rosen::RSTransaction::FlushImplicitTransaction();

        surface_ = surfaceNode_->GetSurface();
        window_->SetRequestedOrientation(Rosen::Orientation::HORIZONTAL);
        window_->Show();
        surfaceId_ = surface_->GetUniqueId();

        int32_t ret = SurfaceUtils::GetInstance()->Add(surfaceId_, surface_);
        EXPECT_EQ(ret, 0);
    }

    void InitMediaChannel(void)
    {
        if (nullptr != mediaChannel_) {
            EXPECT_EQ(nullptr, mediaChannel_);
            return;
        }
        mediaChannel_ = std::make_shared<MediaChannel>();
        EXPECT_NE(nullptr, mediaChannel_);

        std::string consumerClassName("MediaplayerTestConsumer");
        mediaChannel_->CreateConsumer(consumerClassName);
    }

//Audio
private:
    AudioTrack audioTrack_;

public:
    AudioTrack& GetAudioTrack(void)
    {
        return audioTrack_;
    }

    void AudioTrackReset(void)
    {
        audioTrack_.codecId = CodecId::CODEC_AAC;
        audioTrack_.sampleRate = defaultSampleRate;
        audioTrack_.sampleBit = DEFAULT_SAMPLE_BIT;
        audioTrack_.channels = defaultChannels;
    }

public:
    MediaControllerTestBaseImpl()
    {
        Config::GetInstance().Init();
        dispatcher_ = std::make_shared<BufferDispatcher>(maxBufferCapacity, maxBufferCapacityIncrement);
        VideoTestDataLoad();
        VideoTrackReset();
        AudioTrackReset();
        InitWindow();
    }

    ~MediaControllerTestBaseImpl() override
    {
        PlaybackStop();
        VideoTestDataUnLoad();
        BufferDispatcherReset();
    }

public:
    static constexpr int32_t maxBufferCapacity = ::MAX_BUFFER_CAPACITY;
    static constexpr int32_t maxBufferCapacityIncrement = ::BUFFER_CAPACITY_INCREMENT;
    static constexpr int32_t stabilityLoops = 100;
    static constexpr int32_t defaultPositionX = 0;
    static constexpr int32_t defaultPositionY = 0;
    static constexpr int32_t defaultHeight = 720;
    static constexpr int32_t defaultWidth = 1280;
    static constexpr int32_t defaultFrameRate = 30;
    static constexpr int32_t defaultChannels = 2;
    static constexpr int32_t defaultSampleRate = 48000;
    static constexpr int32_t defaultBitsPerSample = 16;
    static constexpr float defaultVolume = 1.0;
};

class MediaControllerUnitTest : public testing::Test {};

namespace {
MediaControllerTestBaseImpl& g_testBase =
    MediaControllerTestBaseImpl::GetInstance();

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Create_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<ContextEventMsg> cem = std::make_shared<ContextEventMsg>();
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_SetMediaChannel_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Init_01,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, false);

    g_testBase.VideoTrackReset();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Init_02,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    g_testBase.GetVideoTrack().codecId = CodecId::CODEC_NONE;
    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_NONE;
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, false);

    g_testBase.VideoTrackReset();
    g_testBase.AudioTrackReset();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Init_03,
         Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    g_testBase.GetAudioTrack().codecId = CodecId::CODEC_H264;
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, false);

    g_testBase.AudioTrackReset();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Play_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Start();
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);

    g_testBase.VideoDataPlayOnce();
    mediaController->Stop();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Start_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Start();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Start_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->Start();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Start_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Release();
    mediaController->Start();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Start_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Start();

    mediaController->Stop();
    mediaController->Start();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Start_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Start();
    mediaController->Start();
    mediaController->Stop();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Start_06, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    mediaController->Start();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_AppendSurface_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_AppendSurface_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::BACKGROUND);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_AppendSurface_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->videoTrack_.codecId = CodecId::CODEC_NONE;
    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);

    g_testBase.VideoTrackReset();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_AppendSurface_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_AppendSurface_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Start();

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_AppendSurface_06, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->videoTrack_.codecId = CodecId::CODEC_NONE;
    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_RemoveSurface_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);

    mediaController->RemoveSurface(g_testBase.GetSurface()->GetUniqueId());
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_SetVolume_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->SetVolume(MediaControllerTestBaseImpl::defaultVolume);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_SetKeyMode_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);

    mediaController->SetKeyMode(g_testBase.GetSurface()->GetUniqueId(), false);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_SetKeyRedirect_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);

    mediaController->SetKeyRedirect(g_testBase.GetSurface()->GetUniqueId(), false);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_OnPlayControllerNotify_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    ret = mediaController->AppendSurface(g_testBase.GetSurface(), SceneType::FOREGROUND);
    EXPECT_EQ(ret, true);

    statusMsg->errorCode = ERR_OK;
    statusMsg->prosumerId = g_testBase.GetId();
    statusMsg->agentId = g_testBase.GetId();
    mediaController->OnPlayControllerNotify(statusMsg);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_OnPlayControllerNotify_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    ProsumerStatusMsg::Ptr statusMsg = std::make_shared<ProsumerStatusMsg>();
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    statusMsg->errorCode = ERR_OK;
    statusMsg->prosumerId = g_testBase.GetId();
    mediaController->OnPlayControllerNotify(statusMsg);
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Stop_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Start();
    EXPECT_EQ(ret, true);

    mediaController->Stop();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Stop_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Stop();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Stop_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    mediaController->Stop();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Stop_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Release();

    mediaController->Stop();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Stop_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Start();
    mediaController->Stop();
    mediaController->Start();
    mediaController->Stop();
}

HWTEST_F(MediaControllerUnitTest, Media_Controller_Test_Release_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = true;
    std::unique_ptr<MediaController> mediaController =
        std::make_unique<MediaController>(g_testBase.GetId());
    EXPECT_NE(nullptr, mediaController);

    mediaController->SetMediaChannel(g_testBase.GetMediaChannel());
    ret = mediaController->Init(g_testBase.GetAudioTrack(), g_testBase.GetVideoTrack());
    EXPECT_EQ(ret, true);

    mediaController->Release();
}

} // namespace
} // namespace Sharing
} // namespace OHOS