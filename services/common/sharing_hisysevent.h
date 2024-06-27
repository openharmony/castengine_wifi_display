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

#ifndef OHOS_SHARING_HISYSEVENT_H
#define OHOS_SHARING_HISYSEVENT_H

#include <string>
#include "common/event_comm.h"

namespace OHOS {
namespace Sharing {
constexpr char WFD_SOURCE[] = "wfdSourceSession";
constexpr char P2P_PKG[] = "WIFI_P2P";
enum class BIZSceneType : int32_t {
    WFD_SOURCE_PLAY,
    P2P_START_DISCOVERY,
    P2P_CONNECT_DEVICE,
    P2P_DISCONNECT_DEVICE,
};

enum class StageResType : int32_t {
    STAGE_RES_IDLE = 0,
    STAGE_RES_SUCCESS,
    STAGE_RES_FAILED,
    STAGE_RES_CANCEL,
    STAGE_RES_UNKNOWN
};

enum class BIZStateType : int32_t {
    BIZ_STATE_BEGIN = 1,
    BIZ_STATE_END,
    BIZ_STATE_IDLE,
};

enum class BIZSceneStage : int32_t {
    // scene5
    WFD_SOURCE_PLAY_TCP_SERVER = 1,
    WFD_SOURCE_PLAY_SEND_M1_REQ, // OPTIONS
    WFD_SOURCE_PLAY_RECV_M1_RSP,
    WFD_SOURCE_PLAY_RECV_M2_REQ, // OPTIONS
    WFD_SOURCE_PLAY_SEND_M2_RSP,
    WFD_SOURCE_PLAY_SEND_M3_REQ, // GET_PARAMETER
    WFD_SOURCE_PLAY_RECV_M3_RSP,
    WFD_SOURCE_PLAY_SEND_M4_REQ, // SET_PARAMETER
    WFD_SOURCE_PLAY_RECV_M4_RSP,
    WFD_SOURCE_PLAY_SEND_M5_REQ, // SET_PARAMETER wfd_trigger_method: SETUP
    WFD_SOURCE_PLAY_RECV_M5_RSP,
    WFD_SOURCE_PLAY_RECV_M6_REQ, // SETUP
    WFD_SOURCE_PLAY_SEND_M6_RSP,
    WFD_SOURCE_PLAY_RECV_M7_REQ, // PLAY
    WFD_SOURCE_PLAY_SEND_M7_RSP,
    WFD_SOURCE_PLAY_RECV_M8_REQ, // TEARDOWN
    WFD_SOURCE_PLAY_SEND_M8_RSP,
    // scene2  P2P_START_DISCOVERY
    P2P_START_DISCOVERY = 1,
    P2P_DEVICE_FOUND,
    // scene P2P_CONNECT_DEVICE
    P2P_CONNECT_DEVICE = 1,

    // scene P2P_DISCONNECT_DEVICE
    P2P_DISCONNECT_DEVICE = 1,

    WFD_SOURCE_SCENE_REMOVE_DEVICE,
    WFD_SOUCE_SCENE_STOP_DISCOVERY,
};

enum class BlzErrorCode : int32_t {
    ERROR_NONE = 0,
    ERROR_FAIL,
};

class SharingHiSysEvent {
public:
    using Ptr = std::shared_ptr<SharingHiSysEvent>;
    explicit SharingHiSysEvent(BIZSceneType scene, std::string callPkg);
    ~SharingHiSysEvent();
    void ReportStart(std::string funcName, BIZSceneStage stage);
    void ReportEnd(std::string funcName, BIZSceneStage stage, BlzErrorCode error = BlzErrorCode::ERROR_NONE);
    void Report(std::string funcName, BIZSceneStage stage, StageResType resType = StageResType::STAGE_RES_SUCCESS,
                std::string peerMac = "");
    void SetCallPkg(const std::string callPkg);
    void SetLocalMac(const std::string mac);
    void SetPeerMac(const std::string mac);
    void ChangeScene(BIZSceneType scene);
    int32_t GetScene() const;

private:
    int32_t bizScene_ = 0;
    std::string toCallPkg_ = "";
    std::string localMac_ = "";
    std::string peerMac_ = "";
    BIZSceneStage lastStage_;
    bool sceneDisposed = false;
};

} // namespace Sharing
} // namespace OHOS
#endif