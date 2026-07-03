/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "audio_aac_decoder.h"
#include <cstdint>
#include <libswresample/swresample.h>
#include <memory>
#include "common_macro.h"
#include "const_def.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
constexpr int32_t MAX_AUDIO_BUFFER_SIZE = 100 * 100 * 1024;

AudioAACDecoder::AudioAACDecoder()
{
    SHARING_LOGD("trace.");
}

AudioAACDecoder::~AudioAACDecoder()
{
    SHARING_LOGD("trace.");
    if (avFrame_) {
        av_frame_free(&avFrame_);
    }

    if (avPacket_) {
        av_packet_free(&avPacket_);
    }

    if (swrContext_) {
        swr_free(&swrContext_);
    }

    if (swrOutBuffer_) {
        av_freep(&swrOutBuffer_);
    }

    if (codecCtx_) {
        avcodec_free_context(&codecCtx_);
    }
}

int32_t AudioAACDecoder::Init(const AudioTrack &audioTrack)
{
    SHARING_LOGD("trace.");
    const AVCodec *dec = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (!dec) {
        SHARING_LOGE("Failed to find codec.");
        return -1;
    }

    codecCtx_ = avcodec_alloc_context3(dec);
    if (!codecCtx_) {
        SHARING_LOGE("Failed to allocate the codec context.");
        return -1;
    }

    if (avcodec_open2(codecCtx_, dec, nullptr) < 0) {
        SHARING_LOGE("Failed to open codec.");
        return -1;
    }

    avPacket_ = av_packet_alloc();
    if (avPacket_ == nullptr) {
        SHARING_LOGE("Failed to alloc packet.");
        return -1;
    }

    avFrame_ = av_frame_alloc();
    if (avFrame_ == nullptr) {
        SHARING_LOGE("Failed to alloc frame.");
        return -1;
    }
    return 0;
}

void AudioAACDecoder::OnFrame(const Frame::Ptr &frame)
{
    if (frame == nullptr) {
        SHARING_LOGE("frame is nullptr!");
        return;
    }

    if (avPacket_ == nullptr || avFrame_ == nullptr || codecCtx_ == nullptr) {
        return;
    }

    av_packet_unref(avPacket_);
    av_frame_unref(avFrame_);

    avPacket_->data = frame->Data();
    avPacket_->size = frame->Size();

    avcodec_send_packet(codecCtx_, avPacket_);
    avcodec_receive_frame(codecCtx_, avFrame_);

    if (swrContext_ == nullptr) {
        int ret = swr_alloc_set_opts2(&swrContext_, &avFrame_->ch_layout, AV_SAMPLE_FMT_S16, avFrame_->sample_rate,
            &avFrame_->ch_layout, (AVSampleFormat)avFrame_->format, avFrame_->sample_rate, 0, nullptr);
        if (swrContext_ == nullptr || ret) {
            SHARING_LOGE("swrContext_ alloc failed!");
            return;
        }

        swr_init(swrContext_);

        swrOutBufferSize_ = av_samples_get_buffer_size(
            nullptr, avFrame_->ch_layout.nb_channels, avFrame_->nb_samples, AV_SAMPLE_FMT_S16, 0);
        if (swrOutBufferSize_ <= 0 || swrOutBufferSize_ > MAX_AUDIO_BUFFER_SIZE) {
            SHARING_LOGE("invalid buffer size %{public}d", swrOutBufferSize_);
            return;
        }

        swrOutBuffer_ = (uint8_t *)av_malloc(swrOutBufferSize_);
        if (swrOutBuffer_ == nullptr) {
            SHARING_LOGE("swrOutBuffer_ av_malloc failed!");
            return;
        }
    }

    int nbSamples = swr_convert(swrContext_, &swrOutBuffer_, avFrame_->nb_samples, (const uint8_t **)avFrame_->data,
                                avFrame_->nb_samples);
    if (nbSamples != avFrame_->nb_samples) {
        SHARING_LOGE("swr_convert failed!");
        return;
    }

    auto pcmFrame = FrameImpl::Create();
    pcmFrame->codecId_ = CODEC_PCM;
    pcmFrame->Assign((char *)swrOutBuffer_, swrOutBufferSize_);
    DeliverFrame(pcmFrame);
}

} // namespace Sharing
} // namespace OHOS