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

#ifndef OHOS_SHARING_AUDIO_AAC_CODEC_H
#define OHOS_SHARING_AUDIO_AAC_CODEC_H

#include <cstdint>
#include <string.h>
#include "audio_decoder.h"
#include "audio_encoder.h"
#include "media_frame_pipeline.h"

constexpr uint8_t numberOfLayers = 1;
constexpr uint8_t minChannelCount = 1;
constexpr uint8_t maxChannelCount = 8;
constexpr uint32_t maxConfigurationSize = 1024;

namespace OHOS {
namespace Sharing {

class AudioAACDecoder : public AudioDecoder {
public:
    AudioAACDecoder();
    ~AudioAACDecoder();

    int32_t Init() override;
    void OnFrame(const Frame::Ptr &frame) override;

private:
    int32_t Decode(uint8_t *encoded, int32_t nSamples, int16_t *decoded);

private:
    char *outBuffer_ = nullptr;
    int32_t errorCode_ = 0;
    uint32_t flags_ = 0;
    uint32_t channels_ = 2;
    uint32_t concealMethod_ = 2;
    uint32_t sampleRate_ = 48000;
};

class AudioAACEncoder : public AudioEncoder {
public:
    AudioAACEncoder();
    ~AudioAACEncoder();

    int32_t Init() override;
    void OnFrame(const Frame::Ptr &frame) override;

private:
    int32_t Encode(int16_t *decoded, int32_t nSamples, uint8_t *encoded);

private:
    char *outBuffer_ = nullptr;
    uint32_t flags_ = 0;
    uint32_t channels_ = 1;
    uint32_t sampleRate_ = 48000;
};

} // namespace Sharing
} // namespace OHOS
#endif
