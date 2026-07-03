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

#include "rtp_decoder_aac.h"
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
            auto ret = sscanf_s(aacExtra->aacConfig_.substr(i * 2, 2).data(), "%02X", &cfg); // 2:unit
            if (ret != 1) {
                MEDIA_LOGE("sscanf_s failed.");
            }
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

    if (rtp->GetPayloadSize() < 2) { // 2: fixed size
        MEDIA_LOGW("AAC payload too short: %{public}zu", rtp->GetPayloadSize());
        return;
    }

    auto auHeaderCount = ((ptr[0] << 8) | ptr[1]) >> 4; // 8:byte offset, 4:byte offset

    auto auHeaderPtr = ptr + 2;            // 2:unit
    ptr = auHeaderPtr + auHeaderCount * 2; // 2:unit

    if (end < ptr) {
        return;
    }
    if (auHeaderCount == 0) {
        return;
    }
    if (!lastDts_) {
        lastDts_ = stamp;
    }

    auto dtsInc_ = (stamp - lastDts_) / static_cast<uint32_t>(auHeaderCount);
    MEDIA_LOGD(
        "RtpDecoderAAC::InputRtp seq: %{public}d payloadLen: %{public}zu  auHeaderCount: %{public}d stamp: %{public}d "
        "dtsInc_: %{public}d\n.",
        rtp->GetSeq(), rtp->GetPayloadSize(), auHeaderCount, stamp, dtsInc_);
    if (dtsInc_ < 0 || dtsInc_ > 100) { // 100:unit
        MEDIA_LOGD("abnormal timestamp.");
        dtsInc_ = 0;
    }

    for (int32_t i = 0; i < auHeaderCount; ++i) {
        uint16_t size = ((auHeaderPtr[0] << 8) | auHeaderPtr[1]) >> 3; // 8:byte offset, 3:byte offset

        if (ptr + size > end) {
            break;
        }

        if (size) {
            frame_->Assign((char *)ptr, size);

            frame_->dts_ = lastDts_ + static_cast<uint32_t>(i) * dtsInc_;
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
            frame_->prefixSize_ = static_cast<size_t>(size);
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
        size_t frame_len = static_cast<size_t>(AdtsHeader::GetAacFrameLength((uint8_t *)ptr, end - ptr));
        MEDIA_LOGD("aac frame len: %{public}zu   frame size: %{public}d.", frame_len, frame->Size());
        if (frame_len < ADTS_HEADER_LEN) {
            break;
        }
        if (frame_len == static_cast<size_t>(frame->Size())) {
            onFrame_(frame);
            return;
        }
        ptr += frame_len;
        if (ptr > end) {
            break;
        }
    }
}
} // namespace Sharing
} // namespace OHOS
