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

#ifndef OHOS_SHARING_AUDIO_G711_CODEC_H
#define OHOS_SHARING_AUDIO_G711_CODEC_H

#include "audio_decoder.h"
#include "audio_encoder.h"
#include "media_frame_pipeline.h"

namespace OHOS {
namespace Sharing {
enum G711_TYPE { G711_ALAW, G711_ULAW };

class AudioG711Encoder : public AudioEncoder {
public:
    AudioG711Encoder(G711_TYPE type);
    ~AudioG711Encoder();

    int Init() override;
    void OnFrame(const Frame::Ptr &frame) override;

private:
    int Encode(int16_t *decoded, int nSamples, uint8_t *encoded);

private:
    std::vector<uint8_t> outBuffer_;

    G711_TYPE type_;
};

class AudioG711Decoder : public AudioDecoder {
public:
    AudioG711Decoder(G711_TYPE type);
    ~AudioG711Decoder();

    int Init() override;
    void OnFrame(const Frame::Ptr &frame) override;

private:
    int Decode(uint8_t *encoded, int nSamples, int16_t *decoded);

private:
    std::vector<uint8_t> outBuffer_;

    G711_TYPE type_ = G711_ALAW;
};
} // namespace Sharing
} // namespace OHOS
#endif