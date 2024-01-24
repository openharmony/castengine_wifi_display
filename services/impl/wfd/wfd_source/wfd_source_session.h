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

#ifndef OHOS_SHARING_WFD_SOURCE_SESSION_H
#define OHOS_SHARING_WFD_SOURCE_SESSION_H

#include <list>
#include <memory>
#include <thread>
#include "agent/session/base_session.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "network/network_factory.h"
#include "protocol/rtsp/include/rtsp_request.h"
#include "protocol/rtsp/include/rtsp_response.h"
#include "timer.h"
#include "utils/utils.h"
#include "wfd_message.h"
#include "wfd_session_def.h"

namespace OHOS {
namespace Sharing {

class WfdSourceSession : public BaseSession,
                         public IServerCallback,
                         public std::enable_shared_from_this<WfdSourceSession> {
public:
    enum class WfdSessionState { M0, M1, M2, M3, M4, M5, M6, M6_SENT, M6_DONE, M7, M7_WAIT, M7_SENT, M7_DONE, M8 };

    WfdSourceSession();
    ~WfdSourceSession() override;

    int32_t HandleEvent(SharingEvent &event) override;
    void UpdateOperation(SessionStatusMsg::Ptr &statusMsg) override;
    void UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg) override;

protected:
    void NotifyProsumerInit(SessionStatusMsg::Ptr &statusMsg);
    void NotifyAgentSessionStatus(SessionStatusMsg::Ptr &statusMsg);

private:
    class WfdSourceNetworkSession : public INetworkSessionCallback,
                                    public std::enable_shared_from_this<WfdSourceNetworkSession> {
    public:
        WfdSourceNetworkSession(std::weak_ptr<INetworkSession> sessionPtr, std::weak_ptr<WfdSourceSession> serverPtr);
        ~WfdSourceNetworkSession();

        void SetKeepAliveTimer();
        void UnsetKeepAliveTimer();

        void OnSessionClose(int32_t fd) override;
        void OnSessionWriteable(int32_t fd) override;
        void OnSessionException(int32_t fd) override;
        void OnSessionReadData(int32_t fd, DataBuffer::Ptr buf) override;

    private:
        void OnSendKeepAlive() const;

    private:
        uint32_t timerId_ = 0;
        std::weak_ptr<INetworkSession> sessionPtr_;
        std::weak_ptr<WfdSourceSession> serverPtr_;
        std::unique_ptr<OHOS::Utils::Timer> timer_ = std::make_unique<OHOS::Utils::Timer>("RtspSourceSessionTimer");
    };

    void HandleSessionInit(SharingEvent &event);
    void HandleProsumerInitState(SharingEvent &event);

    bool StopWfdSession();
    bool StartWfdSession();

    // impl IServerCallback
    void OnServerClose(int32_t fd) override {}
    void OnServerWriteable(int32_t fd) override {}
    void OnServerException(int32_t fd) override {}
    void OnAccept(std::weak_ptr<INetworkSession> session) override;
    void OnServerReadData(int32_t fd, DataBuffer::Ptr buf, INetworkSession::Ptr session = nullptr) override;

    bool HandleRequest(const RtspRequest &request, INetworkSession::Ptr &session);
    bool HandleIDRRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session);
    bool HandlePlayRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session);
    bool HandlePauseRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session);
    bool HandleSetupRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session);
    bool HandleOptionRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session);
    bool HandleTeardownRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session);

    bool HandleResponse(const RtspResponse &response, const std::string &message, INetworkSession::Ptr &session);
    bool HandleM1Response(const RtspResponse &response, const std::string &message, INetworkSession::Ptr &session);
    bool HandleM3Response(const RtspResponse &response, const std::string &message, INetworkSession::Ptr &session);
    bool HandleM4Response(const RtspResponse &response, const std::string &message, INetworkSession::Ptr &session);
    bool HandleM5Response(const RtspResponse &response, const std::string &message, INetworkSession::Ptr &session);
    bool HandleM7Response(const RtspResponse &response, const std::string &message, INetworkSession::Ptr &session);
    bool HandleM8Response(const RtspResponse &response, const std::string &message, INetworkSession::Ptr &session);

    bool SendM1Request(INetworkSession::Ptr &session);
    bool SendM3Request(INetworkSession::Ptr &session);
    bool SendM4Request(INetworkSession::Ptr &session);
    bool SendM5Request(INetworkSession::Ptr &session);
    bool SendM16Request(INetworkSession::Ptr &session);
    bool SendM2Response(int32_t cseq, INetworkSession::Ptr &session);     // onOptionsRequest
    bool SendM6Response(INetworkSession::Ptr &session, int32_t cseq);     // SETUP
    bool SendM7Response(INetworkSession::Ptr &session, int32_t cseq);     // PLAY
    bool SendM8Response(INetworkSession::Ptr &session, int32_t cseq);     // TEARDOWN
    bool SendCommonResponse(int32_t cseq, INetworkSession::Ptr &session); // M4 M5 M8 M16

    void NotifyServiceError();

private:
    uint16_t cseq_ = 0;
    uint16_t sinkRtpPort_ = 0;
    uint16_t sourceRtpPort_ = 0;
    uint16_t wfdDefaultPort_ = DEFAULT_WFD_CTRLPORT;
    uint16_t rtspTimeoutCounts_ = MAX_RTSP_TIMEOUT_COUNTS;

    int32_t rtspServerFd_ = 0;
    int32_t prosumerState_ = ERR_PROSUMER_INIT;
    int32_t rtspTimeout_ = RTSP_SESSION_TIMEOUT;

    std::string sinkIp_;
    std::string rtspUrl_;
    std::string sourceIp_;
    std::string sourceMac_;
    std::string sessionID_;
    std::string lastMessage_;

    WfdVideoFormatsInfo wfdVideoFormatsInfo_;
    AudioFormat audioFormat_ = AUDIO_48000_16_2;
    VideoFormat videoFormat_ = VIDEO_1920X1080_30;
    WfdSessionState wfdState_ = WfdSessionState::M0;
    NetworkFactory::ServerPtr rtspServerPtr_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS

#endif
