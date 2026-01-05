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

#include "audio_sink.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {

AudioSink::AudioSink(uint32_t playerId) : audioRenderer_(nullptr)
{
    SHARING_LOGD("playerId: %{public}u  tid: %{public}d.", playerId, gettid());
    playerId_ = playerId;
}

AudioSink::~AudioSink()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    Stop();
    Release();
    SHARING_LOGD("renderthread release success,playerId: %{public}u.", playerId_);
}

int32_t AudioSink::Prepare()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    audioRenderer_ = AudioStandard::AudioRenderer::Create(AudioStandard::AudioStreamType::STREAM_MUSIC);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    return PLAYER_SUCCESS;
}

int32_t AudioSink::Prepare(int32_t channels, int32_t sampleRate)
{
    SHARING_LOGD("channels(%{public}d) sampleRate(%{public}d) playerId: %{public}u.", channels, sampleRate, playerId_);
    audioRenderer_ = AudioStandard::AudioRenderer::Create(AudioStandard::AudioStreamType::STREAM_MUSIC);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    int32_t res = SetParameters(16, channels, sampleRate);
    audioRenderer_->SetRendererCallback(shared_from_this());
    SHARING_LOGD("leave.");
    return res;
}

int32_t AudioSink::Start()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    audioRenderer_->Start();
    running_ = true;
    needWrite_ = true;
    return PLAYER_SUCCESS;
}

int32_t AudioSink::Stop()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL playerId: %{public}u!", playerId_);
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    if (!running_) {
        SHARING_LOGE("running_ is false playerId: %{public}u!", playerId_);
        return PLAYER_ERROR_INVALID_STATE;
    }

    running_ = false;
    needWrite_ = false;
    audioRenderer_->Stop();
    SHARING_LOGD("success, playerId: %{public}u.", playerId_);
    return PLAYER_SUCCESS;
}

int32_t AudioSink::Pause()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    if (audioRenderer_->Pause() != true) {
        SHARING_LOGE("failed!");
        return -1;
    }

    return PLAYER_SUCCESS;
}

int32_t AudioSink::Drain()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    if (audioRenderer_->Drain() != true) {
        SHARING_LOGE("failed!");
        return -1;
    }

    return PLAYER_SUCCESS;
}

int32_t AudioSink::Flush()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    if (audioRenderer_->Flush() != true) {
        SHARING_LOGE("failed!");
        return -1;
    }

    return PLAYER_SUCCESS;
}

int32_t AudioSink::Release()
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    audioRenderer_ = nullptr;
    return PLAYER_SUCCESS;
}

int32_t AudioSink::SetParameters(int32_t bitsPerSample, int32_t channels, int32_t sampleRate)
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    (void)bitsPerSample;
    SHARING_LOGD("enter, channels:%{public}d, sampleRate:%{public}d.", channels, sampleRate);

    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    AudioStandard::AudioRendererParams params;
    std::vector<AudioStandard::AudioSamplingRate> supportedSampleList =
        AudioStandard::AudioRenderer::GetSupportedSamplingRates();
    if (supportedSampleList.empty()) {
        SHARING_LOGE("GetSupportedSamplingRates empty.");
        return -1;
    }

    bool isValidSampleRate = false;
    for (size_t i = 0; i < supportedSampleList.size(); i++) {
        if (sampleRate <= supportedSampleList[i] && supportedSampleList[i] > 0) {
            params.sampleRate = supportedSampleList[i];
            isValidSampleRate = true;
            break;
        }
    }

    if (!isValidSampleRate) {
        SHARING_LOGE("Unsupported sample ratefailed.");
        return -1;
    }

    std::vector<AudioStandard::AudioChannel> supportedChannelsList =
        AudioStandard::AudioRenderer::GetSupportedChannels();
    if (supportedChannelsList.empty()) {
        SHARING_LOGE("GetSupportedChannels empty.");
        return -1;
    }

    bool isValidChannels = false;
    for (size_t i = 0; i < supportedChannelsList.size(); i++) {
        if (channels == supportedChannelsList[i] && supportedChannelsList[i] > 0) {
            params.channelCount = supportedChannelsList[i];
            isValidChannels = true;
            break;
        }
    }

    if (!isValidChannels) {
        SHARING_LOGE("Unsupported sample ratefailed.");
        return -1;
    }

    params.sampleFormat = AudioStandard::SAMPLE_S16LE;
    params.encodingType = AudioStandard::ENCODING_PCM;
    SHARING_LOGD("channels:%{public}d, sampleRate:%{public}d.", params.channelCount, params.sampleRate);

    if (audioRenderer_->SetParams(params) != 0) {
        SHARING_LOGE("failed.");
        return -1;
    }

    return PLAYER_SUCCESS;
}

int32_t AudioSink::GetParameters(int32_t &bitsPerSample, int32_t &channels, int32_t &sampleRate)
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    AudioStandard::AudioRendererParams params;
    if (audioRenderer_->GetParams(params) != 0) {
        SHARING_LOGE("failed.");
        return -1;
    }

    channels = params.channelCount;
    sampleRate = params.sampleRate;
    bitsPerSample = (int32_t)params.sampleFormat;
    return PLAYER_SUCCESS;
}

int32_t AudioSink::GetBufferSize(int32_t &bufferSize)
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    size_t size = 0;
    if (audioRenderer_->GetBufferSize(size) != 0) {
        SHARING_LOGE("failed!");
        return -1;
    }

    bufferSize = (int32_t)size;
    return PLAYER_SUCCESS;
}

int32_t AudioSink::SetVolume(float volume)
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    if (audioRenderer_->SetVolume(volume) != 0) {
        SHARING_LOGE("failed!");
        return -1;
    }

    return PLAYER_SUCCESS;
}

int32_t AudioSink::GetVolume(float &volume)
{
    SHARING_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    volume = audioRenderer_->GetVolume();
    return PLAYER_SUCCESS;
}

int32_t AudioSink::Write(uint8_t *buffer, size_t size)
{
    MEDIA_LOGD("playerId: %{public}u.", playerId_);
    if (!audioRenderer_) {
        SHARING_LOGE("audioRenderer_ is NULL.");
        return PLAYER_ERROR_EMPTY_INSTANCE;
    }

    size_t bytesWritten = 0;
    int32_t bytesSingle = 0;
    int32_t failNum = 0;
    while (audioRenderer_ && bytesWritten < size && running_ && needWrite_) {
        bytesSingle = audioRenderer_->Write(buffer + bytesWritten, size - bytesWritten);
        if (bytesSingle <= 0) {
            MEDIA_LOGE("audioRenderer Write failed. playerId: %{public}u.", playerId_);
            failNum++;
            if (failNum >= MAX_AUDIO_WRITE_FAIL_NUM) {
                MEDIA_LOGE("audioRenderer write failed too many times, playerId: %{public}u.", playerId_);
                break;
            }
            continue;
        }

        bytesWritten += static_cast<size_t>(bytesSingle);
        if (bytesWritten < static_cast<size_t>(bytesSingle)) {
            MEDIA_LOGE("audioRenderer Write failed playerId: %{public}u.", playerId_);
            continue;
        }
        MEDIA_LOGD("audioRenderer Write end. playerId: %{public}u.", playerId_);
    }

    MEDIA_LOGD("recv data(len:%{public}zu.)", size);
    return PLAYER_SUCCESS;
}

void AudioSink::OnInterrupt(const AudioStandard::InterruptEvent &interruptEvent)
{
    SHARING_LOGI("OnInterrupt hintType: %{public}d", interruptEvent.hintType);
    switch (interruptEvent.hintType) {
        case AudioStandard::InterruptHint::INTERRUPT_HINT_PAUSE:
            // 音频流暂停，暂停音频写入
            needWrite_ = false;
            break;
        case AudioStandard::InterruptHint::INTERRUPT_HINT_RESUME:
            // 音频流恢复，恢复音频写入和重新起播
            Start();
            break;
        default:
            break;
    }
}

void AudioSink::OnStateChange(const AudioStandard::RendererState state, const AudioStandard::StateChangeCmdType cmdType)
{
    SHARING_LOGI("OnStateChange state: %{public}d", state);
}

void AudioSink::SetIsPcSource(bool isPcSource)
{
    SHARING_LOGI("isPcSource: %{public}s", isPcSource ? "true" : "false");
    isPcSource_ = isPcSource;
}

} // namespace Sharing
} // namespace OHOS