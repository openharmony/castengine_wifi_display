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

#include "rtp_unpack_impl.h"
#include <arpa/inet.h>
#include <chrono>
#include "common/common_macro.h"
#include "common/media_log.h"
#include "rtp_codec_aac.h"
#include "rtp_codec_g711.h"
#include "rtp_codec_h264.h"
#include "rtp_codec_ts.h"

namespace OHOS {
namespace Sharing {
RtpUnpackImpl::RtpUnpackImpl(const RtpPlaylodParam &rpp)
{
    CreateRtpDecoder(rpp);
}

RtpUnpackImpl::~RtpUnpackImpl()
{
    SHARING_LOGI("~RtpUnpackImpl.");
    Release();
}

void RtpUnpackImpl::ParseRtp(const char *data, size_t len)
{
    RtpHeader *header = (RtpHeader *)data;
    auto pt = header->pt_;
    auto decoder = rtpDecoder_[pt];

    if (!decoder) {
        if (rtpDecoder_.size() > 2) { // 2:fixed size
            return;
        }
        switch (pt) {
            case 33: { // 33:mpeg-2 ts code
                // todo judge ps/ts
                CreateRtpDecoder(RtpPlaylodParam{pt, 90000, RtpPayloadStream::MPEG2_TS}); // 90000:audio clock cycle
                break;
            }
            default:
                // todo not support this pt
                return;
        }
    }

    rtpSort_[pt]->InputRtp(TRACK_VIDEO, (unsigned char *)data, len);
}

void RtpUnpackImpl::SetOnRtpUnpack(const OnRtpUnpack &cb)
{
    onRtpUnpack_ = std::move(cb);
}

void RtpUnpackImpl::SetOnRtpNotify(const OnRtpNotify &cb)
{
    onRtpNotify_ = std::move(cb);
}

void RtpUnpackImpl::OnRtpSorted(uint16_t seq, const RtpPacket::Ptr &rtp)
{
    RETURN_IF_NULL(rtp);
    if (rtpDecoder_[rtp->GetHeader()->pt_]) {
        MEDIA_LOGD("rtpUnpackImpl::OnRtpSorted seq: %{public}d, pt: %{public}d.", rtp->GetSeq(), rtp->GetHeader()->pt_);

        if (nextOutSeq_ == 0xffff) {
            nextOutSeq_ = 1;
        } else {
            nextOutSeq_ = rtp->GetSeq() + 1;
        }

        rtpDecoder_[rtp->GetHeader()->pt_]->InputRtp(rtp);
    }
}

void RtpUnpackImpl::OnRtpDecode(int32_t pt, const Frame::Ptr &frame)
{
    if (onRtpUnpack_) {
        onRtpUnpack_(rtpSort_[pt]->GetSSRC(), frame);
    }
}

void RtpUnpackImpl::Release()
{
    rtpDecoder_.clear();
    rtpSort_.clear();
}

void RtpUnpackImpl::CreateRtpDecoder(const RtpPlaylodParam &rpp)
{
    switch (rpp.ps_) {
        case RtpPayloadStream::H264:
            rtpDecoder_[rpp.pt_] = std::make_shared<RtpDecoderH264>();
            break;
        case RtpPayloadStream::MPEG4_GENERIC:
            rtpDecoder_[rpp.pt_] = std::make_shared<RtpDecoderAAC>(rpp);
            break;
        case RtpPayloadStream::PCMA: // fall-through
        case RtpPayloadStream::PCMU:
            rtpDecoder_[rpp.pt_] = std::make_shared<RtpDecoderG711>();
            break;
        case RtpPayloadStream::MPEG2_TS:
            rtpDecoder_[rpp.pt_] = std::make_shared<RtpDecoderTs>();
            break;
        case RtpPayloadStream::MPEG2_PS:
            break;
        default:
            // todo log
            break;
    }
    if (rtpDecoder_[rpp.pt_]) {
        auto &ref = rtpSort_[rpp.pt_];
        ref = std::make_shared<RtpPacketSortor>(rpp.sampleRate_);
        ref->SetOnSort(std::bind(&RtpUnpackImpl::OnRtpSorted, this, std::placeholders::_1, std::placeholders::_2));
        rtpDecoder_[rpp.pt_]->SetOnFrame(std::bind(&RtpUnpackImpl::OnRtpDecode, this, rpp.pt_, std::placeholders::_1));
    }
}
} // namespace Sharing
} // namespace OHOS