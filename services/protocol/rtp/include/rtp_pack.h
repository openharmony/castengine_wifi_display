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

#ifndef OHOS_SHARING_RTP_PACK_H
#define OHOS_SHARING_RTP_PACK_H

#include "frame/frame.h"
#include "rtp_packet.h"

namespace OHOS {
namespace Sharing {
class RtpPack {
public:
    using Ptr = std::shared_ptr<RtpPack>;
    // Pack a rtp packget callback
    using OnRtpPack = std::function<void(const RtpPacket::Ptr &rtp)>;

    /**
     * @brief Input a Video/Audio Frame
     * @param frame Video/Audio Frame
     */
    virtual void InputFrame(const Frame::Ptr &frame) = 0;

    /**
     * @brief SetOnRtpPack
     * @param cb pack a rtp packget callback
     */
    virtual void SetOnRtpPack(const OnRtpPack &cb) = 0;

protected:
    RtpPack() = default;
    virtual ~RtpPack() = default;

protected:
    OnRtpPack onRtpPack_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif