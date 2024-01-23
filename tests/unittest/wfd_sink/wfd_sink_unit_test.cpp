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

#include "windowmgr/include/window_manager.h"
#include "common/const_def.h"
#include "wfd_sink_unit_test.h"
#include <gtest/gtest.h>
#include <memory>
#include "common/sharing_log.h"
#include "common/const_def.h"
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Sharing;
void WfdSinkUnitTest::SetUpTestCase() {}
void WfdSinkUnitTest::TearDownTestCase() {}
void WfdSinkUnitTest::SetUp() 
{
    wfdSink_ = WfdSinkFactory::CreateSink(0,"wfd_sink_demo");
    ASSERT_NE(nullptr, wfdSink_);
    ASSERT_EQ(SharingErrorCode::ERR_OK, wfdSink_->Start());
}
void WfdSinkUnitTest::TearDown()
{
    if (wfdSink_ != nullptr) {
        ASSERT_EQ(SharingErrorCode::ERR_OK, wfdSink_->Stop());
    }
}
/**
 * @tc.name: wfd_sink_SetSurface_0100
 * @tc.desc: wfd sink SetSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_SetSurface_0100, TestSize.Level0)
{
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->AppendSurface("0", 0));
    WindowProperty::Ptr windowPropertyPtr = std::make_shared<WindowProperty>();
    windowPropertyPtr->height = 1920;
    windowPropertyPtr->width = 1080;
    windowPropertyPtr->startX = 0;
    windowPropertyPtr->startY = 0;
    windowPropertyPtr->isFull = false;
    windowPropertyPtr->isShow = true;
    int32_t windowId = WindowManager::GetInstance().CreateWindow(windowPropertyPtr);
    sptr<Surface> surface = WindowManager::GetInstance().GetSurface(windowId);
    ASSERT_NE(nullptr, surface);
    if (surface) {
        uint64_t surfaceId = surface->GetUniqueId();
        EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->AppendSurface("0", surfaceId));
    }
}
/**
 * @tc.name: wfd_sink_SetMediaFormat_0100
 * @tc.desc: wfd sink SetMediaFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_SetMediaFormat_0100, TestSize.Level0)
{
    CodecAttr videoAttr;
    videoAttr.codecType = CodecId::CODEC_H264;
    videoAttr.formatId = VideoFormat::VIDEO_1920X1080_30;
    CodecAttr audioAttr;
    audioAttr.codecType = CodecId::CODEC_AAC;
    audioAttr.formatId = AudioFormat::AUDIO_48000_16_2;
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->SetMediaFormat("0", videoAttr, audioAttr));
}
/**
 * @tc.name: wfd_sink_play_0100
 * @tc.desc: wfd sink play
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_play_0100, TestSize.Level0)
{
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->Play("0"));
}
/**
 * @tc.name: wfd_sink_pause_0100
 * @tc.desc: wfd sink pause
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_pause_0100, TestSize.Level0)
{
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->Pause("0"));
}
/**
 * @tc.name: wfd_sink_close_0100
 * @tc.desc: wfd sink close
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_close_0100, TestSize.Level0)
{
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->Close("0"));
}
/**
 * @tc.name: wfd_sink_set_scene_0100
 * @tc.desc: wfd sink set scene type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_set_scene_0100, TestSize.Level0)
{
    OHOS::Sharing::SceneType sceneType = OHOS::Sharing::SceneType::FOREGROUND;
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->SetSceneType("0", 0, sceneType));
}
/**
 * @tc.name: wfd_sink_mute_0100
 * @tc.desc: wfd sink mute
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_mute_0100, TestSize.Level0)
{
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->Mute("0"));
}
/**
 * @tc.name: wfd_sink_unmute_0100
 * @tc.desc: wfd sink unmute
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_unmute_0100, TestSize.Level0)
{
    EXPECT_NE(SharingErrorCode::ERR_OK, wfdSink_->UnMute("0"));
}
/**
 * @tc.name: wfd_sink_GetSinkConfig_0100
 * @tc.desc: wfd sink GetSinkConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WfdSinkUnitTest, wfd_sink_GetSinkConfig_0100, TestSize.Level0)
{
    SinkConfig sinkConfig;
    EXPECT_EQ(SharingErrorCode::ERR_OK, wfdSink_->GetSinkConfig(sinkConfig));
    EXPECT_NE(0, sinkConfig.foregroundMaximum);
    EXPECT_NE(0, sinkConfig.surfaceMaximum);
}
