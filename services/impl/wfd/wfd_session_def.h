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

#include "avcodec_info.h"
#include "common/event_comm.h"
#include "impl/scene/wfd/wfd_def.h"

namespace OHOS {
namespace Sharing {

constexpr uint16_t MAX_RTSP_TIMEOUT_COUNTS = 3;

constexpr uint32_t WFD_SEC_TO_MSEC = 1000;
constexpr uint32_t P2P_CONNECT_TIMEOUT = 60;
constexpr uint32_t P2P_DISCONNECT_TIMEOUT = 3;
constexpr uint32_t RTSP_SESSION_TIMEOUT = 30;
constexpr uint32_t RTSP_INTERACTION_TIMEOUT = 10;
constexpr uint32_t RTSP_TCP_CONNECT_TIMEOUT = 5;
constexpr uint32_t MINIMAL_VIDEO_FORMAT_SIZE = 54;
constexpr uint32_t VIDEO_RESOLUTION_TABLE_LENGTH = 34;
constexpr uint32_t AUDIO_MODES_TABLE_LENGTH = 10;
constexpr uint32_t TYPE_CEA = 0;
constexpr uint32_t TYPE_VESA = 1;
constexpr uint32_t TYPE_HH = 2;
constexpr uint32_t INDEX_CEA = 4;
constexpr uint32_t INDEX_VESA = 5;
constexpr uint32_t INDEX_HH = 6;
constexpr uint32_t HEX_LENGTH = 16;
constexpr uint32_t MAX_CEA_INDEX = 26;
constexpr uint32_t MAX_VESA_INDEX = 33;
constexpr uint32_t MAX_HH_INDEX = 11;
constexpr uint32_t VIDEO_R1_RESOLUTION_SIZE = 17;
constexpr uint32_t VIDEO_R2_RESOLUTION_SIZE = 27;
constexpr uint32_t TYPE_LPCM = 0;
constexpr uint32_t TYPE_AAC = 1;
constexpr uint32_t TYPE_AC3 = 2;
constexpr uint32_t LOG_BASE = 2;
constexpr uint32_t VIDEO_AVC_TYPE = 1;
constexpr uint32_t VIDEO_HEVC_TYPE = 2;
constexpr uint32_t AUDIO_MODE_BIT_LENGTH = 32;
constexpr uint32_t AUDIO_TYPE_INDEX = 0;
constexpr uint32_t AUDIO_MODE_INDEX = 1;
constexpr uint32_t AUDIO_LPCM_MODE_LENGTH = 2;
constexpr uint32_t AUDIO_AAC_MODE_LENGTH = 4;
constexpr uint32_t AUDIO_AAC3_MODE_LENGTH = 3;

const std::string LPCM = "LPCM";
const std::string AAC = "AAC";
const std::string AC3 = "AC3";
const std::string RTSP_METHOD_WFD = "org.wfa.wfd1.0";
const std::string WFD_PARAM_3D_FORMATS = "wfd_3d_formats";
const std::string WFD_PARAM_TRIGGER = "wfd_trigger_method";
const std::string WFD_PARAM_IDR_REQUEST = "wfd_idr_request";
const std::string WFD_PARAM_DISPLAY_EDID = "wfd_display_edid";
const std::string WFD_PARAM_COUPLED_SINK = "wfd_coupled_sink";
const std::string WFD_PARAM_AUDIO_CODECS = "wfd_audio_codecs";
const std::string WFD_PARAM_AUDIO_CODECS_2 = "wfd2_audio_codecs";
const std::string WFD_PARAM_RTP_PORTS = "wfd_client_rtp_ports";
const std::string WFD_PARAM_VIDEO_FORMATS = "wfd_video_formats";
const std::string WFD_PARAM_VIDEO_FORMATS_2 = "wfd2_video_formats";
const std::string WFD_PARAM_TRIGGER_METHOD = "wfd_trigger_method";
const std::string WFD_RTSP_URL_DEFAULT = "rtsp://localhost/wfd1.0";
const std::string WFD_PARAM_UIBC_CAPABILITY = "wfd_uibc_capability";
const std::string WFD_PARAM_PRESENTATION_URL = "wfd_presentation_URL";
const std::string WFD_PARAM_CONTENT_PROTECTION = "wfd_content_protection";
const std::string WFD_PARAM_STANDBY_RESUME = "wfd_standby_resume_capability";
const std::string WFD_PARAM_HWE_VERSION = "wfd_hwe_version";
const std::string WFD_PARAM_HWE_VENDER_ID = "wfd_hwe_vender_id";
const std::string WFD_PARAM_HWE_PRODUCT_ID = "wfd_hwe_product_id";
const std::string WFD_PARAM_HWE_VTP = "wfd_hwe_vtp";
const std::string WFD_PARAM_HWE_VIDEO_60FPS = "wfd_hwe_video_60fps";
const std::string WFD_PARAM_HWE_HEVC_FORMATS = "wfd_hwe_hevc_formats";
const std::string WFD_PARAM_HWE_VERIFICATION_CODE = "wfd_hwe_version";
const std::string WFD_PARAM_HWE_AVSYNC_SINK = "wfd_hwe_avsync_sink";
const std::string WFD_PARAM_CONNECTOR_TYPE = "wfd_connector_type";
const std::string WFD_PARAM_IDR_REQUEST_CAPABILITY = "wfd_idr_request_capability";
const std::string WFD_PARAM_RTCP_CAPABILITY = "microsoft_rtcp_capability";

struct VideoConfigTable {
    int32_t width;
    int32_t height;
    int32_t frameRate;
    bool interlaced;
};

struct AudioConfigTable {
    int32_t sampleRate;
    int32_t sampleFormat;
    int32_t channels;
};

const VideoConfigTable VIDEO_RESOLUTION_TABLE[][VIDEO_RESOLUTION_TABLE_LENGTH] = {
    {
        // CEA Resolution
        {640, 480, 60, false},   {720, 480, 60, false},   {720, 480, 60, true},    {720, 576, 50, false},
        {720, 576, 50, true},    {1280, 720, 30, false},  {1280, 720, 60, false},  {1920, 1080, 30, false},
        {1920, 1080, 60, false}, {1920, 1080, 60, true},  {1280, 720, 25, false},  {1280, 720, 50, false},
        {1920, 1080, 25, false}, {1920, 1080, 50, false}, {1920, 1080, 50, true},  {1280, 720, 24, false},
        {1920, 1080, 24, false}, {3840, 2160, 24, false}, {3840, 2160, 25, false}, {3840, 2160, 30, false},
        {3840, 2160, 50, false}, {3840, 2160, 60, false}, {4096, 2160, 24, false}, {4096, 2160, 25, false},
        {4096, 2160, 30, false}, {4096, 2160, 50, false}, {4096, 2160, 60, false},
    },
    {
        // VESA Resolutions
        {800, 600, 30, false},   {800, 600, 60, false},   {1024, 768, 30, false},  {1024, 768, 60, false},
        {1152, 864, 30, false},  {1152, 864, 60, false},  {1280, 768, 30, false},  {1280, 768, 60, false},
        {1280, 800, 30, false},  {1280, 800, 60, false},  {1360, 768, 30, false},  {1360, 768, 60, false},
        {1366, 768, 30, false},  {1366, 768, 60, false},  {1280, 1024, 30, false}, {1280, 1024, 60, false},
        {1440, 1050, 30, false}, {1440, 1050, 60, false}, {1440, 900, 30, false},  {1440, 900, 60, false},
        {1600, 900, 30, false},  {1600, 900, 60, false},  {1600, 1200, 30, false}, {1600, 1200, 60, false},
        {1680, 1024, 30, false}, {1680, 1024, 60, false}, {1680, 1050, 30, false}, {1680, 1050, 60, false},
        {1920, 1200, 30, false}, {1920, 1200, 60, false}, {2560, 1440, 30, false}, {2560, 1440, 60, false},
        {2560, 1600, 30, false}, {2560, 1600, 60, false},
    },
    {
        // HH Resolutions
        {800, 480, 30, false},
        {800, 480, 60, false},
        {854, 480, 30, false},
        {854, 480, 60, false},
        {864, 480, 30, false},
        {864, 480, 60, false},
        {640, 360, 30, false},
        {640, 360, 60, false},
        {960, 540, 30, false},
        {960, 540, 60, false},
        {848, 480, 30, false},
        {848, 480, 60, false},
    }};

const AudioConfigTable AUDIO_CONFIG_TABLE[][AUDIO_MODES_TABLE_LENGTH] = {
    {
        // LPCM
        {44100, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 2},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 2},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 1},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 6},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 8},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S24LE, 2},
        {96000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 2},
        {96000, MediaAVCodec::AudioSampleFormat::SAMPLE_S24LE, 2},
        {96000, MediaAVCodec::AudioSampleFormat::SAMPLE_S24LE, 6},
        {96000, MediaAVCodec::AudioSampleFormat::SAMPLE_S24LE, 8},
    },
    {
        // AAC
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 2},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 4},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 6},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 8},
    },
    {
        // AC3
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 2},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 4},
        {48000, MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE, 6},
    }

};

struct WfdSceneEventMsg : public InteractionEventMsg {
    using Ptr = std::shared_ptr<WfdSceneEventMsg>;

    std::string mac;
};

struct WfdSinkSessionEventMsg : public SessionEventMsg {
    using Ptr = std::shared_ptr<WfdSinkSessionEventMsg>;

    uint16_t localPort = 0;
    uint16_t remotePort = 0;

    std::string remoteIp;
    std::string localIp;
    std::string mac;

    AudioFormat audioFormat = AUDIO_NONE;
    VideoFormat videoFormat = VIDEO_NONE;
    WfdParamsInfo wfdParamsInfo;
};

struct WfdSourceSessionEventMsg : public SessionEventMsg {
    using Ptr = std::shared_ptr<WfdSourceSessionEventMsg>;

    uint32_t sinkAgentId;
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
    PROFILE_CHP,     // 1 0b0: CHP not supported; 0b1: CHP supported;
    PROFILE_RHP,
    PROFILE_BP,
    PROFILE_MP,
    PROFILE_HIP,
};

// Table 39. Maximum H.264 Level Supported
enum class WfdH264Level {
    LEVEL_31 = 0,
    LEVEL_32,
    LEVEL_40,
    LEVEL_41,
    LEVEL_42,
    LEVEL_50,
    LEVEL_51,
    LEVEL_52,
};

// Table 76. Video codec Type field values in WFD R2
enum class WfdVideoCodec {
    CODEC_H264 = 0,
    CODEC_H265,
};

enum WfdAACMode {
    AAC_48000_16_2 = 0,
    AAC_48000_16_4,
    AAC_48000_16_6,
    AAC_48000_16_8,
};

enum WfdLPCMMode {
    LPCM_44100_16_2 = 0,
    LPCM_48000_16_2,
};

} // namespace Sharing
} // namespace OHOS
#endif
