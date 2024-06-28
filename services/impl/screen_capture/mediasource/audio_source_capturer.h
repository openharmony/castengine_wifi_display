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

#ifndef OHOS_SHARING_AUDIO_SOURCE_CAPTURE_H
#define OHOS_SHARING_AUDIO_SOURCE_CAPTURE_H

#include <iostream>
#include <mutex>
#include <queue>
#include "audio_aac_codec.h"
#include "audio_capturer.h"

namespace OHOS {
namespace Sharing {

class AudioSourceCapturer : public std::enable_shared_from_this<AudioSourceCapturer> {
public:
    explicit AudioSourceCapturer(std::shared_ptr<AudioEncoder> audioEncoder) : audioEncoder_(audioEncoder) {}
    ~AudioSourceCapturer();

    bool InitAudioCapture();
    bool StopAudioCapture();
    bool StartAudioCapture();
    void AudioCaptureThreadWorker();

private:
    size_t audioBufferLen_ = 0;
    volatile bool isAudioCapturing_ = false;
    std::shared_ptr<AudioEncoder> audioEncoder_ = nullptr;
    std::unique_ptr<std::thread> audioCaptureThread_ = nullptr;
    std::unique_ptr<AudioStandard::AudioCapturer> audioCapturer_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
