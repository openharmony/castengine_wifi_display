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
#include "codec_factory.h"
#include "audio_encoder.h"
#include "audio_decoder.h"
#include "video_encoder.h"
#include "video_decoder.h"

namespace OHOS {
namespace Sharing {

class CodecFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CodecFactoryTest, CreateAudioEncoder_G711A)
{
    auto encoder = CodecFactory::CreateAudioEncoder(CODEC_G711A);
    ASSERT_NE(encoder, nullptr);
    EXPECT_EQ(encoder->inited_, false);
}

TEST_F(CodecFactoryTest, CreateAudioEncoder_G711U)
{
    auto encoder = CodecFactory::CreateAudioEncoder(CODEC_G711U);
    ASSERT_NE(encoder, nullptr);
    EXPECT_EQ(encoder->inited_, false);
}

TEST_F(CodecFactoryTest, CreateAudioEncoder_AAC)
{
    auto encoder = CodecFactory::CreateAudioEncoder(CODEC_AAC);
    ASSERT_NE(encoder, nullptr);
    EXPECT_EQ(encoder->inited_, false);
}

TEST_F(CodecFactoryTest, CreateAudioEncoder_PCM)
{
    auto encoder = CodecFactory::CreateAudioEncoder(CODEC_PCM);
    ASSERT_NE(encoder, nullptr);
    EXPECT_EQ(encoder->inited_, false);
}

TEST_F(CodecFactoryTest, CreateAudioEncoder_InvalidFormat)
{
    auto encoder = CodecFactory::CreateAudioEncoder(CODEC_H264);
    EXPECT_EQ(encoder, nullptr);
}

TEST_F(CodecFactoryTest, CreateAudioDecoder_G711A)
{
    auto decoder = CodecFactory::CreateAudioDecoder(CODEC_G711A);
    ASSERT_NE(decoder, nullptr);
}

TEST_F(CodecFactoryTest, CreateAudioDecoder_G711U)
{
    auto decoder = CodecFactory::CreateAudioDecoder(CODEC_G711U);
    ASSERT_NE(decoder, nullptr);
}

TEST_F(CodecFactoryTest, CreateAudioDecoder_AAC)
{
    auto decoder = CodecFactory::CreateAudioDecoder(CODEC_AAC);
    ASSERT_NE(decoder, nullptr);
}

TEST_F(CodecFactoryTest, CreateAudioDecoder_InvalidFormat)
{
    auto decoder = CodecFactory::CreateAudioDecoder(CODEC_H264);
    EXPECT_EQ(decoder, nullptr);
}

TEST_F(CodecFactoryTest, CreateVideoEncoder_NotSupported)
{
    auto encoder = CodecFactory::CreateVideoEncoder(CODEC_H264);
    EXPECT_EQ(encoder, nullptr);
}

TEST_F(CodecFactoryTest, CreateVideoDecoder_NotSupported)
{
    auto decoder = CodecFactory::CreateVideoDecoder(CODEC_H264);
    EXPECT_EQ(decoder, nullptr);
}

} // namespace Sharing
} // namespace OHOS
