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

#include "rtp_maker.h"
#include <arpa/inet.h>
#include <securec.h>

namespace OHOS {
namespace Sharing {
RtpMaker::RtpMaker(uint32_t ssrc, size_t mtuSize, uint8_t payloadType, uint32_t sampleRate, uint16_t seq)
    : pt_(payloadType), seq_(seq), ssrc_(ssrc), sampleRate_(sampleRate), mtuSize_(mtuSize)
{
    if (ssrc_ == 0) {
        ssrc_ = ((uint64_t)this) & 0xFFFFFFFF;
    }
}

RtpMaker::~RtpMaker() {}

size_t RtpMaker::GetMaxSize() const
{
    return mtuSize_ - RtpPacket::RTP_HEADER_SIZE;
}

uint32_t RtpMaker::GetSsrc() const
{
    return ssrc_;
}

RtpPacket::Ptr RtpMaker::MakeRtp(const void *data, size_t len, bool mark, uint32_t stamp)
{
    uint16_t size = (uint16_t)(len + RtpPacket::RTP_HEADER_SIZE);
    auto rtp = std::make_shared<RtpPacket>();
    rtp->SetCapacity(size);
    rtp->SetSize(size);

    auto header = rtp->GetHeader();
    header->version_ = RtpPacket::RTP_VERSION;
    header->padding_ = 0;
    header->ext_ = 0;
    header->csrc_ = 0;
    header->mark_ = mark;
    header->pt_ = pt_;
    header->seq_ = htons(seq_);
    ++seq_;
    header->stamp_ = htonl(uint64_t(stamp) * (sampleRate_ / 1000)); // 1000:unit
    header->ssrc_ = htonl(ssrc_);

    if (data) {
        auto rtpData = rtp->Data();

        auto ret = memcpy_s(rtpData + RtpPacket::RTP_HEADER_SIZE, len, data, len);
        if (ret != EOK) {
            return nullptr;
        }
    }

    return rtp;
}
} // namespace Sharing
} // namespace OHOS