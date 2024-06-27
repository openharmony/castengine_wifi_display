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
#include "sharing_hisysevent.h"
#include "hisysevent.h"
#ifdef HISYSEVENT_DEBUG
#include "sharing_log.h"
#endif
namespace OHOS {
namespace Sharing {
static constexpr char SHARING_DFX_DOMAIN_NAME[] = "WIFI_DISPLAY";
static constexpr char ORG_PKG[] = "sharing_service";
static constexpr char HOST_PKG[] = "cast_engine_service";
static constexpr char EVENT_NAME[] = "MIRACAST_BEHAVIOR";

static const uint8_t WIFI_DISPLAY_SYSTEM_ID = 209;
static const uint8_t WIFI_DISPLAY_MODULE_ID = 11;

SharingHiSysEvent::SharingHiSysEvent(BIZSceneType scene, std::string callPkg)
    : bizScene_(static_cast<int32_t>(scene)), toCallPkg_(callPkg)
{}

SharingHiSysEvent::~SharingHiSysEvent()
{
    ReportEnd(__func__, lastStage_, BlzErrorCode::ERROR_FAIL);
}

void SharingHiSysEvent::ReportStart(std::string funcName, BIZSceneStage stage)
{
#ifdef HISYSEVENT_DEBUG
    SHARING_LOGE("[FUNC:%s][%s][%s][BIZ_SCENE:%d][BIZ_STAGE:%d][BIZ_STATE:%d]", funcName.c_str(),
                 SHARING_DFX_DOMAIN_NAME, EVENT_NAME, bizScene_, static_cast<int32_t>(stage),
                 static_cast<int32_t>(BIZStateType::BIZ_STATE_BEGIN));
#endif
    HiSysEventWrite(SHARING_DFX_DOMAIN_NAME, EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "ORG_PKG", ORG_PKG,
                    "HOST_PKG", HOST_PKG, "FUNC", funcName, "BIZ_SCENE", bizScene_, "BIZ_STATE",
                    static_cast<int32_t>(BIZStateType::BIZ_STATE_BEGIN), "BIZ_STAGE", static_cast<int32_t>(stage),
                    "STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_IDLE), "ERROR_CODE", 0, "TO_CALL_PKG",
                    toCallPkg_, "LOCAL_MAC", localMac_, "PEER_MAC", peerMac_);
    lastStage_ = stage;
}

void SharingHiSysEvent::ReportEnd(std::string funcName, BIZSceneStage stage, BlzErrorCode errCode)
{
    if (sceneDisposed) {
        return;
    }

#ifdef HISYSEVENT_DEBUG
    SHARING_LOGE("[FUNC:%s][%s][%s][BIZ_SCENE:%d][BIZ_STAGE:%d][BIZ_STATE:%d]", funcName.c_str(),
                 SHARING_DFX_DOMAIN_NAME, EVENT_NAME, bizScene_, static_cast<int32_t>(stage),
                 static_cast<int32_t>(BIZStateType::BIZ_STATE_END));
#endif
    int32_t stageResType = static_cast<int32_t>(StageResType::STAGE_RES_SUCCESS);
    if (errCode != BlzErrorCode::ERROR_NONE) {
        stageResType = static_cast<int32_t>(StageResType::STAGE_RES_FAILED);
    }

    int32_t err = (WIFI_DISPLAY_SYSTEM_ID << 24) | (WIFI_DISPLAY_MODULE_ID << 18) | static_cast<int32_t>(errCode);
    HiSysEventWrite(SHARING_DFX_DOMAIN_NAME, EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "ORG_PKG", ORG_PKG,
                    "HOST_PKG", HOST_PKG, "FUNC", funcName, "BIZ_SCENE", bizScene_, "BIZ_STATE",
                    static_cast<int32_t>(BIZStateType::BIZ_STATE_END), "BIZ_STAGE", static_cast<int32_t>(stage),
                    "STAGE_RES", stageResType, "ERROR_CODE", err, "TO_CALL_PKG", toCallPkg_, "LOCAL_MAC", localMac_,
                    "PEER_MAC", peerMac_);
    lastStage_ = stage;
    sceneDisposed = true;
}

void SharingHiSysEvent::Report(std::string funcName, BIZSceneStage stage, StageResType resType, std::string peerMac)
{
    auto tmpPeerMac = peerMac.empty() ? peerMac_ : peerMac;
#ifdef HISYSEVENT_DEBUG
    SHARING_LOGE("[FUNC:%s][%s][%s][BIZ_SCENE:%d][BIZ_STAGE:%d][PEERMAC:%s]", funcName.c_str(), SHARING_DFX_DOMAIN_NAME,
                 EVENT_NAME, bizScene_, static_cast<int32_t>(stage), tmpPeerMac.c_str());
#endif
    HiSysEventWrite(SHARING_DFX_DOMAIN_NAME, EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "ORG_PKG", ORG_PKG,
                    "HOST_PKG", HOST_PKG, "FUNC", funcName, "BIZ_SCENE", bizScene_, "BIZ_STAGE",
                    static_cast<int32_t>(stage), "STAGE_RES", static_cast<int32_t>(resType), "ERROR_CODE",
                    static_cast<int32_t>(BlzErrorCode::ERROR_NONE), "TO_CALL_PKG", toCallPkg_, "LOCAL_MAC", localMac_,
                    "PEER_MAC", tmpPeerMac);
    lastStage_ = stage;
}

void SharingHiSysEvent::SetCallPkg(const std::string callPkg)
{
    toCallPkg_ = callPkg;
}

void SharingHiSysEvent::SetLocalMac(const std::string mac)
{
    localMac_ = mac;
}

void SharingHiSysEvent::SetPeerMac(const std::string mac)
{
    peerMac_ = mac;
}

void SharingHiSysEvent::ChangeScene(BIZSceneType scene)
{
    if (!sceneDisposed) {
        this->ReportEnd(__func__, lastStage_, BlzErrorCode::ERROR_FAIL);
    } else {
        sceneDisposed = false;
    }
    bizScene_ = static_cast<int32_t>(scene);
}

int32_t SharingHiSysEvent::GetScene() const
{
    return bizScene_;
}
} // namespace Sharing
} // namespace OHOS
