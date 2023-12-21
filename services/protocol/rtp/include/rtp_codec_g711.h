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

#ifndef OHOS_SHARING_G711_RTP_CODEC_H
#define OHOS_SHARING_G711_RTP_CODEC_H

#include "frame/frame.h"
#include "frame/h264_frame.h"
#include "rtp_codec.h"
#include "rtp_maker.h"

namespace OHOS {
namespace Sharing {
class RtpDecoderG711 : public RtpDecoder {
public:
    using Ptr = std::shared_ptr<RtpDecoderG711>;
    RtpDecoderG711();
    ~RtpDecoderG711() = default;

    void InputRtp(const RtpPacket::Ptr &rtp) override;
    void SetOnFrame(const OnFrame &cb) override;

private:
    FrameImpl::Ptr ObtainFrame();

private:
    bool dropFlag_ = false;
    uint16_t lastSeq_ = 0;
    size_t maxFrameSize_ = 0;

    FrameImpl::Ptr frame_ = nullptr;
};

class RtpEncoderG711 : public RtpEncoder,
                       public RtpMaker {
public:
    using Ptr = std::shared_ptr<RtpEncoderG711>;

    RtpEncoderG711(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint32_t channels = 1,
                   uint16_t seq = 0);
    ~RtpEncoderG711() override = default;

    void SetOnRtpPack(const OnRtpPack &cb) override;
    void InputFrame(const Frame::Ptr &frame) override;

private:
    uint32_t channels_ = 1;
    FrameImpl::Ptr cacheFrame_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif