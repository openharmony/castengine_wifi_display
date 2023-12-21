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

#include "audio_play_controller.h"
#include <chrono>
#include "common/common_macro.h"
#include "common/const_def.h"
#include "common/event_comm.h"
#include "common/media_log.h"
#include "magic_enum.hpp"
#include "utils/data_buffer.h"

namespace OHOS {
namespace Sharing {

AudioPlayController::AudioPlayController(uint32_t mediaChannelId)
{
    SHARING_LOGD("trace.");
    bufferReceiver_ = std::make_shared<BufferReceiver>();
    mediachannelId_ = mediaChannelId;
}

AudioPlayController::~AudioPlayController()
{
    SHARING_LOGD("delete audio play controller, mediachannelId: %{public}u.", mediachannelId_);
}

bool AudioPlayController::Init(AudioTrack &audioTrack)
{
    SHARING_LOGD("trace.");
    if (CODEC_NONE == audioTrack.codecId) {
        SHARING_LOGW("no need to play.");
        return false;
    }

    audioPlayer_ = std::make_shared<AudioPlayer>();
    if (!audioPlayer_->Init(audioTrack.codecId) ||
        !audioPlayer_->SetAudioFormat(audioTrack.channels, audioTrack.sampleRate)) {
        SHARING_LOGE("audio play init error.");
        return false;
    }

    return true;
}

bool AudioPlayController::Start(BufferDispatcher::Ptr &dispatcher)
{
    SHARING_LOGD("trace.");
    if (nullptr == audioPlayer_) {
        SHARING_LOGE("audio play need init first.");
        return false;
    }

    if (isAudioRunning_) {
        SHARING_LOGW("audio play is running.");
        return true;
    }

    if (audioPlayer_->Start()) {
        isAudioRunning_ = true;
        dispatcher->AttachReceiver(bufferReceiver_);
        StartAudioThread();
    } else {
        return false;
    }

    return true;
}

void AudioPlayController::Stop(BufferDispatcher::Ptr &dispatcher)
{
    SHARING_LOGD("trace.");
    if (dispatcher && bufferReceiver_) {
        dispatcher->DetachReceiver(bufferReceiver_);
    }

    if (audioPlayer_) {
        audioPlayer_->Stop();
        isAudioRunning_ = false;
        if (bufferReceiver_) {
            bufferReceiver_->NotifyReadStop();
        }
        StopAudioThread();
    }
}

void AudioPlayController::Release()
{
    SHARING_LOGD("trace.");
    if (nullptr != audioPlayer_) {
        audioPlayer_->Release();
        audioPlayer_.reset();
    }
}

void AudioPlayController::StartAudioThread()
{
    SHARING_LOGD("trace.");
    if (audioPlayThread_ != nullptr) {
        SHARING_LOGD("thread already exist, mediachannelId: %{public}u.", mediachannelId_);
        return;
    }

    audioPlayThread_ = std::make_shared<std::thread>(&AudioPlayController::AudioPlayThread, this);
    std::string name = "audioplay";
    pthread_setname_np(audioPlayThread_->native_handle(), name.c_str());
    if (audioPlayThread_ == nullptr) {
        SHARING_LOGE("play start create thread error, mediachannelId: %{public}u!", mediachannelId_);
    }
}

void AudioPlayController::StopAudioThread()
{
    SHARING_LOGD("trace.");
    if (audioPlayThread_) {
        if (audioPlayThread_->joinable()) {
            audioPlayThread_->join();
        }
        audioPlayThread_ = nullptr;
    }

    SHARING_LOGD("stop audio thread exit, mediachannelId: %{public}u.", mediachannelId_);
}

void AudioPlayController::AudioPlayThread()
{
    SHARING_LOGD("audio play thread start mediachannelId: %{public}u ,tid: %{public}d.", mediachannelId_, gettid());
    while (isAudioRunning_) {
        MEDIA_LOGD("try read audio data mediaChannelId: %{public}u.", mediachannelId_);
        MediaData::Ptr outData = std::make_shared<MediaData>();
        outData->buff = std::make_shared<DataBuffer>();
        int32_t ret = 0;
        if (bufferReceiver_) {
            ret = bufferReceiver_->RequestRead(MediaType::MEDIA_TYPE_AUDIO, [&outData](const MediaData::Ptr &data) {
                MEDIA_LOGD("request start.");
                if (data == nullptr) {
                    return;
                }
                outData->buff->ReplaceData(data->buff->Peek(), data->buff->Size());
                outData->keyFrame = data->keyFrame;
                outData->mediaType = data->mediaType;
                outData->pts = data->pts;
                outData->isRaw = data->isRaw;
                outData->ssrc = data->ssrc;
                MEDIA_LOGD("request end.");
            });
        }

        if (ret != 0) {
            MEDIA_LOGD("read data null mediaChannelId: %{public}u.", mediachannelId_);
            continue;
        }

        if (audioPlayer_) {
            MEDIA_LOGD("process audio mediaChannelId: %{public}u, size: %{public}d.", mediachannelId_,
                       outData->buff->Size());
            audioPlayer_->ProcessAudioData(outData->buff);
        }
    }

    SHARING_LOGD("audio play thread exit, mediachannelId: %{public}u tid: %{public}d.", mediachannelId_, gettid());
}

void AudioPlayController::SetVolume(float volume)
{
    SHARING_LOGD("Volume: %{public}f.", volume);
    if (audioPlayer_) {
        audioPlayer_->SetVolume(volume);
    }
}

} // namespace Sharing
} // namespace OHOS