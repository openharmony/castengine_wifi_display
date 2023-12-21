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

#ifndef OHOS_SHARING_WFD_DEF_H
#define OHOS_SHARING_WFD_DEF_H

#include "common/const_def.h"
#include "common/event_comm.h"
#include "wfd_msg.h"

namespace OHOS {
namespace Sharing {

constexpr uint32_t SURFACE_MAX_NUMBER = 5;
constexpr uint32_t ACCESS_DEV_MAX_NUMBER = 4;
constexpr uint32_t FOREGROUND_SURFACE_MAX_NUMBER = 2;
constexpr uint32_t WFD_SEC_TO_MSEC = 1000;
constexpr uint32_t P2P_CONNECT_TIMEOUT = 180;
constexpr uint32_t RTSP_INTERACTION_TIMEOUT = 10;

enum class WfdSessionState { M0, M1, M2, M3, M4, M5, M6, M6_SENT, M6_DONE, M7, M7_WAIT, M7_SENT, M7_DONE, M8 };

enum WfdErrorCode {};

enum ConnectionState { CONNECTED, DISCONNECTED, INIT, READY, PLAYING };

struct SinkConfig {
    uint32_t surfaceMaximum = 0;
    uint32_t accessDevMaximum = 0;
    uint32_t foregroundMaximum = 0;
};

struct SourceConfig {
    uint32_t accessDevMaximum = 0;
    uint32_t foregroundMaximum = 0;
    uint32_t surfaceMaximum = 0;
};

struct ConnectionInfo {
    bool isRunning = false;

    int32_t ctrlPort = 0;
    uint32_t agentId = INVALID_ID;
    uint32_t contextId = INVALID_ID;

    uint64_t surfaceId = 0;

    std::string ip;
    std::string mac;
    std::string deviceName;
    std::string primaryDeviceType;
    std::string secondaryDeviceType;

    ConnectionState state;
    CodecId audioCodecId = CodecId::CODEC_AAC;
    CodecId videoCodecId = CodecId::CODEC_H264;

    AudioFormat audioFormatId = AudioFormat::AUDIO_48000_16_2;
    VideoFormat videoFormatId = VideoFormat::VIDEO_1920x1080_30;
};

struct DevSurfaceItem {
    bool deleting = false;

    uint32_t agentId = INVALID_ID;
    uint32_t contextId = INVALID_ID;

    std::string deviceId;

    SceneType sceneType;
    sptr<Surface> surfacePtr;
};

struct WfdSceneEventMsg : public InteractionEventMsg {
    using Ptr = std::shared_ptr<WfdSceneEventMsg>;

    std::string mac;
};

struct WfdVideoFormatsInfo {
    uint16_t native = 0;
    uint16_t preferredDisplayMode = 0;
    uint16_t profile = 0;
    uint16_t level = 0;
    uint32_t ceaMask = 0;
    uint32_t veaMask = 0;
    uint32_t hhMask = 0;
    uint16_t latency = 0;
    uint16_t minSlice = 0;
    uint16_t sliceEncParam = 0;
    uint16_t frameRateCtlSupport = 0;
};

} // namespace Sharing
} // namespace OHOS
#endif
