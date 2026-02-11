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
 * limitations under theater License.
 */

#include <gtest/gtest.h>
#include <memory>
#include "video_source_encoder.h"

namespace OHOS {
namespace Sharing {

class MockVideoSourceEncoderListener : public VideoSourceEncoderListener {
public:
    MockVideoSourceEncoderListener() = default;
    ~MockVideoSourceEncoderListener() override = default;

    void OnFrameBufferUsed() override
    {
        frameBufferUsedCount_++;
    }

    void OnFrame(const Frame::Ptr &frame, FRAME_TYPE frameType, bool keyFrame) override
    {
        frameCount_++;
        lastFrameType_ = frameType;
        lastKeyFrame_ = keyFrame;
    }

    int32_t GetFrameBufferUsedCount() const
    {
        return frameBufferUsedCount_;
    }

    int32_t GetFrameCount() const
    {
        return frameCount_;
    }

    FRAME_TYPE GetLastFrameType() const
    {
        return lastFrameType_;
    }

    bool GetLastKeyFrame() const
    {
        return lastKeyFrame_;
    }

    void Reset()
    {
        frameBufferUsedCount_ = 0;
        frameCount_ = 0;
        lastFrameType_ = SPS_FRAME;
        lastKeyFrame_ = false;
    }

private:
    int32_t frameBufferUsedCount_ = 0;
    int32_t frameCount_ = 0;
    FRAME_TYPE lastFrameType_ = SPS_FRAME;
    bool lastKeyFrame_ = false;
};

class VideoSourceEncoderTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        listener_ = std::make_shared<MockVideoSourceEncoderListener>();
        encoder_ = std::make_shared<VideoSourceEncoder>(listener_);
    }

    void TearDown() override {}

    std::shared_ptr<VideoSourceEncoder> encoder_;
    std::shared_ptr<MockVideoSourceEncoderListener> listener_;
};

TEST_F(VideoSourceEncoderTest, CreateVideoSourceEncoder)
{
    auto listener = std::make_shared<MockVideoSourceEncoderListener>();
    auto encoder = std::make_shared<VideoSourceEncoder>(listener);
    ASSERT_NE(encoder, nullptr);
}

TEST_F(VideoSourceEncoderTest, InitEncoder_H264)
{
    VideoSourceConfigure configure;
    configure.codecType_ = CODEC_H264;
    configure.videoWidth_ = 1920;
    configure.videoHeight_ = 1080;
    configure.frameRate_ = 30;
    bool result = encoder_->InitEncoder(configure);
}

TEST_F(VideoSourceEncoderTest, InitEncoder_H265)
{
    VideoSourceConfigure configure;
    configure.codecType_ = CODEC_H265;
    configure.videoWidth_ = 1920;
    configure.videoHeight_ = 1080;
    configure.frameRate_ = 30;
    bool result = encoder_->InitEncoder(configure);
}

TEST_F(VideoSourceEncoderTest, InitEncoder_DefaultParams)
{
    VideoSourceConfigure configure;
    bool result = encoder_->InitEncoder(configure);
}

TEST_F(VideoSourceEncoderTest, StartEncoder_BeforeInit)
{
    bool result = encoder_->StartEncoder();
    EXPECT_EQ(result, false);
}

TEST_F(VideoSourceEncoderTest, StopEncoder_BeforeInit)
{
    bool result = encoder_->StopEncoder();
    EXPECT_EQ(result, false);
}

TEST_F(VideoSourceEncoderTest, ReleaseEncoder_BeforeInit)
{
    bool result = encoder_->ReleaseEncoder();
    EXPECT_EQ(result, false);
}

TEST_F(VideoSourceEncoderTest, GetEncoderSurface_BeforeInit)
{
    sptr<Surface> &surface = encoder_->GetEncoderSurface();
}

TEST_F(VideoSourceEncoderTest, StartStopCycle)
{
    VideoSourceConfigure configure;
    configure.codecType_ = CODEC_H264;
    configure.videoWidth_ = 1920;
    configure.videoHeight_ = 1080;
    configure.frameRate_ = 30;
    
    if (encoder_->InitEncoder(configure)) {
        bool startResult = encoder_->StartEncoder();
        if (startResult) {
            encoder_->StopEncoder();
        }
    }
}

TEST_F(VideoSourceEncoderTest, MultipleInitCalls)
{
    VideoSourceConfigure configure;
    configure.codecType_ = CODEC_H264;
    configure.videoWidth_ = 1920;
    configure.videoHeight_ = 1080;
    configure.frameRate_ = 30;
    
    bool result1 = encoder_->InitEncoder(configure);
    bool result2 = encoder_->InitEncoder(configure);
}

TEST_F(VideoSourceEncoderTest, InitWithDifferentResolutions)
{
    VideoSourceConfigure configure1;
    configure1.codecType_ = CODEC_H264;
    configure1.videoWidth_ = 1280;
    configure1.videoHeight_ = 720;
    configure1.frameRate_ = 30;
    
    VideoSourceConfigure configure2;
    configure2.codecType_ = CODEC_H264;
    configure2.videoWidth_ = 3840;
    configure2.videoHeight_ = 2160;
    configure2.frameRate_ = 60;
    
    auto encoder1 = std::make_shared<VideoSourceEncoder>(listener_);
    auto encoder2 = std::make_shared<VideoSourceEncoder>(listener_);
    
    bool result1 = encoder1->InitEncoder(configure1);
    bool result2 = encoder2->InitEncoder(configure2);
}

TEST_F(VideoSourceEncoderTest, InitWithDifferentFrameRates)
{
    VideoSourceConfigure configure1;
    configure1.codecType_ = CODEC_H264;
    configure1.videoWidth_ = 1920;
    configure1.videoHeight_ = 1080;
    configure1.frameRate_ = 15;
    
    VideoSourceConfigure configure2;
    configure2.codecType_ = CODEC_H264;
    configure2.videoWidth_ = 1920;
    configure2.videoHeight_ = 1080;
    configure2.frameRate_ = 60;
    
    auto encoder1 = std::make_shared<VideoSourceEncoder>(listener_);
    auto encoder2 = std::make_shared<VideoSourceEncoder>(listener_);
    
    bool result1 = encoder1->InitEncoder(configure1);
    bool result2 = encoder2->InitEncoder(configure2);
}

TEST_F(VideoSourceEncoderTest, ReleaseAndReinit)
{
    VideoSourceConfigure configure;
    configure.codecType_ = CODEC_H264;
    configure.videoWidth_ = 1920;
    configure.videoHeight_ = 1080;
    configure.frameRate_ = 30;
    
    if (encoder_->InitEncoder(configure)) {
        encoder_->ReleaseEncoder();
        bool result = encoder_->InitEncoder(configure);
    }
}

TEST_F(VideoSourceEncoderTest, InitEncoder_InvalidCodecType)
{
    VideoSourceConfigure configure;
    configure.codecType_ = CODEC_NONE;
    configure.videoWidth_ = 1920;
    configure.videoHeight_ = 1080;
    configure.frameRate_ = 30;
    bool result = encoder_->InitEncoder(configure);
    EXPECT_EQ(result, false);
}

TEST_F(VideoSourceEncoderTest, VideoSourceConfigure_DefaultValues)
{
    VideoSourceConfigure configure;
    EXPECT_EQ(configure.screenWidth_, DEFAULT_VIDEO_WIDTH);
    EXPECT_EQ(configure.screenHeight_, DEFAULT_VIDEO_HEIGHT);
    EXPECT_EQ(configure.videoWidth_, DEFAULT_VIDEO_WIDTH);
    EXPECT_EQ(configure.videoHeight_, DEFAULT_VIDEO_HEIGHT);
    EXPECT_EQ(configure.frameRate_, DEFAULT_FRAME_RATE);
    EXPECT_EQ(configure.codecType_, CODEC_H264);
}

} // namespace Sharing
} // namespace OHOS
