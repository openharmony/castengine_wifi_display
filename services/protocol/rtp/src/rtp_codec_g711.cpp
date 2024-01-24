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

#include "rtp_codec_g711.h"
#include "common/common_macro.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
RtpDecoderG711::RtpDecoderG711()
{
    frame_ = ObtainFrame();
}

void RtpDecoderG711::InputRtp(const RtpPacket::Ptr &rtp)
{
    RETURN_IF_NULL(rtp);

    auto payload_size = rtp->GetPayloadSize();
    if (payload_size <= 0) {
        return;
    }
    auto payload = rtp->GetPayload();
    auto stamp = rtp->GetStampMS();
    auto seq = rtp->GetSeq();

    if (frame_->dts_ != stamp || frame_->Size() > maxFrameSize_) {
        if (frame_->Size()) {
            onFrame_(frame_);
        }

        frame_ = ObtainFrame();
        frame_->dts_ = stamp;

        dropFlag_ = false;
    } else if (lastSeq_ != 0 && (uint16_t)(lastSeq_ + 1) != seq) {
        MEDIA_LOGD("rtp lose: %{public}d -> %{public}d.", lastSeq_, seq);
        dropFlag_ = true;
        frame_->Clear();
    }

    if (!dropFlag_) {
        frame_->Append((char *)payload, payload_size);
    }

    lastSeq_ = seq;
    return;
}

void RtpDecoderG711::SetOnFrame(const OnFrame &cb)
{
    onFrame_ = cb;
}

FrameImpl::Ptr RtpDecoderG711::ObtainFrame()
{
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_G711A;
    return frame;
}

RtpEncoderG711::RtpEncoderG711(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType,
                               uint32_t channels, uint16_t seq)
    : RtpMaker(ssrc, mtuSize, payloadType, sampleRate, seq)
{
    cacheFrame_ = FrameImpl::Create();
    cacheFrame_->codecId_ = CODEC_G711A;
    channels_ = channels;
}

void RtpEncoderG711::InputFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    MEDIA_LOGD("rtpEncoderG711::InputFrame enter.");
    auto dur = (cacheFrame_->Size() - cacheFrame_->PrefixSize()) / (8 * channels_);
    auto next_pts = cacheFrame_->Pts() + dur;
    if (next_pts == 0) {
        cacheFrame_->pts_ = frame->Pts();
    } else {
        if ((next_pts + 20) < frame->Pts()) { // 20:interval ms
            cacheFrame_->pts_ = frame->Pts() - dur;
        }
    }

    cacheFrame_->Append(frame->Data() + frame->PrefixSize(), frame->Size() - frame->PrefixSize());

    auto stamp = cacheFrame_->Pts();
    auto ptr = cacheFrame_->Data() + cacheFrame_->PrefixSize();
    auto len = cacheFrame_->Size() - cacheFrame_->PrefixSize();
    auto rtpPack = MakeRtp(ptr, len, false, stamp);
    if (onRtpPack_) {
        MEDIA_LOGD("rtpEncoderG711::InputFrame onRtpPack data %{public}p size: %{public}d.", ptr, (int32_t)len);
        onRtpPack_(rtpPack);
    }

    cacheFrame_->Clear();
    cacheFrame_->pts_ += 46; // 371 / 8k = 46.375ms
}

void RtpEncoderG711::SetOnRtpPack(const OnRtpPack &cb)
{
    onRtpPack_ = cb;
}
} // namespace Sharing
} // namespace OHOS