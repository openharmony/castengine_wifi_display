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

#include "rtp_codec_h264.h"
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

RtpDecoderH264::RtpDecoderH264()
{
    MEDIA_LOGD("trace.");
    frame_ = ObtainFrame();
}

RtpDecoderH264::~RtpDecoderH264() {}

void RtpDecoderH264::InputRtp(const RtpPacket::Ptr &rtp)
{
    RETURN_IF_NULL(rtp);
    auto frame = rtp->GetPayload();
    int32_t length = rtp->GetPayloadSize();
    auto stamp = rtp->GetStampMS();
    auto seq = rtp->GetSeq();
    int32_t nal = H264_TYPE(frame[0]);
    MEDIA_LOGD("rtpDecoderH264::InputRtp length: %{public}d, stamp: %{public}d, seq: %{public}d, nal: %{public}d.",
               length, stamp, seq, nal);

    switch (nal) {
        case 24: // 24:STAP-A Single-time aggregation packet 5.7.1
            UnpackStapA(rtp, frame + 1, length - 1, stamp);
            break;
        case 28: // 28:FU-A Fragmentation unit
            UnpackFuA(rtp, frame, length, stamp, seq);
            break;
        default: {
            if (nal < 24) { // 24:STAP-A Single-time aggregation packet
                // Single NAL Unit Packets
                UnpackSingle(rtp, frame, length, stamp);
                break;
            }
            gopDropped_ = true;
            break;
        }
    }

    if (!gopDropped_ && seq != (uint16_t)(lastSeq_ + 1) && lastSeq_) {
        gopDropped_ = true;
    }
    lastSeq_ = seq;
}

void RtpDecoderH264::SetOnFrame(const OnFrame &cb)
{
    onFrame_ = cb;
}

bool RtpDecoderH264::UnpackSingle(const RtpPacket::Ptr &rtp, const uint8_t *ptr, ssize_t size, uint32_t stamp)
{
    RETURN_FALSE_IF_NULL(frame_);
    MEDIA_LOGD("rtpDecoderH264::UnpackSingle.");
    frame_->Assign("\x00\x00\x00\x01", 4); // 4:avc start code size
    frame_->Append((char *)ptr, size);
    frame_->pts_ = stamp;
    auto key = frame_->KeyFrame();
    OutputFrame(rtp, frame_);
    return key;
}

bool RtpDecoderH264::UnpackStapA(const RtpPacket::Ptr &rtp, const uint8_t *ptr, ssize_t size, uint32_t stamp)
{
    RETURN_FALSE_IF_NULL(ptr);

    auto haveKeyFrame = false;
    auto end = ptr + size;
    while (ptr + 2 < end) {                    // 2:fixed size
        uint16_t len = (ptr[0] << 8) | ptr[1]; // 8:byte offset
        if (!len || ptr + len > end) {
            gopDropped_ = true;
            break;
        }
        ptr += 2; // 2:fixed size
        if (UnpackSingle(rtp, ptr, len, stamp)) {
            haveKeyFrame = true;
        }
        ptr += len;
    }

    return haveKeyFrame;
}

bool RtpDecoderH264::UnpackFuA(const RtpPacket::Ptr &rtp, const uint8_t *ptr, ssize_t size, uint32_t stamp,
                               uint16_t seq)
{
    RETURN_FALSE_IF_NULL(ptr);
    auto nalSuffix = *ptr & (~0x1F);
    FuFlags *fu = (FuFlags *)(ptr + 1);
    if (fu->startBit_) {
        frame_->Assign("\x00\x00\x00\x01", 4); // 4:avc start code size
        uint8_t flag = nalSuffix | fu->nalType_;
        frame_->Append(&flag, 1);
        frame_->pts_ = stamp;
        fuDropped_ = false;
    }

    if (fuDropped_) {
        return false;
    }

    if (!fu->startBit_ && seq != (uint16_t)(lastSeq_ + 1)) {
        fuDropped_ = true;
        frame_->Clear();
        return false;
    }

    frame_->Append((char *)ptr + 2, size - 2); // 2:fixed size

    if (!fu->endBit_) {
        return fu->startBit_ ? frame_->KeyFrame() : false;
    }

    fuDropped_ = true;

    OutputFrame(rtp, frame_);
    return false;
}

H264Frame::Ptr RtpDecoderH264::ObtainFrame()
{
    auto frame = std::make_shared<H264Frame>();
    frame->prefixSize_ = 4; // 4:fixed size
    return frame;
}

void RtpDecoderH264::OutputFrame(const RtpPacket::Ptr &rtp, const H264Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    if (frame->KeyFrame() && gopDropped_) {
        gopDropped_ = false;
        MEDIA_LOGD("new gop received.");
    }

    if (!gopDropped_) {
        onFrame_(frame);
    }
    frame_ = ObtainFrame();
}

RtpEncoderH264::RtpEncoderH264(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq)
    : RtpMaker(ssrc, mtuSize, payloadType, sampleRate, seq)
{}

RtpEncoderH264::~RtpEncoderH264() {}

void RtpEncoderH264::InputFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    auto ptr = frame->Data() + frame->PrefixSize();
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
    // single NAl unit packet
    auto rtp = MakeRtp(data, len, isMark, pts);
    onRtpPack_(rtp);
}
} // namespace Sharing
} // namespace OHOS