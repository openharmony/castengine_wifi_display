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
static constexpr char SHARING_SINK_TO_CALL_PKG[] = "wpa_supplicant";
static constexpr char SHARING_SINK_LOCAL_DEV_TYPE[] = "09C";

void WfdSinkHiSysEvent::SetHiSysEventDevInfo(WfdSinkHiSysEvent::SinkHisyseventDevInfo devInfo)
{
    devInfo_.localDevName_ = GetAnonyDevName(devInfo.localDevName_).c_str();
    devInfo_.localIp_ = GetAnonymousIp(devInfo.localIp_).c_str();
    devInfo_.localWifiMac_ = GetAnonymousMAC(devInfo.localWifiMac_).c_str();
    devInfo_.localNetId_ = devInfo.localNetId_;
    devInfo_.peerDevName_ = GetAnonyDevName(devInfo.peerDevName_).c_str();
    devInfo_.peerIp_ = GetAnonymousIp(devInfo.peerIp_).c_str();
    devInfo_.peerWifiMac_ = GetAnonymousMAC(devInfo.peerWifiMac_).c_str();
    devInfo_.peerNetId_ = devInfo.peerNetId_;
}

void WfdSinkHiSysEvent::GetStartTime(std::chrono::system_clock::time_point startTime)
{
    startTime_ = std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()).count();
}

void WfdSinkHiSysEvent::ChangeHisysEventScene(SinkBizScene scene)
{
    sinkBizScene_ = static_cast<int32_t>(scene);
}

void WfdSinkHiSysEvent::StartReport(const std::string &funcName, SinkStage sinkStage, SinkStageRes sinkStageRes)
{
    if (sinkBizScene_ == static_cast<int32_t>(SinkBizScene::ESTABLISH_MIRRORING)) {
        hiSysEventStart_ = true;
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
                    "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                    "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp_.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                    "PEER_IP", devInfo_.peerIp_.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName_.c_str());
}

void WfdSinkHiSysEvent::Report(const std::string &funcName, SinkStage sinkStage, SinkStageRes sinkStageRes)
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
                    "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                    "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp_.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                    "PEER_IP", devInfo_.peerIp_.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName_.c_str());
}

void WfdSinkHiSysEvent::FirstSceneEndReport(const std::string &funcName, SinkStage sinkStage, SinkStageRes sinkStageRes)
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
                    "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                    "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp_.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                    "PEER_IP", devInfo_.peerIp_.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName_.c_str());
}

void WfdSinkHiSysEvent::ThirdSceneEndReport(const std::string &funcName, SinkStage sinkStage)
{
    if (hiSysEventStart_ == false) {
        SHARING_LOGE("func:%{public}s, sinkStage:%{public}d, scece is Invalid", funcName.c_str(), sinkStage);
        return;
    }
    std::chrono::system_clock::time_point endTimePoint = std::chrono::system_clock::now();
    int endTime = std::chrono::duration_cast<std::chrono::seconds>(endTimePoint.time_since_epoch()).count();
    int64_t duration = endTime - startTime_;

    HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", static_cast<int32_t>(SinkBizScene::DISCONNECT_MIRRORING),
                    "BIZ_STAGE", static_cast<int32_t>(SinkStage::DISCONNECT_COMPLETE),
                    "STAGE_RES", static_cast<int32_t>(SinkStageRes::SUCCESS),
                    "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_END),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                    "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp_.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                    "PEER_IP", devInfo_.peerIp_.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName_.c_str(),
                    "SERVICE_DURATION", duration);
    hiSysEventStart_ = false;
}

void WfdSinkHiSysEvent::ReportError(const std::string &funcName, SinkStage sinkStage, SinkErrorCode errorCode)
{
    if (hiSysEventStart_ == false) {
        SHARING_LOGE("func:%{public}s, sinkStage:%{public}d, scece is Invalid", funcName.c_str(), sinkStage);
        return;
    }
    std::chrono::system_clock::time_point endTimePoint = std::chrono::system_clock::now();
    int endTime = std::chrono::duration_cast<std::chrono::seconds>(endTimePoint.time_since_epoch()).count();
    int64_t duration = endTime - startTime_;
    if(sinkBizScene_ == static_cast<int32_t>(SinkBizScene::ESTABLISH_MIRRORING)) { //第一阶段失败带sceneEnd,不带duration
        HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", sinkBizScene_,
                    "BIZ_STAGE", static_cast<int32_t>(sinkStage),
                    "STAGE_RES", static_cast<int32_t>(SinkStageRes::FAIL),
                    "ERROR_CODE", static_cast<int32_t>(errorCode),
                    "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_END),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                    "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp_.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                    "PEER_IP", devInfo_.peerIp_.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName_.c_str());
    }
    if(sinkBizScene_ == static_cast<int32_t>(SinkBizScene::MIRRORING_STABILITY)) { //第二阶段失败不带sceneEnd,不带duration
        HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", sinkBizScene_,
                    "BIZ_STAGE", static_cast<int32_t>(sinkStage),
                    "STAGE_RES", static_cast<int32_t>(SinkStageRes::FAIL),
                    "ERROR_CODE", static_cast<int32_t>(errorCode),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                    "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp_.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                    "PEER_IP", devInfo_.peerIp_.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName_.c_str());
    }
    if(sinkBizScene_ == static_cast<int32_t>(SinkBizScene::DISCONNECT_MIRRORING)) { //第三阶段失败带sceneEnd,带duration
        HiSysEventWrite(SHARING_SINK_DFX_DOMAIN_NAME, SHARING_SINK_EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "FUNC_NAME", funcName.c_str(),
                    "BIZ_SCENE", sinkBizScene_,
                    "BIZ_STAGE", static_cast<int32_t>(sinkStage),
                    "STAGE_RES", static_cast<int32_t>(SinkStageRes::FAIL),
                    "ERROR_CODE", static_cast<int32_t>(errorCode),
                    "BIZ_STATE", static_cast<int32_t>(SinkBIZState::BIZ_STATE_END),
                    "ORG_PKG", SHARING_SINK_ORG_PKG,
                    "HOST_PKG", SHARING_SINK_HOST_PKG,
                    "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                    "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                    "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                    "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                    "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                    "LOCAL_IP", devInfo_.localIp_.c_str(),
                    "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                    "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                    "PEER_IP", devInfo_.peerIp_.c_str(),
                    "PEER_DEV_NAME", devInfo_.peerDevName_.c_str(),
                    "SERVICE_DURATION", duration);
    }
    hiSysEventStart_ = false;
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
                "TO_CALL_PKG", SHARING_SINK_TO_CALL_PKG,
                "LOCAL_NET_ID", devInfo_.localNetId_.c_str(),
                "LOCAL_WIFI_MAC", devInfo_.localWifiMac_.c_str(),
                "LOCAL_DEV_NAME", devInfo_.localDevName_.c_str(),
                "LOCAL_DEV_TYPE", SHARING_SINK_LOCAL_DEV_TYPE,
                "LOCAL_IP", devInfo_.localIp_.c_str(),
                "PEER_NET_ID", devInfo_.peerNetId_.c_str(),
                "PEER_WIFI_MAC", devInfo_.peerWifiMac_.c_str(),
                "PEER_IP", devInfo_.peerIp_.c_str(),
                "PEER_DEV_NAME", devInfo_.peerDevName_.c_str());
}

}  // namespace Sharing
}  // namespace OHOS