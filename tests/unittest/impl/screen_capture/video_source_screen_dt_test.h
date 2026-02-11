/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef OHOS_SHARING_VIDEO_SOURCE_SCREEN_DT_TEST_H
#define OHOS_SHARING_VIDEO_SOURCE_SCREEN_DT_TEST_H

#include <gtest/gtest.h>
#include "surface.h"
#include "surface.h"
#include "video_source_screen.h"
#include "video_source_encoder.h"

namespace OHOS {
namespace Sharing {
using namespace testing::ext;
using namespace OHOS::Rosen;

class VideoSourceScreenDTTest : public testing::Test {
protected:
    void SetUp() override
    {
        // 创建模拟的 Surface 用于编码器
        encoderSurface_ = Surface::CreateSurfaceAsConsumer();
        videoSourceScreen_ = std::make_shared<VideoSourceScreen>(encoderSurface_);
    }

    void TearDown() override
    {
        if (videoSourceScreen_) {
            videoSourceScreen_->StopScreenSourceCapture();
        }
        encoderSurface_ = nullptr;
        videoSourceScreen_ = nullptr;
    }

    sptr<Surface> encoderSurface_;
    std::shared_ptr<VideoSourceScreen> videoSourceScreen_;
};

} // namespace Sharing
} // namespace OHOS

#endif // OHOS_SHARING_VIDEO_SOURCE_SCREEN_DT_TEST_H
