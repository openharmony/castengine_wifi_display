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

#ifndef OHOS_SHARING_RTP_QUEUE_H
#define OHOS_SHARING_RTP_QUEUE_H

#include <cstdlib>
#include <functional>
#include <map>
#include "rtp_packet.h"

namespace OHOS {
namespace Sharing {
class RtpPacketSortor {
public:
    using Ptr = std::shared_ptr<RtpPacketSortor>;
    using OnSort = std::function<void(uint16_t seq, const RtpPacket::Ptr &packet)>;

    RtpPacketSortor(int32_t sampleRate, size_t kMax = 1024, size_t kMin = 16);
    ~RtpPacketSortor() = default;

    void Clear();
    void Flush();
    void SetOnSort(const OnSort &cb);
    void SortPacket(uint16_t seq, RtpPacket::Ptr packet);
    void InputRtp(TrackType type, uint8_t *ptr, size_t len);

    uint32_t GetSSRC() const;

    size_t GetJitterSize() const;
    size_t GetCycleCount() const;

private:
    void PopPacket();
    void SetSortSize();
    void TryPopPacket();
    void PopIterator(std::map<uint16_t, RtpPacket::Ptr>::iterator it);

private:
    uint16_t nextSeqOut_ = 0;

    uint32_t ssrc_ = 0;
    int32_t sampleRate_ = 0;

    size_t kMin_ = 16;
    size_t kMax_ = 1024;
    size_t seqCycleCount_ = 0;
    size_t maxSortSize_ = kMin_;

    std::map<uint16_t, RtpPacket::Ptr> pktSortCacheMap_;

    OnSort onSort_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif