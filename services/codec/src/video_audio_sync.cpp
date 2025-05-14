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

#include "video_audio_sync.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {

VideoAudioSync::VideoAudioSync()
{
    SHARING_LOGD("Construct in");
}

VideoAudioSync::~VideoAudioSync()
{
    SHARING_LOGD("Destruct in");
    audioPlayController_.reset();
}

bool VideoAudioSync::IsNeedDrop(int64_t videoTimestamp)
{
    if (isFirstFrame_) {
        isFirstFrame_ = false;
        continueDropCount_ = 0;
        return false;
    }
    return ProcessAVSyncStrategy(videoTimestamp);
}

bool VideoAudioSync::ProcessAVSyncStrategy(int64_t videoTimestamp)
{
    int64_t audioPts = 0;
    if (nullptr != audioPlayController_) {
        audioPts = audioPlayController_->GetAudioDecoderTimestamp();
    }

    if (audioPts == 0) {
        continueDropCount_ = 0;
        return false;
    }

    int64_t earlyUs = videoTimestamp - audioPts;
    if (earlyUs < VIDEO_TOO_LATE_US) {
        SHARING_LOGE("Video is too late, something may wrong!");
    } else if (earlyUs < VIDEO_LATE_US && continueDropCount_ <= 0) {
        ++continueDropCount_;
        return true;
    } else if (earlyUs > AUDIO_TOO_LATE_US) {
        SHARING_LOGE("Audio is too late, something may wrong!");
        std::this_thread::sleep_for(std::chrono::microseconds(DROP_ONE_FRAME_TIME));
    } else if (earlyUs > AUDIO_LATE_US) {
        audioPlayController_->DropOneFrame();
    }

    continueDropCount_ = 0;
    return false;
}

void VideoAudioSync::SetAudioPlayController(std::shared_ptr<AudioPlayController> audioPlayController)
{
    audioPlayController_ = audioPlayController;
}

} // namespace Sharing
} // namespace OHOS