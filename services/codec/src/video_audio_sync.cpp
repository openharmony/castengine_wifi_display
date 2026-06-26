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
    if (audioPlayController_) {
        audioPlayController_.reset();
    }
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
    if (audioPlayController_ == nullptr) {
        return false;
    }

    audioPts = audioPlayController_->GetAudioDecoderTimestamp();
    if (audioPts == 0) {
        continueDropCount_ = 0;
        return false;
    }

    int64_t earlyUs = videoTimestamp - audioPts;
    SHARING_LOGD("videoTimestamp: %{public}" PRId64 "; audioPts: %{public}" PRId64 "; earlyUs: %{public}" PRId64 "",
                 videoTimestamp, audioPts, earlyUs);
    if (earlyUs < VIDEO_TOO_LATE_US) {
        SHARING_LOGE("Video is too late, something may wrong! earlyUs = %{public}" PRId64"", earlyUs);
        ++videoTooLateConsecutiveCount_;
        if (videoTooLateConsecutiveCount_ >= CONSECUTIVE_THRESHOLD) {
            ++videoTooLateCount_;
            videoTooLateConsecutiveCount_ = 0;
            SHARING_LOGE("Video is too late consecutive %{public}d times", videoTooLateCount_);
        }
    } else if (earlyUs < VIDEO_LATE_US && continueDropCount_ <= 0) {
        ++continueDropCount_;
        ++videoDropFrameCount_;
        return true;
    } else if (earlyUs > AUDIO_LATE_US) {
        SHARING_LOGE("Audio is too late, drop audio fram! earlyUs =  %{public}" PRId64 "", earlyUs);
        ++audioTooLateConsecutiveCount_;
        if (audioTooLateConsecutiveCount_ >= CONSECUTIVE_THRESHOLD) {
            ++audioTooLateCount_;
            audioTooLateConsecutiveCount_ = 0;
            SHARING_LOGE("Audio is too late consecutive %{public}d times", audioTooLateCount_);
        }
        audioPlayController_->DropOneFrame();
    } else {
        videoTooLateConsecutiveCount_ = 0;
        audioTooLateConsecutiveCount_ = 0;
    }

    continueDropCount_ = 0;
    return false;
}

void VideoAudioSync::SetAudioPlayController(std::shared_ptr<AudioPlayController> audioPlayController)
{
    audioPlayController_ = audioPlayController;
}

void VideoAudioSync::GetAVSyncExceptionCount(uint32_t &videoTooLateCount, uint32_t &audioTooLateCount,
                                             uint32_t &videoDropFrameCount)
{
    videoTooLateCount = videoTooLateCount_;
    audioTooLateCount = audioTooLateCount_;
    videoDropFrameCount = videoDropFrameCount_;
}

void VideoAudioSync::ResetAVSyncExceptionCount()
{
    videoTooLateCount_ = 0;
    audioTooLateCount_ = 0;
    videoDropFrameCount_ = 0;
    videoTooLateConsecutiveCount_ = 0;
    audioTooLateConsecutiveCount_ = 0;
}

} // namespace Sharing
} // namespace OHOS