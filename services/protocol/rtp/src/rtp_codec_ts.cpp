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
constexpr int32_t MAX_RTP_PAYLOAD_SIZE = 188;
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
    if (decodeThread_ && decodeThread_->joinable()) {
        decodeThread_->join();
        decodeThread_ = nullptr;
    }

    if (avFormatContext_) {
        avformat_close_input(&avFormatContext_);
    }

    if (avioContext_) {
        avio_context_free(&avioContext_);
    }

    if (avioCtxBuffer_) {
        avioCtxBuffer_ = nullptr;
    }
}

void RtpDecoderTs::InputRtp(const RtpPacket::Ptr &rtp)
{
    MEDIA_LOGD("trace.");
    RETURN_IF_NULL(rtp);
    if (exit_) {
        return;
    }

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
        if (exit_) {
            SHARING_LOGI("ignore av read frame.");
            break;
        }
        if (packet->stream_index == videoStreamIndex_) {
            SplitH264((char *)packet->data, (size_t)packet->size, 0, [&](const char *buf, size_t len, size_t prefix) {
                if (H264_TYPE(buf[prefix]) == H264Frame::NAL_AUD) {
                    return;
                }
                auto outFrame = std::make_shared<H264Frame>((uint8_t *)buf, len, (uint32_t)packet->dts,
                                                            (uint32_t)packet->pts, prefix);
                std::lock_guard<std::mutex> lock(frameLock);
                if (onFrame_) {
                    onFrame_(outFrame);
                }
            });
        } else if (packet->stream_index == audioStreamIndex_) {
            auto outFrame = std::make_shared<AACFrame>((uint8_t *)packet->data, packet->size, (uint32_t)packet->dts,
                                                       (uint32_t)packet->pts);
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
    RtpDecoderTs *decoder = (RtpDecoderTs *)opaque;
    if (decoder == nullptr) {
        SHARING_LOGE("decoder is nullptr.");
        return 0;
    }
    return decoder->ReadPacket(buf, buf_size);
}

int RtpDecoderTs::ReadPacket(uint8_t *buf, int buf_size)
{
    while (dataQueue_.empty()) {
        if (exit_ == true) {
            SHARING_LOGI("read packet exit.");
            return 0;
        }
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
{
    SHARING_LOGD("RtpEncoderTs CTOR IN");
    merger_.SetType(FrameMerger::H264_PREFIX);
}

RtpEncoderTs::~RtpEncoderTs()
{
    SHARING_LOGD("RtpEncoderTs DTOR IN");
    Release();
}

void RtpEncoderTs::Release()
{
    SHARING_LOGD("trace.");
    {
        std::lock_guard<std::mutex> lock(cbLockMutex_);
        onRtpPack_ = nullptr;
    }
    exit_ = true;
    if (encodeThread_ && encodeThread_->joinable()) {
        encodeThread_->join();
        encodeThread_ = nullptr;
    }

    if (avioContext_) {
        avio_context_free(&avioContext_);
    }

    if (avioCtxBuffer_) {
        avioCtxBuffer_ = nullptr;
    }
}

void RtpEncoderTs::InputFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    if (exit_) {
        return;
    }

    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    switch (frame->GetCodecId()) {
        case CODEC_H264:
            // merge sps, pps and key frame into one packet
            merger_.InputFrame(
                frame, buffer, [this](uint32_t dts, uint32_t pts, const DataBuffer::Ptr &buffer, bool have_key_frame) {
                    auto outFrame =
                        std::make_shared<H264Frame>(buffer->Data(), buffer->Size(), (uint32_t)dts, (uint32_t)pts,
                                                    PrefixSize((char *)buffer->Data(), buffer->Size()));
                    SaveFrame(outFrame);
                });
            break;
        case CODEC_AAC:
            SaveFrame(frame);
            break;
        default:
            SHARING_LOGW("Unknown codec: %d", frame->GetCodecId());
            break;
    }

    if (encodeThread_ == nullptr) {
        encodeThread_ = std::make_unique<std::thread>(&RtpEncoderTs::StartEncoding, this);
    }
}

void RtpEncoderTs::SetOnRtpPack(const OnRtpPack &cb)
{
    SHARING_LOGD("trace.");
    onRtpPack_ = cb;
}

void RtpEncoderTs::StartEncoding()
{
    SHARING_LOGD("trace.");
    avformat_alloc_output_context2(&avFormatContext_, NULL, "mpegts", NULL);
    if (avFormatContext_ == nullptr) {
        SHARING_LOGE("avformat_alloc_output_context2 failed.");
        return;
    }

    out_stream = avformat_new_stream(avFormatContext_, NULL);
    out_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    out_stream->codecpar->codec_id = AV_CODEC_ID_H264;
    out_stream->codecpar->codec_tag = 0;

    avioCtxBuffer_ = (uint8_t *)av_malloc(MAX_RTP_PAYLOAD_SIZE);
    avioContext_ = avio_alloc_context(avioCtxBuffer_, MAX_RTP_PAYLOAD_SIZE, 1, this,
                                      NULL, &RtpEncoderTs::WritePacket, NULL);
    if (avioContext_ == nullptr) {
        SHARING_LOGE("avio_alloc_context failed.");
        return;
    }
    avFormatContext_->pb = avioContext_;
    avFormatContext_->flags |= AVFMT_FLAG_CUSTOM_IO;

    if (avformat_write_header(avFormatContext_, NULL) < 0) {
        SHARING_LOGE("avformat_write_header to output failed.");
        return;
    }

    while (!exit_) {
        AVPacket *packet = av_packet_alloc();
        packet->stream_index = out_stream->index;
        ReadFrame(packet);
        av_write_frame(avFormatContext_, packet);
        RemoveFrameAfterMuxing();
    }

    av_write_trailer(avFormatContext_);
}

void RtpEncoderTs::SaveFrame(Frame::Ptr frame)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    dataQueue_.emplace(frame);
}

int RtpEncoderTs::ReadFrame(AVPacket *packet)
{
    while (dataQueue_.empty()) {
        if (exit_ == true) {
            SHARING_LOGI("exit when read frame.");
            return 0;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100)); // 100: wait times
    }

    std::unique_lock<std::mutex> lock(queueMutex_);
    Frame::Ptr frame = dataQueue_.front();
    packet->data = frame->Data();
    packet->size = frame->Size();
    packet->pts = frame->Pts();
    packet->dts = frame->Dts();
    keyFrame_ = frame->KeyFrame();
    timeStamp_ = frame->Dts();

    return 0;
}

void RtpEncoderTs::RemoveFrameAfterMuxing()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    dataQueue_.pop();
}

int RtpEncoderTs::WritePacket(void *opaque, uint8_t *buf, int buf_size)
{
    RtpEncoderTs *encoder = (RtpEncoderTs *)opaque;
    std::lock_guard<std::mutex> lock(encoder->cbLockMutex_);
    if (encoder->onRtpPack_) {
        auto rtp = encoder->MakeRtp(reinterpret_cast<const void *>(buf), buf_size,
                                    encoder->keyFrame_, encoder->timeStamp_);
        encoder->onRtpPack_(rtp);
    }

    return 0;
}
} // namespace Sharing
} // namespace OHOS
