/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_VIDEO_SOURCE_SCREEN_H
#define OHOS_SHARING_VIDEO_SOURCE_SCREEN_H

#include <surface.h>
#include <thread>
#include "dm_common.h"
#include "screen_manager.h"
#include "sync_fence.h"
#include "timer.h"
#include "video_source_encoder.h"
namespace OHOS {
namespace Sharing {

class VideoSourceScreen : public std::enable_shared_from_this<VideoSourceScreen> {
public:
    class ScreenGroupListener : public Rosen::ScreenManager::IScreenGroupListener {
    public:
        void OnChange(const std::vector<uint64_t> &screenIds, Rosen::ScreenGroupChangeEvent event) override;
    };

    explicit VideoSourceScreen(sptr<OHOS::Surface> encoderSurface) : encoderSurface_(encoderSurface) {}
    ~VideoSourceScreen();

    int32_t ReleaseScreenBuffer() const;
    int32_t InitScreenSource(const VideoSourceConfigure &configure);

    void StopScreenSourceCapture();
    void StartScreenSourceCapture();

private:
    void RemoveScreenFromGroup() const;
    void FrameRateControlTimerWorker();

    int32_t DestroyVirtualScreen() const;
    int32_t RegisterScreenGroupListener();
    int32_t UnregisterScreenGroupListener() const;
    int32_t SetEncoderSurface(sptr<OHOS::Surface> surface);

    uint64_t CreateVirtualScreen(const VideoSourceConfigure &configure);

private:
    uint64_t screenId_ = SCREEN_ID_INVALID;
    uint64_t srcScreenId_ = SCREEN_ID_INVALID;

    OHOS::sptr<OHOS::Surface> encoderSurface_ = nullptr;
    OHOS::sptr<ScreenGroupListener> screenGroupListener_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
