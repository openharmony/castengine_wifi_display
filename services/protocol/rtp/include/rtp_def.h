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

#ifndef OHOS_SHARING_RTP_DEF_H
#define OHOS_SHARING_RTP_DEF_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS {
namespace Sharing {
enum class RtpPayloadStream {
    H264,
    MPEG4_GENERIC, // AAC
    MPEG2_TS,
    MPEG2_PS,
    PCMA, // G.711 alaw
    PCMU, // G.711 ulaw
};

class Extra {
public:
    using Ptr = std::shared_ptr<Extra>;
};

class AACExtra : public Extra {
public:
    using Ptr = std::shared_ptr<AACExtra>;

    // SDP: a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=121056E500
    // 121056E500
    std::string aacConfig_;
};

class H264Extra : public Extra {
public:
    using Ptr = std::shared_ptr<H264Extra>;

    std::vector<uint8_t> sps;
    std::vector<uint8_t> pps;
};

class RtpPlaylodParam {
public:
    uint32_t pt_ = 0;
    int32_t sampleRate_ = 90000;

    RtpPayloadStream ps_ = RtpPayloadStream::H264;

    Extra::Ptr extra_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif