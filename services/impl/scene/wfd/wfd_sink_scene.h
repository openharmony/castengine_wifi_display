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

#ifndef OHOS_SHARING_WFD_SINK_SCENE_H
#define OHOS_SHARING_WFD_SINK_SCENE_H

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "agent/agent_def.h"
#include "interaction/scene/base_scene.h"
#include "wfd_def.h"
#include "wfd_msg.h"
#include "wifi_p2p.h"

namespace OHOS {
namespace Sharing {

class WfdSinkScene final : public BaseScene,
                           public std::enable_shared_from_this<WfdSinkScene> {
public:
    WfdSinkScene();
    ~WfdSinkScene();

public:
    class WfdP2pCallback : public Wifi::IWifiP2pCallback {
    public:
        sptr<IRemoteObject> AsObject() override { return nullptr; }

    public:
        explicit WfdP2pCallback(std::weak_ptr<WfdSinkScene> parent) : parent_(parent) {}

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
        std::weak_ptr<WfdSinkScene> parent_;
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
    void OnInnerError(std::string deviceId, SharingErrorCode errorCode, std::string message = "wfd inner error");
    void OnInnerDestroy(uint32_t contextId, uint32_t agentId, AgentType agentType) final;

    // interactionMgr -> interaction -> scene directly
    void OnDomainMsg(std::shared_ptr<BaseDomainMsg> &msg) final;

    // impl IpcMsgAdapterListener
    void OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) final;

private:
    void WfdP2pStop();
    void WfdP2pStart();

    void ErrorCodeFiltering(int32_t &code);
    void P2pRemoveClient(ConnectionInfo &connectionInfo);

    void OnP2pPeerDisconnected(std::string &mac);
    void OnP2pPeerConnected(ConnectionInfo &connectionInfo);
    void OnConnectionChanged(ConnectionInfo &connectionInfo);
    void OnP2pPeerDisconnected(ConnectionInfo &connectionInfo);

    void OnDecoderDied(ConnectionInfo &connectionInfo);
    void OnDecoderAccelerationDone(ConnectionInfo &connectionInfo);

    int32_t HandleStop(std::shared_ptr<WfdSinkStopReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleStart(std::shared_ptr<WfdSinkStartReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);

    int32_t HandleSetSceneType(std::shared_ptr<SetSceneTypeReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleSetMediaFormat(std::shared_ptr<SetMediaFormatReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleAppendSurface(std::shared_ptr<WfdAppendSurfaceReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleRemoveSurface(std::shared_ptr<WfdRemoveSurfaceReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);

    int32_t HandleMute(std::shared_ptr<MuteReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandlePlay(std::shared_ptr<WfdPlayReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleUnMute(std::shared_ptr<UnMuteReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandlePause(std::shared_ptr<WfdPauseReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleClose(std::shared_ptr<WfdCloseReq> &msg, std::shared_ptr<WfdCommonRsp> &reply);
    int32_t HandleGetConfig(std::shared_ptr<GetSinkConfigReq> &msg, std::shared_ptr<GetSinkConfigRsp> &reply);

private:
    std::atomic_bool isSinkRunning_ = false;

    int32_t ctrlPort_ = DEFAULT_WFD_CTRLPORT;
    int32_t surfaceMaximum_ = SURFACE_MAX_NUMBER;
    int32_t accessDevMaximum_ = ACCESS_DEV_MAX_NUMBER;
    int32_t foregroundMaximum_ = FOREGROUND_SURFACE_MAX_NUMBER;

    std::mutex mutex_;
    std::shared_ptr<Wifi::WifiP2p> p2pInstance_;
    std::map<uint64_t, std::shared_ptr<DevSurfaceItem>> devSurfaceItemMap_;
    std::unordered_map<std::string, std::shared_ptr<ConnectionInfo>> devConnectionMap_;

    CodecId audioCodecId_ = CodecId::CODEC_AAC;
    CodecId videoCodecId_ = CodecId::CODEC_H264;
    AudioFormat audioFormatId_ = AudioFormat::AUDIO_48000_16_2;
    VideoFormat videoFormatId_ = VideoFormat::VIDEO_1920X1080_25;
};

} // namespace Sharing
} // namespace OHOS
#endif
