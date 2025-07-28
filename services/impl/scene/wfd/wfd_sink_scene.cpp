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

#include "wfd_sink_scene.h"
#include <unistd.h>
#include "common/common_macro.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "configuration/include/config.h"
#include "const_def.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "iservice_registry.h"
#include "network/socket/socket_utils.h"
#include "kits/c/wifi_hid2d.h"
#include "system_ability_definition.h"
#include "utils/utils.h"
#include "wfd_session_def.h"

using namespace OHOS::DistributedHardware;
namespace OHOS {
namespace Sharing {
constexpr int P2P_LISTEN_INTERVAL = 500;
constexpr int P2P_LISTEN_PERIOD = 500;
constexpr int DM_MAX_NAME_LENGTH = 32;

void WfdSinkScene::WfdSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId == WIFI_DEVICE_ABILITY_ID) {
        SHARING_LOGI("%{public}s, id is %{public}d.", __FUNCTION__, systemAbilityId);
        auto scene = scene_.lock();
        if (scene == nullptr) {
            SHARING_LOGE("WfdSinkScene is null");
            return;
        }
        scene->OnWifiAbilityResume();
    }
}
 
void WfdSinkScene::WfdSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId,
    const std::string& deviceId)
{
    if (systemAbilityId == WIFI_DEVICE_ABILITY_ID) {
        SHARING_LOGI("%{public}s, id is %{public}d.", __FUNCTION__, systemAbilityId);
        auto scene = scene_.lock();
        if (scene == nullptr) {
            SHARING_LOGE("WfdSinkScene is null");
            return;
        }
        scene->OnWifiAbilityDied();
    }
}

void WfdSinkScene::WfdP2pCallback::OnP2pStateChanged(int32_t state)
{
    SHARING_LOGI("state: %{public}d.", state);
    auto parent = parent_.lock();
    if (parent) {
        switch (state) {
            case (int32_t)Wifi::P2pState::P2P_STATE_NONE:
                break;
            case (int32_t)Wifi::P2pState::P2P_STATE_IDLE:
                break;
            case (int32_t)Wifi::P2pState::P2P_STATE_STARTING:
                break;
            case (int32_t)Wifi::P2pState::P2P_STATE_STARTED:
                if (parent->isSinkRunning_) {
                    parent->WfdP2pStart();
                }
                break;
            case (int32_t)Wifi::P2pState::P2P_STATE_CLOSING:
                break;
            case (int32_t)Wifi::P2pState::P2P_STATE_CLOSED:
                if (parent->isSinkRunning_) {
                    parent->isSinkRunning_ = false;
                    parent->WfdP2pStop();
                    parent->OnInnerError("", SharingErrorCode::ERR_NETWORK_ERROR, "NETWORK ERROR, P2P MODULE STOPPED");
                    WfdSinkHiSysEvent::GetInstance().P2PReportError(__func__,
                                                                    SinkErrorCode::WIFI_DISPLAY_P2P_DISCONNECTED_STATE_CLOSED);
                }
                break;
            default:
                SHARING_LOGI("none process case.");
                break;
        }
    }
}

void WfdSinkScene::WfdP2pCallback::OnP2pPersistentGroupsChanged(void)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WfdP2pCallback::OnP2pThisDeviceChanged(const Wifi::WifiP2pDevice &device)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WfdP2pCallback::OnP2pPeersChanged(const std::vector<Wifi::WifiP2pDevice> &device)
{
    SHARING_LOGD("trace.");
    auto parent = parent_.lock();
    if (parent) {
        SHARING_LOGI("device size: %{public}zu.", device.size());
        for (auto itDev : device) {
            auto status = itDev.GetP2pDeviceStatus();
            SHARING_LOGI("device mac: %{public}s, status: %{public}d.",
                         GetAnonymousMAC(itDev.GetDeviceAddress()).c_str(), status);
            switch (status) {
                case Wifi::P2pDeviceStatus::PDS_AVAILABLE: {
                    ConnectionInfo connectionInfo;
                    connectionInfo.mac = itDev.GetDeviceAddress();
                    connectionInfo.deviceName = itDev.GetDeviceName();
                    connectionInfo.primaryDeviceType = itDev.GetPrimaryDeviceType();
                    connectionInfo.secondaryDeviceType = itDev.GetSecondaryDeviceType();
                    connectionInfo.ctrlPort = itDev.GetWfdInfo().GetCtrlPort();
                    connectionInfo.state = ConnectionState::DISCONNECTED;

                    parent->OnP2pPeerDisconnected(connectionInfo);
                    break;
                }
                case Wifi::P2pDeviceStatus::PDS_CONNECTED: {
                    std::unique_lock<std::mutex> lock(parent->currentConnectDevMutex_);
                    parent->currentConnectDev_.mac = itDev.GetDeviceAddress();
                    parent->currentConnectDev_.deviceName = itDev.GetDeviceName();
                    parent->currentConnectDev_.primaryDeviceType = itDev.GetPrimaryDeviceType();
                    parent->currentConnectDev_.secondaryDeviceType = itDev.GetSecondaryDeviceType();
                    parent->currentConnectDev_.ctrlPort = itDev.GetWfdInfo().GetCtrlPort();
                    parent->currentConnectDev_.ip = "";
                    parent->currentConnectDev_.state = ConnectionState::CONNECTED;
                    break;
                }
                default:
                    SHARING_LOGI("none process case.");
                    break;
            }
        }
    }
}

void WfdSinkScene::WfdP2pCallback::OnP2pPrivatePeersChanged(const std::string &priWfdInfo)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WfdP2pCallback::OnP2pServicesChanged(const std::vector<Wifi::WifiP2pServiceInfo> &srvInfo)
{
    SHARING_LOGD("trace.");
}


void WfdSinkScene::WfdP2pCallback::OnP2pConnectionChanged(const Wifi::WifiP2pLinkedInfo &info)
{
    SHARING_LOGI("trace");
    auto parent = parent_.lock();
    RETURN_IF_NULL(parent);
    if (!parent->isSinkRunning_) {
        return;
    }

    Wifi::P2pConnectedState state = info.GetConnectState();
    if (state == Wifi::P2pConnectedState::P2P_DISCONNECTED) {
        SHARING_LOGI("OnP2pConnectionChanged disconnected");
        std::string mac;
        {
            std::unique_lock<std::mutex> lock(parent->currentConnectDevMutex_);
            mac = parent->currentConnectDev_.mac;
        }
        if (mac != "") {
            parent->OnP2pPeerDisconnected(mac);
        }
        {
            std::unique_lock<std::mutex> lock(parent->localIpMutex_);
            parent->localIp_ = "";
        }
        parent->WfdP2pStart();
        return;
    }

    parent->p2pInstance_->StopP2pListen();
    Wifi::WifiP2pGroupInfo group;
    if (Wifi::ErrCode::WIFI_OPT_SUCCESS != parent->p2pInstance_->GetCurrentGroup(group)) {
        SHARING_LOGE("GetCurrentGroup failed");
        return;
    }
    SHARING_LOGI("group frequency %{public}d", group.GetFrequency());
    if (info.IsGroupOwner()) {
        SHARING_LOGI("sink is go");
        std::unique_lock<std::mutex> lock(parent->localIpMutex_);
        parent->localIp_ = info.GetGroupOwnerAddress();
        return;
    } else {
        SHARING_LOGI("sink is gc");
        std::string remoteIp = info.GetGroupOwnerAddress();
        if (remoteIp == "" || remoteIp == "0.0.0.0") {
            SHARING_LOGE("get remoteIp ip failed");
            return;
        }
        {
            std::unique_lock<std::mutex> lock(parent->currentConnectDevMutex_);
            parent->currentConnectDev_.ip = remoteIp;
        }
        std::string interface = group.GetInterface();
        {
            std::unique_lock<std::mutex> lock(parent->localIpMutex_);
            parent->localIp_ = GetLocalP2pAddress(interface);
            if (parent->localIp_.empty()) {
                SHARING_LOGW("get local ip failed");
                return;
            }
        }
        wfdTrustListManager_.AddBoundDevice(group);
    }
    ConnectionInfo currentConnectDev;
    {
        std::unique_lock<std::mutex> lock(parent->currentConnectDevMutex_);
        currentConnectDev = parent->currentConnectDev_;
    }
    parent->OnP2pPeerConnected(currentConnectDev);
    std::unique_lock<std::mutex> lock(parent->currentConnectDevMutex_);
    parent->currentConnectDev_ = currentConnectDev;
}

void WfdSinkScene::WfdP2pCallback::OnP2pGcJoinGroup(const OHOS::Wifi::GcInfo &info)
{
    SHARING_LOGI("trace.");
    auto parent = parent_.lock();
    if (parent && parent->p2pInstance_) {
        std::vector<Wifi::WifiP2pDevice> devices;
        if (Wifi::ErrCode::WIFI_OPT_SUCCESS != parent->p2pInstance_->QueryP2pDevices(devices)) {
            SHARING_LOGE("QueryP2pDevices failed");
            return;
        }
        SHARING_LOGI("QueryP2pDevices ip:%{private}s addr: %{private}s host: %{private}s.",
                     GetAnonymousIp(info.ip).c_str(), GetAnonymousMAC(info.mac).c_str(),
                     GetAnonyString(info.host).c_str());
        if (info.ip == "0.0.0.0" || info.ip == "") {
            SHARING_LOGE("device: %{private}s leased ip is: 0.0.0.0.", GetAnonymousMAC(info.mac).c_str());
            parent->OnInnerError(info.mac.c_str(), ERR_P2P_DHCP_INVALID_IP, "ip is: 0.0.0.0.");
            return;
        }

        for (auto itDev : devices) {
            if (itDev.GetDeviceAddress() != info.mac) {
                continue;
            }
            ConnectionInfo connectionInfo;
            connectionInfo.ip = info.ip;
            connectionInfo.mac = itDev.GetDeviceAddress();
            connectionInfo.primaryDeviceType = itDev.GetPrimaryDeviceType();
            connectionInfo.secondaryDeviceType = itDev.GetSecondaryDeviceType();
            connectionInfo.ctrlPort = itDev.GetWfdInfo().GetCtrlPort();
            connectionInfo.deviceName = itDev.GetDeviceName();
            connectionInfo.state = ConnectionState::CONNECTED;
            {
                std::unique_lock<std::mutex> lock(parent->currentConnectDevMutex_);
                parent->currentConnectDev_ = connectionInfo;
            }
            SHARING_LOGD("device connected, mac: %{private}s, ip: %{private}s, port: %{private}d",
                GetAnonymousMAC(connectionInfo.mac).c_str(), GetAnonymousIp(connectionInfo.ip).c_str(),
                connectionInfo.ctrlPort);
            parent->OnP2pPeerConnected(connectionInfo);
            Wifi::WifiP2pGroupInfo group;
            parent->p2pInstance_->GetCurrentGroup(group);
            wfdTrustListManager_.AddBoundDevice(group);
            return;
        }
    }
}

void WfdSinkScene::WfdP2pCallback::OnP2pGcLeaveGroup(const OHOS::Wifi::GcInfo &info)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WfdP2pCallback::OnP2pDiscoveryChanged(bool isChange)
{
    SHARING_LOGD("isChange: %{public}d.", isChange);
}

std::unordered_map<Wifi::ErrCode, SinkErrorCode> WfdSinkScene::WfdP2pCallback::wifiErrorMapping_ = {
    { Wifi::WIFI_OPT_FAILED, SinkErrorCode::WIFI_DISPLAY_P2P_FAILED },
    { Wifi::WIFI_OPT_NOT_SUPPORTED, SinkErrorCode::WIFI_DISPLAY_P2P_OPT_NOT_SUPPORTED },
    { Wifi::WIFI_OPT_INVALID_PARAM, SinkErrorCode::WIFI_DISPLAY_P2P_OPT_INVALID_PARAM },
    { Wifi::WIFI_OPT_FORBID_AIRPLANE, SinkErrorCode::WIFI_DISPLAY_P2P_OPT_FORBID_AIRPLANE },
    { Wifi::WIFI_OPT_FORBID_POWSAVING, SinkErrorCode::WIFI_DISPLAY_P2P_OPT_FORBID_POWSAVING },
    { Wifi::WIFI_OPT_PERMISSION_DENIED, SinkErrorCode::WIFI_DISPLAY_P2P_OPT_PERMISSION_DENIED },
    { Wifi::WIFI_OPT_OPEN_FAIL_WHEN_CLOSING, SinkErrorCode::WIFI_DISPLAY_P2P_OPT_OPEN_FAIL_WHEN_CLOSING },
    { Wifi::WIFI_OPT_P2P_NOT_OPENED, SinkErrorCode::WIFI_DISPLAY_P2P_OPT_P2P_NOT_OPENED },
};

bool WfdSinkScene::WfdP2pCallback::GetErrorCode(Wifi::ErrCode errorCode, SinkErrorCode &sharingError)
{
    auto it = wifiErrorMapping_.find(errorCode);
    if (it != wifiErrorMapping_.end()) {
        sharingError = it->second;
        SHARING_LOGI("wifi errorCode: %{public}d, sharingError:%{public}d", errorCode, sharingError);
        return true;
    } else {
        SHARING_LOGI("unknow wifi errorCode: %{public}d", errorCode);
        return false;
    }
}

void WfdSinkScene::WfdP2pCallback::OnP2pActionResult(Wifi::P2pActionCallback action, Wifi::ErrCode code)
{
    SHARING_LOGI("action %{public}hhu, code %{public}d", action, code);
    auto parent = parent_.lock();
    if (parent) {
        SinkErrorCode sharingErrorCode;
        if (code != Wifi::WIFI_OPT_SUCCESS) {
            if (GetErrorCode(code, sharingErrorCode)) {
                SHARING_LOGI("wifi errorCode: %{public}d, sharingErrorCode:%{public}d", code, sharingErrorCode);
                WfdSinkHiSysEvent::GetInstance().P2PReportError(__func__, sharingErrorCode);
            } else {
                SHARING_LOGI("default return action %{public}hhu, code %{public}d", action, code);
                WfdSinkHiSysEvent::GetInstance().P2PReportError(__func__, SinkErrorCode::WIFI_DISPLAY_P2P_FAILED);
            }
        }
    }
}

void WfdSinkScene::WfdP2pCallback::OnConfigChanged(Wifi::CfgType type, char *data, int32_t dataLen)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WfdP2pCallback::OnP2pChrErrCodeReport(const int errCode)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WifiCallback::OnWifiStateChanged(int state)
{
    SHARING_LOGI("OnWifiStateChanged state %{public}d", state);
    if (state == static_cast<int32_t>(Wifi::WifiState::ENABLED)) {
        auto parent = parent_.lock();
        if (parent && parent->isSinkRunning_) {
            parent->WfdP2pStart();
        }
    }
}

void WfdSinkScene::WifiCallback::OnWifiConnectionChanged(int state, const OHOS::Wifi::WifiLinkedInfo &info)
{
    SHARING_LOGI("OnWifiConnectionChanged state %{public}d", state);
    if (state == static_cast<int32_t>(Wifi::ConnState::CONNECTED) ||
        state == static_cast<int32_t>(Wifi::ConnState::DISCONNECTED)) {
        auto parent = parent_.lock();
        if (!parent) {
            return;
        }
        std::string mac;
        {
            std::unique_lock<std::mutex> lock(parent->currentConnectDevMutex_);
            mac = parent->currentConnectDev_.mac;
        }
        if (parent->isSinkRunning_ && mac == "") {
            parent->WfdP2pStart();
        }
    }
}

void WfdSinkScene::WifiCallback::OnWifiRssiChanged(int rssi)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WifiCallback::OnWifiWpsStateChanged(int state, const std::string &pinCode)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WifiCallback::OnStreamChanged(int direction)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::WifiCallback::OnDeviceConfigChanged(OHOS::Wifi::ConfigChange value)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::DeviceNameObserver::OnChange()
{
    SHARING_LOGI("deviceName observer onChange");
    auto scene = scene_.lock();
    if (scene && scene->isSinkRunning_) {
        scene->InitP2pName();
    }
}

WfdSinkScene::WfdSinkScene()
{
    SHARING_LOGI("id: %{public}u.", GetId());
    currentConnectDev_.state = ConnectionState::DISCONNECTED;
}

WfdSinkScene::~WfdSinkScene()
{
    SHARING_LOGI("id: %{public}u.", GetId());
    Release();
}

void WfdSinkScene::Initialize()
{
    SHARING_LOGD("trace.");
    SharingValue::Ptr values = nullptr;

    auto ret = Config::GetInstance().GetConfig("sharingWfd", "ctrlport", "defaultWfdCtrlport", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(ctrlPort_);
    }

    ret = Config::GetInstance().GetConfig("sharingWfd", "abilityLimit", "accessDevMaximum", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(accessDevMaximum_);
    }

    ret = Config::GetInstance().GetConfig("sharingWfd", "abilityLimit", "surfaceMaximum", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(surfaceMaximum_);
    }

    ret = Config::GetInstance().GetConfig("sharingWfd", "abilityLimit", "foregroundMaximum", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(foregroundMaximum_);
    }

    ret = Config::GetInstance().GetConfig("sharingWfd", "mediaFormat", "videoCodec", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t videoCodec;
        values->GetValue<int32_t>(videoCodec);
        videoCodecId_ = static_cast<CodecId>(videoCodec);
    }

    ret = Config::GetInstance().GetConfig("sharingWfd", "mediaFormat", "videoFormat", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t videoFormat;
        values->GetValue<int32_t>(videoFormat);
        videoFormatId_ = static_cast<VideoFormat>(videoFormat);
    }

    ret = Config::GetInstance().GetConfig("sharingWfd", "mediaFormat", "audioCodec", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t audioCodec;
        values->GetValue<int32_t>(audioCodec);
        audioCodecId_ = static_cast<CodecId>(audioCodec);
    }

    ret = Config::GetInstance().GetConfig("sharingWfd", "mediaFormat", "audioFormat", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t audioFormat;
        values->GetValue<int32_t>(audioFormat);
        audioFormatId_ = static_cast<AudioFormat>(audioFormat);
    }

    RegisterP2pListener();
    RegisterWifiStatusChangeListener();
    RegisterDevNameObserver();
    InitP2pName();
    isInitialized_ = true;
    RegisterWfdAbilityListener();
}

void WfdSinkScene::OnWifiAbilityResume()
{
    SHARING_LOGI("%{public}s.", __FUNCTION__);
    if (isInitialized_) {
        return;
    }
    RegisterP2pListener();
    RegisterWifiStatusChangeListener();
    InitP2pName();
    isInitialized_ = true;
}

void WfdSinkScene::OnWifiAbilityDied()
{
    SHARING_LOGI("%{public}s.", __FUNCTION__);
    std::string mac;
    {
        std::unique_lock<std::mutex> lock(currentConnectDevMutex_);
        mac = currentConnectDev_.mac;
    }
    if (mac != "") {
        OnP2pPeerDisconnected(mac);
    }
    isInitialized_ = false;
}

void WfdSinkScene::RegisterDevNameObserver()
{
    SHARING_LOGI("RegisterDevNameObserver");
    if (deviceNameObserver_ != nullptr) {
        SHARING_LOGI("unregister old name observer");
        UnRegisterDevNameObserver();
    }

    deviceNameObserver_ = new (std::nothrow) DeviceNameObserver(shared_from_this());
    if (deviceNameObserver_ == nullptr) {
        SHARING_LOGE("new deviceNameObserver error");
        return;
    }
    int32_t ret = DataShareHelper::GetInstance().RegisterObserver(deviceNameObserver_);
    SHARING_LOGI("register name observer ret=%{public}d", ret);
}

void WfdSinkScene::UnRegisterDevNameObserver()
{
    int32_t ret = DataShareHelper::GetInstance().UnregisterObserver(deviceNameObserver_);
    SHARING_LOGI("unregister name observer ret=%{public}d", ret);
    deviceNameObserver_ = nullptr;
}

void WfdSinkScene::InitP2pName()
{
    DmKit::InitDeviceManager();
    std::string deviceName;
    if (DeviceManager::GetInstance().GetLocalDisplayDeviceName(DM_PKG_NAME, DM_MAX_NAME_LENGTH, deviceName) != DM_OK) {
        SHARING_LOGW("getLocalDeviceInfo from dm failed");
    } else {
        if (p2pInstance_) {
            p2pInstance_->SetP2pDeviceName(deviceName);
        }
    }
}

void WfdSinkScene::RegisterP2pListener()
{
    p2pInstance_ = Wifi::WifiP2p::GetInstance(WIFI_P2P_ABILITY_ID);
    RETURN_IF_NULL(p2pInstance_);
    sptr<WfdP2pCallback> wfdP2pCallback(new WfdP2pCallback(shared_from_this()));
    std::vector<std::string> event = {EVENT_P2P_PEER_DEVICE_CHANGE, EVENT_P2P_CONN_STATE_CHANGE,
                                      EVENT_P2P_GC_JOIN_GROUP, EVENT_P2P_GC_LEAVE_GROUP, EVENT_P2P_ACTION_RESULT};
    p2pInstance_->RegisterCallBack(wfdP2pCallback, event);
}

void WfdSinkScene::RegisterWifiStatusChangeListener()
{
    auto deviceInstance = Wifi::WifiDevice::GetInstance(WIFI_DEVICE_ABILITY_ID);
    RETURN_IF_NULL(deviceInstance);
    auto callback = sptr<Wifi::IWifiDeviceCallBack> (new WifiCallback(shared_from_this()));
    std::vector<std::string> events = {EVENT_STA_POWER_STATE_CHANGE, EVENT_STA_CONN_STATE_CHANGE};
    deviceInstance->RegisterCallBack(callback, events);
}

void WfdSinkScene::Release()
{
    SHARING_LOGD("trace.");
    UnRegisterWfdAbilityListener();
    UnRegisterDevNameObserver();
    std::unique_lock<std::mutex> lock(mutex_);
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        for (auto &item : devConnectionMap_) {
            uint32_t contextId = INVALID_ID;
            uint32_t agentId = INVALID_ID;
            if (item.second == nullptr) {
                continue;
            }

            contextId = item.second->contextId;
            agentId = item.second->agentId;

            if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
                continue;
            }

            auto sessionMsg = std::make_shared<WfdSinkSessionEventMsg>();
            sessionMsg->type = EVENT_SESSION_TEARDOWN;
            sessionMsg->toMgr = MODULE_CONTEXT;
            sessionMsg->dstId = contextId;
            sessionMsg->agentId = agentId;

            SharingEvent event;
            event.eventMsg = std::move(sessionMsg);
            sharingAdapter->ForwardEvent(contextId, agentId, event, true);

            P2pRemoveClient(*(item.second));

            sharingAdapter->DestroyAgent(contextId, agentId);
        }
    }

    devConnectionMap_.clear();
    devSurfaceItemMap_.clear();
    p2pInstance_.reset();
}

void WfdSinkScene::OnDomainMsg(std::shared_ptr<BaseDomainMsg> &msg)
{
    SHARING_LOGD("trace.");
}

void WfdSinkScene::OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);
    SHARING_LOGI("recv msg, msg id: %{public}d.", msg->GetMsgId());
    switch (msg->GetMsgId()) {
        case WfdSinkStartReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSinkStartReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleStart(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdSinkStopReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSinkStopReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleStop(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdAppendSurfaceReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdAppendSurfaceReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleAppendSurface(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdRemoveSurfaceReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdRemoveSurfaceReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleRemoveSurface(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case SetMediaFormatReq::MSG_ID: {
            auto data = std::static_pointer_cast<SetMediaFormatReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleSetMediaFormat(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case SetSceneTypeReq::MSG_ID: {
            auto data = std::static_pointer_cast<SetSceneTypeReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleSetSceneType(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdPlayReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdPlayReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandlePlay(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdPauseReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdPauseReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandlePause(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case MuteReq::MSG_ID: {
            auto data = std::static_pointer_cast<MuteReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleMute(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case UnMuteReq::MSG_ID: {
            auto data = std::static_pointer_cast<UnMuteReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleUnMute(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdCloseReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdCloseReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleClose(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case GetSinkConfigReq::MSG_ID: {
            auto data = std::static_pointer_cast<GetSinkConfigReq>(msg);
            auto rsp = std::make_shared<GetSinkConfigRsp>();

            rsp->foregroundMaximum = INVALID_ID;
            HandleGetConfig(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdGetBoundDevicesReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdGetBoundDevicesReq>(msg);
            auto rsp = std::make_shared<WfdGetBoundDevicesRsp>();

            HandleGetBoundDevices(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdDeleteBoundDeviceReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdDeleteBoundDeviceReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            HandleDeleteBoundDevice(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        default:
            SHARING_LOGW("unknown msg request.");
            break;
    }
}

void WfdSinkScene::SetWifiScene(uint32_t scene)
{
    SHARING_LOGI("SetWifiScene %{public}u start", scene);
    std::string ifName = "p2p0";
    Hid2dUpperScene upperScene;
    upperScene.scene = scene;
    upperScene.fps = -1;
    upperScene.bw = 0;
    if (Hid2dSetUpperScene(ifName.c_str(), &upperScene) != 0) {
        SHARING_LOGE("SetWifiScene scene: %{public}u error", scene);
    }
    SHARING_LOGI("SetWifiScene %{public}u over", scene);
}

int32_t WfdSinkScene::HandleStart(std::shared_ptr<WfdSinkStartReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)msg;
    (void)reply;
    if (isSinkRunning_) {
        SHARING_LOGW("p2p sink is running.");
        return 0;
    }
    RETURN_INVALID_IF_NULL(p2pInstance_);
    int32_t ret = 0;
    int32_t status = 0;
    p2pInstance_->GetP2pEnableStatus(status);
    switch (status) {
        case (int32_t)Wifi::P2pState::P2P_STATE_NONE:
        case (int32_t)Wifi::P2pState::P2P_STATE_IDLE:
        case (int32_t)Wifi::P2pState::P2P_STATE_STARTING:
        case (int32_t)Wifi::P2pState::P2P_STATE_CLOSING:
            WfdSinkHiSysEvent::GetInstance().P2PReportError(__func__,
                                                            SinkErrorCode::WIFI_DISPLAY_P2P_DISCONNECTED_STATE_CLOSED);
            OnInnerError(0, 0, SharingErrorCode::ERR_STATE_EXCEPTION, "HandleStart current p2p state: CLOSING");
            ret = -1;
            break;
        case (int32_t)Wifi::P2pState::P2P_STATE_STARTED:
            isSinkRunning_ = true;
            WfdP2pStart();
            break;
        case (int32_t)Wifi::P2pState::P2P_STATE_CLOSED:
            if (Wifi::ErrCode::WIFI_OPT_SUCCESS != p2pInstance_->EnableP2p()) {
                SHARING_LOGE("EnableP2p failed");
                return -1;
            }
            isSinkRunning_ = true;
            WfdP2pStart();
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return ret;
}

int32_t WfdSinkScene::HandleStop(std::shared_ptr<WfdSinkStopReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("handle stop, now connect device num: %{public}zu.", devConnectionMap_.size());
    (void)msg;
    (void)reply;
    if (!isSinkRunning_) {
        SHARING_LOGW("p2p sink is not running.");
        return -1;
    }

    isSinkRunning_ = false;
    WfdP2pStop();

    return 0;
}

int32_t WfdSinkScene::HandleAppendSurface(std::shared_ptr<WfdAppendSurfaceReq> &msg,
                                          std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleAppendSurface can't find the dev");
            WfdSinkHiSysEvent::GetInstance().ReportError(__func__, "", SinkStage::SESSION_NEGOTIATION,
                                                        SinkErrorCode::WIFI_DISPLAY_BAD_PARAMETER);
            return -1;
        }
        int32_t surfaceNum = 0;
        for (auto itemSurface = devSurfaceItemMap_.begin(); itemSurface != devSurfaceItemMap_.end();) {
            if (!itemSurface->second->deleting) {
                surfaceNum++;
            }
            itemSurface++;
        }

        if (surfaceNum >= surfaceMaximum_) {
            lock.unlock();
            SHARING_LOGE("surface is too much.");
            OnInnerError(0, 0, SharingErrorCode::ERR_SERVICE_LIMIT, "HandleAppendSurface surface is too much");
            WfdSinkHiSysEvent::GetInstance().ReportError(__func__, "", SinkStage::SESSION_NEGOTIATION,
                                                        SinkErrorCode::WIFI_DISPLAY_ADD_SURFACE_ERROR);
            return -1;
        }

        sptr<IBufferProducer> producer = iface_cast<IBufferProducer>(msg->surface);
        sptr<Surface> surfacePtr = Surface::CreateSurfaceAsProducer(producer);
        if (surfacePtr == nullptr) {
            SHARING_LOGE("invalid surface.");
            return -1;
        }

        uint64_t surfaceId = surfacePtr->GetUniqueId();
        SHARING_LOGI("get surfaceId %{public}" PRIx64 ".", surfaceId);
        if (devSurfaceItemMap_.count(surfaceId)) {
            SHARING_LOGW("this surface is using, surfaceId: %{public}" PRIx64 ".", surfaceId);
            lock.unlock();
            OnInnerError(0, 0, SharingErrorCode::ERR_STATE_EXCEPTION, "HandleAppendSurface this surface is using");
            WfdSinkHiSysEvent::GetInstance().ReportError(__func__, "", SinkStage::SESSION_NEGOTIATION,
                                                        SinkErrorCode::WIFI_DISPLAY_ADD_SURFACE_ERROR);
            return ERR_STATE_EXCEPTION;
        }

        int32_t foregroundSurfaceNum = 0;
        for (auto item : devSurfaceItemMap_) {
            if ((item.second != nullptr) && (item.second->deviceId == msg->deviceId) && (!item.second->deleting)) {
                OnInnerError(0, 0, SharingErrorCode::ERR_STATE_EXCEPTION, "Only one surface can be set.");
                return ERR_STATE_EXCEPTION;
            }

            if (item.second != nullptr && item.second->sceneType == SceneType::FOREGROUND) {
                foregroundSurfaceNum++;
            }
        }

        auto devSurfaceItem = std::make_shared<DevSurfaceItem>();
        devSurfaceItem->contextId = itemDev->second->contextId;
        devSurfaceItem->agentId = itemDev->second->agentId;
        devSurfaceItem->deviceId = itemDev->second->mac;
        devSurfaceItem->surfacePtr = surfacePtr;
        devSurfaceItem->sceneType = (foregroundSurfaceNum >= foregroundMaximum_) ? BACKGROUND : FOREGROUND;
        devSurfaceItemMap_.emplace(surfaceId, devSurfaceItem);

        if (itemDev->second->isRunning) {
            uint32_t contextId = itemDev->second->contextId;
            uint32_t agentId = itemDev->second->agentId;

            auto startSessionMsg = std::make_shared<WfdSinkSessionEventMsg>();
            startSessionMsg->type = EVENT_WFD_REQUEST_IDR;
            startSessionMsg->toMgr = MODULE_CONTEXT;
            startSessionMsg->dstId = contextId;
            startSessionMsg->agentId = agentId;

            SharingEvent event;
            event.eventMsg = std::move(startSessionMsg);
            if (sharingAdapter) {
                sharingAdapter->ForwardEvent(contextId, agentId, event, false);
                sharingAdapter->AppendSurface(itemDev->second->contextId, itemDev->second->agentId, surfacePtr,
                                              devSurfaceItem->sceneType);
            }
            itemDev->second->state = ConnectionState::PLAYING;
            OnConnectionChanged(*itemDev->second);
        }
    }
    return 0;
}

int32_t WfdSinkScene::HandleRemoveSurface(std::shared_ptr<WfdRemoveSurfaceReq> &msg,
                                          std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleRemoveSurface can't find the dev");
            WfdSinkHiSysEvent::GetInstance().ReportError(__func__, "", SinkStage::SESSION_NEGOTIATION,
                                                        SinkErrorCode::WIFI_DISPLAY_BAD_PARAMETER);
            return -1;
        }

        auto item = devSurfaceItemMap_.find(msg->surfaceId);
        if (item == devSurfaceItemMap_.end()) {
            lock.unlock();
            SHARING_LOGE("can not find surfaceid, surfaceid: %{public}" PRId64 ".", msg->surfaceId);
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleAppendSurface can't find the dev");
            return -1;
        }

        SHARING_LOGW("del surface, surfaceid: %{public}" PRId64 ".", msg->surfaceId);
        sharingAdapter->RemoveSurface(itemDev->second->contextId, itemDev->second->agentId, msg->surfaceId);
    }

    return 0;
}

int32_t WfdSinkScene::HandleSetMediaFormat(std::shared_ptr<SetMediaFormatReq> &msg,
                                           std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleSetMediaFormat can not find dev");
            WfdSinkHiSysEvent::GetInstance().ReportError(__func__, "", SinkStage::SESSION_NEGOTIATION,
                                                        SinkErrorCode::WIFI_DISPLAY_BAD_PARAMETER);
            return -1;
        }
        itemDev->second->videoCodecId = static_cast<CodecId>(msg->videoAttr.codecType);
        itemDev->second->videoFormatId = static_cast<VideoFormat>(msg->videoAttr.formatId);
        itemDev->second->audioCodecId = static_cast<CodecId>(msg->audioAttr.codecType);
        itemDev->second->audioFormatId = static_cast<AudioFormat>(msg->audioAttr.formatId);
    }

    return 0;
}

int32_t WfdSinkScene::HandleSetSceneType(std::shared_ptr<SetSceneTypeReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    uint32_t contextId = INVALID_ID;
    uint32_t agentId = INVALID_ID;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleSetSceneType can not find dev");
            return -1;
        }

        auto itemSurface = devSurfaceItemMap_.find(msg->surfaceId);
        if (itemSurface == devSurfaceItemMap_.end() || itemSurface->second == nullptr
            || itemSurface->second->deleting) {
            lock.unlock();
            SHARING_LOGE("can not find surfaceid, surfaceid: %{public}" PRId64 ".", msg->surfaceId);
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleSetSceneType can't find the surfaceId");
            return -1;
        }

        contextId = itemDev->second->contextId;
        agentId = itemDev->second->agentId;

        uint32_t foregroundSurfaceNum = 1;
        if (msg->sceneType == SceneType::FOREGROUND) {
            for (auto item : devSurfaceItemMap_) {
                if ((item.first == msg->surfaceId) && (!item.second->deleting)) {
                    continue;
                }

                if (item.second->sceneType == SceneType::FOREGROUND) {
                    foregroundSurfaceNum++;
                }
            }

            if (foregroundSurfaceNum > static_cast<uint32_t>(foregroundMaximum_)) {
                lock.unlock();
                SHARING_LOGE("foreground surfaces is too much.");
                OnInnerError(0, 0, SharingErrorCode::ERR_SERVICE_LIMIT,
                             "HandleSetSceneType foreground surfaces is too much");
                return -1;
            } else {
                itemSurface->second->sceneType = SceneType::FOREGROUND;
            }
        } else {
            itemSurface->second->sceneType = SceneType::BACKGROUND;
        }
    }

    if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
        SHARING_LOGW("invalid contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
        return 0;
    }

    bool keyFrame = msg->sceneType == SceneType::BACKGROUND ? true : false;
    auto ret = sharingAdapter->SetKeyPlay(contextId, agentId, msg->surfaceId, keyFrame);
    if (!keyFrame) {
        sharingAdapter->SetKeyRedirect(contextId, agentId, msg->surfaceId, true);
    }

    return ret;
}

int32_t WfdSinkScene::HandlePlay(std::shared_ptr<WfdPlayReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandlePlay can not find dev");
            return -1;
        }

        uint32_t contextId = itemDev->second->contextId;
        uint32_t agentId = itemDev->second->agentId;
        if (contextId == INVALID_ID || agentId == INVALID_ID) {
            lock.unlock();
            SHARING_LOGE("connected, create sink agent failed, devMac: %{private}s.",
                         GetAnonymousMAC(msg->deviceId).c_str());
            return -1;
        }

        if (itemDev->second->isRunning == true) {
            lock.unlock();
            return sharingAdapter->Resume(contextId, agentId, MEDIA_TYPE_AV);
        } else {
            itemDev->second->isRunning = true;
        }

        lock.unlock();

        auto startSessionMsg = std::make_shared<WfdSinkSessionEventMsg>();
        startSessionMsg->type = EVENT_SESSION_INIT;
        startSessionMsg->toMgr = MODULE_CONTEXT;
        startSessionMsg->dstId = contextId;
        startSessionMsg->agentId = agentId;
        startSessionMsg->remoteIp = itemDev->second->ip;
        {
            std::unique_lock<std::mutex> lock(localIpMutex_);
            startSessionMsg->localIp = localIp_;
        }
        startSessionMsg->mac = itemDev->second->mac;
        startSessionMsg->remotePort = itemDev->second->ctrlPort;
        startSessionMsg->videoFormat = itemDev->second->videoFormatId;
        startSessionMsg->audioFormat = itemDev->second->audioFormatId;
        startSessionMsg->localPort = SocketUtils::GetAvailableUdpPortPair();
        startSessionMsg->wfdParamsInfo = wfdParamsInfo_;

        SharingEvent event;
        event.eventMsg = std::move(startSessionMsg);

        sharingAdapter->ForwardEvent(contextId, agentId, event, true);
        sharingAdapter->Start(contextId, agentId);

        for (auto &item : devSurfaceItemMap_) {
            if ((item.second != nullptr) && (itemDev->first == item.second->deviceId) && (!item.second->deleting)) {
                if (item.second->surfacePtr != nullptr) {
                    sharingAdapter->AppendSurface(contextId, agentId, item.second->surfacePtr, item.second->sceneType);
                }
            }
        }

        sharingAdapter->Play(contextId, agentId);
    }

    return 0;
}

int32_t WfdSinkScene::HandlePause(std::shared_ptr<WfdPauseReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    uint32_t contextId = INVALID_ID;
    uint32_t agentId = INVALID_ID;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandlePause can not find dev");
            return -1;
        }

        contextId = itemDev->second->contextId;
        agentId = itemDev->second->agentId;
    }

    if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
        return -1;
    }

    return sharingAdapter->Pause(contextId, agentId, MEDIA_TYPE_AV);
}

int32_t WfdSinkScene::HandleMute(std::shared_ptr<MuteReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    uint32_t contextId = INVALID_ID;
    uint32_t agentId = INVALID_ID;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleMute can not find dev");
            return -1;
        }

        contextId = itemDev->second->contextId;
        agentId = itemDev->second->agentId;
    }

    if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
        return -1;
    }

    return sharingAdapter->Pause(contextId, agentId, MEDIA_TYPE_AUDIO);
}

int32_t WfdSinkScene::HandleUnMute(std::shared_ptr<UnMuteReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    uint32_t contextId = INVALID_ID;
    uint32_t agentId = INVALID_ID;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleUnMute can not find dev");
            return -1;
        }

        contextId = itemDev->second->contextId;
        agentId = itemDev->second->agentId;
    }

    if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
        return -1;
    }

    return sharingAdapter->Resume(contextId, agentId, MEDIA_TYPE_AUDIO);
}

int32_t WfdSinkScene::HandleClose(std::shared_ptr<WfdCloseReq> &msg, std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    uint32_t contextId = INVALID_ID;
    uint32_t agentId = INVALID_ID;

    WfdSinkHiSysEvent::GetInstance().ChangeHisysEventScene(SinkBizScene::DISCONNECT_MIRRORING);
    WfdSinkHiSysEvent::GetInstance().StartReport(__func__, "", SinkStage::RECEIVE_DISCONNECT_EVENT,
                                                SinkStageRes::SUCCESS);

    ConnectionInfo connectionInfo;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(msg->deviceId);
        if (itemDev == devConnectionMap_.end() || itemDev->second == nullptr) {
            lock.unlock();
            SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonymousMAC(msg->deviceId).c_str());
            OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "HandleClose can not find dev");
            return -1;
        }

        connectionInfo.ip = itemDev->second->ip;
        connectionInfo.mac = itemDev->second->mac;
        connectionInfo.state = ConnectionState::DISCONNECTED;
        connectionInfo.surfaceId = itemDev->second->surfaceId;
        connectionInfo.deviceName = itemDev->second->deviceName;
        connectionInfo.primaryDeviceType = itemDev->second->primaryDeviceType;
        connectionInfo.secondaryDeviceType = itemDev->second->secondaryDeviceType;

        contextId = itemDev->second->contextId;
        agentId = itemDev->second->agentId;

        for (auto itemSurface = devSurfaceItemMap_.begin(); itemSurface != devSurfaceItemMap_.end();) {
            if ((itemSurface->second != nullptr) && (msg->deviceId == itemSurface->second->deviceId)) {
                itemSurface->second->deleting = true;
            }
            itemSurface++;
        }

        P2pRemoveClient(connectionInfo);

        devConnectionMap_.erase(msg->deviceId);
    }

    if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
        return -1;
    }

    OnConnectionChanged(connectionInfo);
    auto sessionMsg = std::make_shared<WfdSinkSessionEventMsg>();
    sessionMsg->type = EVENT_SESSION_TEARDOWN;
    sessionMsg->toMgr = MODULE_CONTEXT;
    sessionMsg->dstId = contextId;
    sessionMsg->agentId = agentId;

    SharingEvent event;
    event.eventMsg = std::move(sessionMsg);
    sharingAdapter->ForwardEvent(contextId, agentId, event, true);

    return sharingAdapter->DestroyAgent(contextId, agentId);
}

int32_t WfdSinkScene::HandleGetConfig(std::shared_ptr<GetSinkConfigReq> &msg, std::shared_ptr<GetSinkConfigRsp> &reply)
{
    SHARING_LOGD("trace.");
    (void)msg;
    RETURN_INVALID_IF_NULL(reply);
    reply->accessDevMaximum = static_cast<uint32_t>(accessDevMaximum_);
    reply->foregroundMaximum = static_cast<uint32_t>(foregroundMaximum_);
    reply->surfaceMaximum = static_cast<uint32_t>(surfaceMaximum_);

    return 0;
}

int32_t WfdSinkScene::HandleGetBoundDevices(std::shared_ptr<WfdGetBoundDevicesReq> &msg,
                                            std::shared_ptr<WfdGetBoundDevicesRsp> &reply)
{
    SHARING_LOGI("%{public}s.", __FUNCTION__);
    (void)msg;
    (void)reply;
    reply->trustDevices = wfdTrustListManager_.GetAllBoundDevices();
    return 0;
}

int32_t WfdSinkScene::HandleDeleteBoundDevice(std::shared_ptr<WfdDeleteBoundDeviceReq> &msg,
                                              std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGI("%{public}s.", __FUNCTION__);
    (void)reply;
    RETURN_INVALID_IF_NULL(msg);
    reply->ret = wfdTrustListManager_.DeleteBoundDeviceGroup(msg->deviceAddress);
    return 0;
}

void WfdSinkScene::RegisterWfdAbilityListener()
{
    SHARING_LOGI("%{public}s.", __FUNCTION__);
    std::unique_lock<std::mutex> lock(mutex_);
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        SHARING_LOGE("get system ability manager fail");
        return;
    }
    sysAbilityListener_ = new (std::nothrow) WfdSystemAbilityListener(shared_from_this());
    if (sysAbilityListener_ == nullptr) {
        SHARING_LOGE("sysAbilityListener create failed.");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(WIFI_DEVICE_ABILITY_ID, sysAbilityListener_);
    SHARING_LOGI("result is %{public}d.", ret);
}

void WfdSinkScene::UnRegisterWfdAbilityListener()
{
    SHARING_LOGI("%{public}s.", __FUNCTION__);
    std::unique_lock<std::mutex> lock(mutex_);
    if (sysAbilityListener_ == nullptr) {
        SHARING_LOGE("listener is null");
        return;
    }
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        SHARING_LOGE("get system ability manager fail");
        return;
    }
    int32_t ret = samgrProxy->UnSubscribeSystemAbility(WIFI_DEVICE_ABILITY_ID, sysAbilityListener_);
    SHARING_LOGI("result is %{public}d.", ret);
    sysAbilityListener_ = nullptr;
}

void WfdSinkScene::WfdP2pStart()
{
    SHARING_LOGI("trace.");
    if (p2pInstance_) {
        Wifi::WifiP2pWfdInfo wfdInfo;
        wfdInfo.SetWfdEnabled(true);
        wfdInfo.SetDeviceInfo(0x11);
        wfdInfo.SetCtrlPort(ctrlPort_);
        wfdInfo.SetMaxThroughput(0x00c8);

        p2pInstance_->SetP2pWfdInfo(wfdInfo);
        p2pInstance_->StartP2pListen(P2P_LISTEN_PERIOD, P2P_LISTEN_INTERVAL);
    }
}

void WfdSinkScene::WfdP2pStop()
{
    SHARING_LOGI("trace.");
    std::unique_lock<std::mutex> lock(mutex_);
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        for (auto item : devConnectionMap_) {
            uint32_t contextId = item.second->contextId;
            uint32_t agentId = item.second->agentId;

            if ((contextId != INVALID_ID) && (agentId != INVALID_ID)) {
                auto sessionMsg = std::make_shared<WfdSinkSessionEventMsg>();
                sessionMsg->type = EVENT_SESSION_TEARDOWN;
                sessionMsg->toMgr = MODULE_CONTEXT;
                sessionMsg->dstId = contextId;
                sessionMsg->agentId = agentId;

                SharingEvent event;
                event.eventMsg = std::move(sessionMsg);
                sharingAdapter->ForwardEvent(contextId, agentId, event, true);

                P2pRemoveClient(*(item.second));

                sharingAdapter->DestroyAgent(contextId, agentId);
            }
        }
    }

    if (p2pInstance_) {
        p2pInstance_->StopP2pListen();
    }

    devSurfaceItemMap_.clear();
    devConnectionMap_.clear();
}

void WfdSinkScene::FillAndReportDeviceInfo(const ConnectionInfo &connectionInfo)
{
    Wifi::WifiP2pGroupInfo group;
    p2pInstance_->GetCurrentGroup(group);
    int32_t netWorkId = group.GetNetworkId();
    Wifi::WifiP2pDevice deviceInfo;
    p2pInstance_->QueryP2pLocalDevice(deviceInfo);

    WfdSinkHiSysEvent::SinkHisyseventDevInfo devInfo;
    {
        std::unique_lock<std::mutex> lock(localIpMutex_);
        devInfo.localIp = localIp_.c_str();
    }
    devInfo.localWifiMac = deviceInfo.GetDeviceAddress();
    devInfo.localDevName = deviceInfo.GetDeviceName();
    devInfo.localNetId = std::to_string(netWorkId);
    devInfo.peerDevName = connectionInfo.deviceName.c_str();
    devInfo.peerIp = connectionInfo.ip.c_str();
    devInfo.peerNetId = std::to_string(netWorkId);
    devInfo.peerWifiMac = connectionInfo.mac.c_str();

    WfdSinkHiSysEvent::GetInstance().SetHiSysEventDevInfo(devInfo);
}

void WfdSinkScene::OnP2pPeerConnected(ConnectionInfo &connectionInfo)
{
    SHARING_LOGD("trace.");
    if (!isSinkRunning_) {
        SHARING_LOGW("sink service is not running.");
        return;
    }

    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_IF_NULL(sharingAdapter);

    if (devConnectionMap_.size() >= (uint32_t)accessDevMaximum_) {
        SHARING_LOGE("too more device.");
        P2pRemoveClient(connectionInfo);

        auto ipcAdapter = ipcAdapter_.lock();
        RETURN_IF_NULL(ipcAdapter);

        auto msg = std::make_shared<WfdErrorMsg>();
        msg->message = "Maximum number of devices reached";
        msg->errorCode = ERR_RECEIVING_LIMIT;

        auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());
        ipcAdapter->SendRequest(msg, reply);

        return;
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (devConnectionMap_.count(connectionInfo.mac)) {
            SHARING_LOGW("devcie is alerady connected, mac: %{private}s.", GetAnonymousMAC(connectionInfo.mac).c_str());
            return;
        }

        uint32_t contextId = INVALID_ID;
        uint32_t agentId = INVALID_ID;

        sharingAdapter->CreateAgent(contextId, agentId, AgentType::SINK_AGENT, "WfdSinkSession");
        if (contextId == INVALID_ID || agentId == INVALID_ID) {
            lock.unlock();
            SHARING_LOGE("connected, create sink agent failed, devMac: %{private}s.",
                         GetAnonymousMAC(connectionInfo.mac).c_str());
            return;
        } else {
            SHARING_LOGI("connected, create sink agent, contextId: %{public}u, "
                "agentId: %{public}u, devMac: %{private}s, devIp: %{private}s.", contextId, agentId,
                GetAnonymousMAC(connectionInfo.mac).c_str(), GetAnonymousIp(connectionInfo.ip).c_str());
                FillAndReportDeviceInfo(connectionInfo);
                std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
                WfdSinkHiSysEvent::GetInstance().GetStartTime(startTime);
                WfdSinkHiSysEvent::GetInstance().ChangeHisysEventScene(SinkBizScene::ESTABLISH_MIRRORING);
                WfdSinkHiSysEvent::GetInstance().StartReport(__func__, "wpa_supplicant", SinkStage::P2P_CONNECT_SUCCESS,
                                                            SinkStageRes::SUCCESS);
        }

        connectionInfo.contextId = contextId;
        connectionInfo.agentId = agentId;
        connectionInfo.videoCodecId = videoCodecId_;
        connectionInfo.videoFormatId = videoFormatId_;
        connectionInfo.audioCodecId = audioCodecId_;
        connectionInfo.audioFormatId = audioFormatId_;

        std::shared_ptr<ConnectionInfo> connectionInfoPtr = std::make_shared<ConnectionInfo>(connectionInfo);
        RETURN_IF_NULL(connectionInfoPtr);
        devConnectionMap_.emplace(connectionInfo.mac, connectionInfoPtr);
        SHARING_LOGI("connected, devMac: %{private}s, devIp: %{private}s.", GetAnonymousMAC(connectionInfo.mac).c_str(),
            GetAnonymousIp(connectionInfo.ip).c_str());
    }
    SetWifiScene(1);
    OnConnectionChanged(connectionInfo);
}

void WfdSinkScene::OnP2pPeerDisconnected(ConnectionInfo &connectionInfo)
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_IF_NULL(sharingAdapter);

    uint32_t contextId = INVALID_ID;
    uint32_t agentId = INVALID_ID;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(connectionInfo.mac);
        if (itemDev == devConnectionMap_.end()) {
            lock.unlock();
            SHARING_LOGW("can not find dev, mac: %{private}s.", GetAnonymousMAC(connectionInfo.mac).c_str());
            return;
        }
        contextId = itemDev->second->contextId;
        agentId = itemDev->second->agentId;
        connectionInfo.surfaceId = itemDev->second->surfaceId;

        for (auto itemSurface = devSurfaceItemMap_.begin(); itemSurface != devSurfaceItemMap_.end();) {
            if (connectionInfo.mac == itemSurface->second->deviceId) {
                itemSurface->second->deleting = true;
            }
            itemSurface++;
        }

        P2pRemoveClient(connectionInfo);

        devConnectionMap_.erase(connectionInfo.mac);
        SHARING_LOGI("disconnected, contextId: %{public}u, agentId: %{public}u, devMac: %{private}s.", contextId,
                     agentId, GetAnonymousMAC(connectionInfo.mac).c_str());
    }

    OnConnectionChanged(connectionInfo);

    if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
        return;
    }

    auto sessionMsg = std::make_shared<WfdSinkSessionEventMsg>();
    sessionMsg->type = EVENT_SESSION_TEARDOWN;
    sessionMsg->toMgr = MODULE_CONTEXT;
    sessionMsg->dstId = contextId;
    sessionMsg->agentId = agentId;

    SharingEvent event;
    event.eventMsg = std::move(sessionMsg);

    sharingAdapter->ForwardEvent(contextId, agentId, event, true);
    sharingAdapter->DestroyAgent(contextId, agentId);
}

void WfdSinkScene::OnP2pPeerDisconnected(const std::string &mac)
{
    SHARING_LOGD("trace.");
    uint32_t contextId = INVALID_ID;
    uint32_t agentId = INVALID_ID;
    std::shared_ptr<ConnectionInfo> connectionInfo = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto itemDev = devConnectionMap_.find(mac);
        if (itemDev == devConnectionMap_.end()) {
            lock.unlock();
            SHARING_LOGW("can not find dev, mac: %{private}s.", GetAnonymousMAC(mac).c_str());
            return;
        }
        contextId = itemDev->second->contextId;
        agentId = itemDev->second->agentId;
        connectionInfo = itemDev->second;

        for (auto itemSurface = devSurfaceItemMap_.begin(); itemSurface != devSurfaceItemMap_.end();) {
            if (mac == itemSurface->second->deviceId) {
                itemSurface->second->deleting = true;
            }
            itemSurface++;
        }

        SHARING_LOGI("disconnected, contextId: %{public}u, agentId: %{public}u, devMac: %{private}s.", contextId,
                     agentId, GetAnonymousMAC(mac).c_str());
        P2pRemoveClient(*connectionInfo);

        devConnectionMap_.erase(mac);
    }

    if ((contextId == INVALID_ID) || (agentId == INVALID_ID)) {
        return;
    }

    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        sharingAdapter->DestroyAgent(contextId, agentId);
    }

    if (connectionInfo != nullptr) {
        connectionInfo->state = ConnectionState::DISCONNECTED;
        OnConnectionChanged(*connectionInfo);
    }
}

void WfdSinkScene::ErrorCodeFiltering(int32_t &code)
{
    SHARING_LOGD("the error code is %{public}d.", code);
    switch (ABSTRACT_ERR_BASE(code)) {
        case SharingErrorCode::ERR_CONTEXT_AGENT_BASE: // fall-through
        case SharingErrorCode::ERR_SESSION_BASE:
            code = SharingErrorCode::ERR_GENERAL_ERROR;
            SHARING_LOGD("the error change to %{public}d.", code);
            break;
        case SharingErrorCode::ERR_PROSUMER_BASE: {
            switch (code) {
                case ERR_PROSUMER_START:
                    code = SharingErrorCode::ERR_CONNECTION_FAILURE;
                    break;
                case ERR_PROSUMER_TIMEOUT:
                    code = SharingErrorCode::ERR_CONNECTION_TIMEOUT;
                    break;
                case ERR_PROSUMER_DESTROY:
                    code = SharingErrorCode::ERR_STATE_EXCEPTION;
                    break;
                default:
                    code = SharingErrorCode::ERR_GENERAL_ERROR;
                    break;
            }
            break;
        }
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void WfdSinkScene::OnInnerError(uint32_t contextId, uint32_t agentId, SharingErrorCode errorCode, std::string message)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdErrorMsg>();
    msg->contextId = contextId;
    msg->agentId = agentId;
    msg->errorCode = errorCode;

    for (auto &item : devConnectionMap_) {
        if ((contextId == item.second->contextId) && (agentId == item.second->agentId)) {
            msg->mac = item.second->mac;
            break;
        }
    }

    if (errorCode == SharingErrorCode::ERR_PROSUMER_TIMEOUT) {
        msg->message =
            "contextId: " + std::to_string(contextId) + ", agentId: " + std::to_string(agentId) + ", producer timeout";
    } else {
        msg->message = std::move(message);
    }

    ErrorCodeFiltering(msg->errorCode);
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    SHARING_LOGW("receive errorCode: %{public}d.", errorCode);
    ipcAdapter->SendRequest(msg, reply);
}

void WfdSinkScene::OnInnerError(std::string deviceId, SharingErrorCode errorCode, std::string message)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdErrorMsg>();
    msg->message = message;
    msg->mac = deviceId;
    msg->errorCode = errorCode;

    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());

    SHARING_LOGW("receive errorCode: %{public}d.", errorCode);
    ipcAdapter->SendRequest(msg, reply);
}

void WfdSinkScene::OnInnerDestroy(uint32_t contextId, uint32_t agentId, AgentType agentType)
{
    SHARING_LOGI("HandleInnerDestroy, contextId: %{public}u, agentId: %{public}u, agentType: %{public}s.", contextId,
                 agentId, std::string(magic_enum::enum_name(agentType)).c_str());

    std::unique_lock<std::mutex> lock(mutex_);
    for (auto itemSurface = devSurfaceItemMap_.begin(); itemSurface != devSurfaceItemMap_.end();) {
        if ((itemSurface->second != nullptr) && (contextId == itemSurface->second->contextId) &&
            (agentId == itemSurface->second->agentId)) {
            itemSurface = devSurfaceItemMap_.erase(itemSurface);
        } else {
            itemSurface++;
        }
    }

    for (auto &item : devConnectionMap_) {
        if ((item.second != nullptr) && (contextId == item.second->contextId) && (agentId == item.second->agentId)) {
            ConnectionInfo connectionInfo;
            connectionInfo.ip = item.second->ip;
            connectionInfo.mac = item.second->mac;
            connectionInfo.deviceName = item.second->deviceName;
            connectionInfo.primaryDeviceType = item.second->primaryDeviceType;
            connectionInfo.secondaryDeviceType = item.second->secondaryDeviceType;
            connectionInfo.ctrlPort = item.second->ctrlPort;
            connectionInfo.state = ConnectionState::DISCONNECTED;

            SHARING_LOGI(
                "disconnected, contextId: %{public}u, agentId: %{public}u, devMac: %{private}s, devIp: %{private}s.",
                contextId, agentId, GetAnonymousMAC(connectionInfo.mac).c_str(),
                GetAnonymousIp(connectionInfo.ip).c_str());
            OnConnectionChanged(connectionInfo);

            P2pRemoveClient(connectionInfo);

            devConnectionMap_.erase(item.second->mac);
            break;
        }
    }
}

void WfdSinkScene::OnInnerEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    SHARING_LOGI("eventType: %{public}s.", std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    switch (event.eventMsg->type) {
        case EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED: {
            auto msg = ConvertEventMsg<WfdSceneEventMsg>(event);
            if (msg) {
                std::unique_lock<std::mutex> lock(mutex_);
                auto itConnection = devConnectionMap_.find(msg->mac);
                if (itConnection == devConnectionMap_.end()) {
                    SHARING_LOGD("can't find dev %{private}s.", GetAnonymousMAC(msg->mac).c_str());
                    break;
                }

                itConnection->second->state = ConnectionState::PLAYING;
                OnConnectionChanged(*itConnection->second);
            }
            break;
        }
        case EventType::EVENT_WFD_NOTIFY_RTSP_TEARDOWN: {
            auto msg = ConvertEventMsg<WfdSceneEventMsg>(event);
            if (msg) {
                OnP2pPeerDisconnected(msg->mac);
            }
            break;
        }
        case EventType::EVENT_INTERACTION_ACCELERATION_DONE: {
            auto msg = ConvertEventMsg<InteractionEventMsg>(event);
            SHARING_LOGD("On acceleration done, contextId: %{public}d agentId: %{public}d.", msg->contextId,
                         msg->agentId);
            std::unique_lock<std::mutex> lock(mutex_);
            for (auto connectInfoPair : devConnectionMap_) {
                if (connectInfoPair.second != nullptr && connectInfoPair.second->contextId == msg->contextId &&
                    connectInfoPair.second->agentId == msg->agentId) {
                    SHARING_LOGD("On acceleration done, device found.");
                    auto connectInfo = connectInfoPair.second;
                    connectInfo->surfaceId = msg->surfaceId;

                    OnDecoderAccelerationDone(*connectInfo);
                    break;
                }
                SHARING_LOGW("On acceleration done, device not found.");
            }
            break;
        }
        case EventType::EVENT_INTERACTION_STATE_REMOVE_SURFACE: {
            auto msg = ConvertEventMsg<InteractionEventMsg>(event);
            SHARING_LOGD("On state remove surface, agentId: %{public}d.", msg->agentId);
            std::unique_lock<std::mutex> lock(mutex_);
            devSurfaceItemMap_.erase(msg->surfaceId);
            break;
        }
        case EVENT_INTERACTION_DECODER_DIED: {
            auto msg = ConvertEventMsg<InteractionEventMsg>(event);
            auto surfaceItem = devSurfaceItemMap_.find(msg->surfaceId);
            if (surfaceItem != devSurfaceItemMap_.end() && surfaceItem->second != nullptr) {
                auto itConnection = devConnectionMap_.find(surfaceItem->second->deviceId);
                OnDecoderDied(*itConnection->second);
            }
            break;
        }
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void WfdSinkScene::OnConnectionChanged(ConnectionInfo &connectionInfo)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdConnectionChangedMsg>();
    msg->ip = connectionInfo.ip;
    msg->mac = connectionInfo.mac;
    msg->state = connectionInfo.state;
    msg->surfaceId = connectionInfo.surfaceId;
    msg->deviceName = connectionInfo.deviceName;
    msg->primaryDeviceType = connectionInfo.primaryDeviceType;
    msg->secondaryDeviceType = connectionInfo.secondaryDeviceType;

    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());
    ipcAdapter->SendRequest(msg, reply);
}

void WfdSinkScene::P2pRemoveClient(ConnectionInfo &connectionInfo)
{
    SHARING_LOGI("p2p remove client: %{private}s.", GetAnonymousMAC(connectionInfo.mac).c_str());
    {
        std::unique_lock<std::mutex> lock(currentConnectDevMutex_);
        currentConnectDev_.mac = "";
    }
    SetWifiScene(0);
    if (!p2pInstance_) {
        SHARING_LOGE("p2p instance is null");
        return;
    }

    OHOS::Wifi::GcInfo info;
    info.mac = connectionInfo.mac;
    info.ip = connectionInfo.ip;
    info.host = connectionInfo.deviceName;

    p2pInstance_->RemoveGroupClient(info);
}

void WfdSinkScene::OnDecoderAccelerationDone(ConnectionInfo &connectionInfo)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdDecoderAccelerationDoneMsg>();
    msg->surfaceId = connectionInfo.surfaceId;

    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());
    ipcAdapter->SendRequest(msg, reply);
    SHARING_LOGD("device ip: %{private}s, mac: %{private}s, state: %{public}s.",
                 GetAnonymousIp(connectionInfo.ip).c_str(), GetAnonymousMAC(connectionInfo.mac).c_str(),
                 std::string(magic_enum::enum_name(connectionInfo.state)).c_str());
}

void WfdSinkScene::OnDecoderDied(ConnectionInfo &connectionInfo)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdSurfaceFailureMsg>();
    msg->surfaceId = connectionInfo.surfaceId;
    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());
    ipcAdapter->SendRequest(msg, reply);

    SHARING_LOGD("failed at device ip: %{private}s, mac: %{private}s, state: %{public}s.",
        GetAnonymousIp(connectionInfo.ip).c_str(), GetAnonymousMAC(connectionInfo.mac).c_str(),
        std::string(magic_enum::enum_name(connectionInfo.state)).c_str());
}

void WfdSinkScene::OnRemoteDied()
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter) {
        sharingAdapter->ReleaseScene(GetId());
    }
}

REGISTER_CLASS_REFLECTOR(WfdSinkScene);
} // namespace Sharing
} // namespace OHOS
