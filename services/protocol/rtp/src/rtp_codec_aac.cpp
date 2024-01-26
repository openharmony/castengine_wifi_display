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

#include "rtp_codec_aac.h"
#include <memory>
#include <securec.h>
#include "adts.h"
#include "common/common_macro.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
RtpDecoderAAC::RtpDecoderAAC(const RtpPlaylodParam &rpp) : rpp_(rpp)
{
    MEDIA_LOGD("trace.");
    frame_ = ObtainFrame();
    auto aacExtra = std::static_pointer_cast<AACExtra>(rpp_.extra_);
    if (aacExtra && !aacExtra->aacConfig_.empty()) {
        for (size_t i = 0; i < aacExtra->aacConfig_.size() / 2; ++i) { // 2:unit
            uint32_t cfg;
            sscanf_s(aacExtra->aacConfig_.substr(i * 2, 2).data(), "%02X", &cfg); // 2:unit
            cfg &= 0x00FF;
            aacConfig_.push_back((char)cfg);
        }
    }
    MEDIA_LOGD("aacConfig_: %{public}s.", aacConfig_.c_str());
}

RtpDecoderAAC::~RtpDecoderAAC() {}

void RtpDecoderAAC::InputRtp(const RtpPacket::Ptr &rtp)
{
    RETURN_IF_NULL(rtp);
    MEDIA_LOGD("rtpDecoderAAC::InputRtp.");
    auto stamp = rtp->GetStampMS();

    auto ptr = rtp->GetPayload();

    auto end = ptr + rtp->GetPayloadSize();

    auto auHeaderCount = ((ptr[0] << 8) | ptr[1]) >> 4; // 8:byte offset, 4:byte offset

    auto auHeaderPtr = ptr + 2;            // 2:unit
    ptr = auHeaderPtr + auHeaderCount * 2; // 2:unit

    if (end < ptr) {
        return;
    }

    if (!lastDts_) {
        lastDts_ = stamp;
    }

    auto dtsInc_ = (stamp - lastDts_) / auHeaderCount;
    MEDIA_LOGD(
        "RtpDecoderAAC::InputRtp seq: %{public}d payloadLen: %{public}zu  auHeaderCount: %{public}d stamp: %{public}d "
        "dtsInc_: %{public}d\n.",
        rtp->GetSeq(), rtp->GetPayloadSize(), auHeaderCount, stamp, dtsInc_);
    if (dtsInc_ < 0 && dtsInc_ > 100) { // 100:unit
        MEDIA_LOGD("abnormal timestamp.");
        dtsInc_ = 0;
    }

    for (int32_t i = 0; i <= auHeaderCount; ++i) {
        uint16_t size = ((auHeaderPtr[0] << 8) | auHeaderPtr[1]) >> 3; // 8:byte offset, 3:byte offset

        if (ptr + size > end) {
            break;
        }

        if (size) {
            frame_->Assign((char *)ptr, size);

            frame_->dts_ = lastDts_ + i * dtsInc_;
            ptr += size;
            auHeaderPtr += 2; // 2:byte offset
            FlushData();
        }
    }

    lastDts_ = stamp;
}

void RtpDecoderAAC::SetOnFrame(const OnFrame &cb)
{
    onFrame_ = cb;
}

FrameImpl::Ptr RtpDecoderAAC::ObtainFrame()
{
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_AAC;
    return frame;
}

void RtpDecoderAAC::FlushData()
{
    if (frame_ == nullptr) {
        return;
    }

    auto ptr = frame_->Data();
    if ((ptr[0] == 0xFF && (ptr[1] & 0xF0) == 0xF0) && frame_->Size() > ADTS_HEADER_LEN) {
        frame_->prefixSize_ = ADTS_HEADER_LEN;
    } else {
        char adtsHeader[128] = {0};
        auto size = AdtsHeader::DumpAacConfig(aacConfig_, frame_->Size(), (uint8_t *)adtsHeader, sizeof(adtsHeader));
        if (size > 0) {
            auto buff = std::make_shared<DataBuffer>();
            buff->Assign(adtsHeader, size);
            buff->Append(frame_->Data(), frame_->Size());
            frame_->Clear();
            frame_->ReplaceData(reinterpret_cast<const char *>(buff->Data()), buff->Size());
            frame_->prefixSize_ = size;
        }
    }

    InputFrame(frame_);
    frame_ = ObtainFrame();
}

void RtpDecoderAAC::InputFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    if (!frame->PrefixSize()) {
        return;
    }

    auto ptr = frame->Data();
    auto end = frame->Data() + frame->Size();
    while (ptr < end) {
        size_t frame_len = AdtsHeader::GetAacFrameLength((uint8_t *)ptr, end - ptr);
        MEDIA_LOGD("aac frame len: %{public}zu   frame size: %{public}d.", frame_len, frame->Size());
        if (frame_len < ADTS_HEADER_LEN) {
            break;
        }
        if (frame_len == frame->Size()) {
            onFrame_(frame);
            return;
        }
        ptr += frame_len;
        if (ptr > end) {
            break;
        }
    }
}

RtpEncoderAAC::RtpEncoderAAC(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq)
    : RtpMaker(ssrc, mtuSize, payloadType, sampleRate, seq)
{
    MEDIA_LOGD("trace.");
}

RtpEncoderAAC::~RtpEncoderAAC() {}

void RtpEncoderAAC::InputFrame(const Frame::Ptr &frame)
{
    RETURN_IF_NULL(frame);
    MEDIA_LOGD("rtpEncoderAAC::InputFrame.");
    auto stamp = frame->Dts();
    auto data = frame->Data() + frame->PrefixSize();
    auto len = frame->Size() - frame->PrefixSize();
    auto ptr = (char *)data;
    auto remain_size = len;
    auto max_size = GetMaxSize() - 4; // 4:avc start code size
    while (remain_size > 0) {
        if (remain_size <= max_size) {
            sectionBuf_[0] = 0;
            sectionBuf_[1] = 16;                                                 // 16:constants
            sectionBuf_[2] = (len >> 5) & 0xFF;                                  // 5:byte offset,2:byte offset
            sectionBuf_[3] = ((len & 0x1F) << 3) & 0xFF;                         // 3:byte offset
            auto ret = memcpy_s(sectionBuf_ + 4, remain_size, ptr, remain_size); // 4:byte offset
            if (ret != EOK) {
                MEDIA_LOGE("mem copy data failed.");
                break;
            }
            MakeAACRtp(sectionBuf_, remain_size + 4, true, stamp); // 4:byte offset
            break;
        }
        sectionBuf_[0] = 0;
        sectionBuf_[1] = 16;                                           // 16:byte offset
        sectionBuf_[2] = ((len) >> 5) & 0xFF;                          // 2:byte offset, 5:byte offset
        sectionBuf_[3] = ((len & 0x1F) << 3) & 0xFF;                   // 3:byte offset
        auto ret = memcpy_s(sectionBuf_ + 4, max_size, ptr, max_size); // 4:byte offset
        if (ret != EOK) {
            MEDIA_LOGE("mem copy data failed.");
            break;
        }
        MakeAACRtp(sectionBuf_, max_size + 4, false, stamp); // 4:byte offset
        ptr += max_size;
        remain_size -= max_size;
    }
}

void RtpEncoderAAC::SetOnRtpPack(const OnRtpPack &cb)
{
    onRtpPack_ = cb;
}

void RtpEncoderAAC::MakeAACRtp(const void *data, size_t len, bool mark, uint32_t stamp)
{
    MEDIA_LOGD("rtpEncoderAAC::MakeAACRtp len: %{public}zu, stamp: %{public}d.", len, stamp);
    auto rtp = MakeRtp(data, len, mark, stamp);
    if (onRtpPack_) {
        onRtpPack_(rtp);
    }
}
} // namespace Sharing
} // namespace OHOS