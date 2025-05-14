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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_VIDEO_AUDIO_SYNC_H
#define OHOS_SHARING_VIDEO_AUDIO_SYNC_H

#include <memory>
#include "audio_play_controller.h"

namespace OHOS {
namespace Sharing {

class VideoAudioSync : public std::enable_shared_from_this<VideoAudioSync> {
public:
    VideoAudioSync();
    ~VideoAudioSync();

    bool IsNeedDrop(int64_t videoTimestamp);
    void SetAudioPlayController(std::shared_ptr<AudioPlayController> audioPlayController);

private:
    bool ProcessAVSyncStrategy(int64_t videoTimestamp);

    static constexpr int64_t AUDIO_TOO_LATE_US = 300 * 1000;
    static constexpr int64_t AUDIO_LATE_US = 100 * 1000;
    static constexpr int64_t VIDEO_LATE_US = -100 * 1000;
    static constexpr int64_t VIDEO_TOO_LATE_US = -200 * 1000;
    static constexpr int64_t DROP_ONE_FRAME_TIME = 1000 * 1000 / 30;

    bool isFirstFrame_ = true;
    int32_t continueDropCount_ = 0;
    std::shared_ptr<AudioPlayController> audioPlayController_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif