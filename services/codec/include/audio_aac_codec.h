/*
 * Copyright (c) 2023-2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include <memory>
#include <stdint.h>
#include <string.h>
#include "audio_decoder.h"
#include "audio_encoder.h"
#include "media_frame_pipeline.h"
#include "protocol/frame/h264_frame.h"
#include "utils/data_buffer.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
};

namespace OHOS {
namespace Sharing {
class AudioAACDecoder : public AudioDecoder {
public:
    AudioAACDecoder();
    ~AudioAACDecoder();

    int32_t Init() override;
    void OnFrame(const Frame::Ptr &frame) override;

private:
    uint8_t *swrOutBuffer_ = nullptr;
    int32_t swrOutBufferSize_ = 0;

    std::unique_ptr<DataBuffer> buffer_;

    AVFrame *avFrame_ = nullptr;
    AVPacket *avPacket_ = nullptr;
    AVCodecContext *codecCtx_ = nullptr;
    SwrContext *swrContext_ = nullptr;
};

class AudioAACEncoder : public AudioEncoder {
public:
    AudioAACEncoder();
    ~AudioAACEncoder();

    int32_t Init(uint32_t channels = 2, uint32_t sampleBit = 16, uint32_t sampleRate = 44100) override;
    void OnFrame(const Frame::Ptr &frame) override ;
private:
    int InitSwr();
    void DoSwr(const Frame::Ptr &frame);
    int AddSamplesToFifo(uint8_t **samples, int frame_size);
    void InitEncoderCtx(uint32_t channels, uint32_t sampleBit, uint32_t sampleRate);
private:
    uint32_t inChannels_ = 2;
    uint32_t inSampleBit_ = 16;
    uint32_t inSampleRate_ = 44100;

    std::unique_ptr<DataBuffer> buffer_;

    AVCodecContext *enc_ = nullptr;
    AVFrame *encFrame_ = nullptr;
    AVPacket *encPacket_ = nullptr;

    SwrContext *swr_ = nullptr;
    //buffer for swr out put
    uint8_t **swrData_;
    AVAudioFifo *fifo_ = nullptr;

    int64_t nextOutPts_;
    uint8_t* outBuffer_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
