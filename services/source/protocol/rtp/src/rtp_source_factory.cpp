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

#include "rtp_source_factory.h"
#include "rtp_pack_impl.h"

namespace OHOS {
namespace Sharing {
RtpPack::Ptr RtpSourceFactory::CreateRtpPack(uint32_t ssrc, size_t mtuSize, uint32_t sampleRate, uint8_t pt,
                                             RtpPayloadStream ps, uint32_t audioChannels)
{
    return std::make_shared<RtpPackImpl>(ssrc, mtuSize, sampleRate, pt, ps, audioChannels);
}
} // namespace Sharing
} // namespace OHOS
