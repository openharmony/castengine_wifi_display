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

#include "media_frame_pipeline.h"
#include "common/common_macro.h"
#include "frame.h"
#include "media_log.h"

namespace OHOS {
namespace Sharing {
inline bool IsAudioFrame(const Frame::Ptr &frame)
{
    RETURN_FALSE_IF_NULL(frame);
    return frame->GetCodecId() == CODEC_G711A || frame->GetCodecId() == CODEC_G711U ||
        frame->GetCodecId() == CODEC_AAC || frame->GetCodecId() == CODEC_PCM;
}

FrameSource::~FrameSource()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> alock(audioDstsMutex_);
    for (auto &audioDst : audioDsts_) {
        audioDst->UnsetAudioSource();
    }
    alock.unlock();
    audioDsts_.clear();
};

void FrameSource::AddAudioDestination(std::shared_ptr<FrameDestination> dst)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(dst);
    std::unique_lock<std::shared_mutex> lock(audioDstsMutex_);
    audioDsts_.push_back(dst);
    lock.unlock();
    dst->SetAudioSource(shared_from_this());
}

void FrameSource::RemoveAudioDestination(std::shared_ptr<FrameDestination> dst)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(dst);
    std::unique_lock<std::shared_mutex> lock(audioDstsMutex_);
    audioDsts_.remove(dst);
    lock.unlock();
    dst->UnsetAudioSource();
}

void FrameSource::DeliverFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    if (IsAudioFrame(frame)) {
        std::shared_lock<std::shared_mutex> lock(audioDstsMutex_);
        for (auto &audioDst_ : audioDsts_) {
            audioDst_->OnFrame(frame);
        }
    } else {
        SHARING_LOGE("unknown frame Type!");
    }
}

void FrameDestination::SetAudioSource(std::shared_ptr<FrameSource> src)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> lock(audioSrcMutex_);
    audioSrc_ = src;
}

void FrameDestination::UnsetAudioSource()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> lock(audioSrcMutex_);
    audioSrc_.reset();
}
} // namespace Sharing
} // namespace OHOS