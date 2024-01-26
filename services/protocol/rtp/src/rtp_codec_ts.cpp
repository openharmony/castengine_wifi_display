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

#include "rtp_codec_ts.h"
#include "adts.h"
#include "common/common_macro.h"
#include "common/media_log.h"
#include "frame/aac_frame.h"
#include "frame/h264_frame.h"
#include "mpeg_ts/mpeg_ts_demuxer.h"
#include "mpeg_ts/mpeg_ts_muxer.h"

constexpr uint32_t TIMESTAMP_DIVIDER = 90;
namespace OHOS {
namespace Sharing {
RtpDecoderTs::RtpDecoderTs()
{
    MEDIA_LOGD("RtpDecoderTs CTOR IN.");
    tsDemuxer_ = std::make_shared<MpegTsDemuxer>();
    merger_.SetType(FrameMerger::NONE);

    tsDemuxer_->SetOnDecode([this](int32_t stream, int32_t codecId, int32_t flags, int64_t pts, int64_t dts,
                                   const void *data,
                                   size_t bytes) { OnDecode(stream, codecId, flags, pts, dts, data, bytes); });
    tsDemuxer_->SetOnStream([this](int32_t stream, int32_t codecId, const void *extra, size_t bytes, int32_t finish) {
        OnStream(stream, codecId, extra, bytes, finish);
    });
}

RtpDecoderTs::~RtpDecoderTs()
{
    MEDIA_LOGD("RtpDecoderTs DTOR IN.");
}

void RtpDecoderTs::InputRtp(const RtpPacket::Ptr &rtp)
{
    MEDIA_LOGD("RtpDecoderTs::InputRtp IN.");
    RETURN_IF_NULL(rtp);
    if (tsDemuxer_) {
        tsDemuxer_->Input(rtp->GetPayload(), rtp->GetPayloadSize());
    }
}

void RtpDecoderTs::SetOnFrame(const OnFrame &cb)
{
    onFrame_ = cb;
}

void RtpDecoderTs::OnDecode(int32_t stream, int32_t codecId, int32_t flags, int64_t pts, int64_t dts, const void *data,
                            size_t bytes)
{
    MEDIA_LOGD("RtpDecoderTs::OnDecode stream: %{public}d codecID: %{public}d flag: %{public}d pts: %{public}" PRIx64
               " dts: "
               "%{public}" PRIx64 " data len: %{public}zu\n.",
               stream, codecId, flags, pts, dts, bytes);
    pts /= TIMESTAMP_DIVIDER;
    dts /= TIMESTAMP_DIVIDER;

    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();

    switch (codecId) {
        case PSI_STREAM_H264: {
            auto inFrame = std::make_shared<H264Frame>((uint8_t *)data, bytes, (uint32_t)dts, (uint32_t)pts,
                                                       PrefixSize((char *)data, bytes));
            auto fn = [this](uint32_t dts, uint32_t pts, const DataBuffer::Ptr &buffer, bool have_key_frame) {
                MEDIA_LOGD("RtpDecoderTs H264 merger output success.");
                auto outFrame =
                    std::make_shared<H264Frame>(buffer->Data(), buffer->Size(), (uint32_t)dts, (uint32_t)pts,
                                                PrefixSize((char *)buffer->Data(), buffer->Size()));
                onFrame_(outFrame);
            };
            merger_.InputFrame(inFrame, buffer, fn);
            break;
        }

        case PSI_STREAM_AAC: {
            uint8_t *ptr = (uint8_t *)data;
            if (!(bytes > 7 && ptr[0] == 0xFF && (ptr[1] & 0xF0) == 0xF0)) { // 7:fixed size
                break;
            } else {
                auto outFrame = std::make_shared<AACFrame>((uint8_t *)data, bytes, (uint32_t)dts, (uint32_t)pts);
                onFrame_(outFrame);
            }
            break;
        }
        default:
            break;
    }
}

void RtpDecoderTs::OnStream(int32_t stream, int32_t codecId, const void *extra, size_t bytes, int32_t finish)
{
    MEDIA_LOGD("rtpDecoderTs::OnStream stream: %{public}d codecID: %{public}d data len: %{public}zu.", stream, codecId,
               bytes);
    // todo
}

void RtpDecoderTs::FrameReady(const Frame::Ptr &frame)
{
    if (onFrame_) {
        onFrame_(frame);
    }
}

RtpEncoderTs::RtpEncoderTs(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq)
    : RtpMaker(ssrc, mtuSize, payloadType, sampleRate, seq)
{
    MEDIA_LOGD("RtpEncoderTs CTOR IN");
    merger_.SetType(FrameMerger::H264_PREFIX);
    tsMuxer_ = std::make_shared<MpegTsMuxer>();
    tsMuxer_->SetOnMux([this](const std::shared_ptr<DataBuffer> &buffer, uint32_t stamp, bool keyPos) {
        if (onRtpPack_) {
            auto rtp = MakeRtp(reinterpret_cast<const void *>(buffer->Peek()), buffer->Size(), keyPos, stamp);
            onRtpPack_(rtp);
        }
    });
}

RtpEncoderTs::~RtpEncoderTs()
{
    MEDIA_LOGD("RtpEncoderTs DTOR IN");
}

void RtpEncoderTs::InputFrame(const Frame::Ptr &frame)
{
    if (!frame) {
        return;
    }
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    switch (frame->GetCodecId()) {
        case CODEC_H264:
            // for sps, pps and key frame in one packet
            merger_.InputFrame(
                frame, buffer, [this](uint32_t dts, uint32_t pts, const DataBuffer::Ptr &buffer, bool have_key_frame) {
                    MEDIA_LOGD("RtpEncoderTs H264 merger output success, dts %{public}d,"
                               "pts %{public}d,BufferSize %{public}d, havekeyframe %{public}d",
                               dts, pts, buffer->Size(), have_key_frame);
                    // have_key_frame
                    auto outFrame =
                        std::make_shared<H264Frame>(buffer->Data(), buffer->Size(), (uint32_t)dts, (uint32_t)pts,
                                                    PrefixSize((char *)buffer->Data(), buffer->Size()));
                    tsMuxer_->InputFrame(outFrame);
                });
            break;
        case CODEC_AAC:
            tsMuxer_->InputFrame(frame);
            break;
        default:
            MEDIA_LOGW("Unknown codec: %d", frame->GetCodecId());
            break;
    }
}

void RtpEncoderTs::SetOnRtpPack(const OnRtpPack &cb)
{
    onRtpPack_ = cb;
}
} // namespace Sharing
} // namespace OHOS
