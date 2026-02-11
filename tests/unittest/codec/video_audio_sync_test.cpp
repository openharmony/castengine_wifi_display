/*
 * Copyright (c) 2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <memory>
#include "video_audio_sync.h"

namespace OHOS {
namespace Sharing {

class MockAudioPlayController : public AudioPlayController {
public:
    MockAudioPlayController() = default;
    ~MockAudioPlayController() override = default;

    int64_t GetAudioDecoderTimestamp() override
    {
        return audioTimestamp_;
    }

    void DropOneFrame() override
    {
        dropFrameCount_++;
    }

    void SetAudioTimestamp(int64_t timestamp)
    {
        audioTimestamp_ = timestamp;
    }

    int32_t GetDropFrameCount() const
    {
        return dropFrameCount_;
    }

private:
    int64_t audioTimestamp_ = 0;
    int32_t dropFrameCount_ = 0;
};

class VideoAudioSyncTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        videoAudioSync_ = std::make_shared<VideoAudioSync>();
        audioPlayController_ = std::make_shared<MockAudioPlayController>();
        videoAudioSync_->SetAudioPlayController(audioPlayController_);
    }

    void TearDown() override {}

    std::shared_ptr<VideoAudioSync> videoAudioSync_;
    std::shared_ptr<MockAudioPlayController> audioPlayController_;
};

TEST_F(VideoAudioSyncTest, CreateVideoAudioSync)
{
    auto sync = std::make_shared<VideoAudioSync>();
    ASSERT_NE(sync, nullptr);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_FirstFrame)
{
    bool result = videoAudioSync_->IsNeedDrop(1000);
    EXPECT_EQ(result, false);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_AudioTimestampZero)
{
    audioPlayController_->SetAudioTimestamp(0);
    bool result = videoAudioSync_->IsNeedDrop(1000);
    EXPECT_EQ(result, false);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_VideoTooLate)
{
    videoAudioSync_->IsNeedDrop(1000);
    audioPlayController_->SetAudioTimestamp(300000);
    bool result = videoAudioSync_->IsNeedDrop(0);
    EXPECT_EQ(result, false);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_VideoLate)
{
    videoAudioSync_->IsNeedDrop(1000);
    audioPlayController_->SetAudioTimestamp(150000);
    bool result = videoAudioSync_->IsNeedDrop(0);
    EXPECT_EQ(result, true);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_AudioLate)
{
    videoAudioSync_->IsNeedDrop(1000);
    audioPlayController_->SetAudioTimestamp(0);
    bool result = videoAudioSync_->IsNeedDrop(150000);
    EXPECT_EQ(result, false);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_AudioTooLate)
{
    videoAudioSync_->IsNeedDrop(1000);
    audioPlayController_->SetAudioTimestamp(0);
    bool result = videoAudioSync_->IsNeedDrop(400000);
    EXPECT_EQ(result, false);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_Synced)
{
    videoAudioSync_->IsNeedDrop(1000);
    audioPlayController_->SetAudioTimestamp(50000);
    bool result = videoAudioSync_->IsNeedDrop(55000);
    EXPECT_EQ(result, false);
}

TEST_F(VideoAudioSyncTest, SetAudioPlayController_Null)
{
    auto sync = std::make_shared<VideoAudioSync>();
    sync->SetAudioPlayController(nullptr);
    bool result = sync->IsNeedDrop(1000);
    EXPECT_EQ(result, false);
}

TEST_F(VideoAudioSyncTest, IsNeedDrop_ContinueDropCount)
{
    videoAudioSync_->IsNeedDrop(1000);
    audioPlayController_->SetAudioTimestamp(150000);
    videoAudioSync_->IsNeedDrop(0);
    bool result = videoAudioSync_->IsNeedDrop(0);
    EXPECT_EQ(result, false);
}

} // namespace Sharing
} // namespace OHOS
