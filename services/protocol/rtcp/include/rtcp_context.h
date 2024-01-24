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

#ifndef OHOS_SHARING_RTCP_CONTEXT_H
#define OHOS_SHARING_RTCP_CONTEXT_H

#include <map>
#include <cstdint>
#include "rtcp.h"
#include "rtcp_def.h"
#include "utils/data_buffer.h"

namespace OHOS {
namespace Sharing {
class RtcpContext {
public:
    virtual ~RtcpContext() = default;

    virtual void OnRtcp(RtcpHeader *rtcp) = 0;

    // seq: rtp seq, stamp: rtp stamp(samples), sampleRate: 90000 for video, bytes: rtp packet size
    virtual void OnRtp(uint16_t seq, uint32_t stamp, uint64_t ntpStampMs, uint32_t sampleRate, size_t bytes);

    virtual DataBuffer::Ptr CreateRtcpSR(uint32_t rtcpSSRC)
    {
        return nullptr;
    }

    virtual DataBuffer::Ptr CreateRtcpXRDLRR(uint32_t rtcpSSRC, uint32_t rtpSSRC)
    {
        return nullptr;
    }

    virtual DataBuffer::Ptr CreateRtcpRR(uint32_t rtcpSSRC, uint32_t rtpSSRC)
    {
        return nullptr;
    }

    virtual size_t GetLost()
    {
        return 0;
    }

    virtual size_t GetLostInterval()
    {
        return 0;
    }

    virtual size_t GetExpectedPackets() const
    {
        return 0;
    }

    virtual size_t GetExpectedPacketsInterval()
    {
        return 0;
    }

protected:
    uint32_t lastRtpStamp_ = 0;
    // size of rtp packets
    size_t bytes_ = 0;
    // number of rtp packets
    size_t packets_ = 0;
    uint64_t lastNtpStampMs_ = 0;
};

class RtcpSenderContext : public RtcpContext {
public:
    uint32_t GetRtt(uint32_t ssrc) const;
    void OnRtcp(RtcpHeader *rtcp) override;

    DataBuffer::Ptr CreateRtcpSR(uint32_t rtcp_ssrc) override;
    DataBuffer::Ptr CreateRtcpXRDLRR(uint32_t rtcp_ssrc, uint32_t rtp_ssrc) override;

private:
    std::map<uint32_t, uint32_t> rtt_; // ssrc, rtt
    std::map<uint32_t, uint32_t> xrXrrtrRecvLastRr_; // ssrc, last rr
    std::map<uint32_t, uint64_t> senderReportNtp_; // last sr, lsr ntp stamp
    std::map<uint32_t, uint64_t> xrRrtrRecvSysStamp_; // ssrc, xr rrtr sys stamp
};

class RtcpReceiverContext : public RtcpContext {
public:
    using Ptr = std::shared_ptr<RtcpReceiverContext>;

    void OnRtcp(RtcpHeader *rtcp) override;
    void OnRtp(uint16_t seq, uint32_t stamp, uint64_t ntpStampMs, uint32_t sampleRate, size_t bytes) override;

    DataBuffer::Ptr CreateRtcpRR(uint32_t rtcp_ssrc, uint32_t rtp_ssrc) override;

    size_t GetLost() override;
    size_t GetLostInterval() override;
    size_t GetExpectedPackets() const override;
    size_t GetExpectedPacketsInterval() override;

private:
    // max rtp seq num
    uint16_t seqMax_ = 0;
    // first seq num
    uint16_t seqBase_ = 0;
    // rtp cycle num
    uint16_t seqCycles_ = 0;
    uint16_t lastRtpSeq_ = 0;

    // last SR timestamp
    uint32_t lastSrLsr_ = 0;

    // timestamp jitter
    double jitter_ = 0;
    // ntp system time
    uint64_t lastSrNtpSys_ = 0;
    uint64_t lastRtpSysStamp_ = 0;

    size_t lastLost_ = 0;
    // last expected rtp packet num
    size_t lastExpected_ = 0;
    // rtp packet num of last
    size_t lastCyclePackets_ = 0;
};

} // namespace Sharing
} // namespace OHOS

#endif