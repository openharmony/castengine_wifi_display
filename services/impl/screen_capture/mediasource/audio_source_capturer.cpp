/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "audio_source_capturer.h"
#include <media_description.h>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unistd.h>
#include "const_def.h"
#include "frame.h"
#include "sharing_log.h"
#include "utils/data_buffer.h"

using namespace OHOS;
using namespace OHOS::AudioStandard;
using namespace OHOS::MediaAVCodec;
using namespace std;

namespace OHOS {
namespace Sharing {

AudioSourceCapturer::~AudioSourceCapturer()
{
    audioEncoder_ = nullptr;
}

bool AudioSourceCapturer::InitAudioCapture()
{
    SHARING_LOGD("trace.");

    AudioStandard::AudioCapturerOptions options;
    options.capturerInfo.capturerFlags = 0;
    options.capturerInfo.sourceType = AudioStandard::SourceType::SOURCE_TYPE_PLAYBACK_CAPTURE;
    options.streamInfo.channels = AudioStandard::AudioChannel::STEREO;
    options.streamInfo.encoding = AudioStandard::AudioEncodingType::ENCODING_PCM;
    options.streamInfo.format = AudioStandard::AudioSampleFormat::SAMPLE_S16LE;
    options.streamInfo.samplingRate = AudioStandard::AudioSamplingRate::SAMPLE_RATE_48000;

    AudioStandard::AppInfo appInfo;
    appInfo.appUid = 0;

    audioCapturer_ = AudioStandard::AudioCapturer::Create(options, appInfo);
    if (audioCapturer_ == nullptr) {
        SHARING_LOGE("create AudioCapturer failed!");
        return false;
    }

    return true;
}

bool AudioSourceCapturer::StartAudioCapture()
{
    SHARING_LOGD("trace.");
    if (audioCapturer_ == nullptr) {
        SHARING_LOGE("start capture fail - invalid capturer!");
        return false;
    }

    if (!audioCapturer_->Start()) {
        SHARING_LOGE("audioCapturer Start() failed!");
        return false;
    }

    if (audioCapturer_->GetBufferSize(audioBufferLen_) != 0 || audioBufferLen_ == 0) {
        audioBufferLen_ = 0;
        SHARING_LOGE("audioCapturer GetBufferSize failed!");
        return false;
    }
    
    isAudioCapturing_ = true;
    audioCaptureThread_ = std::make_unique<std::thread>(&AudioSourceCapturer::AudioCaptureThreadWorker, this);

    SHARING_LOGD("Audio capture start successful.");
    return true;
}

bool AudioSourceCapturer::StopAudioCapture()
{
    SHARING_LOGD("trace.");
    isAudioCapturing_ = false;

    if (audioCaptureThread_ != nullptr && audioCaptureThread_->joinable()) {
        audioCaptureThread_->join();
        audioCaptureThread_.reset();
        audioCaptureThread_ = nullptr;
    }

    if (audioCapturer_) {
        audioCapturer_->Flush();
        audioCapturer_->Stop();
    }

    return true;
}

void AudioSourceCapturer::AudioCaptureThreadWorker()
{
    SHARING_LOGD("trace.");
    int32_t bytesRead = 0;
    bool isBlockingRead = true;

    auto pcmFrame = FrameImpl::Create();
    pcmFrame->codecId_ = CODEC_PCM;

    SHARING_LOGI("audio capture buffer size: %{public}d.", audioBufferLen_);
    uint8_t *frame = (uint8_t *)malloc(audioBufferLen_);
    if (!frame) {
        SHARING_LOGE("malloc buffer failed.");
        return;
    }

    while (isAudioCapturing_) {
        bytesRead = audioCapturer_->Read(*frame, audioBufferLen_, isBlockingRead);
        SHARING_LOGD("audio capture thread get pcm data, size: %{public}d.", bytesRead);
        if (bytesRead > 0 && audioEncoder_) {
            pcmFrame->Clear();
            pcmFrame->Assign((char *)frame, static_cast<uint32_t>(bytesRead));

            audioEncoder_->OnFrame(pcmFrame);
        }
    }

    if (frame) {
        free(frame);
    }

    SHARING_LOGD("audio capture thread exit.");
}
} // namespace Sharing
} // namespace OHOS
