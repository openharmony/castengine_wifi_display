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

#include "audio_aac_codec.h"
#include <cstdint>
#include <libswresample/swresample.h>
#include <memory>
#include <securec.h>
#include "common_macro.h"
#include "const_def.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
constexpr int32_t MAX_AUDIO_BUFFER_SIZE = 100 * 100 * 1024;
constexpr uint32_t ADTS_HEADER_SIZE = 7;
constexpr uint32_t ADTS_HEADER_BEGIN = 0xFF;
constexpr uint32_t ADTS_HEADER_END = 0xFC;
constexpr uint32_t ADTS_HEADER_MPEG4_AACLC = 0xF1;
constexpr uint32_t ADTS_HEADER_PROFILE_SHIFT = 6;
constexpr uint32_t ADTS_HEADER_SAMPLE_MASK = 0x0F;
constexpr uint32_t ADTS_HEADER_SAMPLE_SHIFT = 2;
constexpr uint32_t ADTS_HEADER_CHANNEL_SHIFT = 2;
constexpr uint32_t ADTS_HEADER_CHANNEL_MASK = 0x01;
constexpr uint32_t ADTS_HEADER_CHANNEL_SHIFT1 = 6;
constexpr uint32_t ADTS_HEADER_CHANNEL_MASK1 = 0x03;
constexpr uint32_t ADTS_HEADER_DATA_SZIE_OFFSET = 7;
constexpr uint32_t ADTS_HEADER_DATA_SZIE_SHIFT = 11;
constexpr uint32_t ADTS_HEADER_DATA_SZIE_SHIFT1 = 3;
constexpr uint32_t ADTS_HEADER_DATA_SZIE_SHIFT2 = 5;
constexpr uint32_t ADTS_HEADER_DATA_SZIE_MASK = 0xFF;
constexpr uint32_t ADTS_HEADER_DATA_SZIE_MASK1 = 0x1F;
constexpr uint32_t ADTS_HEADER_INDEX_2 = 2;
constexpr uint32_t ADTS_HEADER_INDEX_3 = 3;
constexpr uint32_t ADTS_HEADER_INDEX_4 = 4;
constexpr uint32_t ADTS_HEADER_INDEX_5 = 5;
constexpr uint32_t ADTS_HEADER_INDEX_6 = 6;

static std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
static uint64_t duration = 0;

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
        swrContext_ = swr_alloc_set_opts(nullptr, (int64_t)avFrame_->channel_layout, // out_ch_layout
                                         AV_SAMPLE_FMT_S16,                          // out_sample_fmt
                                         avFrame_->sample_rate,                      // out_sample_rate
                                         (int64_t)avFrame_->channel_layout,          // in_ch_layout
                                         (AVSampleFormat)avFrame_->format,           // AV_SAMPLE_FMT_FLTP
                                         avFrame_->sample_rate,                      // out_sample_rate
                                         0, nullptr);
        if (swrContext_ == nullptr) {
            SHARING_LOGE("swrContext_ alloc failed!");
            return;
        }

        swr_init(swrContext_);

        swrOutBufferSize_ =
            av_samples_get_buffer_size(nullptr, avFrame_->channels, avFrame_->nb_samples, AV_SAMPLE_FMT_S16, 0);
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

AudioAACEncoder::AudioAACEncoder()
{
    SHARING_LOGD("trace.");
}

AudioAACEncoder::~AudioAACEncoder()
{
    SHARING_LOGD("trace.");
    if (encFrame_) {
        av_frame_free(&encFrame_);
    }

    if (encPacket_) {
        av_packet_free(&encPacket_);
    }

    if (swr_) {
        swr_free(&swr_);
    }

    if (swrData_) {
        av_freep(&swrData_);
    }

    if (outBuffer_) {
        av_freep(&outBuffer_);
    }

    if (enc_) {
        avcodec_free_context(&enc_);
    }
}

int AudioAACEncoder::InitSwr()
{
    int64_t in_ch_layout = AV_CH_LAYOUT_STEREO;
    if (inChannels_ == 1) {
        in_ch_layout = AV_CH_LAYOUT_MONO;
    }
    AVSampleFormat in_sample_fmt = AV_SAMPLE_FMT_S16;
    if (inSampleBit_ == AUDIO_SAMPLE_BIT_U8) {
        in_sample_fmt = AV_SAMPLE_FMT_U8;
    }
    int in_sample_rate = (int)inSampleRate_;
    swr_ = swr_alloc_set_opts(NULL, enc_->channel_layout, enc_->sample_fmt, enc_->sample_rate, in_ch_layout,
                              in_sample_fmt, in_sample_rate, 0, NULL);
    if (!swr_) {
        SHARING_LOGE("alloc swr failed.");
    }

    int error;
    char errBuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    if ((error = swr_init(swr_)) < 0) {
        SHARING_LOGE("open swr(%{public}d:%{public}s)", error,
                     av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, error));
    }

    if (!(swrData_ = (uint8_t **)calloc(enc_->channels, sizeof(*swrData_)))) {
        SHARING_LOGE("alloc swr buffer failed!");
    }

    if ((error = av_samples_alloc(swrData_, NULL, enc_->channels, enc_->frame_size, enc_->sample_fmt, 0)) < 0) {
        SHARING_LOGE("alloc swr buffer(%{public}d:%{public}s)\n", error,
                     av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, error));
    }

    return 0;
}

void AudioAACEncoder::InitEncoderCtx(uint32_t channels, uint32_t sampleBit, uint32_t sampleRate)
{
    enc_->sample_rate = (int32_t)sampleRate; // dst_samplerate;
    enc_->channels = (int32_t)channels;      // dst_channels;
    enc_->channel_layout = (uint64_t)av_get_default_channel_layout(channels);
    enc_->bit_rate = AUDIO_BIT_RATE_12800;
    enc_->time_base.num = 1;
    enc_->time_base.den = (int32_t)sampleRate;
    enc_->compression_level = 1;
    enc_->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
}

void AudioAACEncoder::InitEncPacket()
{
    av_init_packet(encPacket_);
    encPacket_->data = NULL;
    encPacket_->size = 0;
}

int32_t AudioAACEncoder::Init(uint32_t channels, uint32_t sampleBit, uint32_t sampleRate)
{
    SHARING_LOGD("trace.");
    inChannels_ = channels;
    inSampleBit_ = sampleBit;
    inSampleRate_ = sampleRate;
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec) {
        SHARING_LOGE("Codec not found failed!");
        return 1;
    }

    enc_ = avcodec_alloc_context3(codec);
    if (!enc_) {
        SHARING_LOGE("Could not allocate audio codec context ");
        return 1;
    }
    enc_->sample_fmt = codec->sample_fmts[0]; // only supports AV_SAMPLE_FMT_FLTP
    InitEncoderCtx(channels, sampleBit, sampleRate);

    if (avcodec_open2(enc_, codec, NULL) < 0) {
        SHARING_LOGE("Could not open codec");
    }

    encFrame_ = av_frame_alloc();
    if (!encFrame_) {
        SHARING_LOGE("Could not allocate audio encode in frame");
        return 1;
    }
    encFrame_->format = enc_->sample_fmt;
    encFrame_->nb_samples = enc_->frame_size;
    encFrame_->channel_layout = enc_->channel_layout;

    if (av_frame_get_buffer(encFrame_, 0) < 0) {
        SHARING_LOGE("Could not get audio frame buffer");
        return 1;
    }
    encPacket_ = av_packet_alloc();
    if (!encPacket_) {
        SHARING_LOGE("Could not allocate audio encode out packet");
        return 1;
    }
    if (!(fifo_ = av_audio_fifo_alloc(enc_->sample_fmt, enc_->channels, enc_->frame_size))) {
        SHARING_LOGE("Could not allocate FIFO");
        return 1;
    }
    auto bufferSize = av_samples_get_buffer_size(nullptr, encFrame_->channels, encFrame_->nb_samples,
                                                 AVSampleFormat(encFrame_->format), 0);
    outBuffer_ = (uint8_t *)av_malloc(bufferSize);
    if (outBuffer_ == nullptr) {
        SHARING_LOGE("outBuffer_ av_malloc failed!");
        return 1;
    }

    return 0;
}

int AudioAACEncoder::AddSamplesToFifo(uint8_t **samples, int frame_size)
{
    char errBuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    int error;

    if ((error = av_audio_fifo_realloc(fifo_, av_audio_fifo_size(fifo_) + frame_size)) < 0) {
        SHARING_LOGE("Could not reallocate FIFO(%{public}d:%{public}s)", error,
                     av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, error));
    }

    if ((error = av_audio_fifo_write(fifo_, (void **)samples, frame_size)) < frame_size) {
        SHARING_LOGE("Could not write data to FIFO(%{public}d:%{public}s)", error,
                     av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, error));
    }

    return 0;
}

void AddAdtsHeader(uint8_t *data, int dataSize)
{
    // ADTS header format (7 or 9 bytes):
    // 12 bits syncword (0xFFF)
    // 1 bit MPEG version (0 for MPEG-4, 1 for MPEG-2)
    // 2 bits layer (always 0 for MPEG-4)
    // 1 bit protection absent
    // 2 bits profile (audio object type)
    // 4 bits sampling frequency index
    // 1 bit private bit
    // 3 bits channel configuration
    // 1 bit original/copy
    // 1 bit home
    // variable bits variable header length
    // 16 bits frame length
    // 16 bits buffer fullness
    // 1 bit number of raw data blocks in frame (set to 0)

    uint8_t adtsHeader[ADTS_HEADER_SIZE];
    int profile = 2;                // 2: AAC LC
    int samplingFrequencyIndex = 3; // 3: 48Khz, 4: 44.1kHz
    int channelConfiguration = 2;   // 2: Stereo

    adtsHeader[0] = ADTS_HEADER_BEGIN;
    adtsHeader[1] = ADTS_HEADER_MPEG4_AACLC;
    adtsHeader[ADTS_HEADER_INDEX_2] =
        (static_cast<uint32_t>(profile - 1) << ADTS_HEADER_PROFILE_SHIFT) |
        ((static_cast<uint32_t>(samplingFrequencyIndex) & ADTS_HEADER_SAMPLE_MASK) << ADTS_HEADER_SAMPLE_SHIFT) |
        ((static_cast<uint32_t>(channelConfiguration) >> ADTS_HEADER_CHANNEL_SHIFT) & ADTS_HEADER_CHANNEL_MASK);
    adtsHeader[ADTS_HEADER_INDEX_3] =
        ((static_cast<uint32_t>(channelConfiguration) & ADTS_HEADER_CHANNEL_MASK1) << ADTS_HEADER_CHANNEL_SHIFT1) |
        (static_cast<uint32_t>(dataSize + ADTS_HEADER_DATA_SZIE_OFFSET) >> ADTS_HEADER_DATA_SZIE_SHIFT);
    adtsHeader[ADTS_HEADER_INDEX_4] =
        (static_cast<uint32_t>(dataSize + ADTS_HEADER_DATA_SZIE_OFFSET) >> ADTS_HEADER_DATA_SZIE_SHIFT1) &
        ADTS_HEADER_DATA_SZIE_MASK;
    adtsHeader[ADTS_HEADER_INDEX_5] =
        (static_cast<uint32_t>(dataSize + ADTS_HEADER_DATA_SZIE_OFFSET) << ADTS_HEADER_DATA_SZIE_SHIFT2) |
        ADTS_HEADER_DATA_SZIE_MASK1;
    adtsHeader[ADTS_HEADER_INDEX_6] = ADTS_HEADER_END;

    if (memcpy_s(data, sizeof(adtsHeader), adtsHeader, sizeof(adtsHeader)) != EOK) {
        SHARING_LOGE("copy adtsHeader failed!");
    }
}

void AudioAACEncoder::DoSwr(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    int err = 0;
    int error = 0;
    int in_samples = frame->Size();
    uint8_t *in_sample[1];
    in_sample[0] = frame->Data();
    char errBuf[AV_ERROR_MAX_STRING_SIZE] = {0};

    do {
        int sample_size = (int)(inChannels_ * inSampleBit_ / 8);
        if (sample_size == 0) {
            continue;
        }
        in_samples = in_samples / sample_size;

        int frame_size = swr_convert(swr_, swrData_, enc_->frame_size, (const uint8_t **)in_sample, in_samples);
        if ((error = frame_size) < 0) {
            SHARING_LOGE("Could not convert input samples(%{public}d:%{public}s)", error,
                         av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, error));
        }

        in_sample[0] = NULL;
        in_samples = 0;
        if ((err = AddSamplesToFifo(swrData_, frame_size)) != 0) {
            SHARING_LOGE("write samples failed");
        }
    } while (swr_get_out_samples(swr_, in_samples) >= enc_->frame_size);
}

void AudioAACEncoder::OnFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    if (duration == 0) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        duration = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    }

    int error = 0;
    if ((error = InitSwr()) != 0 && !swr_) {
        SHARING_LOGE("resample init failed!");
        return;
    }
    DoSwr(frame);

    char errBuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    encFrame_->format = AV_SAMPLE_FMT_FLTP;
    while (av_audio_fifo_size(fifo_) >= enc_->frame_size) {
        if (av_frame_make_writable(encFrame_) < 0) {
            SHARING_LOGE("Could not make writable frame");
        }
        if (av_audio_fifo_read(fifo_, (void **)encFrame_->data, enc_->frame_size) < enc_->frame_size) {
            SHARING_LOGE("Could not read data from FIFO");
        }
        encFrame_->pts = nextOutPts_;
        nextOutPts_ += enc_->frame_size;
        error = avcodec_send_frame(enc_, encFrame_);
        if (error < 0) {
            SHARING_LOGE("send failed:%{public}s", av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, error));
        }

        InitEncPacket();
        while (error >= 0) {
            error = avcodec_receive_packet(enc_, encPacket_);
            if (error == AVERROR(EAGAIN) || error == AVERROR_EOF) {
                break;
            } else if (error < 0) {
                SHARING_LOGE("recv failed:%{public}s", av_make_error_string(errBuf, AV_ERROR_MAX_STRING_SIZE, error));
            }

            encPacket_->dts = av_rescale(encPacket_->dts, 1000, enc_->time_base.den); // rescale time base 1000.
            encPacket_->pts = av_rescale(encPacket_->pts, 1000, enc_->time_base.den); // rescale time base 1000.
            if (memcpy_s(outBuffer_ + ADTS_HEADER_SIZE, encPacket_->size, encPacket_->data, encPacket_->size) != EOK) {
                SHARING_LOGE("copy data failed!");
                break;
            }
            AddAdtsHeader((uint8_t *)outBuffer_, encPacket_->size);
            auto aacFrame = FrameImpl::Create();
            aacFrame->codecId_ = CODEC_AAC;
            aacFrame->pts_ = (uint32_t)((int64_t)duration + encPacket_->pts);
            aacFrame->Assign((char *)outBuffer_, encPacket_->size + 7); // 7: size offset
            DeliverFrame(aacFrame);
        }
    }
}
} // namespace Sharing
} // namespace OHOS
