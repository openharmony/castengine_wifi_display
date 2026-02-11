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
#include <memory>
#include "audio_aac_codec.h"
#include "media_frame_pipeline.h"

namespace OHOS {
namespace Sharing {

class AudioAACCodecTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AudioAACCodecTest, CreateAACEncoder)
{
    AudioAACEncoder encoder;
    EXPECT_EQ(encoder.inited_, false);
}

TEST_F(AudioAACCodecTest, InitAEncoder_DefaultParams)
{
    AudioAACEncoder encoder;
    int32_t result = encoder.Init();
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoder.inited_, true);
}

TEST_F(AudioAACCodecTest, InitAEncoder_CustomParams)
{
    AudioAACEncoder encoder;
    int32_t result = encoder.Init(2, 16, 44100);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoder.inited_, true);
}

TEST_F(AudioAACCodecTest, InitAEncoder_Mono)
{
    AudioAACEncoder encoder;
    int32_t result = encoder.Init(1, 16, 44100);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoder.inited_, true);
}

TEST_F(AudioAACCodecTest, CreateAACDecoder)
{
    AudioAACDecoder decoder;
}

TEST_F(AudioAACCodecTest, InitAACDecoder)
{
    AudioAACDecoder decoder;
    AudioTrack track;
    track.codecId_ = CODEC_AAC;
    track.channels_ = 2;
    track.sampleRate_ = 44100;
    int32_t result = decoder.Init(track);
    EXPECT_EQ(result, 0);
}

TEST_F(AudioAACCodecTest, OnFrame_Encoder_NullFrame)
{
    AudioAACEncoder encoder;
    encoder.Init();
    encoder.OnFrame(nullptr);
}

TEST_F(AudioAACCodecTest, OnFrame_Decoder_NullFrame)
{
    AudioAACDecoder decoder;
    AudioTrack track;
    decoder.Init(track);
    decoder.OnFrame(nullptr);
}

} // namespace Sharing
} // namespace OHOS
