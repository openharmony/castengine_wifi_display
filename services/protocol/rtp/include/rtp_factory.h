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

#ifndef OHOS_SHARING_RTP_FACTORY_H
#define OHOS_SHARING_RTP_FACTORY_H

#include "rtp_def.h"
#include "rtp_pack.h"
#include "rtp_unpack.h"

namespace OHOS {
namespace Sharing {
class RtpFactory {
public:
    /**
     * @brief Create RtpUnpack instance
     */
    static RtpUnpack::Ptr CreateRtpUnpack();
    static RtpUnpack::Ptr CreateRtpUnpack(const RtpPlaylodParam &rpp);

    /**
     * @brief Create RptPack instance
     * @param ssrc ssrc
     * @param mtuSize mtu size
     * @param sampleRate Video is 90000, audio is sample rate
     * @param pt payload type
     */
    static RtpPack::Ptr CreateRtpPack(uint32_t ssrc, size_t mtuSize, uint32_t sampleRate, uint8_t pt,
                                      RtpPayloadStream ps, uint32_t audioChannels = 1);

private:
    RtpFactory() = delete;
    ~RtpFactory() = delete;
};
} // namespace Sharing
} // namespace OHOS
#endif