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

#ifndef OHOS_SHARING_RTP_ENCODER_AAC_H
#define OHOS_SHARING_RTP_ENCODER_AAC_H

#include "frame/frame.h"
#include "frame/h264_frame.h"
#include "rtp_encoder.h"
#include "rtp_maker.h"

namespace OHOS {
namespace Sharing {
class RtpEncoderAAC : public RtpEncoder,
                      public RtpMaker {
public:
    using Ptr = std::shared_ptr<RtpEncoderAAC>;

    RtpEncoderAAC(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq = 0);
    ~RtpEncoderAAC();

    void SetOnRtpPack(const OnRtpPack &cb) override;
    void InputFrame(const Frame::Ptr &frame) override;
    void MakeAACRtp(const void *data, size_t len, bool mark, uint32_t stamp);

private:
    unsigned char sectionBuf_[1600] = {0};
};
} // namespace Sharing
} // namespace OHOS
#endif
