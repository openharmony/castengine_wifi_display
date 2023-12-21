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

#ifndef OHOS_SHARING_RTP_PACKGET_H
#define OHOS_SHARING_RTP_PACKGET_H

#include <cstdint>
#include <cstdio>
#include <memory>
#include "frame/frame.h"

namespace OHOS {
namespace Sharing {
class RtpHeader {
public:
    uint8_t *GetExtData();
    uint8_t *GetCsrcData();
    uint8_t *GetPayloadData();
    uint16_t GetExtReserved() const;

    size_t GetExtSize() const;
    size_t GetCsrcSize() const;
    size_t GetPayloadSize(size_t rtp_size) const;

private:
    size_t GetPayloadOffset() const;
    size_t GetPaddingSize(size_t rtp_size) const;

public:
#if __BYTE_ORDER == __BIG_ENDIAN

    uint32_t version_ : 2;
    // padding
    uint32_t padding_ : 1;

    uint32_t ext_ : 1;
    // csrc
    uint32_t csrc_ : 4;
    // mark
    uint32_t mark_ : 1;

    uint32_t pt_ : 7;
#else
    // csrc
    uint32_t csrc_ : 4;

    uint32_t ext_ : 1;
    // padding
    uint32_t padding_ : 1;

    uint32_t version_ : 2;

    uint32_t pt_ : 7;
    // mark
    uint32_t mark_ : 1;
#endif

    uint32_t seq_ : 16;

    uint32_t stamp_;
    // ssrc
    uint32_t ssrc_;

    uint8_t payload_;
} __attribute__((packed));

class RtpPacket : public DataBuffer {
public:
    using Ptr = std::shared_ptr<RtpPacket>;
    enum {
        RTP_VERSION = 2,
        RTP_HEADER_SIZE = 12,
    };

    uint16_t GetSeq();
    uint32_t GetSSRC();
    uint32_t GetStamp();
    uint8_t *GetPayload();
    uint32_t GetStampMS();
    RtpHeader *GetHeader();
    size_t GetPayloadSize();

public:
    uint32_t sampleRate_;

    uint64_t ntpStamp_;

    TrackType type_ = TRACK_INVALID;
};
} // namespace Sharing
} // namespace OHOS
#endif