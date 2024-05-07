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

#include "wfd_source_session.h"
#include <iomanip>
#include "common/common_macro.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "mediachannel/media_channel_def.h"
#include "utils/utils.h"
#include "wfd_media_def.h"
#include "wfd_message.h"

namespace OHOS {
namespace Sharing {
WfdSourceSession::WfdSourceNetworkSession::WfdSourceNetworkSession(std::weak_ptr<INetworkSession> sessionPtr,
                                                                   std::weak_ptr<WfdSourceSession> serverPtr)
    : sessionPtr_(sessionPtr), serverPtr_(serverPtr)
{
    SHARING_LOGD("trace.");
}

WfdSourceSession::WfdSourceNetworkSession::~WfdSourceNetworkSession()
{
    SHARING_LOGD("trace.");
    if (timer_ != nullptr) {
        timer_->Shutdown();
        timer_.reset();
    }
}

void WfdSourceSession::WfdSourceNetworkSession::UnsetKeepAliveTimer()
{
    SHARING_LOGD("trace.");
    if (timer_ != nullptr) {
        timer_->Unregister(timerId_);
        timerId_ = 0;
    }
}

void WfdSourceSession::WfdSourceNetworkSession::SetKeepAliveTimer()
{
    SHARING_LOGD("trace.");
    int32_t interval = RTSP_INTERACTION_TIMEOUT * WFD_SEC_TO_MSEC;
    if (timer_ != nullptr) {
        timerId_ = timer_->Register(std::bind(&WfdSourceNetworkSession::OnSendKeepAlive, this), interval);
        timer_->Setup();
    }
}

void WfdSourceSession::WfdSourceNetworkSession::OnSessionReadData(int32_t fd, DataBuffer::Ptr buf)
{
    SHARING_LOGD("trace.");
    auto server = serverPtr_.lock();
    if (server) {
        auto session = sessionPtr_.lock();
        if (session) {
            server->OnServerReadData(fd, buf, session);
        }
    }
}

void WfdSourceSession::WfdSourceNetworkSession::OnSessionWriteable(int32_t fd)
{
    SHARING_LOGD("trace.");
}

void WfdSourceSession::WfdSourceNetworkSession::OnSessionClose(int32_t fd)
{
    SHARING_LOGD("trace.");
    auto server = serverPtr_.lock();
    if (server) {
        auto statusMsg = std::make_shared<SessionStatusMsg>();
        statusMsg->msg = std::make_shared<EventMsg>();
        statusMsg->status = STATE_SESSION_ERROR;
        statusMsg->msg->errorCode = ERR_NETWORK_ERROR;
        server->NotifyAgentSessionStatus(statusMsg);
    }
}

void WfdSourceSession::WfdSourceNetworkSession::OnSessionException(int32_t fd)
{
    SHARING_LOGD("trace.");
    auto server = serverPtr_.lock();
    if (server) {
        auto statusMsg = std::make_shared<SessionStatusMsg>();
        statusMsg->msg = std::make_shared<EventMsg>();
        statusMsg->status = STATE_SESSION_ERROR;
        statusMsg->msg->errorCode = ERR_NETWORK_ERROR;
        server->NotifyAgentSessionStatus(statusMsg);
    }
}

void WfdSourceSession::WfdSourceNetworkSession::OnSendKeepAlive() const
{
    SHARING_LOGD("trace.");
    auto session = sessionPtr_.lock();
    if (session) {
        auto server = serverPtr_.lock();
        if (server) {
            server->SendM16Request(session);
        }
    }
}

WfdSourceSession::WfdSourceSession()
{
    SHARING_LOGD("trace.");
    std::stringstream ss;
    ss << std::setw(8) << std::setfill('f') << (int64_t)this; // width:8
    sessionID_ = ss.str();
}

WfdSourceSession::~WfdSourceSession()
{
    SHARING_LOGD("trace.");
    StopWfdSession();
}

void WfdSourceSession::NotifyServiceError()
{
    SHARING_LOGD("trace.");
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    statusMsg->status = STATE_SESSION_ERROR;
    statusMsg->msg->requestId = 0;
    statusMsg->msg->errorCode = ERR_INTERACTION_FAILURE;

    NotifyAgentSessionStatus(statusMsg);
}

void WfdSourceSession::UpdateOperation(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);

    status_ = static_cast<SessionRunningStatus>(statusMsg->status);
    SHARING_LOGI("status: %{public}s.", std::string(magic_enum::enum_name(status_)).c_str());
    switch (status_) {
        case SESSION_START:
            if (!StartWfdSession()) {
                SHARING_LOGE("session start connection failed, sessionId: %{public}u.", GetId());
                statusMsg->msg->errorCode = ERR_CONNECTION_FAILURE;
                statusMsg->status = STATE_SESSION_ERROR;
                break;
            } else {
                return;
            }
        case SESSION_STOP:
            statusMsg->status = STATE_SESSION_STOPED;
            StopWfdSession();
            break;
        case SESSION_PAUSE:
            statusMsg->status = STATE_SESSION_PAUSED;
            break;
        case SESSION_RESUME:
            statusMsg->status = STATE_SESSION_RESUMED;
            break;
        case SESSION_DESTROY:
            statusMsg->status = STATE_SESSION_DESTROYED;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    NotifyAgentSessionStatus(statusMsg);
}

void WfdSourceSession::UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);

    SHARING_LOGI("update media notify status: %{public}s.",
                 std::string(magic_enum::enum_name(static_cast<MediaNotifyStatus>(statusMsg->status))).c_str());
    switch (statusMsg->status) {
        case STATE_PROSUMER_CREATE_SUCCESS:
            NotifyProsumerInit(statusMsg);
            break;
        case STATE_PROSUMER_START_SUCCESS:
            break;
        case STATE_PROSUMER_STOP_SUCCESS:
            break;
        case STATE_PROSUMER_DESTROY_SUCCESS:
            break;
        case STATE_PROSUMER_RESUME_SUCCESS:
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void WfdSourceSession::NotifyProsumerInit(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);

    auto eventMsg = std::make_shared<WfdProducerEventMsg>();
    eventMsg->type = EventType::EVENT_WFD_MEDIA_INIT;
    eventMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
    eventMsg->port = sinkRtpPort_;
    eventMsg->ip = sinkIp_;
    eventMsg->localPort = sourceRtpPort_;
    eventMsg->localIp = sourceIp_;
    SHARING_LOGD("sinkRtpPort %{public}d, sinkIp %{public}s sourceRtpPort %{public}d.", sinkRtpPort_,
                 GetAnonyString(sinkIp_).c_str(), sourceRtpPort_);
    statusMsg->msg = std::move(eventMsg);
    statusMsg->status = NOTIFY_SESSION_PRIVATE_EVENT;

    NotifyAgentSessionStatus(statusMsg);
}

int32_t WfdSourceSession::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("event: type: %{public}s, from: %{public}u, sessionId: %{public}u.",
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), event.eventMsg->fromMgr, GetId());

    switch (event.eventMsg->type) {
        case EventType::EVENT_SESSION_INIT: {
            HandleSessionInit(event);
            break;
        }
        case EventType::EVENT_WFD_STATE_MEDIA_INIT: {
            HandleProsumerInitState(event);
            break;
        }
        case EventType::EVENT_AGENT_KEYMODE_STOP:
            break;
        case EventType::EVENT_SESSION_TEARDOWN:
            break;
        default:
            break;
    }
    return 0;
}

void WfdSourceSession::NotifyAgentSessionStatus(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    if (statusMsg->status == NOTIFY_PROSUMER_CREATE) {
        statusMsg->className = "WfdRtpProducer";
    }
    Notify(statusMsg);
}

bool WfdSourceSession::StartWfdSession()
{
    SHARING_LOGD("trace.");
    if (wfdDefaultPort_ > 0) {
        if (!NetworkFactory::CreateTcpServer(wfdDefaultPort_, shared_from_this(), rtspServerPtr_, "")) {
            SHARING_LOGE("start rtsp server [port:%{public}d] failed.", wfdDefaultPort_);
            return false;
        }
    }
    SHARING_LOGD("start reveiver server success.");
    return true;
}

bool WfdSourceSession::StopWfdSession()
{
    SHARING_LOGD("trace.");
    if (rtspServerPtr_) {
        rtspServerPtr_->Stop();
        rtspServerPtr_.reset();
    }
    return true;
}

void WfdSourceSession::HandleSessionInit(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);
    auto inputMsg = ConvertEventMsg<WfdSourceSessionEventMsg>(event);
    if (inputMsg) {
        sourceMac_ = inputMsg->mac;
        sourceIp_ = inputMsg->ip;
        wfdDefaultPort_ = inputMsg->remotePort != 0 ? inputMsg->remotePort : DEFAULT_WFD_CTRLPORT;
        sourceRtpPort_ = inputMsg->localPort;
        videoFormat_ = inputMsg->videoFormat;
        audioFormat_ = inputMsg->audioFormat;
        SHARING_LOGD("sourceIp %s, sourceMac %s controlPort %d sourceRtpPort %d videoFormat %d audioFormat %d.",
                     GetAnonyString(sourceIp_).c_str(), GetAnonyString(sourceMac_).c_str(), wfdDefaultPort_,
                     sourceRtpPort_, videoFormat_, audioFormat_);
    } else {
        SHARING_LOGE("unknow event msg.");
    }
}

void WfdSourceSession::HandleProsumerInitState(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    auto inputMsg = ConvertEventMsg<WfdSourceSessionEventMsg>(event);
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    statusMsg->status = STATE_SESSION_ERROR;
    statusMsg->msg->errorCode = ERR_SESSION_START;

    if (inputMsg) {
        statusMsg->msg->requestId = inputMsg->requestId;
        if (inputMsg->errorCode == ERR_OK) {
            statusMsg->status = STATE_SESSION_STARTED;
            statusMsg->msg->errorCode = ERR_OK;
        } else {
            SHARING_LOGE("producer inited failed, producerId: %{public}u.", inputMsg->prosumerId);
        }
    } else {
        SHARING_LOGE("producer inited failed: unknow msg.");
        return;
    }

    NotifyAgentSessionStatus(statusMsg);
}

void WfdSourceSession::OnAccept(std::weak_ptr<INetworkSession> session)
{
    SHARING_LOGD("trace.");
    auto sessionPtr = session.lock();
    if (sessionPtr) {
        rtspServerFd_ = sessionPtr->GetSocketInfo()->GetPeerFd();
        sourceIp_ = sessionPtr->GetSocketInfo()->GetLocalIp();
        sinkIp_ = sessionPtr->GetSocketInfo()->GetPeerIp();
        SHARING_LOGD("primarySinkIp %s ,sourceIp %s.", GetAnonyString(sinkIp_).c_str(),
                     GetAnonyString(sourceIp_).c_str());
        auto sa = std::make_shared<WfdSourceNetworkSession>(session, shared_from_this());
        sessionPtr->RegisterCallback(std::move(sa));
        sessionPtr->Start();
        if (!SendM1Request(sessionPtr)) {
            SHARING_LOGE("WFD source SendM1Request error.");
        }
    }
}

void WfdSourceSession::OnServerReadData(int32_t fd, DataBuffer::Ptr buf, INetworkSession::Ptr session)
{
    SHARING_LOGD("trace.");
    if (rtspServerFd_ != fd || session == nullptr) {
        return;
    }

    RtspResponse response;
    RtspRequest request;
    std::string message(buf->Peek(), buf->Size());
    SHARING_LOGD("sessionId: %{public}u, Recv WFD sink message:\n%{public}s", GetId(), message.c_str());
    auto ret = response.Parse(message);
    if (ret.code == RtspErrorType::OK) {
        SHARING_LOGD("Recv RTSP Response message.");
        HandleResponse(response, message, session);

        if (ret.info.size() > 2 && ret.info.back() == '$') { // size at least bigger than 2
            ret.info.pop_back();
            SHARING_LOGW("packet splicing need parse again\n%{public}s.", ret.info.c_str());
            ret = request.Parse(ret.info);
            if (ret.code == RtspErrorType::OK) {
                SHARING_LOGD("Recv RTSP Request [method:%{public}s] message.", request.GetMethod().c_str());
                HandleRequest(request, session);
                return;
            }
        }
        return;
    }

    ret = request.Parse(message);
    if (ret.code == RtspErrorType::OK) {
        SHARING_LOGD("Recv RTSP Request [method:%{public}s] message.", request.GetMethod().c_str());
        HandleRequest(request, session);
        return;
    }

    SHARING_LOGD("invalid WFD rtsp message.");
    if (ret.code == RtspErrorType::INCOMPLETE_MESSAGE) {
        lastMessage_ = message;
        SHARING_LOGD("return with '%{public}s'.", ret.info.c_str());
        return;
    } else {
        message = lastMessage_ + message;
        ret = request.Parse(message);
        if (ret.code == RtspErrorType::OK) {
            SHARING_LOGD("spliced the Request message Successfully.");
            HandleRequest(request, session);
            return;
        } else {
            lastMessage_.clear();
            return;
        }
    }
}

bool WfdSourceSession::HandleRequest(const RtspRequest &request, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (!lastMessage_.empty()) {
        lastMessage_.clear();
    }

    int incomingCSeq = request.GetCSeq();
    if (request.GetMethod() == RTSP_METHOD_OPTIONS) { // M2
        return HandleOptionRequest(request, incomingCSeq, session);
    } else if (request.GetMethod() == RTSP_METHOD_SETUP) { // M6
        return HandleSetupRequest(request, incomingCSeq, session);
    } else if (request.GetMethod() == RTSP_METHOD_PLAY) { // M7
        return HandlePlayRequest(request, incomingCSeq, session);
    } else if (request.GetMethod() == RTSP_METHOD_PAUSE) {
        return HandlePauseRequest(request, incomingCSeq, session);
    } else if (request.GetMethod() == RTSP_METHOD_TEARDOWN) {
        return HandleTeardownRequest(request, incomingCSeq, session);
    } else if (request.GetMethod() == RTSP_METHOD_SET_PARAMETER) {
        return HandleIDRRequest(request, incomingCSeq, session);
    } else {
        SHARING_LOGE("RTSP Request [method:%{public}s] message shouldn't be here.", request.GetMethod().c_str());
    }
    return true;
}

bool WfdSourceSession::HandleIDRRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (request.GetSession() == sessionID_) {
        std::list<std::string> params = request.GetBody();
        for (auto &param : params) {
            if (param == WFD_PARAM_IDR_REQUEST) {
                SHARING_LOGD("receive idr request.");
                return SendCommonResponse(cseq, session);
            }
        }
    }
    return true;
}

bool WfdSourceSession::HandleOptionRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (SendM2Response(cseq, session)) {
        if (SendM3Request(session)) {
            return true;
        }
    }
    return false;
}

bool WfdSourceSession::HandleSetupRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    return SendM6Response(session, cseq);
}

bool WfdSourceSession::HandlePlayRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    statusMsg->msg->requestId = 0;
    statusMsg->msg->errorCode = ERR_OK;
    if (prosumerState_ == ERR_PROSUMER_PAUSE) {
        statusMsg->status = NOTIFY_PROSUMER_RESUME;
    } else {
        statusMsg->status = NOTIFY_PROSUMER_START;
        if (session != nullptr) {
            auto sa = session->GetCallback();
            std::shared_ptr<WfdSourceNetworkSession> agent = std::static_pointer_cast<WfdSourceNetworkSession>(sa);
            if (agent != nullptr) {
                agent->SetKeepAliveTimer();
            }
        }
        prosumerState_ = ERR_PROSUMER_START;
    }
    NotifyAgentSessionStatus(statusMsg);
    return SendM7Response(session, cseq);
}

bool WfdSourceSession::HandlePauseRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    prosumerState_ = ERR_PROSUMER_PAUSE;
    statusMsg->status = NOTIFY_PROSUMER_PAUSE;
    statusMsg->msg->errorCode = ERR_PROSUMER_PAUSE;
    prosumerState_ = ERR_PROSUMER_START;
    NotifyAgentSessionStatus(statusMsg);
    return SendCommonResponse(cseq, session);
}

bool WfdSourceSession::HandleTeardownRequest(const RtspRequest &request, int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    statusMsg->status = NOTIFY_PROSUMER_DESTROY;
    statusMsg->msg->errorCode = ERR_PROSUMER_DESTROY;
    NotifyAgentSessionStatus(statusMsg);
    prosumerState_ = ERR_PROSUMER_STOP;
    if (session != nullptr) {
        auto sa = session->GetCallback();
        std::shared_ptr<WfdSourceNetworkSession> agent = std::static_pointer_cast<WfdSourceNetworkSession>(sa);
        if (agent != nullptr) {
            agent->UnsetKeepAliveTimer();
        }
    }
    return SendCommonResponse(cseq, session);
}

bool WfdSourceSession::HandleResponse(const RtspResponse &response, const std::string &message,
                                      INetworkSession::Ptr &session)
{
    SHARING_LOGD("sessionID %{public}s.", response.GetSession().c_str());
    if (!lastMessage_.empty()) {
        lastMessage_.clear();
    }

    if (response.GetCSeq() != cseq_) {
        SHARING_LOGE("sessionId: %{public}u, response CSeq(%{public}d) does not match expected CSeq(%{public}d).",
                     GetId(), response.GetCSeq(), cseq_);
        return false;
    }
    if (wfdState_ >= WfdSessionState::M7) {
        rtspTimeoutCounts_ = MAX_RTSP_TIMEOUT_COUNTS;
    }
    switch (wfdState_) {
        case WfdSessionState::M1:
            return HandleM1Response(response, message, session);
        case WfdSessionState::M3:
            return HandleM3Response(response, message, session);
        case WfdSessionState::M4:
            return HandleM4Response(response, message, session);
        case WfdSessionState::M5:
            return HandleM5Response(response, message, session);
        case WfdSessionState::M7_WAIT:
            return HandleM7Response(response, message, session);
        case WfdSessionState::M8:
            return HandleM8Response(response, message, session);
        default:
            break;
    }
    return true;
}

bool WfdSourceSession::HandleM1Response(const RtspResponse &response, const std::string &message,
                                        INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    (void)message;
    (void)session;
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'OPTIONS' method error.");
        return false;
    }

    std::string publics = response.GetToken(RTSP_TOKEN_PUBLIC);
    if (publics.empty() || publics.find(RTSP_METHOD_WFD) == std::string::npos ||
        publics.find(RTSP_METHOD_SET_PARAMETER) == std::string::npos ||
        publics.find(RTSP_METHOD_GET_PARAMETER) == std::string::npos) {
        SHARING_LOGE("WFD source peer do not support all methods.");
        return false;
    } else {
        SHARING_LOGI("WFD RTSP M1 response ok.");
        return true;
    }
}

bool WfdSourceSession::HandleM3Response(const RtspResponse &response, const std::string &message,
                                        INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    (void)message;
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'SETUP' method error.");
        return false;
    }

    WfdRtspM3Response m3Res;
    auto ret = m3Res.Parse(message);
    if (ret.code == RtspErrorType::OK) {
        sinkRtpPort_ = m3Res.GetClientRtpPorts();
        audioFormat_ = m3Res.GetAudioCodecs();
        videoFormat_ = m3Res.GetVideoFormats();
        wfdVideoFormatsInfo_ = m3Res.GetWfdVideoFormatsInfo();
        SHARING_LOGD("sinkRtpPort:%{public}d audioFormat: %{public}d videoFormat: %{public}d.", sinkRtpPort_,
                     audioFormat_, videoFormat_);
        std::string value = m3Res.GetContentProtection();
        if (value == "" || value == "none") {
            SHARING_LOGE("WFD sink doesn't support hdcp.");
        }
        if (!SendM4Request(session)) {
            SHARING_LOGE("send m4 request error.");
            return false;
        }
        return true;
    } else {
        SHARING_LOGE("WFD source parse 'SETUP' message error.");
        return false;
    }
}

bool WfdSourceSession::HandleM4Response(const RtspResponse &response, const std::string &message,
                                        INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    (void)message;
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'SETUP' method error.");
        return false;
    }

    WfdRtspM4Response m4Res;
    auto ret = m4Res.Parse(message);
    if (ret.code == RtspErrorType::OK) {
        if (!SendM5Request(session)) {
            SHARING_LOGE("send m5 request error.");
            return false;
        }
        SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
        statusMsg->msg = std::make_shared<EventMsg>();
        statusMsg->msg->requestId = 0;
        statusMsg->msg->errorCode = ERR_OK;
        statusMsg->status = NOTIFY_PROSUMER_CREATE;

        NotifyAgentSessionStatus(statusMsg);
    }
    return true;
}

bool WfdSourceSession::HandleM5Response(const RtspResponse &response, const std::string &message,
                                        INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    (void)message;
    (void)session;
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'SETUP' method error.");
        return false;
    }

    WfdRtspM5Response m5Res;
    auto ret = m5Res.Parse(message);
    if (ret.code != RtspErrorType::OK) {
        return false;
    }
    return true;
}

bool WfdSourceSession::HandleM7Response(const RtspResponse &response, const std::string &message,
                                        INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    (void)message;
    (void)session;
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'PLAY' method error.");
        wfdState_ = WfdSessionState::M6;
        return false;
    } else {
        SHARING_LOGI("WFD RTSP PLAY ok, start receiver to recv the stream.");
        wfdState_ = WfdSessionState::M7;
    }

    SHARING_LOGI("WFD RTSP PLAY ok, start receiver to recv the stream.");
    return true;
}

bool WfdSourceSession::HandleM8Response(const RtspResponse &response, const std::string &message,
                                        INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    (void)message;
    (void)session;
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'TEARDOWN' method error.");
        return false;
    }

    // notify wfd scene rtsp teardown
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto eventMsg = std::make_shared<WfdSceneEventMsg>();
    eventMsg->type = EventType::EVENT_WFD_NOTIFY_RTSP_TEARDOWN;
    eventMsg->toMgr = ModuleType::MODULE_INTERACTION;
    eventMsg->mac = sourceMac_;
    statusMsg->msg = std::move(eventMsg);
    statusMsg->status = NOTIFY_SESSION_PRIVATE_EVENT;
    NotifyAgentSessionStatus(statusMsg);

    SHARING_LOGI("WFD RTSP TEARDOWN ok, stop recv the stream, disconnect socket.");
    return true;
}

bool WfdSourceSession::SendM1Request(INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM1Request m1Request(++cseq_);
    std::string m1Req(m1Request.Stringify());
    SHARING_LOGD("%{public}s.", m1Req.c_str());

    bool ret = session->Send(m1Req.c_str(), m1Req.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M1 request.");
        return false;
    }
    wfdState_ = WfdSessionState::M1;
    return ret;
}

bool WfdSourceSession::SendM2Response(int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM2Response m2Response(cseq, RTSP_STATUS_OK);
    std::string m2Res(m2Response.Stringify());
    SHARING_LOGD("%{public}s.", m2Res.c_str());

    bool ret = session->Send(m2Res.c_str(), m2Res.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M2 response.");
        return false;
    }
    wfdState_ = WfdSessionState::M2;
    return ret;
}

bool WfdSourceSession::SendM3Request(INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM3Request m3Request(++cseq_, WFD_RTSP_URL_DEFAULT);
    std::string m3Req(m3Request.Stringify());
    SHARING_LOGD("%{public}s.", m3Req.c_str());

    bool ret = session->Send(m3Req.c_str(), m3Req.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M3 request.");
        return false;
    }
    wfdState_ = WfdSessionState::M3;
    return ret;
}

bool WfdSourceSession::SendM4Request(INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }

    WfdRtspM4Request m4Request(++cseq_, WFD_RTSP_URL_DEFAULT);
    m4Request.SetPresentationUrl(sourceIp_);
    m4Request.SetVideoFormats(wfdVideoFormatsInfo_, videoFormat_);
    m4Request.SetAudioCodecs(audioFormat_);
    m4Request.SetClientRtpPorts(sinkRtpPort_);
    std::string m4Req(m4Request.Stringify());
    SHARING_LOGD("%{public}s.", m4Req.c_str());

    bool ret = session->Send(m4Req.c_str(), m4Req.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M4 request.");
        return false;
    }
    wfdState_ = WfdSessionState::M4;
    return ret;
}

bool WfdSourceSession::SendM5Request(INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM5Request m5Request(++cseq_);
    m5Request.SetTriggerMethod(RTSP_METHOD_SETUP);
    std::string m5Req(m5Request.Stringify());
    SHARING_LOGD("%{public}s.", m5Req.c_str());

    bool ret = session->Send(m5Req.c_str(), m5Req.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M5 request.");
        return false;
    }
    wfdState_ = WfdSessionState::M5;
    return ret;
}

bool WfdSourceSession::SendM6Response(INetworkSession::Ptr &session, int32_t cseq)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM6Response m6Response(cseq, RTSP_STATUS_OK, sessionID_, rtspTimeout_);
    m6Response.SetClientPort(sinkRtpPort_);
    m6Response.SetServerPort(sourceRtpPort_);
    std::string m6Res(m6Response.StringifyEx());
    SHARING_LOGD("%{public}s.", m6Res.c_str());

    bool ret = session->Send(m6Res.c_str(), m6Res.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M6 response.");
    }
    wfdState_ = WfdSessionState::M7_WAIT;
    return ret;
}

bool WfdSourceSession::SendM7Response(INetworkSession::Ptr &session, int32_t cseq)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM7Response m7Response(cseq, RTSP_STATUS_OK, sessionID_);
    m7Response.SetSession(sessionID_);
    m7Response.SetTimeout(rtspTimeout_);
    std::string m7Res(m7Response.StringifyEx());
    SHARING_LOGD("%{public}s.", m7Res.c_str());

    bool ret = session->Send(m7Res.c_str(), m7Res.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M7 response.");
    }
    wfdState_ = WfdSessionState::M7;
    return ret;
}

bool WfdSourceSession::SendM8Response(INetworkSession::Ptr &session, int32_t cseq)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM8Response m8Response(cseq, RTSP_STATUS_OK);
    m8Response.SetSession(sessionID_);
    std::string m8Res(m8Response.Stringify());
    SHARING_LOGD("%{public}s.", m8Res.c_str());

    bool ret = session->Send(m8Res.c_str(), m8Res.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M8 response.");
    }
    wfdState_ = WfdSessionState::M8;
    return ret;
}

bool WfdSourceSession::SendCommonResponse(int32_t cseq, INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (!rtspServerPtr_ || !session) {
        return false;
    }
    RtspResponse m4Response(cseq, RTSP_STATUS_OK);
    m4Response.SetSession(sessionID_);
    std::string m4Res(m4Response.Stringify());
    SHARING_LOGD("%{public}s.", m4Res.c_str());

    bool ret = session->Send(m4Res.c_str(), m4Res.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M4 response.");
    }
    return ret;
}

bool WfdSourceSession::SendM16Request(INetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    if (rtspTimeoutCounts_ <= 0) {
        NotifyServiceError();
        return false;
    }

    if (!rtspServerPtr_ || !session) {
        return false;
    }
    WfdRtspM16Request m16Request(++cseq_, WFD_RTSP_URL_DEFAULT, sessionID_);
    std::string m16Req(m16Request.Stringify());
    SHARING_LOGD("%{public}s.", m16Req.c_str());

    bool ret = session->Send(m16Req.c_str(), m16Req.size());
    if (!ret) {
        SHARING_LOGE("Failed to send M16 request.");
    }
    rtspTimeoutCounts_--;
    return ret;
}

REGISTER_CLASS_REFLECTOR(WfdSourceSession);
} // namespace Sharing
} // namespace OHOS
