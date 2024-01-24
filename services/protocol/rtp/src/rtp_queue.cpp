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

#include "rtp_queue.h"
#include <arpa/inet.h>
#include <iostream>
#include <limits>
#include <securec.h>
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
RtpPacketSortor::RtpPacketSortor(int32_t sampleRate, size_t kMax, size_t kMin)
    : sampleRate_(sampleRate), kMin_(kMin), kMax_(kMax)
{}

void RtpPacketSortor::InputRtp(TrackType type, uint8_t *ptr, size_t len)
{
    if (len < RtpPacket::RTP_HEADER_SIZE) {
        // hilog rtp packet is too small
        return;
    }
    // todo jduge rtp size
    if (!sampleRate_) {
        return;
    }
    RtpHeader *header = (RtpHeader *)ptr;
    if (header->version_ != RtpPacket::RTP_VERSION) {
        // hilog error version error
        return;
    }
    if (!header->GetPayloadSize(len)) {
        return;
    }

    auto rtp = std::make_shared<RtpPacket>();
    rtp->SetCapacity(len);
    rtp->SetSize(len);
    rtp->sampleRate_ = sampleRate_;
    rtp->type_ = type;

    auto data = rtp->Data();
    auto ret = memcpy_s(data, len, ptr, len);
    if (ret != EOK) {
        return;
    }
    MEDIA_LOGD("rtp payload size: %{public}zu.", rtp->GetPayloadSize());

    if (ssrc_ != rtp->GetSSRC() || rtp->GetSeq() == 0) {
        ssrc_ = rtp->GetSSRC();
        Flush();
        Clear();
        nextSeqOut_ = rtp->GetSeq();
    }

    SortPacket(rtp->GetSeq(), rtp);
    return;
}

void RtpPacketSortor::SetOnSort(const OnSort &cb)
{
    onSort_ = std::move(cb);
}

void RtpPacketSortor::Clear()
{
    seqCycleCount_ = 0;
    pktSortCacheMap_.clear();
    nextSeqOut_ = 0;
    maxSortSize_ = kMin_;
}

size_t RtpPacketSortor::GetJitterSize() const
{
    return pktSortCacheMap_.size();
}

size_t RtpPacketSortor::GetCycleCount() const
{
    return seqCycleCount_;
}

void RtpPacketSortor::SortPacket(uint16_t seq, RtpPacket::Ptr packet)
{
    if (seq < nextSeqOut_) {
        if (nextSeqOut_ < seq + kMax_) {
            return;
        }
    } else if (nextSeqOut_ && seq - nextSeqOut_ > ((std::numeric_limits<uint16_t>::max)() >> 1)) {
        MEDIA_LOGD("nextSeqOut_ && seq - nextSeqOut_ > ((std::numeric_limits<uint16_t>::max)() >> 1)");
        return;
    }

    pktSortCacheMap_.emplace(seq, std::move(packet));

    TryPopPacket();
}

void RtpPacketSortor::Flush()
{
    while (!pktSortCacheMap_.empty()) {
        PopIterator(pktSortCacheMap_.begin());
    }
}

uint32_t RtpPacketSortor::GetSSRC() const
{
    return ssrc_;
}

void RtpPacketSortor::PopPacket()
{
    auto it = pktSortCacheMap_.begin();
    if (it->first >= nextSeqOut_) {
        PopIterator(it);
        return;
    }

    if (nextSeqOut_ - it->first > (0xFFFF >> 1)) {
        if (pktSortCacheMap_.size() < 2 * kMin_) { // 2:fixed size
            return;
        }
        ++seqCycleCount_;

        auto hit = pktSortCacheMap_.upper_bound((nextSeqOut_ - pktSortCacheMap_.size()));
        while (hit != pktSortCacheMap_.end()) {
            onSort_(hit->first, hit->second);
            hit = pktSortCacheMap_.erase(hit);
        }

        PopIterator(pktSortCacheMap_.begin());
        return;
    }

    pktSortCacheMap_.erase(it);
}

void RtpPacketSortor::PopIterator(std::map<uint16_t, RtpPacket::Ptr>::iterator it)
{
    auto seq = it->first;
    auto data = std::move(it->second);
    pktSortCacheMap_.erase(it);
    nextSeqOut_ = seq + 1;
    onSort_(seq, data);
}

void RtpPacketSortor::TryPopPacket()
{
    int32_t count = 0;
    while ((!pktSortCacheMap_.empty() && pktSortCacheMap_.begin()->first == nextSeqOut_)) {
        PopPacket();
        ++count;
    }

    if (count) {
        SetSortSize();
    } else if (pktSortCacheMap_.size() > maxSortSize_) {
        PopPacket();
        SetSortSize();
    }
}

void RtpPacketSortor::SetSortSize()
{
    maxSortSize_ = kMin_ + pktSortCacheMap_.size();
    if (maxSortSize_ > kMax_) {
        maxSortSize_ = kMax_;
    }
}
} // namespace Sharing
} // namespace OHOS