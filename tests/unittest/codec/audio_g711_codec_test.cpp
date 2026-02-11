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
#include "audio_g711_codec.h"
#include "media_frame_pipeline.h"

namespace OHOS {
namespace Sharing {

class AudioG711CodecTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AudioG711CodecTest, CreateG711Encoder_ALAW)
{
    AudioG711Encoder encoder(G711_TYPE::G711_ALAW);
    EXPECT_EQ(encoder.inited_, false);
}

TEST_F(AudioG711CodecTest, CreateG711Encoder_ULAW)
{
    AudioG711Encoder encoder(G711_TYPE::G711_ULAW);
    EXPECT_EQ(encoder.inited_, false);
}

TEST_F(AudioG711CodecTest, InitG711Encoder_DefaultParams)
{
    AudioG711Encoder encoder(G711_TYPE::G711_ALAW);
    int result = encoder.Init();
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoder.inited_, true);
}

TEST_F(AudioG711CodecTest, InitG711Encoder_CustomParams)
{
    AudioG711Encoder encoder(G711_TYPE::G711_ULAW);
    int result = encoder.Init(1, 16, 8000);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(encoder.inited_, true);
}

TEST_F(AudioG711CodecTest, CreateG711Decoder_ALAW)
{
    AudioG711Decoder decoder(G711_TYPE::G711_ALAW);
}

TEST_F(AudioG711CodecTest, CreateG711Decoder_ULAW)
{
    AudioG711Decoder decoder(G711_TYPE::G711_ULAW);
}

TEST_F(AudioG711CodecTest, OnFrame_NullFrame)
{
    AudioG711Encoder encoder(G711_TYPE::G711_ALAW);
    encoder.Init();
    encoder.OnFrame(nullptr);
}

TEST_F(AudioG711CodecTest, OnFrame_Decoder_NullFrame)
{
    AudioG711Decoder decoder(G711_TYPE::G711_ALAW);
    AudioTrack track;
    decoder.Init(track);
    decoder.OnFrame(nullptr);
}

} // namespace Sharing
} // namespace OHOS
