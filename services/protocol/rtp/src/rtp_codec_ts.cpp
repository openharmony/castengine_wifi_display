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

#include <securec.h>
#include "rtp_codec_ts.h"
#include "common/common_macro.h"
#include "common/media_log.h"
#include "frame/aac_frame.h"
#include "frame/h264_frame.h"

namespace OHOS {
namespace Sharing {
constexpr int32_t FF_BUFFER_SIZE = 1500;

RtpDecoderTs::RtpDecoderTs()
{
    MEDIA_LOGD("RtpDecoderTs CTOR IN.");
}

RtpDecoderTs::~RtpDecoderTs()
{
    MEDIA_LOGD("RtpDecoderTs DTOR IN.");
    exit_ = true;
    if (decodeThread_ && decodeThread_->joinable()) {
        decodeThread_->join();
    }

    if (avFormatContext_) {
        avformat_close_input(&avFormatContext_);
    }

    if (avioContext_) {
        avio_context_free(&avioContext_);
    }

    if (avioCtxBuffer_) {
        av_freep(&avioCtxBuffer_);
    }
}

void RtpDecoderTs::InputRtp(const RtpPacket::Ptr &rtp)
{
    MEDIA_LOGD("trace.");
    RETURN_IF_NULL(rtp);

    if (decodeThread_ == nullptr) {
        decodeThread_ = std::make_unique<std::thread>(&RtpDecoderTs::StartDecoding, this);
    }

    auto payload_size = rtp->GetPayloadSize();
    if (payload_size <= 0) {
        return;
    }

    std::lock_guard<std::mutex> lock(queueMutex_);
    dataQueue_.emplace(rtp);
}

void RtpDecoderTs::SetOnFrame(const OnFrame &cb)
{
    onFrame_ = cb;
}

void RtpDecoderTs::StartDecoding()
{
    SHARING_LOGE("trace.");
    avFormatContext_ = avformat_alloc_context();
    if (avFormatContext_ == nullptr) {
        SHARING_LOGE("avformat_alloc_context failed.");
        return;
    }

    avioCtxBuffer_ = (uint8_t *)av_malloc(FF_BUFFER_SIZE);

    avioContext_ =
        avio_alloc_context(avioCtxBuffer_, FF_BUFFER_SIZE, 0, this, &RtpDecoderTs::StaticReadPacket, nullptr, nullptr);
    if (avioContext_ == nullptr) {
        SHARING_LOGE("avio_alloc_context failed.");
        return;
    }
    avFormatContext_->pb = avioContext_;

    int ret = avformat_open_input(&avFormatContext_, nullptr, nullptr, nullptr);
    if (ret != 0) {
        SHARING_LOGE("avformat_open_input failed.");
        return;
    }

    for (uint32_t i = 0; i < avFormatContext_->nb_streams; i++) {
        if (avFormatContext_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex_ = i;
            SHARING_LOGD("find video stream %{public}u.", i);
        } else if (avFormatContext_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex_ = i;
            SHARING_LOGD("find audio stream %{public}u.", i);
        }
    }

    AVPacket *packet = av_packet_alloc();

    while (!exit_ && av_read_frame(avFormatContext_, packet) >= 0) {
        if (packet->stream_index == videoStreamIndex_) {
            SplitH264((char *)packet->data, (size_t)packet->size, 0, [&](const char *buf, size_t len, size_t prefix) {
                if (H264_TYPE(buf[prefix]) == H264Frame::NAL_AUD) {
                    return;
                }
                auto outFrame = std::make_shared<H264Frame>((uint8_t *)buf, len, (uint32_t)packet->dts,
                                                            (uint32_t)packet->pts, prefix);
                if (onFrame_) {
                    onFrame_(outFrame);
                }
            });
        } else if (packet->stream_index == audioStreamIndex_) {
            auto outFrame = std::make_shared<AACFrame>((uint8_t *)packet->data, packet->size, (uint32_t)packet->dts,
                                                       (uint32_t)packet->pts);
            if (onFrame_) {
                onFrame_(outFrame);
            }
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    SHARING_LOGD("ts decoding Thread_ exit.");
}

int RtpDecoderTs::StaticReadPacket(void *opaque, uint8_t *buf, int buf_size)
{
    RtpDecoderTs *decoder = (RtpDecoderTs *)opaque;
    return decoder->ReadPacket(buf, buf_size);
}

int RtpDecoderTs::ReadPacket(uint8_t *buf, int buf_size)
{
    while (dataQueue_.empty()) {
        std::this_thread::sleep_for(std::chrono::microseconds(5)); // 5: wait times
    }

    std::unique_lock<std::mutex> lock(queueMutex_);
    auto &rtp = dataQueue_.front();
    auto data = rtp->GetPayload();
    int length = rtp->GetPayloadSize();

    auto ret = memcpy_s(buf, length, data, length);
    if (ret != EOK) {
        return 0;
    }

    dataQueue_.pop();
    return length;
}

RtpEncoderTs::RtpEncoderTs(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq)
    : RtpMaker(ssrc, mtuSize, sampleRate, payloadType, seq)
{}

RtpEncoderTs::~RtpEncoderTs() {}

void RtpEncoderTs::InputFrame(const Frame::Ptr &frame) {}

void RtpEncoderTs::SetOnRtpPack(const OnRtpPack &cb)
{
    onRtpPack_ = cb;
}
} // namespace Sharing
} // namespace OHOS
