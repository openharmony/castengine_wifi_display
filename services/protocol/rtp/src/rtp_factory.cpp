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

#include "rtp_factory.h"
#include "rtp_pack_impl.h"
#include "rtp_unpack_impl.h"

namespace OHOS {
namespace Sharing {
RtpPack::Ptr RtpFactory::CreateRtpPack(uint32_t ssrc, size_t mtuSize, uint32_t sampleRate, uint8_t pt,
                                       RtpPayloadStream ps, uint32_t audioChannels)
{
    return std::make_shared<RtpPackImpl>(ssrc, mtuSize, sampleRate, pt, ps, audioChannels);
}

RtpUnpack::Ptr RtpFactory::CreateRtpUnpack()
{
    return std::make_shared<RtpUnpackImpl>();
}

RtpUnpack::Ptr RtpFactory::CreateRtpUnpack(const RtpPlaylodParam &rpp)
{
    return std::make_shared<RtpUnpackImpl>(rpp);
}
} // namespace Sharing
} // namespace OHOS