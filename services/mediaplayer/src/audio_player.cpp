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

#include "audio_player.h"
#include "common/media_log.h"
#include "protocol/frame/aac_frame.h"
#include "protocol/frame/frame.h"

namespace OHOS {
namespace Sharing {

AudioPlayer::~AudioPlayer()
{
    SHARING_LOGD("trace.");
    Release();
}

bool AudioPlayer::Init(CodecId audioCodecId)
{
    SHARING_LOGD("trace.");
    playerId_ = GetId();
    audioCodecId_ = audioCodecId;
    audioSink_ = std::make_shared<AudioSink>(playerId_);
    audioDecoderReceiver_ = std::make_shared<AudioDecoderReceiver>(audioSink_);

    audioDecoder_ = CodecFactory::CreateAudioDecoder(audioCodecId_);
    if (audioDecoder_ == nullptr) {
        SHARING_LOGE("CreateAudioDecoder failed for CodecId:%{public}d!", (int32_t)audioCodecId_);
        return false;
    }

    audioDecoder_->Init();
    return true;
}

bool AudioPlayer::SetAudioFormat(int32_t channel, int32_t sampleRate)
{
    SHARING_LOGD("trace.");
    channel_ = channel;
    sampleRate_ = sampleRate;
    return true;
}

bool AudioPlayer::Start()
{
    SHARING_LOGD("trace.");
    if (isRunning_) {
        SHARING_LOGW("audio player is running.");
        return true;
    }

    if (CODEC_NONE == audioCodecId_ || nullptr == audioDecoder_ || nullptr == audioDecoderReceiver_ ||
        nullptr == audioSink_) {
        SHARING_LOGE("start must be after init, CodecId:%{public}d!", (int32_t)audioCodecId_);
        return false;
    }

    if (audioSink_->Prepare(channel_, sampleRate_) != 0 || audioSink_->Start() != 0) {
        SHARING_LOGE("audio player start failed.");
        return false;
    }

    audioDecoder_->AddAudioDestination(audioDecoderReceiver_);
    isRunning_ = true;
    return true;
}

void AudioPlayer::SetVolume(float volume)
{
    SHARING_LOGD("trace.");
    if (audioSink_) {
        audioSink_->SetVolume(volume);
    }
}

void AudioPlayer::Stop()
{
    SHARING_LOGD("trace.");
    if (isRunning_) {
        audioDecoder_->RemoveAudioDestination(audioDecoderReceiver_);
        audioSink_->Stop();
        isRunning_ = false;
    }
}

void AudioPlayer::Release()
{
    SHARING_LOGD("trace.");
    if (audioSink_ != nullptr) {
        audioSink_->Release();
        audioSink_ = nullptr;
    }
}

void AudioPlayer::ProcessAudioData(DataBuffer::Ptr data)
{
    MEDIA_LOGD("trace.");
    if (!isRunning_) {
        return;
    }

    if (data == nullptr || audioDecoder_ == nullptr || data->Peek() == nullptr || data->Size() <= 0) {
        return;
    }

    auto frame = FrameImpl::Create();
    frame->codecId_ = audioCodecId_;
    frame->Assign(data->Peek(), data->Size());
    audioDecoder_->OnFrame(frame);
}

} // namespace Sharing
} // namespace OHOS