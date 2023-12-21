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

#ifndef OHOS_SHARING_RTP_MAKER_H
#define OHOS_SHARING_RTP_MAKER_H

#include <functional>
#include <memory>
#include "rtp_packet.h"

namespace OHOS {
namespace Sharing {
class RtpMaker {
public:
    using Ptr = std::shared_ptr<RtpMaker>;

    RtpMaker(uint32_t ssrc, size_t mtuSize, uint8_t payloadType, uint32_t sampleRate, uint16_t seq = 0);
    ~RtpMaker();

    uint32_t GetSsrc() const;
    size_t GetMaxSize() const;
    RtpPacket::Ptr MakeRtp(const void *data, size_t len, bool mark, uint32_t stamp);

private:
    uint8_t pt_ = 0;
    uint16_t seq_ = 0;
    uint32_t ssrc_ = 0;
    uint32_t sampleRate_ = 0;

    size_t mtuSize_ = 0;
};
} // namespace Sharing
} // namespace OHOS
#endif