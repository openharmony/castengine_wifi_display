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

#ifndef OHOS_SHARING_RTP_UNPACK_IMPL_H
#define OHOS_SHARING_RTP_UNPACK_IMPL_H

#include "rtp_codec.h"
#include "rtp_def.h"
#include "rtp_queue.h"
#include "rtp_unpack.h"

namespace OHOS {
namespace Sharing {
class RtpUnpackImpl : public RtpUnpack {
public:
    RtpUnpackImpl() = default;
    RtpUnpackImpl(const RtpPlaylodParam &rpp);
    ~RtpUnpackImpl();

    void SetSdp(const std::string &sdp);
    void SetOnRtpUnpack(const OnRtpUnpack &cb) override;
    void SetOnRtpNotify(const OnRtpNotify &cb) override;

    void Release() override;
    void ParseRtp(const char *data, size_t len) override;

private:
    void OnRtpDecode(int32_t pt, const Frame::Ptr &frame);
    void OnRtpSorted(uint16_t seq, const RtpPacket::Ptr &rtp);

    void CreateRtpDecoder(const RtpPlaylodParam &rpp);

private:
    uint16_t nextOutSeq_ = 0;

    std::map<uint8_t, RtpDecoder::Ptr> rtpDecoder_;
    std::map<uint8_t, RtpPacketSortor::Ptr> rtpSort_;
    std::chrono::steady_clock::time_point lastReportMissTime_ = std::chrono::steady_clock::now();
};
} // namespace Sharing
} // namespace OHOS
#endif