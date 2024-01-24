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

#ifndef OHOS_SHARING_RTCP_DEF_H
#define OHOS_SHARING_RTCP_DEF_H

#include <cstdint>

namespace OHOS {
namespace Sharing {
enum class RtcpType : uint8_t {
    RTCP_FIR = 192,
    RTCP_NACK = 193,
    RTCP_SMPTETC = 194,
    RTCP_IJ = 195,
    RTCP_SR = 200,
    RTCP_RR = 201,
    RTCP_SDES = 202,
    RTCP_BYE = 203,
    RTCP_APP = 204,
    RTCP_RTPFB = 205, // Generic RTP Feedback
    RTCP_PSFB = 206,  // Payload-specific Feedback
    RTCP_XR = 207,    // RTCP Extended Reports, refer to https://www.rfc-editor.org/rfc/rfc3611
    RTCP_AVB = 208,
    RTCP_RSI = 209,
    RTCP_TOKEN = 210
};

enum class SdesType : uint8_t {
    RTCP_SDES_END = 0,
    RTCP_SDES_CNAME = 1,
    RTCP_SDES_NAME = 2,
    RTCP_SDES_EMAIL = 3,
    RTCP_SDES_PHONE = 4,
    RTCP_SDES_LOC = 5,
    RTCP_SDES_TOOL = 6,
    RTCP_SDES_NOTE = 7,
    RTCP_SDES_PRIVATE = 8,
};

enum class PsfbType : uint8_t {
    RTCP_PSFB_PLI = 1,  // Picture Loss Indication (PLI)
    RTCP_PSFB_SLI = 2,  // Slice Loss Indication (SLI)
    RTCP_PSFB_RPSI = 3, // Reference Picture Selection Indication (RPSI)
    RTCP_PSFB_FIR = 4,  // Full Intra Request Command (FIR)
    RTCP_PSFB_TSTR = 5, // Temporal-Spatial Trade-off Request (TSTR)
    RTCP_PSFB_TSTN = 6, // Temporal-Spatial Trade-off Notification (TSTN)
    RTCP_PSFB_VBCM = 7, // VBCM
    RTCP_PSFB_REMB = 15 // REMB / Application layer FB (AFB) message
};

enum class RtpfbType : uint8_t {
    RTCP_RTPFB_NACK = 1,  // Generic NACK
    RTCP_RTPFB_TMMBR = 3, // Temp. Max Media Stream Bitrate Request
    RTCP_RTPFB_TMMBN = 4, // Temp. Max Media Stream Bitrate Notification
    RTCP_RTPFB_TWCC = 15  // transport-cc
};
} // namespace Sharing
} // namespace OHOS
#endif