/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wfd_source_scene.h"
#include <display_manager.h>
#include <unistd.h>
#include "common/common_macro.h"
#include "common/const_def.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "configuration/include/config.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "network/socket/socket_utils.h"
#include "screen_capture_def.h"
#include "utils/utils.h"
#include "wfd_session_def.h"

namespace OHOS {
namespace Sharing {

void WfdSourceScene::WfdP2pCallback::OnP2pStateChanged(int32_t state)
{
    SHARING_LOGD("state: %{public}d.", state);
    auto scene = scene_.lock();
    if (scene) {
        switch (static_cast<Wifi::P2pState>(state)) {
            case Wifi::P2pState::P2P_STATE_NONE:
                break;
            case Wifi::P2pState::P2P_STATE_IDLE:
                break;
            case Wifi::P2pState::P2P_STATE_STARTING:
                break;
            case Wifi::P2pState::P2P_STATE_STARTED:
                if (scene->isSourceRunning_) {
                    scene->WfdP2pStart();
                }
                break;
            case Wifi::P2pState::P2P_STATE_CLOSING:
                break;
            case Wifi::P2pState::P2P_STATE_CLOSED:
                if (scene->isSourceRunning_) {
                    scene->isSourceRunning_ = false;
                    scene->WfdP2pStop();
                    scene->OnInnerError(0, 0, SharingErrorCode::ERR_NETWORK_ERROR,
                                         "NETWORK ERROR, P2P MODULE STOPPED");
                }
                break;
            default:
                SHARING_LOGI("none process case.");
                break;
        }
    }
}

void WfdSourceScene::WfdP2pCallback::OnP2pPersistentGroupsChanged(void)
{
    SHARING_LOGD("trace.");
}

void WfdSourceScene::WfdP2pCallback::OnP2pThisDeviceChanged(const Wifi::WifiP2pDevice &device)
{
    SHARING_LOGD("trace.");
    auto scene = scene_.lock();
    if (scene == nullptr) {
        SHARING_LOGW("scene is nullptr.");
        return;
    }

    if (scene->p2pInstance_ == nullptr) {
        SHARING_LOGW("p2pInstance is nullptr.");
        return;
    }

    Wifi::WifiP2pLinkedInfo info;
    if (scene->p2pInstance_->QueryP2pLinkedInfo(info)) {
        SHARING_LOGE("failed to query p2p link info.");
        return;
    }

    Wifi::P2pConnectedState state = info.GetConnectState();
    SHARING_LOGD("ConnectState: %{public}d.", state);
    if (state != Wifi::P2pConnectedState::P2P_DISCONNECTED) {
        return;
    }

    if (scene->connDev_ != nullptr) {
        scene->OnP2pPeerDisconnected(scene->connDev_->mac);
    }
}

void WfdSourceScene::WfdP2pCallback::OnP2pPeersChanged(const std::vector<Wifi::WifiP2pDevice> &device)
{
    SHARING_LOGD("trace.");
    auto scene = scene_.lock();
    RETURN_IF_NULL(scene);

    if (!scene->isSourceDiscovering) {
        SHARING_LOGI("p2p source is not discovering.");
        return;
    }

    SHARING_LOGI("device size: %{public}zu.", device.size());
    std::vector<WfdCastDeviceInfo> foundedDevices;
    for (auto itDev : device) {
        auto status = itDev.GetP2pDeviceStatus();
        SHARING_LOGI("device name: %{public}s, mac: %{private}s, status: %{public}d.",
                     GetAnonyString(itDev.GetDeviceName()).c_str(), GetAnonyString(itDev.GetDeviceAddress()).c_str(),
                     status);
        if (status == Wifi::P2pDeviceStatus::PDS_AVAILABLE) {
            std::string subelement;
            Wifi::WifiP2pWfdInfo wfdInfo(itDev.GetWfdInfo());
            wfdInfo.GetDeviceInfoElement(subelement);
            SHARING_LOGI("\tsession available: %{public}d"
                         "\tdevice info: %{private}s.",
                         wfdInfo.isSessionAvailable(), subelement.c_str());

            if (wfdInfo.isSessionAvailable()) {
                WfdCastDeviceInfo deviceInfo;
                deviceInfo.deviceId = itDev.GetDeviceAddress();
                deviceInfo.deviceName = itDev.GetDeviceName();
                deviceInfo.primaryDeviceType = itDev.GetPrimaryDeviceType();
                deviceInfo.secondaryDeviceType = itDev.GetSecondaryDeviceType();

                foundedDevices.emplace_back(deviceInfo);
            }
        }
    }

    if (!foundedDevices.empty()) {
        scene->OnDeviceFound(foundedDevices);
    }
}

void WfdSourceScene::WfdP2pCallback::OnP2pServicesChanged(const std::vector<Wifi::WifiP2pServiceInfo> &srvInfo)
{
    SHARING_LOGD("trace.");
}

void WfdSourceScene::WfdP2pCallback::OnP2pConnectionChanged(const Wifi::WifiP2pLinkedInfo &info)
{
    SHARING_LOGD("trace.");
    Wifi::P2pConnectedState state = info.GetConnectState();
    SHARING_LOGD("ConnectState: %{public}d.", state);
    if (state != Wifi::P2pConnectedState::P2P_CONNECTED) {
        return;
    }

    auto scene = scene_.lock();
    if (scene == nullptr) {
        SHARING_LOGW("scene is nullptr.");
        return;
    }

    SHARING_LOGD("goip: %{private}s.", GetAnonyString(info.GetGroupOwnerAddress()).c_str());
    if (info.GetGroupOwnerAddress() == "") {
        return;
    }

    Wifi::WifiP2pGroupInfo group;
    if (Wifi::ErrCode::WIFI_OPT_SUCCESS != scene->p2pInstance_->GetCurrentGroup(group)) {
        SHARING_LOGE("GetCurrentGroup failed");
        return;
    }

    Wifi::WifiP2pDevice goDevice = group.GetOwner();
    if (info.GetGroupOwnerAddress() == "0.0.0.0") {
        SHARING_LOGE("goDevice: %{private}s leased ip is: 0.0.0.0.",
                     GetAnonyString(goDevice.GetDeviceAddress()).c_str());
        return;
    }

    ConnectionInfo connectionInfo;
    connectionInfo.ip = info.GetGroupOwnerAddress();
    connectionInfo.mac = goDevice.GetDeviceAddress();
    connectionInfo.deviceName = goDevice.GetDeviceName();
    connectionInfo.primaryDeviceType = goDevice.GetPrimaryDeviceType();
    connectionInfo.secondaryDeviceType = goDevice.GetSecondaryDeviceType();
    connectionInfo.ctrlPort = goDevice.GetWfdInfo().GetCtrlPort();
    connectionInfo.state = ConnectionState::CONNECTED;

    SHARING_LOGD("device connected, mac: %{private}s, ip: %{private}s, port: %{private}d.",
                 GetAnonyString(connectionInfo.mac).c_str(), GetAnonyString(connectionInfo.ip).c_str(),
                 connectionInfo.ctrlPort);
    scene->OnP2pPeerConnected(connectionInfo);
}

void WfdSourceScene::WfdP2pCallback::OnP2pGcJoinGroup(const OHOS::Wifi::GcInfo &info)
{
    SHARING_LOGD("trace.");
}

void WfdSourceScene::WfdP2pCallback::OnP2pGcLeaveGroup(const OHOS::Wifi::GcInfo &info)
{
    SHARING_LOGD("trace.");
}

void WfdSourceScene::WfdP2pCallback::OnP2pDiscoveryChanged(bool isChange)
{
    SHARING_LOGD("isChange: %{public}d.", isChange);
}

void WfdSourceScene::WfdP2pCallback::OnP2pActionResult(Wifi::P2pActionCallback action, Wifi::ErrCode code)
{
    SHARING_LOGD("action: %{public}hhu, code: %{public}d.", action, code);
}

void WfdSourceScene::WfdP2pCallback::OnConfigChanged(Wifi::CfgType type, char *data, int32_t dataLen)
{
    (void)type;
    (void)data;
    (void)dataLen;
    SHARING_LOGD("trace.");
}

WfdSourceScene::WfdSourceScene()
{
    SHARING_LOGD("id: %{public}u.", GetId());
}

WfdSourceScene::~WfdSourceScene()
{
    SHARING_LOGD("id: %{public}u.", GetId());
    Release();
}

void WfdSourceScene::OnCheckWfdConnection()
{
    ResetCheckWfdConnectionTimer();
    OnInnerError(0, 0, SharingErrorCode::ERR_CONNECTION_TIMEOUT, "wfd connection timeout");
}

void WfdSourceScene::SetCheckWfdConnectionTimer()
{
    int32_t interval = P2P_CONNECT_TIMEOUT * WFD_SEC_TO_MSEC;
    if (timer_ != nullptr) {
        timerId_ = timer_->Register(std::bind(&WfdSourceScene::OnCheckWfdConnection, this), interval);
        timer_->Setup();
    }
}

void WfdSourceScene::ResetCheckWfdConnectionTimer()
{
    if (timer_ != nullptr && timerId_ != 0) {
        timer_->Unregister(timerId_);
        timerId_ = 0;
    }
}

void WfdSourceScene::Initialize()
{
    SHARING_LOGD("trace.");
    SharingValue::Ptr values = nullptr;

    auto ret = Config::GetInstance().GetConfig("khSharingWfd", "ctrlport", "defaultWfdCtrlport", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(ctrlPort_);
    }

    ret = Config::GetInstance().GetConfig("khSharingWfd", "mediaFormat", "videoCodec", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t videoCodec;
        values->GetValue<int32_t>(videoCodec);
        videoCodecId_ = static_cast<CodecId>(videoCodec);
        videoCodecId_ = CodecId::CODEC_H264;
    }

    ret = Config::GetInstance().GetConfig("khSharingWfd", "mediaFormat", "videoFormat", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t videoFormat;
        values->GetValue<int32_t>(videoFormat);
        videoFormat_ = static_cast<VideoFormat>(videoFormat);
        videoFormat_ = VideoFormat::VIDEO_1920X1080_25;
    }

    ret = Config::GetInstance().GetConfig("khSharingWfd", "mediaFormat", "audioCodec", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t audioCodec;
        values->GetValue<int32_t>(audioCodec);
        audioCodecId_ = static_cast<CodecId>(audioCodec);
        audioCodecId_ = CodecId::CODEC_AAC;
    }

    ret = Config::GetInstance().GetConfig("khSharingWfd", "mediaFormat", "audioFormat", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        int32_t audioFormat;
        values->GetValue<int32_t>(audioFormat);
        audioFormat_ = static_cast<AudioFormat>(audioFormat);
        audioFormat_ = AudioFormat::AUDIO_48000_16_2;
    }

    p2pInstance_ = Wifi::WifiP2p::GetInstance(WIFI_P2P_ABILITY_ID);
    sptr<WfdP2pCallback> wfdP2pCallback(new WfdP2pCallback(shared_from_this()));

    std::vector<std::string> event = {  EVENT_P2P_PEER_DEVICE_CHANGE,
                                        EVENT_P2P_DEVICE_STATE_CHANGE,
                                        EVENT_P2P_CONN_STATE_CHANGE,
                                        EVENT_P2P_STATE_CHANGE,
                                        EVENT_P2P_SERVICES_CHANGE,
                                        EVENT_P2P_DISCOVERY_CHANGE};
    p2pInstance_->RegisterCallBack(wfdP2pCallback, event);
}

void WfdSourceScene::Release()
{
    SHARING_LOGD("trace.");
    if (timer_ != nullptr) {
        timer_->Shutdown();
        timer_.reset();
    }
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if ((connDev_ != nullptr) && (connDev_->contextId != INVALID_ID) && (connDev_->agentId != INVALID_ID)) {
            auto sessionMsg = std::make_shared<WfdSourceSessionEventMsg>();
            sessionMsg->type = EVENT_SESSION_TEARDOWN;
            sessionMsg->toMgr = MODULE_CONTEXT;
            sessionMsg->dstId = connDev_->contextId;
            sessionMsg->agentId = connDev_->agentId;

            SharingEvent event;
            event.eventMsg = std::move(sessionMsg);
            sharingAdapter->ForwardEvent(connDev_->contextId, connDev_->agentId, event, true);
            sharingAdapter->DestroyAgent(connDev_->contextId, connDev_->agentId);
            if ((contextId_ != INVALID_ID) && (agentId_ != INVALID_ID)) {
                sharingAdapter->DestroyAgent(contextId_, agentId_);
            }
        }
    }

    if (p2pInstance_) {
        p2pInstance_->RemoveGroup();
    }
    p2pInstance_.reset();
}

void WfdSourceScene::OnDomainMsg(std::shared_ptr<BaseDomainMsg> &msg)
{
    SHARING_LOGD("trace.");
}

void WfdSourceScene::OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(msg);
    SHARING_LOGI("recv msg, msg id: %{public}d.", msg->GetMsgId());
    switch (msg->GetMsgId()) {
        case WfdSourceStartDiscoveryReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSourceStartDiscoveryReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleStartDiscovery(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdSourceStopDiscoveryReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSourceStopDiscoveryReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleStopDiscovery(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdSourceAddDeviceReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSourceAddDeviceReq>(msg);
            SHARING_LOGI("add deviceId: %{public}s.", data->deviceId.c_str());
            ResetCheckWfdConnectionTimer();
            auto rsp = std::make_shared<WfdCommonRsp>();
            rsp->ret = WfdSourceScene::HandleAddDevice(data, rsp);
            SetCheckWfdConnectionTimer();
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case WfdSourceRemoveDeviceReq::MSG_ID: {
            auto data = std::static_pointer_cast<WfdSourceRemoveDeviceReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();

            rsp->ret = HandleRemoveDevice(data, rsp);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        case DestroyScreenCaptureReq::MSG_ID: {
            auto data = std::static_pointer_cast<DestroyScreenCaptureReq>(msg);
            auto rsp = std::make_shared<WfdCommonRsp>();
            rsp->ret = HandleDestroyScreenCapture(data);
            reply = std::static_pointer_cast<BaseMsg>(rsp);
            break;
        }
        default:
            SHARING_LOGW("unknown msg request.");
            break;
    }
}

int32_t WfdSourceScene::HandleStartDiscovery(std::shared_ptr<WfdSourceStartDiscoveryReq> &msg,
                                             std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    int32_t ret = 0;
    if (p2pInstance_ == nullptr) {
        SHARING_LOGW("p2pInstance is nullptr.");
        return -1;
    }
    if (!isSourceRunning_) {
        int32_t status = 0;
        p2pInstance_->GetP2pEnableStatus(status);
        switch (static_cast<Wifi::P2pState>(status)) {
            case Wifi::P2pState::P2P_STATE_NONE:     // fall-through
            case Wifi::P2pState::P2P_STATE_IDLE:     // fall-through
            case Wifi::P2pState::P2P_STATE_STARTING: // fall-through
            case Wifi::P2pState::P2P_STATE_CLOSING: {
                OnInnerError(0, 0, SharingErrorCode::ERR_STATE_EXCEPTION, "HandleStart current p2p state: CLOSING");
                ret = -1;
                break;
            }
            case Wifi::P2pState::P2P_STATE_STARTED: {
                isSourceRunning_ = true;
                WfdP2pStart();
                break;
            }
            case Wifi::P2pState::P2P_STATE_CLOSED: {
                isSourceRunning_ = true;
                p2pInstance_->EnableP2p();
                break;
            }
            default: {
                SHARING_LOGI("none process case.");
                break;
            }
        }
    }
    if (ret == 0) {
        ret = p2pInstance_->DiscoverDevices();
        isSourceDiscovering = true;
    }
    return ret;
}

int32_t WfdSourceScene::HandleStopDiscovery(std::shared_ptr<WfdSourceStopDiscoveryReq> &msg,
                                            std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    if (!isSourceRunning_) {
        SHARING_LOGW("p2p source is not running.");
        return -1;
    }

    if (p2pInstance_ == nullptr) {
        SHARING_LOGW("p2pInstance is nullptr.");
        return -1;
    }

    int32_t ret = p2pInstance_->StopDiscoverDevices();
    isSourceDiscovering = false;
    return ret;
}

int32_t WfdSourceScene::HandleAddDevice(std::shared_ptr<WfdSourceAddDeviceReq> &msg,
                                        std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    if (!isSourceRunning_) {
        SHARING_LOGW("p2p source is not running.");
        return -1;
    }

    if (p2pInstance_ == nullptr) {
        SHARING_LOGW("p2pInstance is nullptr.");
        return -1;
    }
    auto displayIds = OHOS::Rosen::DisplayManager::GetInstance().GetAllDisplayIds();
    bool findDisplayId = false;
    for (auto displayId : displayIds) {
        SHARING_LOGD("displayId = %{public}" PRIu64, displayId);
        if (msg->screenId == displayId) {
            findDisplayId = true;
            break;
        }
    }
    if (!findDisplayId) {
        SHARING_LOGE("can't find screenId %{public}" PRIu64, msg->screenId);
        return ERR_BAD_PARAMETER;
    }

    Wifi::WifiP2pConfig config;
    config.SetDeviceAddress(msg->deviceId);
    config.SetDeviceAddressType(OHOS::Wifi::RANDOM_DEVICE_ADDRESS);
    screenId_ = msg->screenId;
    int32_t ret = p2pInstance_->P2pConnect(config);
    SHARING_LOGD("connect device: %s, ret = %d", GetAnonyString(msg->deviceId).c_str(), ret);
    return ret;
}

int32_t WfdSourceScene::HandleRemoveDevice(std::shared_ptr<WfdSourceRemoveDeviceReq> &msg,
                                           std::shared_ptr<WfdCommonRsp> &reply)
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    std::lock_guard<std::mutex> lock(mutex_);
    if ((connDev_ == nullptr) || (connDev_->mac != msg->deviceId)) {
        SHARING_LOGE("can not find dev, mac: %{public}s.", GetAnonyString(msg->deviceId).c_str());
        return -1;
    }
    auto sessionMsg = std::make_shared<WfdSourceSessionEventMsg>();
    sessionMsg->type = EVENT_SESSION_TEARDOWN;
    sessionMsg->toMgr = MODULE_CONTEXT;
    sessionMsg->dstId = connDev_->contextId;
    sessionMsg->agentId = connDev_->agentId;

    SharingEvent event;
    event.eventMsg = std::move(sessionMsg);
    sharingAdapter->ForwardEvent(connDev_->contextId, connDev_->agentId, event, true);

    connDev_.reset();
    int ret = sharingAdapter->DestroyAgent(contextId_, agentId_);
    if (p2pInstance_) {
        p2pInstance_->RemoveGroup();
    }
    return ret;
}

int32_t WfdSourceScene::CreateScreenCapture()
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        std::unique_lock<std::mutex> lock(mutex_);

        sharingAdapter->CreateAgent(contextId_, agentId_, AgentType::SINK_AGENT, "ScreenCaptureSession");
        if (contextId_ == INVALID_ID || agentId_ == INVALID_ID) {
            lock.unlock();
            SHARING_LOGE("Create ScreenCapture sink agent failed");
            return ERR_AGENT_CREATE;
        } else {
            SHARING_LOGI("Create ScreenCapture sink agent, contextId: %{public}u, agentId: %{public}u", contextId_,
                         agentId_);
        }

        auto startSessionMsg = std::make_shared<ScreenCaptureSessionEventMsg>();
        startSessionMsg->mediaType = MEDIA_TYPE_AV;
        startSessionMsg->type = EVENT_SESSION_INIT;
        startSessionMsg->toMgr = MODULE_CONTEXT;
        startSessionMsg->dstId = contextId_;
        startSessionMsg->agentId = agentId_;
        startSessionMsg->screenId = screenId_;
        videoFormat_ = VideoFormat::VIDEO_1920X1080_25;

        startSessionMsg->videoFormat = videoFormat_;
        startSessionMsg->audioFormat = audioFormat_;

        SharingEvent event;
        event.eventMsg = std::move(startSessionMsg);

        sharingAdapter->ForwardEvent(contextId_, agentId_, event, true);
        sharingAdapter->Start(contextId_, agentId_);
    }

    return ERR_OK;
}

int32_t WfdSourceScene::HandleDestroyScreenCapture(std::shared_ptr<DestroyScreenCaptureReq> &msg)
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        sharingAdapter->DestroyAgent(contextId_, agentId_);
    }
    return ERR_OK;
}

int32_t WfdSourceScene::AppendCast(const std::string &deviceId)
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    RETURN_INVALID_IF_NULL(sharingAdapter);

    std::unique_lock<std::mutex> lock(mutex_);
    if ((connDev_ == nullptr) || (connDev_->mac != deviceId)) {
        SHARING_LOGE("can not find dev, mac: %{private}s.", GetAnonyString(deviceId).c_str());
        OnInnerError(0, 0, SharingErrorCode::ERR_BAD_PARAMETER, "AppendCast can't find the dev");
        return -1;
    }
    auto port = SocketUtils::GetAvailableUdpPortPair();
    if (port == 0) {
        SHARING_LOGE("get udp port failed.");
        return -1;
    }

    uint32_t contextId = contextId_;
    uint32_t agentId = agentId_;
    sharingAdapter->CreateAgent(contextId, agentId, AgentType::SRC_AGENT, "WfdSourceSession");
    if (contextId == INVALID_ID || agentId == INVALID_ID) {
        lock.unlock();
        SHARING_LOGE("create source agent failed.");
        return -1;
    }
    SHARING_LOGI("create source agent, contextId: %{public}u, agentId: %{public}u", contextId, agentId);
    connDev_->contextId = contextId;
    connDev_->agentId = agentId;

    if (connDev_->isRunning == true) {
        return 0;
    }
    connDev_->isRunning = true;

    auto startSessionMsg = std::make_shared<WfdSourceSessionEventMsg>();
    startSessionMsg->type = EVENT_SESSION_INIT;
    startSessionMsg->toMgr = MODULE_CONTEXT;
    startSessionMsg->dstId = contextId;
    startSessionMsg->agentId = agentId;
    startSessionMsg->ip = connDev_->ip;
    startSessionMsg->mac = connDev_->mac;
    startSessionMsg->remotePort = connDev_->ctrlPort;
    startSessionMsg->videoFormat = connDev_->videoFormatId;
    startSessionMsg->audioFormat = connDev_->audioFormatId;
    startSessionMsg->localPort = port;

    SharingEvent event;
    event.eventMsg = std::move(startSessionMsg);
    sharingAdapter->ForwardEvent(contextId, agentId, event, true);
    sharingAdapter->Start(contextId, agentId);

    return 0;
}

void WfdSourceScene::WfdP2pStart()
{
    SHARING_LOGD("trace.");
    if (p2pInstance_) {
        p2pInstance_->RemoveGroup();

        Wifi::WifiP2pWfdInfo wfdInfo;
        wfdInfo.SetWfdEnabled(true);
        wfdInfo.SetDeviceInfo(0x10);
        wfdInfo.SetCtrlPort(ctrlPort_);
        wfdInfo.SetMaxThroughput(0x00c8);

        p2pInstance_->SetP2pWfdInfo(wfdInfo);

        SHARING_LOGD("WfdSourceScene DiscoverDevices.");
        p2pInstance_->DiscoverDevices();
        isSourceDiscovering = true;
    }
}

void WfdSourceScene::WfdP2pStop()
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if ((connDev_ != nullptr) && (connDev_->contextId != INVALID_ID) && (connDev_->agentId != INVALID_ID)) {
            auto sessionMsg = std::make_shared<WfdSourceSessionEventMsg>();
            sessionMsg->type = EVENT_SESSION_TEARDOWN;
            sessionMsg->toMgr = MODULE_CONTEXT;
            sessionMsg->dstId = connDev_->contextId;
            sessionMsg->agentId = connDev_->agentId;
            SharingEvent event;
            event.eventMsg = std::move(sessionMsg);
            sharingAdapter->ForwardEvent(connDev_->contextId, connDev_->agentId, event, true);
        }

        if ((contextId_ != INVALID_ID) && (agentId_ != INVALID_ID)) {
            sharingAdapter->DestroyAgent(contextId_, agentId_);
        }
    }

    if (p2pInstance_) {
        p2pInstance_->RemoveGroup();
    }
}

void WfdSourceScene::OnDeviceFound(const std::vector<WfdCastDeviceInfo> &deviceInfos)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdSourceDeviceFoundMsg>();
    msg->deviceInfos = deviceInfos;

    auto reply = std::static_pointer_cast<BaseMsg>(std::make_shared<WfdCommonRsp>());
    ipcAdapter->SendRequest(msg, reply);
}

void WfdSourceScene::OnP2pPeerConnected(ConnectionInfo &connectionInfo)
{
    SHARING_LOGD("trace.");
    if (!isSourceRunning_) {
        SHARING_LOGW("source service is not running.");
        return;
    }

    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if ((connDev_ != nullptr) && (connDev_->mac == connectionInfo.mac)) {
            SHARING_LOGW("devcie is alerady connected, mac: %s.", GetAnonyString(connectionInfo.mac).c_str());
            return;
        }
        connectionInfo.videoCodecId = videoCodecId_;
        connectionInfo.videoFormatId = videoFormat_;
        connectionInfo.audioCodecId = audioCodecId_;
        connectionInfo.audioFormatId = audioFormat_;

        connDev_ = std::make_unique<ConnectionInfo>(connectionInfo);
        SHARING_LOGI("connected, devMac: %s, devIp: %s.", GetAnonyString(connectionInfo.mac).c_str(),
                     GetAnonyString(connectionInfo.ip).c_str());
    }

    OnConnectionChanged(connectionInfo);
}

void WfdSourceScene::OnP2pPeerDisconnected(ConnectionInfo &connectionInfo)
{
    SHARING_LOGD("trace.");
    OnP2pPeerDisconnected(connectionInfo.mac);
}

void WfdSourceScene::OnP2pPeerDisconnected(const std::string &mac)
{
    SHARING_LOGD("trace.");
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if ((connDev_ == nullptr) || (connDev_->mac != mac)) {
            SHARING_LOGW("can not find dev, mac: %s.", GetAnonyString(mac).c_str());
            return;
        }
        connDev_->state = ConnectionState::DISCONNECTED;
        OnConnectionChanged(*connDev_);
        connDev_.reset();
    }

    if ((contextId_ == INVALID_ID) || (agentId_ == INVALID_ID)) {
        return;
    }

    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter != nullptr) {
        sharingAdapter->DestroyAgent(contextId_, agentId_);
    }

    if (p2pInstance_) {
        p2pInstance_->RemoveGroup();
    }
}

void WfdSourceScene::ErrorCodeFiltering(int32_t &code)
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

void WfdSourceScene::OnInnerError(uint32_t contextId, uint32_t agentId, SharingErrorCode errorCode, std::string message)
{
    SHARING_LOGD("trace.");
    auto ipcAdapter = ipcAdapter_.lock();
    RETURN_IF_NULL(ipcAdapter);

    auto msg = std::make_shared<WfdErrorMsg>();
    msg->contextId = contextId;
    msg->agentId = agentId;
    msg->errorCode = errorCode;

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

void WfdSourceScene::OnInnerDestroy(uint32_t contextId, uint32_t agentId, AgentType agentType)
{
    SHARING_LOGI("HandleInnerDestroy, contextId: %{public}u, agentId: %{public}u, agentType: %{public}s.", contextId,
                 agentId, std::string(magic_enum::enum_name(agentType)).c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (connDev_ == nullptr) {
        SHARING_LOGE("connDev_ is nullptr.");
        return;
    }

    if ((contextId == connDev_->contextId) && ((agentId == connDev_->agentId) || agentId == agentId_)) {
        connDev_->state = ConnectionState::DISCONNECTED;
        OnConnectionChanged(*connDev_);
    }
}

void WfdSourceScene::OnInnerEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    SHARING_LOGI("eventType: %{public}s.", std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    switch (event.eventMsg->type) {
        case EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED: {
            auto msg = ConvertEventMsg<WfdSceneEventMsg>(event);
            if (msg) {
                std::lock_guard<std::mutex> lock(mutex_);
                if ((connDev_ == nullptr) || (connDev_->mac != msg->mac)) {
                    SHARING_LOGE("connDev_ is nullptr or mac:%{private}s doesn't match.",
                                 GetAnonyString(msg->mac).c_str());
                    return;
                }
                connDev_->state = ConnectionState::PLAYING;
                OnConnectionChanged(*connDev_);
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
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void WfdSourceScene::OnConnectionChanged(ConnectionInfo &connectionInfo)
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

    if (connectionInfo.state == ConnectionState::CONNECTED) {
        ResetCheckWfdConnectionTimer();
        if (CreateScreenCapture() == ERR_OK) {
            if (AppendCast(connectionInfo.mac) != 0) {
                SHARING_LOGE("append cast error.");
                OnInnerError(0, 0, SharingErrorCode::ERR_GENERAL_ERROR, "wfd connection timeout");
            }
        } else {
            SHARING_LOGE("create screen capture error.");
            OnInnerError(0, 0, SharingErrorCode::ERR_GENERAL_ERROR, "wfd connection timeout");
        }
    }
}

void WfdSourceScene::OnRemoteDied()
{
    SHARING_LOGD("trace.");
    auto sharingAdapter = sharingAdapter_.lock();
    if (sharingAdapter) {
        sharingAdapter->ReleaseScene(GetId());
    }
}

REGISTER_CLASS_REFLECTOR(WfdSourceScene);
} // namespace Sharing
} // namespace OHOS
