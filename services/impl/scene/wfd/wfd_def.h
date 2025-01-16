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

struct WfdParamsInfo {
    std::string contentProtection = "none";
    std::string uibcCapability = "none";
    std::string connectorType = "05";
    std::string microsofRtcpCapability = "none";
    std::string idrRequestCapablity = "support";
    std::string displayEdid = std::string("0002 00FFFFFFFFFFFF0022F6300000000000FF1D0103807844780ACF74A3574CB02") +
                              std::string("309484C21080081C0814081800101010101010101010108E80030F2705A80B0588A0") +
                              std::string("0C48E2100001E023A801871382D40582C4500C48E2100001E000000FC00485541574") +
                              std::string("54920494E430A2020000000FD003B461F8C3C000A20202020202001BB02036FF25B0") +
                              std::string("58403011213141607901F20225D5F6061626465665E63020611152F0907071507505") +
                              std::string("707003D07C067070383010000E200FF6E030C001000B83C2000800102030467D") +
                              std::string("85DC401788003EB0146D000260B11D578547BE61146D0007080E305FF01E50F0") +
                              std::string("0801900E3060D010000000000000000000000000000000034");
    std::string hweVersion = "none";
    std::string hweVenderId = "none";
    std::string hweProductId = "none";
    std::string hweVtp = "none";
    std::string hweHevcFormats = "none";
    std::string hweVideo60FPS = "none";
    std::string hweAvSyncSink = "none";
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
