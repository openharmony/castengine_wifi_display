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

#ifndef OHOS_SHARING_RTP_DECODER_H264_H
#define OHOS_SHARING_RTP_DECODER_H264_H

#include "frame/frame.h"
#include "frame/h264_frame.h"
#include "rtp_decoder.h"

namespace OHOS {
namespace Sharing {
class RtpDecoderH264 : public RtpDecoder {
public:
    using Ptr = std::shared_ptr<RtpDecoderH264>;

    RtpDecoderH264();
    ~RtpDecoderH264();

    void InputRtp(const RtpPacket::Ptr &rtp) override;
    void SetOnFrame(const OnFrame &cb) override;

private:
    H264Frame::Ptr ObtainFrame();

    bool UnpackStapA(const RtpPacket::Ptr &rtp, const uint8_t *ptr, ssize_t size, uint32_t stamp);
    bool UnpackSingle(const RtpPacket::Ptr &rtp, const uint8_t *ptr, ssize_t size, uint32_t stamp);
    bool UnpackFuA(const RtpPacket::Ptr &rtp, const uint8_t *ptr, ssize_t size, uint32_t stamp, uint16_t seq);

    void OutputFrame(const RtpPacket::Ptr &rtp, const H264Frame::Ptr &frame);

private:
    bool fuDropped_ = true;
    bool gopDropped_ = false;

    uint16_t lastSeq_ = 0;

    H264Frame::Ptr frame_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
