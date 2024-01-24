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
    VideoFormat videoFormatId = VideoFormat::VIDEO_1920X1080_30;
};

struct DevSurfaceItem {
    bool deleting = false;

    uint32_t agentId = INVALID_ID;
    uint32_t contextId = INVALID_ID;

    std::string deviceId;

    SceneType sceneType;
    sptr<Surface> surfacePtr;
};

} // namespace Sharing
} // namespace OHOS
#endif
