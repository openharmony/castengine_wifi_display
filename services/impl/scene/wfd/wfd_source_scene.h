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

#ifndef OHOS_SHARING_WFD_SOURCE_SCENE_H
#define OHOS_SHARING_WFD_SOURCE_SCENE_H

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "agent/agent_def.h"
#include "interaction/scene/base_scene.h"
#include "timer.h"
#include "wfd_def.h"
#include "wfd_msg.h"
#include "wifi_p2p.h"

namespace OHOS {
namespace Sharing {

class WfdSourceScene final : public BaseScene,
                             public std::enable_shared_from_this<WfdSourceScene> {
public:
    WfdSourceScene();
    ~WfdSourceScene();

public:
    class WfdP2pCallback : public Wifi::IWifiP2pCallback {
    public:
        sptr<IRemoteObject> AsObject() override { return nullptr; }

    public:
        explicit WfdP2pCallback(std::weak_ptr<WfdSourceScene> scene) : scene_(scene) {}

        void OnP2pStateChanged(int32_t state) override;
        void OnP2pPersistentGroupsChanged(void) override;
        void OnP2pDiscoveryChanged(bool isChange) override;
        void OnP2pGcJoinGroup(const OHOS::Wifi::GcInfo &info) override;
        void OnP2pGcLeaveGroup(const OHOS::Wifi::GcInfo &info) override;
        void OnP2pThisDeviceChanged(const Wifi::WifiP2pDevice &device) override;
        void OnP2pConnectionChanged(const Wifi::WifiP2pLinkedInfo &info) override;
        void OnConfigChanged(Wifi::CfgType type, char *data, int32_t dataLen) override;
        void OnP2pPeersChanged(const std::vector<Wifi::WifiP2pDevice> &device) override;
        void OnP2pActionResult(Wifi::P2pActionCallback action, Wifi::ErrCode code) override;
        void OnP2pServicesChanged(const std::vector<Wifi::WifiP2pServiceInfo> &srvInfo) override;

    private:
        std::weak_ptr<WfdSourceScene> scene_;
    };

    friend class WfdP2pCallback;

protected:
    void SetIpcAdapter(const std::weak_ptr<IpcMsgAdapter> &adapter) final
    {
        SHARING_LOGD("trace.");
        ipcAdapter_ = adapter;
        auto ipcAdapter = ipcAdapter_.lock();
        if (ipcAdapter) {
            ipcAdapter->SetListener(shared_from_this());
        }
    }

protected:
    void Release();

    // impl BaseScene
    void Initialize() final;
    void OnRemoteDied() final;
    void OnInnerEvent(SharingEvent &event) final;
    void OnInnerError(uint32_t contextId, uint32_t agentId, SharingErrorCode errorCode,
                      std::string message = "wfd inner error") final;
    void OnInnerDestroy(uint32_t contextId, uint32_t agentId, AgentType agentType) final;

    // interactionMgr -> interaction -> scene directly
    void OnDomainMsg(std::shared_ptr<BaseDomainMsg> &msg) final;

    // impl IpcMsgAdapterListener
    void OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) final;

private:
    void WfdP2pStop();
    void WfdP2pStart();

    void ErrorCodeFiltering(int32_t &code);

    void OnP2pPeerDisconnected(const std::string &mac);
    void OnP2pPeerConnected(ConnectionInfo &connectionInfo);
    void OnP2pPeerDisconnected(ConnectionInfo &connectionInfo);

    void OnCheckWfdConnection();
    void OnConnectionChanged(ConnectionInfo &connectionInfo);
    void OnDeviceFound(const std::vector<WfdCastDeviceInfo> &deviceInfos);

    void SetCheckWfdConnectionTimer();
    void ResetCheckWfdConnectionTimer();

    int32_t CreateScreenCapture();
    int32_t AppendCast(const std::string &deviceId);
    int32_t HandleDestroyScreenCapture(std::shared_ptr<DestroyScreenCaptureReq> &msg);
    int32_t HandleStartDiscovery(std::shared_ptr<WfdSourceStartDiscoveryReq> &msg,
                                 std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleStopDiscovery(std::shared_ptr<WfdSourceStopDiscoveryReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleAddDevice(std::shared_ptr<WfdSourceAddDeviceReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleRemoveDevice(std::shared_ptr<WfdSourceRemoveDeviceReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);

private:
    std::atomic_bool isSourceRunning_ = false;
    std::atomic_bool isSourceDiscovering = false;

    uint32_t timerId_ = 0;
    uint32_t agentId_ = INVALID_ID;
    uint32_t contextId_ = INVALID_ID;

    int32_t ctrlPort_ = DEFAULT_WFD_CTRLPORT;

    uint64_t screenId_ = 0;

    CodecId audioCodecId_ = CodecId::CODEC_AAC;
    CodecId videoCodecId_ = CodecId::CODEC_H264;
    AudioFormat audioFormat_ = AudioFormat::AUDIO_48000_16_2;
    VideoFormat videoFormat_ = VideoFormat::VIDEO_1920X1080_25;

    std::mutex mutex_;
    std::shared_ptr<Wifi::WifiP2p> p2pInstance_;
    std::unique_ptr<ConnectionInfo> connDev_ = nullptr;
    std::unique_ptr<OHOS::Utils::Timer> timer_ = std::make_unique<OHOS::Utils::Timer>("WfdConnectTimeoutTimer");
};

} // namespace Sharing
} // namespace OHOS
#endif
