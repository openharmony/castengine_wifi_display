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

#ifndef OHOS_SHARING_WFD_SINK_SESSION_H
#define OHOS_SHARING_WFD_SINK_SESSION_H

#include <cstdint>
#include <future>
#include <list>
#include <memory>
#include <thread>
#include "agent/session/base_session.h"
#include "network/network_factory.h"
#include "protocol/rtsp/include/rtsp_request.h"
#include "protocol/rtsp/include/rtsp_response.h"
#include "utils/timeout_timer.h"
#include "wfd_session_def.h"

namespace OHOS {
namespace Sharing {

class WfdSinkSession : public BaseSession,
                       public IClientCallback,
                       public std::enable_shared_from_this<WfdSinkSession> {
public:
    WfdSinkSession();
    ~WfdSinkSession() override;

    void UpdateOperation(SessionStatusMsg::Ptr &statusMsg) override;
    void UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg) override;

    int32_t HandleEvent(SharingEvent &event) override;

protected:
    void NotifyProsumerInit(SessionStatusMsg::Ptr &statusMsg);
    void NotifyAgentSessionStatus(SessionStatusMsg::Ptr &statusMsg);

private:
    // impl IClientCallback
    void OnClientClose(int32_t fd) override;
    void OnClientException(int32_t fd) override {}
    void OnClientWriteable(int32_t fd) override {}
    void OnClientConnect(bool isSuccess) override {}
    void OnClientReadData(int32_t fd, DataBuffer::Ptr buf) override;

    bool StartWfdSession();
    bool StopWfdSession();

    void HandleSessionInit(SharingEvent &event);
    void HandleProsumerInitState(SharingEvent &event);

    void NotifySessionInterrupted();
    void NotifyServiceError(SharingErrorCode errorCode = ERR_INTERACTION_FAILURE);

    bool HandleM4Request(const std::string &message);
    bool HandleTriggerMethod(int32_t cseq, const std::string &method);
    void HandleRequest(const RtspRequest &request, const std::string &message);
    void HandleSetParamRequest(const RtspRequest &request, const std::string &message);

    void HandleM2Response(const RtspResponse &response, const std::string &message);
    void HandleM6Response(const RtspResponse &response, const std::string &message);
    void HandleM7Response(const RtspResponse &response, const std::string &message);
    void HandleM8Response(const RtspResponse &response, const std::string &message);
    void HandleCommonResponse(const RtspResponse &response, const std::string &message);

    bool SendM2Request();                  // M2/OPTIONS
    bool SendM6Request();                  // M6/SETUP
    bool SendM7Request();                  // M7/PLAY
    bool SendM8Request();                  // M8/TEARDOWN
    bool SendIDRRequest();                 // M13/SET_PARAMETER wfd-idr-request
    bool SendM1Response(int32_t cseq);     // M1/OPTIONS
    bool SendCommonResponse(int32_t cseq); // M4, M5/SET_PARAMETER Triger, M8, M16/GET_PARAMETER keep-alive
    bool SendM3Response(int32_t cseq, std::list<std::string> &params);

private:
    enum class WfdSessionState { INIT, READY, PLAYING, STOPPING };

    bool connected_ = false;

    uint16_t localRtpPort_ = 0;
    uint16_t remoteRtspPort_ = 0;

    int32_t cseq_ = 0;
    int32_t keepAliveTimeout_ = 0;

    std::string rtspUrl_;
    std::string remoteMac_;
    std::string rtspSession_;
    std::string lastMessage_;
    std::string remoteRtspIp_;

    std::unique_ptr<TimeoutTimer> timeoutTimer_ = nullptr;
    std::unique_ptr<TimeoutTimer> keepAliveTimer_ = nullptr;
    std::map<int, std::function<void(const RtspResponse &response, const std::string &message)>> responseHandlers_;

    AudioFormat audioFormat_ = AUDIO_48000_16_2;
    VideoFormat videoFormat_ = VIDEO_1920X1080_30;
    NetworkFactory::ClientPtr rtspClient_ = nullptr;
    WfdSessionState wfdState_ = WfdSessionState::INIT;
};

} // namespace Sharing
} // namespace OHOS
#endif
