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

#ifndef OHOS_SHARING_RTP_ENCODER_H264_H
#define OHOS_SHARING_RTP_ENCODER_H264_H

#include "frame/frame.h"
#include "frame/h264_frame.h"
#include "rtp_encoder.h"
#include "rtp_maker.h"

namespace OHOS {
namespace Sharing {
class RtpEncoderH264 : public RtpEncoder,
                       public RtpMaker {
public:
    using Ptr = std::shared_ptr<RtpEncoderH264>;

    RtpEncoderH264(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq = 0);
    ~RtpEncoderH264();

    void SetOnRtpPack(const OnRtpPack &cb) override;
    void InputFrame(const Frame::Ptr &frame) override;

private:
    void InsertConfigFrame(uint32_t pts);
    bool InputFrame(const Frame::Ptr &frame, bool isMark);
    void PackRtp(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos);
    void PackRtpFu(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos);
    void PackSingle(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos);
    void PackRtpStapA(const uint8_t *data, size_t len, uint32_t pts, bool isMark, bool gopPos);

private:
    Frame::Ptr sps_ = nullptr;
    Frame::Ptr pps_ = nullptr;
    Frame::Ptr lastFrame_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
