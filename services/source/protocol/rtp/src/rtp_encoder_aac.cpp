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

#include "rtp_encoder_aac.h"
#include <securec.h>
#include "common/common_macro.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
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
    auto len = (size_t)frame->Size() - frame->PrefixSize();
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
    MEDIA_LOGD("rtpEncoderAAC::MakeAACRtp len: %{public}zu, stamp: %{public}u.", len, stamp);
    RETURN_IF_NULL(data);
    auto rtp = MakeRtp(data, len, mark, stamp);
    if (onRtpPack_) {
        onRtpPack_(rtp);
    }
}
} // namespace Sharing
} // namespace OHOS
