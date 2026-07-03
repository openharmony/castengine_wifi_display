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

#include "rtp_decoder_ts.h"
#include <securec.h>
#include "common/common_macro.h"
#include "common/media_log.h"
#include "frame/aac_frame.h"
#include "frame/h264_frame.h"

namespace OHOS {
namespace Sharing {
constexpr int32_t FF_BUFFER_SIZE = 1500;
static std::mutex frameLock;

RtpDecoderTs::RtpDecoderTs()
{
    MEDIA_LOGD("RtpDecoderTs CTOR IN.");
}

RtpDecoderTs::~RtpDecoderTs()
{
    SHARING_LOGI("RtpDecoderTs DTOR IN.");
    Release();
}

void RtpDecoderTs::Release()
{
    {
        std::lock_guard<std::mutex> lock(frameLock);
        onFrame_ = nullptr;
    }

    exit_ = true;
    queueCond_.notify_one();
    if (decodeThread_ && decodeThread_->joinable()) {
        decodeThread_->join();
        decodeThread_ = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        while (!dataQueue_.empty()) {
            dataQueue_.pop();
        }
    }

    if (avFormatContext_) {
        avformat_close_input(&avFormatContext_);
    }

    if (avioContext_) {
        av_freep(&avioContext_->buffer);
        avio_context_free(&avioContext_);
    }
}

void RtpDecoderTs::InputRtp(const RtpPacket::Ptr &rtp)
{
    MEDIA_LOGD("trace.");
    RETURN_IF_NULL(rtp);
    if (exit_) {
        SHARING_LOGE("ignore rtp seq:%{public}d, exit", rtp->GetSeq());
        return;
    }

    if (decodeThread_ == nullptr) {
        decodeThread_ = std::make_unique<std::thread>(&RtpDecoderTs::StartDecoding, this);
    }

    auto payload_size = rtp->GetPayloadSize();
    if (payload_size <= 0) {
        SHARING_LOGE("ignore rtp seq:%{public}d, payload size invalid", rtp->GetSeq());
        return;
    }

    std::lock_guard<std::mutex> lock(queueMutex_);
    dataQueue_.emplace(rtp);
    queueCond_.notify_one();
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

    auto buffer = (uint8_t *)av_malloc(FF_BUFFER_SIZE);
    if (buffer == nullptr) {
        SHARING_LOGE("av_malloc failed.");
        return;
    }
    avioContext_ =
        avio_alloc_context(buffer, FF_BUFFER_SIZE, 0, this, &RtpDecoderTs::StaticReadPacket, nullptr, nullptr);
    if (avioContext_ == nullptr) {
        SHARING_LOGE("avio_alloc_context failed.");
        av_freep(&buffer);
        return;
    }
    avFormatContext_->pb = avioContext_;

    int ret = avformat_open_input(&avFormatContext_, nullptr, nullptr, nullptr);
    if (ret != 0) {
        SHARING_LOGE("avformat_open_input failed.");
        return;
    }

    AVRational videoTimeBase;
    AVRational audioTimeBase;
    AVRational PtsTimeBase = {1, US_PER_SEC};

    for (int32_t i = 0; i < static_cast<int32_t>(avFormatContext_->nb_streams); i++) {
        if (avFormatContext_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoTimeBase = avFormatContext_->streams[i]->time_base;
            if (videoTimeBase.den == 0) {
                videoTimeBase = AV_TIME_BASE_Q;
            }
            videoStreamIndex_ = i;
            SHARING_LOGD("find video stream %{public}u.", i);
        } else if (avFormatContext_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioTimeBase = avFormatContext_->streams[i]->time_base;
            if (audioTimeBase.den == 0) {
                audioTimeBase = AV_TIME_BASE_Q;
            }
            audioStreamIndex_ = i;
            SHARING_LOGD("find audio stream %{public}u.", i);
        }
    }

    AVPacket *packet = av_packet_alloc();
    if (packet == nullptr) {
        av_freep(&buffer);
        return;
    }
    while (!exit_ && av_read_frame(avFormatContext_, packet) >= 0) {
        if (exit_) {
            SHARING_LOGI("ignore av read frame.");
            break;
        }
        if (packet->stream_index == videoStreamIndex_) {
            SplitH264((char *)packet->data, (size_t)packet->size, 0, [&](const char *buf, size_t len, size_t prefix) {
                if (H264_TYPE(buf[prefix]) == H264Frame::NAL_AUD) {
                    return;
                }
                int64_t ptsUsec = av_rescale_q(packet->pts, videoTimeBase, PtsTimeBase);
                auto outFrame = std::make_shared<H264Frame>((uint8_t *)buf, len, (uint32_t)packet->dts,
                                                            (uint64_t)ptsUsec, prefix);
                std::lock_guard<std::mutex> lock(frameLock);
                if (onFrame_) {
                    onFrame_(outFrame);
                }
            });
        } else if (packet->stream_index == audioStreamIndex_) {
            int64_t ptsUsec = av_rescale_q(packet->pts, audioTimeBase, PtsTimeBase);
            auto outFrame = std::make_shared<AACFrame>((uint8_t *)packet->data, packet->size, (uint32_t)packet->dts,
                                                        (uint64_t)ptsUsec);
            std::lock_guard<std::mutex> lock(frameLock);
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
    RETURN_INVALID_IF_NULL(opaque);
    RETURN_INVALID_IF_NULL(buf);
    RtpDecoderTs *decoder = (RtpDecoderTs *)opaque;
    if (decoder == nullptr) {
        SHARING_LOGE("decoder is nullptr.");
        return 0;
    }
    return decoder->ReadPacket(buf, buf_size);
}

int RtpDecoderTs::ReadPacket(uint8_t *buf, int buf_size)
{
    RETURN_INVALID_IF_NULL(buf);
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (dataQueue_.empty()) {
        if (exit_ == true) {
            SHARING_LOGI("read packet exit.");
            return -1;
        }
        queueCond_.wait(lock, [this] { return !dataQueue_.empty() || exit_; });
        if (exit_) {
            SHARING_LOGI("read packet exit.");
            return -1;
        }
        if (dataQueue_.empty()) {
            return 0;
        }
    }
    auto &rtp = dataQueue_.front();
    auto data = rtp->GetPayload();
    if (data == nullptr) {
        SHARING_LOGE("payload null");
        return 0;
    }
    int length = static_cast<int>(rtp->GetPayloadSize());
    if (length > buf_size) {
        SHARING_LOGE("rtp length exceed buf_size!");
        return 0;
    }
    auto ret = memcpy_s(buf, length, data, length);
    if (ret != EOK) {
        return 0;
    }

    dataQueue_.pop();
    return length;
}
} // namespace Sharing
} // namespace OHOS
