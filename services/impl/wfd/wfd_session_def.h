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

#ifndef OHOS_SHARING_WFD_SESSION_DEF_H
#define OHOS_SHARING_WFD_SESSION_DEF_H

#include "common/event_comm.h"

namespace OHOS {
namespace Sharing {

constexpr uint16_t MAX_RTSP_TIMEOUT_COUNTS = 3;

constexpr uint32_t WFD_SEC_TO_MSEC = 1000;
constexpr uint32_t P2P_CONNECT_TIMEOUT = 180;
constexpr uint32_t RTSP_SESSION_TIMEOUT = 30;
constexpr uint32_t RTSP_INTERACTION_TIMEOUT = 10;
constexpr uint32_t MINIMAL_VIDEO_FORMAT_SIZE = 54;

const std::string RTSP_METHOD_WFD = "org.wfa.wfd1.0";
const std::string WFD_PARAM_3D_FORMATS = "wfd_3d_formats";
const std::string WFD_PARAM_TRIGGER = "wfd_trigger_method";
const std::string WFD_PARAM_IDR_REQUEST = "wfd_idr_request";
const std::string WFD_PARAM_DISPLAY_EDID = "wfd_display_edid";
const std::string WFD_PARAM_COUPLED_SINK = "wfd_coupled_sink";
const std::string WFD_PARAM_AUDIO_CODECS = "wfd_audio_codecs";
const std::string WFD_PARAM_RTP_PORTS = "wfd_client_rtp_ports";
const std::string WFD_PARAM_VIDEO_FORMATS = "wfd_video_formats";
const std::string WFD_PARAM_TRIGGER_METHOD = "wfd_trigger_method";
const std::string WFD_RTSP_URL_DEFAULT = "rtsp://localhost/wfd1.0";
const std::string WFD_PARAM_UIBC_CAPABILITY = "wfd_uibc_capability";
const std::string WFD_PARAM_PRESENTATION_URL = "wfd_presentation_URL";
const std::string WFD_PARAM_CONTENT_PROTECTION = "wfd_content_protection";
const std::string WFD_PARAM_STANDBY_RESUME = "wfd_standby_resume_capability";

struct WfdSceneEventMsg : public InteractionEventMsg {
    using Ptr = std::shared_ptr<WfdSceneEventMsg>;

    std::string mac;
};

struct WfdSinkSessionEventMsg : public SessionEventMsg {
    using Ptr = std::shared_ptr<WfdSinkSessionEventMsg>;

    uint16_t localPort = 0;
    uint16_t remotePort = 0;

    std::string ip;
    std::string mac;

    AudioFormat audioFormat = AUDIO_48000_16_2;
    VideoFormat videoFormat = VIDEO_1920X1080_30;
};

struct WfdSourceSessionEventMsg : public SessionEventMsg {
    using Ptr = std::shared_ptr<WfdSourceSessionEventMsg>;

    uint16_t localPort = 0;
    uint16_t remotePort = 0;

    std::string ip;
    std::string mac;

    AudioFormat audioFormat = AUDIO_48000_16_2;
    VideoFormat videoFormat = VIDEO_1920X1080_30;
};

// Table 34. Supported CEA Resolution/Refresh Rates
enum WfdCeaResolution {
    CEA_640_480_P60 = 0,
    CEA_720_480_P60,
    CEA_720_480_I60,
    CEA_720_576_P50,
    CEA_720_576_I50,
    CEA_1280_720_P30 = 5,
    CEA_1280_720_P60,
    CEA_1920_1080_P30 = 7,
    CEA_1920_1080_P60,
    CEA_1920_1080_I60,
    CEA_1280_720_P25 = 10,
    CEA_1280_720_P50,
    CEA_1920_1080_P25 = 12,
    CEA_1920_1080_P50,
    CEA_1920_1080_I50,
    CEA_1280_720_P24 = 15,
    CEA_1920_1080_P24 = 16
};

// Table 35. Supported VESA Resolution/Refresh Rates
enum WfdVesaResolution {
    VESA_800_600_P30 = 0,
    VESA_800_600_P60,
    VESA_1366_768_P30 = 12,
    VESA_1366_768_P60,
    VESA_1280_1024_P30,
    VESA_1280_1024_P60 = 15,
    VESA_1920_1200_P30 = 28
};

// Table 36. Supported HH Resolutions/Refresh Rates
enum WfdHhResolution {
    HH_800_480_P30 = 0,
    HH_800_480_P60 = 1,
    HH_960_540_P30 = 8,
    HH_960_540_P60,
    HH_848_480_P60 = 11
};

// Table 37. Display Native Resolution Refresh Rate
enum class WfdResolutionType : uint8_t {
    RESOLUTION_CEA,  // 0b000
    RESOLUTION_VESA, // 0b001
    RESOLUTION_HH    // 0b010
};

// Table 38. Profiles Bitmap
enum class WfdH264Profile {
    PROFILE_CBP = 0, // 0 0b0: CBP not supported; 0b1: CBP supported;
    PROFILE_CHP      // 1 0b0: CHP not supported; 0b1: CHP supported;
};

// Table 39. Maximum H.264 Level Supported
enum class WfdH264Level {
    LEVEL_31 = 0,
    LEVEL_32,
    LEVEL_40,
    LEVEL_41,
    LEVEL_42,
};

} // namespace Sharing
} // namespace OHOS
#endif
