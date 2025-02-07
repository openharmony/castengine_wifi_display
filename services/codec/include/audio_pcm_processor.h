/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#ifndef OHOS_SHARING_AUDIO_PCM_PROCESSOR_H
#define OHOS_SHARING_AUDIO_PCM_PROCESSOR_H

#include "audio_encoder.h"
#include "media_frame_pipeline.h"
extern "C" {
#include <libavutil/audio_fifo.h>
#include <libavutil/mem.h>
};

namespace OHOS {
namespace Sharing {
class AudioPcmProcessor : public AudioEncoder {
public:
    AudioPcmProcessor();
    ~AudioPcmProcessor();

    int32_t Init(uint32_t channels = 2, uint32_t sampleBit = 16, uint32_t sampleRate = 44100) override;
    void OnFrame(const Frame::Ptr &frame) override ;
private:
    void PcmLittleToBigEndian(uint8_t *data, int32_t size);

private:
    uint32_t channels_ = 0;
    uint32_t sampleBit_ = 0;
    uint32_t sampleRate_ = 0;
    uint32_t sampleSize_ = 0;
    AVAudioFifo *fifo_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
