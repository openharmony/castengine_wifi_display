/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <memory>
#include "audio_pcm_processor.h"
#include "media_frame_pipeline.h"

namespace OHOS {
namespace Sharing {

class AudioPcmProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AudioPcmProcessorTest, CreatePcmProcessor)
{
    AudioPcmProcessor processor;
    EXPECT_EQ(processor.inited_, false);
}

TEST_F(AudioPcmProcessorTest, InitPcmProcessor_DefaultParams)
{
    AudioPcmProcessor processor;
    int32_t result = processor.Init();
    EXPECT_EQ(result, 0);
    EXPECT_EQ(processor.inited_, true);
}

TEST_F(AudioPcmProcessorTest, InitPcmProcessor_CustomParams)
{
    AudioPcmProcessor processor;
    int32_t result = processor.Init(2, 16, 44100);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(processor.inited_, true);
}

TEST_F(AudioPcmProcessorTest, InitPcmProcessor_Mono)
{
    AudioPcmProcessor processor;
    int32_t result = processor.Init(1, 16, 44100);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(processor.inited_, true);
}

TEST_F(AudioPcmProcessorTest, InitPcmProcessor_DifferentSampleRates)
{
    AudioPcmProcessor processor;
    int32_t result = processor.Init(2, 16, 48000);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(processor.inited_, true);
}

TEST_F(AudioPcmProcessorTest, OnFrame_NullFrame)
{
    AudioPcmProcessor processor;
    processor.Init();
    processor.OnFrame(nullptr);
}

TEST_F(AudioPcmProcessorTest, OnFrame_BeforeInit)
{
    AudioPcmProcessor processor;
    processor.OnFrame(nullptr);
}

} // namespace Sharing
} // namespace OHOS
