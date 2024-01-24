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

#include "rtcp_context.h"
#include <netinet/in.h>
#include "common/common_macro.h"
#include "common/media_log.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {
void RtcpContext::OnRtp(uint16_t /*seq*/, uint32_t stamp, uint64_t ntpStampMs, uint32_t /*sampleRate*/, size_t bytes)
{
    ++packets_;
    bytes_ += bytes;
    lastRtpStamp_ = stamp;
    lastNtpStampMs_ = ntpStampMs;
}

//------------------------------ RtcpSenderContext ------------------------------//
DataBuffer::Ptr RtcpSenderContext::CreateRtcpSR(uint32_t rtcpSSRC)
{
    auto rtcp = RtcpSR::Create(0);
    rtcp->SetNtpStamp(lastNtpStampMs_);
    rtcp->rtpts_ = htonl(lastRtpStamp_);
    rtcp->ssrc_ = htonl(rtcpSSRC);
    rtcp->packetCount_ = htonl((uint32_t)packets_);
    rtcp->octetCount_ = htonl((uint32_t)bytes_);

    auto lastSrLsr =
        ((ntohl(rtcp->ntpmsw_) & 0xFFFF) << 16) | ((ntohl(rtcp->ntplsw_) >> 16) & 0xFFFF); // 16:byte offset
    senderReportNtp_[lastSrLsr] = GetCurrentMillisecond();
    if (senderReportNtp_.size() >= 5) { // 5:fixed capacity
        senderReportNtp_.erase(senderReportNtp_.begin());
    }

    DataBuffer::Ptr ret = std::make_shared<DataBuffer>();
    ret->PushData((char *)rtcp.get(), rtcp->GetSize());
    return ret;
}

void RtcpSenderContext::OnRtcp(RtcpHeader *rtcp)
{
    RETURN_IF_NULL(rtcp);
    switch ((RtcpType)rtcp->pt_) {
        case RtcpType::RTCP_RR: {
            auto rtcpRR = (RtcpRR *)rtcp;
            for (auto &item : rtcpRR->GetItemList()) {
                if (!item->lastSrStamp_) {
                    continue;
                }
                auto it = senderReportNtp_.find(item->lastSrStamp_);
                if (it == senderReportNtp_.end()) {
                    continue;
                }
                // time: sender (send SR) -> receiver (recv SR) -> receiver (send RR) -> sender (recv RR)
                auto msInc = GetCurrentMillisecond() - it->second;
                // time: receiver (recv SR) -> receiver (send RR)
                auto delayMs = (uint64_t)item->delaySinceLastSr_ * 1000 / 65536; // 1000:unit, 65536:max seq
                // time: [sender (send SR) -> receiver (recv SR)] + [receiver (send RR) -> sender (recv RR)]
                auto rtt = (int32_t)(msInc - delayMs);
                if (rtt >= 0) {
                    rtt_[item->ssrc_] = rtt;
                }
            }
            break;
        }
        case RtcpType::RTCP_XR: {
            auto rtcp_xr = (RtcpXRRRTR *)rtcp;
            if (rtcp_xr->bt_ == 4) { // 4:xrXrrtr
                xrXrrtrRecvLastRr_[rtcp_xr->ssrc_] =
                    ((rtcp_xr->ntpmsw_ & 0xFFFF) << 16) | ((rtcp_xr->ntplsw_ >> 16) & 0xFFFF); // 16:byte offset
                xrRrtrRecvSysStamp_[rtcp_xr->ssrc_] = GetCurrentMillisecond();
            } else if (rtcp_xr->bt_ == 5) { // 5:dlrr
                SHARING_LOGD("for sender not recive dlrr.");
            } else {
                SHARING_LOGD("not support xr bt.");
            }
            break;
        }
        default:
            break;
    }
}

DataBuffer::Ptr RtcpSenderContext::CreateRtcpXRDLRR(uint32_t rtcp_ssrc, uint32_t rtp_ssrc)
{
    return nullptr;
}

uint32_t RtcpSenderContext::GetRtt(uint32_t ssrc) const
{
    auto it = rtt_.find(ssrc);
    if (it == rtt_.end()) {
        return 0;
    }

    return it->second;
}

//------------------------------ RtcpReceiverContext ------------------------------//
void RtcpReceiverContext::OnRtp(uint16_t seq, uint32_t stamp, uint64_t ntpStampMs, uint32_t sampleRate, size_t bytes)
{
    // the receiver performs complex statistical operations
    auto sysStamp = GetCurrentMillisecond();
    if (lastRtpSysStamp_) {
        // calculate timestamp jitter value
        double diff = double((int64_t(sysStamp) - int64_t(lastRtpSysStamp_)) * (sampleRate / double(1000.0)) -
                             (int64_t(stamp) - int64_t(lastRtpStamp_)));
        if (diff < 0) {
            diff = -diff;
        }
        // jitter unit: sampling numbers
        jitter_ += (diff - jitter_) / 16.0; // 16.0:jitter unit
    } else {
        jitter_ = 0;
    }

    if (lastRtpSeq_ > 0xFF00 && seq < 0xFF && (!seqCycles_ || packets_ - lastCyclePackets_ > 0x1FFF)) {
        // if the last seq > 0xff00 and the current seq < 0xff,
        // and no loopback occurs or the interval from the last loopback exceeds 0x1fff packets,
        // the loopback is considered
        ++seqCycles_;
        lastCyclePackets_ = packets_;
        seqMax_ = seq;
    } else if (seq > seqMax_) {
        // maximum seq before loop back
        seqMax_ = seq;
    }

    if (!seqBase_) {
        // record the first rtp's seq
        seqBase_ = seq;
    } else if (!seqCycles_ && seq < seqBase_) {
        // if no loopback occurs, the latest seq is taken as the base seq
        seqBase_ = seq;
    }

    lastRtpSeq_ = seq;
    lastRtpSysStamp_ = sysStamp;

    RtcpContext::OnRtp(seq, stamp, ntpStampMs, sampleRate, bytes);
}

void RtcpReceiverContext::OnRtcp(RtcpHeader *rtcp)
{
    RETURN_IF_NULL(rtcp);
    switch ((RtcpType)rtcp->pt_) {
        case RtcpType::RTCP_SR: {
            auto rtcpSR = (RtcpSR *)rtcp;
            // last SR timestamp (LSR): 32 bits
            //  The middle 32 bits out of 64 in the NTP timestamp (as explained in
            //  Section 4) received as part of the most recent RTCP sender report
            //  (SR) packet from source SSRC_n.  If no SR has been received yet,
            //  the field is set to zero.
            lastSrLsr_ = (((ntohl(rtcpSR->ntpmsw_) & 0xffff) << 16) | // 16:byte offset
                          ((ntohl(rtcpSR->ntplsw_) >> 16) & 0xffff)); // 16:byte offset
            lastSrNtpSys_ = GetCurrentMillisecond();
            break;
        }
        default:
            break;
    }
}

DataBuffer::Ptr RtcpReceiverContext::CreateRtcpRR(uint32_t rtcpSSRC, uint32_t rtpSSRC)
{
    auto rtcp = RtcpRR::Create(1);
    rtcp->ssrc_ = htonl(rtcpSSRC);

    ReportItem *item = (ReportItem *)&rtcp->items_;
    item->ssrc_ = htonl(rtpSSRC);

    uint8_t fraction = 0;
    auto expectedInterval = GetExpectedPacketsInterval();
    if (expectedInterval != 0) {
        fraction = uint8_t((GetLostInterval() << 8) / expectedInterval); // 8:byte offset
    }

    // fraction = packet loss rate (percentage) * 256
    item->fractionLost_ = fraction;
    item->cumulative_ = htonl(uint32_t(GetLost())) >> 8; // 8:byte offset
    item->seqCycles_ = htons(seqCycles_);
    item->seqMax_ = htons(seqMax_);
    item->jitter_ = htonl(uint32_t(jitter_));
    item->lastSrStamp_ = htonl(lastSrLsr_);

    // now - recv Last SR time
    auto delay = GetCurrentMillisecond() - lastSrNtpSys_;
    // in units of 1/65536 seconds
    auto dlsr = (uint32_t)(delay / 1000.0f * 65536); // 1000.0:unit, 65536:max seq
    item->delaySinceLastSr_ = htonl(lastSrLsr_ ? dlsr : 0);

    DataBuffer::Ptr ret = std::make_shared<DataBuffer>();
    ret->PushData((char *)rtcp.get(), rtcp->GetSize());
    return ret;
}

size_t RtcpReceiverContext::GetExpectedPackets() const
{
    return (seqCycles_ << 16) + seqMax_ - seqBase_ + 1; // 16:byte offset
}

size_t RtcpReceiverContext::GetExpectedPacketsInterval()
{
    auto expected = GetExpectedPackets();
    auto ret = expected - lastExpected_;
    lastExpected_ = expected;
    return ret;
}

size_t RtcpReceiverContext::GetLost()
{
    return GetExpectedPackets() - packets_;
}

size_t RtcpReceiverContext::GetLostInterval()
{
    auto lost = GetLost();
    auto ret = lost - lastLost_;
    lastLost_ = lost;
    return ret;
}

} // namespace Sharing
} // namespace OHOS