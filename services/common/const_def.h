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

#ifndef OHOS_SHARING_CONST_DEF_H
#define OHOS_SHARING_CONST_DEF_H

#include <string>

namespace OHOS {
namespace Sharing {

enum MediaType { MEDIA_TYPE_AV, MEDIA_TYPE_AUDIO, MEDIA_TYPE_VIDEO };

enum SceneType { FOREGROUND = 0, BACKGROUND = 1 };

enum VideoFormat {
    VIDEO_640X480_25 = 0,
    VIDEO_640X480_30,
    VIDEO_640X480_60,
    VIDEO_1280X720_25,
    VIDEO_1280X720_30,
    VIDEO_1280X720_60,
    VIDEO_1920X1080_25,
    VIDEO_1920X1080_30,
    VIDEO_1920X1080_60
};

enum AudioFormat {
    AUDIO_8000_8_1 = 0,
    AUDIO_8000_8_2,
    AUDIO_8000_16_1,
    AUDIO_8000_16_2,

    AUDIO_11025_8_1 = 10,
    AUDIO_11025_8_2,
    AUDIO_11025_16_1,
    AUDIO_11025_16_2,

    AUDIO_22050_8_1 = 20,
    AUDIO_22050_8_2,
    AUDIO_22050_16_1,
    AUDIO_22050_16_2,

    AUDIO_44100_8_1 = 30,
    AUDIO_44100_8_2,
    AUDIO_44100_16_1,
    AUDIO_44100_16_2,
    AUDIO_44100_16_4,

    AUDIO_48000_8_1 = 40,
    AUDIO_48000_8_2,
    AUDIO_48000_16_1,
    AUDIO_48000_16_2,
    AUDIO_48000_16_4
};

constexpr uint32_t INVALID_ID = 0;
constexpr int32_t DEFAULT_CHANNEL = 2;
constexpr int32_t DEFAULT_SAMPLERATE = 48000;

constexpr uint16_t MIN_PORT = 6700;
constexpr uint16_t MAX_PORT = 7000;
constexpr uint16_t DEFAULT_WFD_CTRLPORT = 7236;

constexpr uint32_t DEFAULT_VIDEO_WIDTH = 1920;
constexpr uint32_t DEFAULT_VIDEO_HEIGHT = 1080;
constexpr uint32_t DEFAULT_WINDOW_WIDTH = 1920;
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 1080;
constexpr uint32_t DEFAULT_CAPTURE_VIDEO_WIDTH = 1280;
constexpr uint32_t DEFAULT_CAPTURE_VIDEO_HEIGHT = 720;
constexpr uint32_t DEFAULT_FRAME_RATE = 30;
constexpr uint32_t DEFAULT_SAMPLE_BIT = 16;

constexpr int32_t MAX_CONTEXT_NUM = 20;
constexpr int32_t MAX_SRC_AGENT_NUM = 20;
constexpr int32_t MAX_SINK_AGENT_NUM = 20;
constexpr int64_t MAX_YUV420_BUFFER_SIZE = 2560 * 1600 * (3 / 2) * 2;
constexpr int32_t MAX_RTSP_PLAYER_NUM = 2;

constexpr int32_t SCREEN_CAPTURE_ENCODE_BITRATE = 2000000;
constexpr uint64_t SCREEN_ID_INVALID = -1ULL;
constexpr float DEFAULT_SCREEN_DENSITY = 2.0;
constexpr int32_t DEFAULT_SCREEN_FLAGS = 0;

const std::string DUMMY_EMPTY = "";

enum TrackType { TRACK_INVALID = -1, TRACK_VIDEO = 0, TRACK_AUDIO, TRACK_TITLE, TRACK_APPLICATION, TRACK_MAX };

#define CODEC_MAP(XX)                                                \
    XX(CODEC_DEFAULT, TRACK_VIDEO, 0, "default", PSI_STREAM_DEFAULT) \
    XX(CODEC_H264, TRACK_VIDEO, 1, "H264", PSI_STREAM_H264)          \
    XX(CODEC_H265, TRACK_VIDEO, 2, "H265", PSI_STREAM_H265)          \
    XX(CODEC_AAC, TRACK_AUDIO, 3, "mpeg4-generic", PSI_STREAM_AAC)   \
    XX(CODEC_G711A, TRACK_AUDIO, 4, "PCMA", PSI_STREAM_AUDIO_G711A)  \
    XX(CODEC_G711U, TRACK_AUDIO, 5, "PCMU", PSI_STREAM_AUDIO_G711U)  \
    XX(CODEC_OPUS, TRACK_AUDIO, 6, "opus", PSI_STREAM_AUDIO_OPUS)    \
    XX(CODEC_L16, TRACK_AUDIO, 7, "L16", PSI_STREAM_RESERVED)        \
    XX(CODEC_VP8, TRACK_VIDEO, 8, "VP8", PSI_STREAM_VP8)             \
    XX(CODEC_VP9, TRACK_VIDEO, 9, "VP9", PSI_STREAM_VP9)             \
    XX(CODEC_AV1, TRACK_VIDEO, 10, "AV1X", PSI_STREAM_AV1)           \
    XX(CODEC_PCM, TRACK_AUDIO, 11, "PCM_S16LE", PSI_STREAM_AUDIO_PCM_S16LE)

typedef enum {
    CODEC_NONE = -1,
#define XX(name, type, value, str, mpeg_id) name = (value),
    CODEC_MAP(XX)
#undef XX
        CODEC_MAX
} CodecId;

#define ABSTRACT_ERR_BASE(errorCode) ((errorCode) / 100 * 100)

enum SharingErrorCode {
    // common
    ERR_OK = 0,
    // framework 18800001~18800099
    ERR_GENERAL_ERROR = 18800001,
    ERR_BAD_PARAMETER,
    ERR_INVALID_ID,
    ERR_SERVICE_LIMIT,
    ERR_STATE_EXCEPTION,
    ERR_CONNECTION_FAILURE,
    ERR_INTERACTION_FAILURE,
    ERR_CONNECTION_TIMEOUT,
    ERR_CONNECTION_REFUSED,
    ERR_BAD_PROTOCOL,
    ERR_NETWORK_ERROR,
    ERR_UNAUTHORIZED,
    ERR_INVALID_URL,
    ERR_REMOTE_DIED,
    ERR_TCP_KEEPALIVE_TIMEOUT,
    ERR_UDP_KEEPALIVE_TIMEOUT,
    ERR_PROTOCOL_INTERACTION_TIMEOUT,
    // interaction 18800101~18800199
    ERR_INVALID_SURFACE_ID = 18800101,
    ERR_SURFACE_FAILURE,
    // context + agent 18800201~18800299
    ERR_CONTEXT_AGENT_BASE = 18800200,
    ERR_CONTEXT_CREATE = 18800201,
    ERR_AGENT_CREATE,
    ERR_AGENT_START,
    ERR_AGENT_PAUSE,
    ERR_AGENT_RESUME,
    // session 18800301~18800399
    ERR_SESSION_BASE = 18800300,
    ERR_SESSION_CREATE = 18800301,
    ERR_SESSION_START,
    ERR_SESSION_PAUSE,
    ERR_SESSION_RESUME,
    ERR_INTAKE_TIMEOUT,
    // prosumer 18800501~18800599
    ERR_PROSUMER_BASE = 18800500,
    ERR_PROSUMER_INIT = 18800501,
    ERR_PROSUMER_CREATE,
    ERR_PROSUMER_START,
    ERR_PROSUMER_PAUSE,
    ERR_PROSUMER_RESUME,
    ERR_PROSUMER_TIMEOUT,
    ERR_PROSUMER_STOP,
    ERR_PROSUMER_DESTROY,
    ERR_PROSUMER_VIDEO_CAPTURE,
    ERR_PROSUMER_AUDIO_CAPTURE,
    // decode + encode + play 18800601~18800699
    ERR_DECODE_ERROR = 18800601,
    ERR_DECODE_FORMAT,
    ERR_DECODE_SCARCE_CAPACITY,
    ERR_DECODE_DISABLE_ACCELERATION,
    ERR_ENCODE_ERROR,
    ERR_ENCODE_FORMAT,
    ERR_ENCODE_SCARCE_CAPACITY,
    ERR_PLAY_START,
    ERR_PLAY_STOP,

    // application: wfd 18802000~18802999
    ERR_WFD_START_CODE = 18802000,
    ERR_RECEIVING_LIMIT = SharingErrorCode::ERR_WFD_START_CODE + 101,
    ERR_P2P_SINK_DISCONNECTED,
    ERR_P2P_SOURCE_DISCONNECTED,
    ERR_P2P_SOURCE_HEARTBEAT_ERROR,
    ERR_P2P_DHCP_INVALID_IP,
};

enum SharingMsgId {
    // service: 0 ~ 999
    // chat: 1000 ~ 1999
    CHAT_MSG_ID_START = 1000,
    // wfd: 2000 ~ 2999
    WFD_MSG_ID_START = 2000,
    // player: 3000 ~ 3999
    PLAYER_MSG_ID_START = 3000,
    // any push: 4000 ~ 4999
    ANY_PUSH_MSG_ID_START = 4000
};

} // namespace Sharing
} // namespace OHOS
#endif
