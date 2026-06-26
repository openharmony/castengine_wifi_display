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

#include "rtp_decoder_g711.h"
#include "common/common_macro.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
RtpDecoderG711::RtpDecoderG711()
{
    frame_ = ObtainFrame();
}

void RtpDecoderG711::InputRtp(const RtpPacket::Ptr &rtp)
{
    RETURN_IF_NULL(rtp);
    RETURN_IF_NULL(frame_);
    auto payload_size = rtp->GetPayloadSize();
    if (payload_size <= 0) {
        return;
    }
    auto payload = rtp->GetPayload();
    auto stamp = rtp->GetStampMS();
    auto seq = rtp->GetSeq();

    if (frame_->dts_ != stamp || frame_->Size() > maxFrameSize_) {
        if (frame_->Size()) {
            onFrame_(frame_);
        }

        frame_ = ObtainFrame();
        frame_->dts_ = stamp;

        dropFlag_ = false;
    } else if (lastSeq_ != 0 && (uint16_t)(lastSeq_ + 1) != seq) {
        MEDIA_LOGD("rtp lose: %{public}d -> %{public}d.", lastSeq_, seq);
        dropFlag_ = true;
        frame_->Clear();
    }

    if (!dropFlag_) {
        frame_->Append((char *)payload, payload_size);
    }

    lastSeq_ = seq;
    return;
}

void RtpDecoderG711::SetOnFrame(const OnFrame &cb)
{
    onFrame_ = cb;
}

FrameImpl::Ptr RtpDecoderG711::ObtainFrame()
{
    auto frame = FrameImpl::Create();
    frame->codecId_ = CODEC_G711A;
    return frame;
}
} // namespace Sharing
} // namespace OHOS
