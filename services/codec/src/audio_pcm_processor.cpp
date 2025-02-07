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

#include "audio_pcm_processor.h"
#include "const_def.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
constexpr uint32_t LPCM_PES_PAYLOAD_PRIVATE_SIZE = 4;
constexpr uint32_t LPCM_PES_PAYLOAD_DATA_SIZE = 1920;
constexpr uint32_t LPCM_PES_PAYLOAD_TIME_DURATION = 10;
constexpr uint32_t FIFO_SAMPLES = 3840;
constexpr uint8_t AUDIO_SAMPLING_FREQUENCY_48K = 2 << 3;
constexpr uint8_t NUMBER_OF_AUDIO_CHANNEL_STEREO = 1;

static std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
static uint64_t duration = 0;
static uint8_t data[LPCM_PES_PAYLOAD_DATA_SIZE];

AudioPcmProcessor::AudioPcmProcessor()
{
    SHARING_LOGD("trace.");
}

AudioPcmProcessor::~AudioPcmProcessor()
{
    av_audio_fifo_free(fifo_);
    SHARING_LOGD("trace.");
}

int32_t AudioPcmProcessor::Init(uint32_t channels, uint32_t sampleBit, uint32_t sampleRate)
{
    channels_ = channels;
    sampleBit_ = sampleBit;
    sampleRate_ = sampleRate;
    if (channels_ == 0 || sampleBit_ == 0 || sampleRate_ == 0) {
        SHARING_LOGE("Invalid pcm parameters!");
        return 1;
    }

    sampleSize_ = sampleBit * channels / AUDIO_SAMPLE_BIT_U8;
    if (!(fifo_ = av_audio_fifo_alloc(AV_SAMPLE_FMT_S16, channels, FIFO_SAMPLES))) {
        SHARING_LOGE("Could not allocate FIFO");
        return 1;
    }
    return 0;
}

void AudioPcmProcessor::OnFrame(const Frame::Ptr &frame)
{
    if (frame == nullptr) {
        SHARING_LOGE("frame is nullptr!");
        return;
    }

    if (channels_ == 0 || sampleBit_ == 0 || sampleRate_ == 0) {
        SHARING_LOGE("Invalid pcm parameters!");
        return;
    }

    if (duration == 0) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        duration = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    }

    PcmLittleToBigEndian(frame->Data(), frame->Size());
    uint8_t *captureData[] = {frame->Data()};
    uint8_t *payloadData[] = {data};
    int32_t payloadSampleNum = (int32_t)(LPCM_PES_PAYLOAD_DATA_SIZE / sampleSize_);
    av_audio_fifo_write(fifo_, reinterpret_cast<void **>(captureData), frame->Size() / sampleSize_);
    while (av_audio_fifo_size(fifo_) >= payloadSampleNum) {
        av_audio_fifo_read(fifo_, reinterpret_cast<void **>(payloadData), payloadSampleNum);
        auto pcmFrame = FrameImpl::Create();
        pcmFrame->pts_ = duration;
        pcmFrame->codecId_ = CODEC_PCM;
        pcmFrame->SetCapacity(LPCM_PES_PAYLOAD_PRIVATE_SIZE + LPCM_PES_PAYLOAD_DATA_SIZE);
        pcmFrame->Append(0xa0);          // 0xa0 - sub_stream_id
        pcmFrame->Append(0x06);          // 0x06 - number_of_frame_header
        pcmFrame->Append(0x00);          // audio_emphasis_flag
        pcmFrame->Append(AUDIO_SAMPLING_FREQUENCY_48K | NUMBER_OF_AUDIO_CHANNEL_STEREO);
        pcmFrame->Append(payloadData[0], LPCM_PES_PAYLOAD_DATA_SIZE);
        DeliverFrame(pcmFrame);
        duration += LPCM_PES_PAYLOAD_TIME_DURATION;
    }
}

void AudioPcmProcessor::PcmLittleToBigEndian(uint8_t *data, int32_t size)
{
    int32_t sampleSize = (int32_t)(sampleSize_ / channels_);
    if (sampleSize <= 1) {
        return;
    }

    uint8_t tmpData;
    int32_t count = size / sampleSize;
    for (int32_t i = 0; i < count; i++) {
        tmpData = data[i * sampleSize];
        data[i * sampleSize] = data[i * sampleSize + 1];
        data[i * sampleSize + 1] = tmpData;
    }
}
} // namespace Sharing
} // namespace OHOS
