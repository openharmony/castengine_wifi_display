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

#include "rtp_packet.h"
#include <arpa/inet.h>
#include <cstdlib>

namespace OHOS {
namespace Sharing {
#define AV_RB16(x) ((((const uint8_t *)(x))[0] << 8) | ((const uint8_t *)(x))[1])

size_t RtpHeader::GetCsrcSize() const
{
    return csrc_ << 2; // 2:byte offset
}

uint8_t *RtpHeader::GetCsrcData()
{
    if (!csrc_) {
        return nullptr;
    }

    return &payload_;
}

size_t RtpHeader::GetExtSize() const
{
    if (!ext_) {
        return 0;
    }
    auto ext_ptr = &payload_ + GetCsrcSize();

    return AV_RB16(ext_ptr + 2) << 2; // 2:byte offset
}

uint16_t RtpHeader::GetExtReserved() const
{
    if (!ext_) {
        return 0;
    }
    auto ext_ptr = &payload_ + GetCsrcSize();
    return AV_RB16(ext_ptr);
}

uint8_t *RtpHeader::GetExtData()
{
    if (!ext_) {
        return nullptr;
    }
    auto ext_ptr = &payload_ + GetCsrcSize();

    return ext_ptr + 4; // 4:byte offset
}

size_t RtpHeader::GetPayloadOffset() const
{
    return GetCsrcSize() + (ext_ ? (4 + GetExtSize()) : 0); // 4:byte offset
}

uint8_t *RtpHeader::GetPayloadData()
{
    return &payload_ + GetPayloadOffset();
}

size_t RtpHeader::GetPaddingSize(size_t rtp_size) const
{
    if (!padding_) {
        return 0;
    }
    auto end = (uint8_t *)this + rtp_size - 1;
    return *end;
}

size_t RtpHeader::GetPayloadSize(size_t rtp_size) const
{
    auto invalid_size = GetPayloadOffset() + GetPaddingSize(rtp_size);
    if (invalid_size + RtpPacket::RTP_HEADER_SIZE >= rtp_size) {
        return 0;
    }

    return rtp_size - invalid_size - RtpPacket::RTP_HEADER_SIZE;
}

RtpHeader *RtpPacket::GetHeader()
{
    return (RtpHeader *)Data();
}

uint16_t RtpPacket::GetSeq()
{
    return ntohs(GetHeader()->seq_);
}

uint32_t RtpPacket::GetStamp()
{
    return ntohl(GetHeader()->stamp_);
}

uint32_t RtpPacket::GetStampMS()
{
    return GetStamp() * uint64_t(1000) / sampleRate_; // 1000:unit
}

uint32_t RtpPacket::GetSSRC()
{
    return ntohl(GetHeader()->ssrc_);
}

uint8_t *RtpPacket::GetPayload()
{
    return GetHeader()->GetPayloadData();
}

size_t RtpPacket::GetPayloadSize()
{
    return GetHeader()->GetPayloadSize(Size());
}
} // namespace Sharing
} // namespace OHOS