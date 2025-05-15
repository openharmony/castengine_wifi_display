/*
 * Copyright (c) 2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "sharing_sink_hisysevent.h"
#include "hisysevent.h"
#include "sharing_log.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {

static constexpr char SHARING_SINK_DFX_DOMAIN_NAME[] = "WIFI_DISPLAY";
static constexpr char SHARING_SINK_EVENT_NAME[] = "MIRACAST_SINK_BEHAVIOR";
static constexpr char SHARING_SINK_ORG_PKG[] = "wifi_display_sink";
static constexpr char SHARING_SINK_HOST_PKG[] = "cast_engine_service";
static constexpr char SHARING_SINK_LOCAL_DEV_TYPE[] = "09C";
static constexpr uint8_t DECODE_TIME_MAP_CAPACITY = 60;
static constexpr int32_t DECODE_TIME_OUT = 300 * 1000; // 300ms
static constexpr uint8_t FREEZE_COUNT = 5;
static constexpr int32_t REPORT_INTERVAL_MS = 10 * 60 * 1000; // 10min

WfdSinkHiSysEvent& WfdSinkHiSysEvent::GetInstance()
{
    static WfdSinkHiSysEvent instance;
    return instance;
}

void WfdSinkHiSysEvent::SetHiSysEventDevInfo(WfdSinkHiSysEvent::SinkHisyseventDevInfo devInfo)
{
    devInfo_.localDevName = GetAnonyDevName(devInfo.localDevName).c_str();
    devInfo_.localIp = GetAnonymousIp(devInfo.localIp).c_str();
    devInfo_.localWifiMac = GetAnonymousMAC(devInfo.localWifiMac).c_str();
    devInfo_.localNetId = devInfo.localNetId;
    devInfo_.peerDevName = GetAnonyDevName(devInfo.peerDevName).c_str();
    devInfo_.peerIp = GetAnonymousIp(devInfo.peerIp).c_str();
    devInfo_.peerWifiMac = GetAnonymousMAC(devInfo.peerWifiMac).c_str();
    devInfo_.peerNetId = devInfo.peerNetId;
}

void WfdSinkHiSysEvent::GetStartTime(std::chrono::system_clock::time_point startTime)
{
    startTime_ = std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()).count();
}

void WfdSinkHiSysEvent::ChangeHisysEventScene(SinkBizScene scene)
{
    sinkBizScene_ = static_cast<int32_t>(scene);
}

void WfdSinkHiSysEvent::StartReport(const std::string &funcName, const std::string &toCallpkg,
                                    SinkStage sinkStage, SinkStageRes sinkStageRes)
{
    if (sinkBizScene_ == static_cast<int32_t>(SinkBizScene::ESTABLISH_MIRRORING)) {
        hiSysEventStart_ = true;
        Reset();
    }
    if (hiSysEventStart_ == false) {
        SHARING_LOGE("func:%{public}s, sinkStage:%{public}d, scece is Invalid", funcName.c_str(), sinkStage);
        return;
    }
    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", sinkBizScene_,
                    "BIZ_STAGE", static_cast<int32_t>(sinkStage),
                    "STAGE_RES", static_cast<int32_t>(sinkStageRes),
                    "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_BEGIN),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", toCallpkg.c_str(),
                    "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
                    "PEER_IP", devInfo_.peerIp.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName.c_str());
}

void WfdSinkHiSysEvent::Report(const std::string &funcName, const std::string &toCallpkg,
                               SinkStage sinkStage, SinkStageRes sinkStageRes)
{
    if (hiSysEventStart_ == false) {
        SHARING_LOGE("func:%{public}s, sinkStage:%{public}d, scece is Invalid", funcName.c_str(), sinkStage);
        return;
    }
    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", sinkBizScene_,
                    "BIZ_STAGE", static_cast<int32_t>(sinkStage),
                    "STAGE_RES", static_cast<int32_t>(sinkStageRes),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", toCallpkg.c_str(),
                    "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
                    "PEER_IP", devInfo_.peerIp.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName.c_str());
}

void WfdSinkHiSysEvent::FirstSceneEndReport(const std::string &funcName, const std::string &toCallpkg,
                                            SinkStage sinkStage, SinkStageRes sinkStageRes)
{
    if (hiSysEventStart_ == false) {
        SHARING_LOGE("func:%{public}s, sinkStage:%{public}d, scece is Invalid", funcName.c_str(), sinkStage);
        return;
    }
    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", sinkBizScene_,
                    "BIZ_STAGE", static_cast<int32_t>(sinkStage),
                    "STAGE_RES", static_cast<int32_t>(sinkStageRes),
                    "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_END),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", toCallpkg.c_str(),
                    "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
                    "PEER_IP", devInfo_.peerIp.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName.c_str());
}

void WfdSinkHiSysEvent::ThirdSceneEndReport(const std::string &funcName, const std::string &toCallpkg,
                                            SinkStage sinkStage)
{
    if (hiSysEventStart_ == false) {
        SHARING_LOGE("func:%{public}s, sinkStage:%{public}d, scece is Invalid", funcName.c_str(), sinkStage);
        return;
    }
    std::chrono::system_clock::time_point endTimePoint = std::chrono::system_clock::now();
    int32_t endTime = std::chrono::duration_cast<std::chrono::seconds>(endTimePoint.time_since_epoch()).count();
    int32_t duration = endTime - startTime_;

    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", static_cast<int32_t>(SinkBizScene::DISCONNECT_MIRRORING),
                    "BIZ_STAGE", static_cast<int32_t>(SinkStage::DISCONNECT_COMPLETE),
                    "STAGE_RES", static_cast<int32_t>(SinkStageRes::SUCCESS),
                    "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_END),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", toCallpkg.c_str(),
                    "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
                    "PEER_IP", devInfo_.peerIp.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName.c_str(),
                    "SERVICE_DURATION", duration);
    hiSysEventStart_ = false;
}

void WfdSinkHiSysEvent::ReportEstablishMirroringError(const std::string &funcName, const std::string &toCallpkg,
    SinkStage sinkStage, SinkErrorCode errorCode, int32_t duration)
{
    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FUNC_NAME", funcName.c_str(),
        "BIZ_SCENE", sinkBizScene_,
        "BIZ_STAGE", static_cast<int32_t>(sinkStage),
        "STAGE_RES", static_cast<int32_t>(SinkStageRes::FAIL),
        "ERROR_CODE", static_cast<int32_t>(errorCode),
        "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_END),
        "ORG_PKG", SHARING_SINK_ORG_PKG,
        "HOST_PKG", SHARING_SINK_HOST_PKG,
        "TO_CALL_PKG", toCallpkg.c_str(),
        "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
        "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
        "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
        "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
        "LOCAL_IP", devInfo_.localIp.c_str(),
        "PEER_NET_ID", devInfo_.peerNetId.c_str(),
        "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
        "PEER_IP", devInfo_.peerIp.c_str(),
        "PEER_DEV_NAME", devInfo_.peerDevName.c_str());
}

void WfdSinkHiSysEvent::ReportStabilityError(const std::string &funcName, const std::string &toCallpkg,
    SinkStage sinkStage, SinkErrorCode errorCode)
{
    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FUNC_NAME", funcName.c_str(),
        "BIZ_SCENE", sinkBizScene_,
        "BIZ_STAGE", static_cast<int32_t>(sinkStage),
        "STAGE_RES", static_cast<int32_t>(SinkStageRes::FAIL),
        "ERROR_CODE", static_cast<int32_t>(errorCode),
        "ORG_PKG", SHARING_SINK_ORG_PKG,
        "HOST_PKG", SHARING_SINK_HOST_PKG,
        "TO_CALL_PKG", toCallpkg.c_str(),
        "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
        "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
        "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
        "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
        "LOCAL_IP", devInfo_.localIp.c_str(),
        "PEER_NET_ID", devInfo_.peerNetId.c_str(),
        "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
        "PEER_IP", devInfo_.peerIp.c_str(),
        "PEER_DEV_NAME", devInfo_.peerDevName.c_str());
}

void WfdSinkHiSysEvent::ReportDisconnectError(const std::string &funcName, const std::string &toCallpkg,
    SinkStage sinkStage, SinkErrorCode errorCode, int32_t duration)
{
    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME,
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FUNC_NAME", funcName.c_str(),
        "BIZ_SCENE", sinkBizScene_,
        "BIZ_STAGE", static_cast<int32_t>(sinkStage),
        "STAGE_RES", static_cast<int32_t>(SinkStageRes::FAIL),
        "ERROR_CODE", static_cast<int32_t>(errorCode),
        "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_END),
        "ORG_PKG", SHARING_SINK_ORG_PKG,
        "HOST_PKG", SHARING_SINK_HOST_PKG,
        "TO_CALL_PKG", toCallpkg.c_str(),
        "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
        "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
        "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
        "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
        "LOCAL_IP", devInfo_.localIp.c_str(),
        "PEER_NET_ID", devInfo_.peerNetId.c_str(),
        "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
        "PEER_IP", devInfo_.peerIp.c_str(),
        "PEER_DEV_NAME", devInfo_.peerDevName.c_str(),
        "SERVICE_DURATION", duration);
}


void WfdSinkHiSysEvent::ReportError(const std::string &funcName, const std::string &toCallpkg,
                                    SinkStage sinkStage, SinkErrorCode errorCode)
{
    if (hiSysEventStart_ == false) {
        SHARING_LOGE("func:%{public}s, sinkStage:%{public}d, scece is Invalid", funcName.c_str(), sinkStage);
        return;
    }

    std::chrono::system_clock::time_point endTimePoint = std::chrono::system_clock::now();
    int endTime = std::chrono::duration_cast<std::chrono::seconds>(endTimePoint.time_since_epoch()).count();
    int32_t duration = endTime - startTime_;

    switch (static_cast<SinkBizScene>(sinkBizScene_)) {
        case SinkBizScene::ESTABLISH_MIRRORING:
            ReportEstablishMirroringError(funcName, toCallpkg, sinkStage, errorCode, duration);
            hiSysEventStart_ = false;
            break;
        case SinkBizScene::MIRRORING_STABILITY:
            ReportStabilityError(funcName, toCallpkg, sinkStage, errorCode);
            break;
        case SinkBizScene::DISCONNECT_MIRRORING:
            ReportDisconnectError(funcName, toCallpkg, sinkStage, errorCode, duration);
            hiSysEventStart_ = false;
            break;
        default:
            break;
    }
}

void WfdSinkHiSysEvent::P2PReportError(const std::string &funcName, SinkErrorCode errorCode)
{
    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", static_cast<int32_t>(SinkBizScene::ESTABLISH_MIRRORING),
                    "BIZ_STAGE", static_cast<int32_t>(SinkStage::P2P_CONNECT_SUCCESS),
                    "STAGE_RES", static_cast<int32_t>(SinkStageRes::FAIL),
                    "ERROR_CODE", static_cast<int32_t>(errorCode),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", "wpa_supplicant",
                    "LOCAL_NET_ID", devInfo_.localNetId.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac.c_str(),
                    "PEER_IP", devInfo_.peerIp.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName.c_str());
}

int32_t WfdSinkHiSysEvent::GetCurrentScene()
{
    return sinkBizScene_;
}

void WfdSinkHiSysEvent::ReportDecodeError(MediaReportType type)
{
    if (hiSysEventStart_ == false) {
        return;
    }

    std::chrono::system_clock::time_point nowTimePoint = std::chrono::system_clock::now();
    int64_t nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTimePoint.time_since_epoch()).count();
    if (lastFreezeReportTime_ == 0 || nowTime < lastFreezeReportTime_) {
        lastFreezeReportTime_ = nowTime;
    }

    if ((nowTime - lastFreezeReportTime_) >= REPORT_INTERVAL_MS) {
        Reset();
    }

    if (isFreezeReportInTenMinute) {
        return;
    }

    if (type == MediaReportType::AUDIO) {
        audioFreezeCount_++;
    }
    if (type == MediaReportType::VIDEO) {
        videoFreezeCount_++;
    }
    if (audioFreezeCount_ >= FREEZE_COUNT) {
        ReportStabilityError(__func__, "", SinkStage::AUDIO_DECODE, SinkErrorCode::WIFI_DISPLAY_AUDIO_DECODE_TIMEOUT);
        lastFreezeReportTime_ = nowTime;
        isFreezeReportInTenMinute = true;
        SHARING_LOGE("ReportStabilityError audioFreeze");
    }
    if (videoFreezeCount_ >= FREEZE_COUNT) {
        ReportStabilityError(__func__, "", SinkStage::VIDEO_DECODE, SinkErrorCode::WIFI_DISPLAY_VIDEO_DECODE_TIMEOUT);
        lastFreezeReportTime_ = nowTime;
        isFreezeReportInTenMinute = true;
        SHARING_LOGE("ReportStabilityError videoFreeze");
    }
}

void WfdSinkHiSysEvent::RecordMediaDecodeStartTime(MediaReportType type, uint64_t pts)
{
    std::unique_lock<std::mutex> decodeLock(decodeMutex_);
    if (type == MediaReportType::AUDIO) {
        RecordDecordStartTime(pts, audioDecoderTime_);
    } else if (type == MediaReportType::VIDEO) {
        RecordDecordStartTime(pts, videoDecoderTime_);
    } else {
        SHARING_LOGD("not process");
    }
}

void WfdSinkHiSysEvent::MediaDecodeTimProc(MediaReportType type, uint64_t pts)
{
    std::unique_lock<std::mutex> decodeLock(decodeMutex_);
    if (type == MediaReportType::AUDIO) {
        ReportDecodeTime(pts, audioDecoderTime_, type);
    } else if (type == MediaReportType::VIDEO) {
        ReportDecodeTime(pts, videoDecoderTime_, type);
    } else {
        SHARING_LOGD("not process");
    }
}

void WfdSinkHiSysEvent::Reset()
{
    isFreezeReportInTenMinute = false;
    lastFreezeReportTime_ = 0;
    videoDecoderTime_.clear();
    audioDecoderTime_.clear();
    videoFreezeCount_ = 0;
    audioFreezeCount_ = 0;
}

uint64_t WfdSinkHiSysEvent::GetCurTimeInUs()
{
    return std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now())
        .time_since_epoch()
        .count();
}

void WfdSinkHiSysEvent::RecordDecordStartTime(uint64_t pts, std::map<uint64_t, uint64_t> &decodeMap)
{
    if (decodeMap.size() >= DECODE_TIME_MAP_CAPACITY) {
        decodeMap.clear();
    }
    decodeMap[pts] = GetCurTimeInUs();
}

void WfdSinkHiSysEvent::ReportDecodeTime(uint64_t pts, std::map<uint64_t, uint64_t> &decodeMap, MediaReportType type)
{
    uint64_t decodeEndTime = GetCurTimeInUs();
    auto decodeStartTime = decodeMap.find(pts);
    if (decodeStartTime == decodeMap.end()) {
        return;
    }
    if (decodeEndTime >= decodeStartTime->second) {
        auto interval = decodeEndTime - decodeStartTime->second;
        if (interval > DECODE_TIME_OUT) {
            SHARING_LOGE("decode time out type %{public}d, time %{public}" PRIu64, type, interval);
            decodeMap.erase(decodeStartTime);
            ReportDecodeError(type);
            return;
        }
    }
    decodeMap.erase(decodeStartTime);
}

} // namespace Sharing
} // namespace OHOS