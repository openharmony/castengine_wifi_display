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
#include "common/common_macro.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {

static const uint16_t MAX_SEQ = 0xFFFF;
static const uint16_t HALF_MAX_SEQ = (MAX_SEQ >> 1);

RtpPacketSortor::RtpPacketSortor(int32_t sampleRate, size_t kMax, size_t kMin)
    : sampleRate_(sampleRate), kMin_(kMin), kMax_(kMax)
{}

void RtpPacketSortor::InputRtp(TrackType type, uint8_t *ptr, size_t len)
{
    RETURN_IF_NULL(ptr);
    if (len < RtpPacket::RTP_HEADER_SIZE) {
        // hilog rtp packet is too small
        SHARING_LOGE("ignore rtp, invalid size:%{public}zu", len);
        return;
    }
    // todo jduge rtp size
    if (!sampleRate_) {
        SHARING_LOGE("ignore rtp, invalid sampleRate:%{public}d", sampleRate_);
        return;
    }
    RtpHeader *header = (RtpHeader *)ptr;
    if (header->version_ != RtpPacket::RTP_VERSION) {
        SHARING_LOGE("ignore rtp, invalid version:%{public}u", header->version_);
        return;
    }
    if (!header->GetPayloadSize(len)) {
        SHARING_LOGE("ignore rtp, invalid payload size");
        return;
    }

    auto rtp = std::make_shared<RtpPacket>();
    RETURN_IF_NULL(rtp);
    rtp->ReplaceData(reinterpret_cast<char*>(ptr), len);
    rtp->sampleRate_ = (uint32_t)sampleRate_;
    rtp->type_ = type;

    if ((size_t)rtp->Size() != len) {
        SHARING_LOGE("ignore rtp seq:%{public}hu, size mismatch", rtp->GetSeq());
        return;
    }
    MEDIA_LOGD("rtp payload size: %{public}zu.", rtp->GetPayloadSize());

    if (ssrc_ != rtp->GetSSRC()) {
        ssrc_ = rtp->GetSSRC();
        Flush();
        Clear();
        nextSeqOut_ = rtp->GetSeq();
        SHARING_LOGI("ssrc change, seq:%{public}hu", rtp->GetSeq());
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
    pktSortCacheMap_.clear();
    nextSeqOut_ = 0;
    maxSortSize_ = kMin_;
}

size_t RtpPacketSortor::GetJitterSize() const
{
    return pktSortCacheMap_.size();
}

bool RtpPacketSortor::IsSeqValid(uint16_t seq) const
{
    if (seq < nextSeqOut_) {
        if (nextSeqOut_ - seq < HALF_MAX_SEQ) {
            return false;
        }
        // in case of 65535->0, may receive seq < nextSeqOut_
    } else if (seq - nextSeqOut_ > HALF_MAX_SEQ) {
        return false;
    }
    return true;
}

void RtpPacketSortor::SortPacket(uint16_t seq, RtpPacket::Ptr packet)
{
    RETURN_IF_NULL(packet);
    if (!IsSeqValid(seq)) {
        SHARING_LOGW("ignore rtp seq:%{public}hu out of expect range, expect seq:%{public}hu", seq, nextSeqOut_);
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

void RtpPacketSortor::PopIterator(std::map<uint16_t, RtpPacket::Ptr>::iterator it)
{
    auto seq = it->first;
    auto data = std::move(it->second);
    pktSortCacheMap_.erase(it);
    nextSeqOut_ = seq == MAX_SEQ ? 0 : seq + 1;
    if (onSort_) {
        onSort_(seq, data);
    }
}

void RtpPacketSortor::TryPopPacket()
{
    auto it = pktSortCacheMap_.lower_bound((nextSeqOut_));
    // eg: expect 65530, but map = {0,1,...} or expect 65530, but map = {65531, 65532,...}
    if (it == pktSortCacheMap_.end() || it->first != nextSeqOut_) {
        if (pktSortCacheMap_.size() <= maxSortSize_) {
            // cache not full, wait nextSeqOut_
            return;
        }
        // cache full, not wait nextSeqOut_ anymore,set next packet in cache as new expect seq
        if (it == pktSortCacheMap_.end()) {
            it = pktSortCacheMap_.begin();
        }
        nextSeqOut_ = it->first;
        SHARING_LOGI("cache full, set new expect seq as:%{public}hu", nextSeqOut_);
    }

    while (it != pktSortCacheMap_.end() && it->first == nextSeqOut_) {
        PopIterator(it);
        it = pktSortCacheMap_.lower_bound(nextSeqOut_);
    }
    SetSortSize();
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