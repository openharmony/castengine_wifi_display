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
#include "media_frame_pipeline.h"
#include "frame.h"

namespace OHOS {
namespace Sharing {

class MockFrameDestination : public FrameDestination {
public:
    MockFrameDestination() = default;
    ~MockFrameDestination() override = default;

    void OnFrame(const Frame::Ptr &frame) override
    {
        frameCount_++;
        lastFrame_ = frame;
    }

    int32_t GetFrameCount() const
    {
        return frameCount_;
    }

    Frame::Ptr GetLastFrame() const
    {
        return lastFrame_;
    }

    void Reset()
    {
        frameCount_ = 0;
        lastFrame_ = nullptr;
    }

private:
    int32_t frameCount_ = 0;
    Frame::Ptr lastFrame_ = nullptr;
};

class MockFrameSource : public FrameSource {
public:
    MockFrameSource() = default;
    ~MockFrameSource() override = default;

    void DeliverTestFrame(const Frame::Ptr &frame)
    {
        DeliverFrame(frame);
    }
};

class MediaFramePipelineTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        frameSource_ = std::make_shared<MockFrameSource>();
        frameDestination_ = std::make_shared<MockFrameDestination>();
    }

    void TearDown() override {}

    std::shared_ptr<MockFrameSource> frameSource_;
    std::shared_ptr<MockFrameDestination> frameDestination_;
};

TEST_F(MediaFramePipelineTest, CreateFrameSource)
{
    auto source = std::make_shared<MockFrameSource>();
    ASSERT_NE(source, nullptr);
}

TEST_F(MediaFramePipelineTest, CreateFrameDestination)
{
    auto destination = std::make_shared<MockFrameDestination>();
    ASSERT_NE(destination, nullptr);
}

TEST_F(MediaFramePipelineTest, AddAudioDestination)
{
    frameSource_->AddAudioDestination(frameDestination_);
    EXPECT_TRUE(frameDestination_->HasAudioSource());
}

TEST_F(MediaFramePipelineTest, RemoveAudioDestination)
{
    frameSource_->AddAudioDestination(frameDestination_);
    frameSource_->RemoveAudioDestination(frameDestination_);
    EXPECT_FALSE(frameDestination_->HasAudioSource());
}

TEST_F(MediaFramePipelineTest, AddAudioDestination_Null)
{
    frameSource_->AddAudioDestination(nullptr);
}

TEST_F(MediaFramePipelineTest, RemoveAudioDestination_Null)
{
    frameSource_->RemoveAudioDestination(nullptr);
}

TEST_F(MediaFramePipelineTest, DeliverFrame_G711A)
{
    frameSource_->AddAudioDestination(frameDestination_);
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_G711A;
    frameSource_->DeliverTestFrame(frame);
    EXPECT_EQ(frameDestination_->GetFrameCount(), 1);
}

TEST_F(MediaFramePipelineTest, DeliverFrame_G711U)
{
    frameSource_->AddAudioDestination(frameDestination_);
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_G711U;
    frameSource_->DeliverTestFrame(frame);
    EXPECT_EQ(frameDestination_->GetFrameCount(), 1);
}

TEST_F(MediaFramePipelineTest, DeliverFrame_AAC)
{
    frameSource_->AddAudioDestination(frameDestination_);
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_AAC;
    frameSource_->DeliverTestFrame(frame);
    EXPECT_EQ(frameDestination_->GetFrameCount(), 1);
}

TEST_F(MediaFramePipelineTest, DeliverFrame_PCM)
{
    frameSource_->AddAudioDestination(frameDestination_);
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_PCM;
    frameSource_->DeliverTestFrame(frame);
    EXPECT_EQ(frameDestination_->GetFrameCount(), 1);
}

TEST_F(MediaFramePipelineTest, DeliverFrame_Null)
{
    frameSource_->AddAudioDestination(frameDestination_);
    frameSource_->DeliverTestFrame(nullptr);
    EXPECT_EQ(frameDestination_->GetFrameCount(), 0);
}

TEST_F(MediaFramePipelineTest, DeliverFrame_UnknownCodec)
{
    frameSource_->AddAudioDestination(frameDestination_);
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_NONE;
    frameSource_->DeliverTestFrame(frame);
    EXPECT_EQ(frameDestination_->GetFrameCount(), 0);
}

TEST_F(MediaFramePipelineTest, DeliverFrame_MultipleDestinations)
{
    auto destination2 = std::make_shared<MockFrameDestination>();
    frameSource_->AddAudioDestination(frameDestination_);
    frameSource_->AddAudioDestination(destination2);
    
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_PCM;
    frameSource_->DeliverTestFrame(frame);
    
    EXPECT_EQ(frameDestination_->GetFrameCount(), 1);
    EXPECT_EQ(destination2->GetFrameCount(), 1);
}

TEST_F(MediaFramePipelineTest, UnsetAudioSource)
{
    frameSource_->AddAudioDestination(frameDestination_);
    EXPECT_TRUE(frameDestination_->HasAudioSource());
    frameDestination_->UnsetAudioSource();
    EXPECT_FALSE(frameDestination_->HasAudioSource());
}

TEST_F(MediaFramePipelineTest, SetAudioSource)
{
    frameSource_->AddAudioDestination(frameDestination_);
    EXPECT_TRUE(frameDestination_->HasAudioSource());
}

} // namespace Sharing
} // namespace OHOS
