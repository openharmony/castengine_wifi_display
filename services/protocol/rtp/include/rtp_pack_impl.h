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

#ifndef OHOS_SHARING_RTP_PACK_IMPL_H
#define OHOS_SHARING_RTP_PACK_IMPL_H

#include <map>
#include "rtp_codec.h"
#include "rtp_def.h"
#include "rtp_pack.h"
#include "rtp_packet.h"

namespace OHOS {
namespace Sharing {
class RtpPackImpl : public RtpPack {
public:
    RtpPackImpl(uint32_t ssrc, size_t mtuSize, uint32_t sampleRate, uint8_t pt, RtpPayloadStream ps,
                uint32_t channels = 1);
    ~RtpPackImpl() = default;

    void SetOnRtpPack(const OnRtpPack &cb) override;
    void InputFrame(const Frame::Ptr &frame) override;

private:
    void InitEncoder();

private:
    uint8_t pt_ = 0;
    uint16_t seq_ = 0;
    uint32_t ssrc_ = 0;
    uint32_t channels_ = 1;
    uint32_t sampleRate_ = 0;

    size_t mtuSize_ = 0;

    RtpEncoder::Ptr rtpEncoder_ = nullptr;
    RtpPayloadStream ps_ = RtpPayloadStream::H264;
};
} // namespace Sharing
} // namespace OHOS
#endif