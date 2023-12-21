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

#ifndef OHOS_SHARING_RTP_CODEC_H
#define OHOS_SHARING_RTP_CODEC_H

#include <functional>
#include <memory>
#include "rtp_packet.h"

namespace OHOS {
namespace Sharing {
class RtpDecoder {
public:
    using Ptr = std::shared_ptr<RtpDecoder>;
    using OnFrame = std::function<void(const Frame::Ptr &frame)>;

    virtual void SetOnFrame(const OnFrame &cb) = 0;
    virtual void InputRtp(const RtpPacket::Ptr &rtp) = 0;

protected:
    RtpDecoder() = default;
    virtual ~RtpDecoder() = default;

protected:
    OnFrame onFrame_ = nullptr;
};

class RtpEncoder {
public:
    using Ptr = std::shared_ptr<RtpEncoder>;
    using OnRtpPack = std::function<void(const RtpPacket::Ptr &rtp)>;

    virtual void SetOnRtpPack(const OnRtpPack &cb) = 0;
    virtual void InputFrame(const Frame::Ptr &frame) = 0;

protected:
    RtpEncoder() = default;
    virtual ~RtpEncoder() = default;

protected:
    OnRtpPack onRtpPack_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif