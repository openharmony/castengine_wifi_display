/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rtp_encoder_h264.h"
#include <securec.h>
#include "common/common_macro.h"
#include "common/media_log.h"
#include "frame/h264_frame.h"

namespace OHOS {
namespace Sharing {
class FuFlags {
public:
#if __BYTE_ORDER == __BIG_ENDIAN
    unsigned startBit_ : 1;
    unsigned endBit_ : 1;
    unsigned reserved : 1;
    unsigned nalType : 5;
#else
    unsigned nalType_ : 5;
    unsigned reserved_ : 1;
    unsigned endBit_ : 1;
    unsigned startBit_ : 1;
#endif
} __attribute__((packed));

RtpEncoderH264::RtpEncoderH264(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq)
    : RtpMaker(ssrc, mtuSize, payloadType, sampleRate, seq)
{}

RtpEncoderH264::~RtpEncoderH264() {}

void RtpEncoderH264::InputFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    auto ptr = frame->Data() + frame->PrefixSize();
    RETURN_IF_NULL(ptr);
    switch (H264_TYPE(ptr[0])) {
        case H264Frame::NAL_SPS: {
            sps_ = frame;
            return;
        }
        case H264Frame::NAL_PPS: {
            pps_ = frame;
            return;
        }
        default:
            break;
    }

    if (lastFrame_) {
        InputFrame(lastFrame_, lastFrame_->Pts() != frame->Pts());
    }
    lastFrame_ = frame;
}

void RtpEncoderH264::SetOnRtpPack(const OnRtpPack &cb)
{
    onRtpPack_ = cb;
}

void RtpEncoderH264::InsertConfigFrame(uint32_t pts)
{
    if (!sps_ || !pps_) {
        return;
    }

    PackRtp(sps_->Data() + sps_->PrefixSize(), sps_->Size() - sps_->PrefixSize(), pts, false, false);
    PackRtp(pps_->Data() + pps_->PrefixSize(), pps_->Size() - pps_->PrefixSize(), pts, false, false);
}

bool RtpEncoderH264::InputFrame(const Frame::Ptr &frame, bool isMark)
{
    RETURN_FALSE_IF_NULL(frame);
    if (frame->KeyFrame()) {
        InsertConfigFrame(frame->Pts());
    }
    PackRtp(frame->Data() + frame->PrefixSize(), frame->Size() - frame->PrefixSize(), frame->Pts(), isMark, false);
    return true;
}

void RtpEncoderH264::PackRtp(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos)
{
    RETURN_IF_NULL(data);
    if (len <= GetMaxSize()) {
        PackSingle(data, len, pts, isMark, gopPos);
    } else {
        PackRtpFu(data, len, pts, isMark, gopPos);
    }
}

void RtpEncoderH264::PackRtpFu(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos)
{
    RETURN_IF_NULL(data);
    auto packetSize = GetMaxSize() - 2; // 2:fixed size
    if (len <= packetSize + 1) {
        PackRtpStapA(data, len, pts, isMark, gopPos);
        return;
    }

    auto fuChar0 = (data[0] & (~0x1F)) | 28; // 28:fixed size
    auto fuChar1 = H264_TYPE(data[0]);
    FuFlags *fuFlags = (FuFlags *)(&fuChar1);
    fuFlags->startBit_ = 1;
    fuFlags->endBit_ = 0;

    size_t offset = 1;
    while (!fuFlags->endBit_) {
        if (!fuFlags->startBit_ && len <= offset + packetSize) {
            // FU-A end
            packetSize = len - offset;
            fuFlags->endBit_ = 1;
        }

        auto rtp = MakeRtp(nullptr, packetSize + 2, fuFlags->endBit_ && isMark, pts); // 2:fixed size

        uint8_t *payload = rtp->GetPayload();

        payload[0] = fuChar0;

        payload[1] = fuChar1;

        auto ret = memcpy_s(payload + 2, packetSize, (uint8_t *)data + offset, packetSize); // 2:fixed size
        if (ret != EOK) {
            return;
        }

        offset += packetSize;
        fuFlags->startBit_ = 0;
        onRtpPack_(rtp);
    }
}

void RtpEncoderH264::PackRtpStapA(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos)
{
    RETURN_IF_NULL(data);
    auto rtp = MakeRtp(nullptr, len + 3, isMark, pts); // 3:fixed size
    uint8_t *payload = rtp->GetPayload();
    // STAP-A
    payload[0] = (data[0] & (~0x1F)) | 24;                       // 24:fixed size
    payload[1] = (len >> 8) & 0xFF;                              // 8:byte offset
    payload[2] = len & 0xff;                                     // 2:byte offset
    auto ret = memcpy_s(payload + 3, len, (uint8_t *)data, len); // 3:fixed size
    if (ret != EOK) {
        return;
    }

    onRtpPack_(rtp);
}

void RtpEncoderH264::PackSingle(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos)
{
    RETURN_IF_NULL(data);
    // single NAl unit packet
    auto rtp = MakeRtp(data, len, isMark, pts);
    onRtpPack_(rtp);
}
} // namespace Sharing
} // namespace OHOS
