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

#include "rtp_pack_impl.h"
#include "rtp_codec_g711.h"
#include "rtp_codec_h264.h"
#include "rtp_codec_ts.h"
namespace OHOS {
namespace Sharing {
RtpPackImpl::RtpPackImpl(uint32_t ssrc, size_t mtuSize, uint32_t sampleRate, uint8_t pt, RtpPayloadStream ps,
                         uint32_t channels)
    : pt_(pt), ssrc_(ssrc), channels_(channels), sampleRate_(sampleRate), mtuSize_(mtuSize), ps_(ps)
{
    InitEncoder();
}

void RtpPackImpl::InputFrame(const Frame::Ptr &frame)
{
    if (rtpEncoder_) {
        rtpEncoder_->InputFrame(frame);
    }
}

void RtpPackImpl::SetOnRtpPack(const OnRtpPack &cb)
{
    onRtpPack_ = cb;
}

void RtpPackImpl::InitEncoder()
{
    switch (ps_) {
        case RtpPayloadStream::H264:
            rtpEncoder_ = std::make_shared<RtpEncoderH264>(ssrc_, mtuSize_, sampleRate_, pt_, seq_);
            break;
        case RtpPayloadStream::MPEG4_GENERIC:
            break;
        case RtpPayloadStream::PCMA: // fall-through
        case RtpPayloadStream::PCMU:
            rtpEncoder_ = std::make_shared<RtpEncoderG711>(ssrc_, mtuSize_, sampleRate_, pt_, channels_, seq_);
            break;
        case RtpPayloadStream::MPEG2_TS:
            rtpEncoder_ = std::make_shared<RtpEncoderTs>(ssrc_, mtuSize_, sampleRate_, pt_, seq_);
            break;
        case RtpPayloadStream::MPEG2_PS:
            break;
        default:
            // todo log
            break;
    }
    if (rtpEncoder_) {
        rtpEncoder_->SetOnRtpPack([this](const RtpPacket::Ptr &rtp) { onRtpPack_(rtp); });
    }
}
} // namespace Sharing
} // namespace OHOS
