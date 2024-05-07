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

#include "wfd_sink_session.h"
#include <memory>
#include "common/common.h"
#include "common/common_macro.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "mediachannel/media_channel_def.h"
#include "utils/utils.h"
#include "wfd_media_def.h"
#include "wfd_message.h"

namespace OHOS {
namespace Sharing {
#define WFD_INTERACTION_TIMEOUT 10

#define WFD_TIMEOUT_5_SECOND           5
#define WFD_TIMEOUT_6_SECOND           6
#define WFD_KEEP_ALIVE_TIMEOUT_MIN     10
#define WFD_KEEP_ALIVE_TIMEOUT_DEFAULT 60

WfdSinkSession::WfdSinkSession()
{
    SHARING_LOGI("sessionId: %{public}u.", GetId());
}

WfdSinkSession::~WfdSinkSession()
{
    SHARING_LOGI("sessionId: %{public}u.", GetId());

    if (rtspClient_) {
        connected_ = false;
        rtspClient_->Disconnect();
        rtspClient_.reset();
    }

    timeoutTimer_.reset();
    keepAliveTimer_.reset();
}

int32_t WfdSinkSession::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    if (interrupting_ && status_ == SESSION_INTERRUPT) {
        SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
        NotifySessionInterrupted();
        return 0;
    }

    SHARING_LOGI("eventType: %{public}s, wfd sessionId: %{public}u.",
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), GetId());
    switch (event.eventMsg->type) {
        case EventType::EVENT_SESSION_INIT:
            HandleSessionInit(event);
            break;
        case EventType::EVENT_AGENT_STATE_PROSUMER_INIT:
            HandleProsumerInitState(event);
            break;
        case EventType::EVENT_AGENT_KEYMODE_STOP:
        case EventType::EVENT_WFD_REQUEST_IDR:
            SendIDRRequest();
            break;
        case EventType::EVENT_SESSION_TEARDOWN:
            SendM8Request();
            break;
        case EventType::EVENT_AGENT_STATE_WRITE_WARNING:
            NotifyServiceError(ERR_INTAKE_TIMEOUT);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    if (interrupting_ && status_ == SESSION_INTERRUPT) {
        SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
        NotifySessionInterrupted();
    }

    return 0;
}

void WfdSinkSession::HandleSessionInit(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);
    auto inputMsg = ConvertEventMsg<WfdSinkSessionEventMsg>(event);
    if (inputMsg) {
        remoteMac_ = inputMsg->mac;
        remoteRtspIp_ = inputMsg->ip;
        remoteRtspPort_ = inputMsg->remotePort != 0 ? inputMsg->remotePort : DEFAULT_WFD_CTRLPORT;
        localRtpPort_ = inputMsg->localPort;

        videoFormat_ = inputMsg->videoFormat;
        audioFormat_ = inputMsg->audioFormat;
    } else {
        SHARING_LOGE("unknow event msg.");
    }
}

void WfdSinkSession::HandleProsumerInitState(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    auto inputMsg = ConvertEventMsg<WfdSinkSessionEventMsg>(event);
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
    }

    NotifyAgentSessionStatus(statusMsg);
}

void WfdSinkSession::UpdateOperation(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);

    status_ = static_cast<SessionRunningStatus>(statusMsg->status);
    SHARING_LOGI("status: %{public}s.", std::string(magic_enum::enum_name(status_)).c_str());
    if (interrupting_ && status_ == SESSION_INTERRUPT) {
        SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
        NotifySessionInterrupted();
        return;
    }

    switch (status_) {
        case SESSION_START: {
            if (!StartWfdSession()) {
                SHARING_LOGE("session start connection failed, sessionId: %{public}u.", GetId());
                statusMsg->msg->errorCode = ERR_CONNECTION_FAILURE;
                statusMsg->status = STATE_SESSION_ERROR;
                break;
            }
            connected_ = true;
            if (interrupting_ && status_ == SESSION_INTERRUPT) {
                SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
                connected_ = false;
                NotifySessionInterrupted();
            }

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
        case SESSION_INTERRUPT:
            interrupting_ = true;
            return;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    if (interrupting_ && (status_ != SESSION_STOP && status_ != SESSION_DESTROY)) {
        if (status_ == SESSION_INTERRUPT) {
            SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
            NotifySessionInterrupted();
        }
        SHARING_LOGW("session: %{public}u has been interrupted.", GetId());
        return;
    }

    NotifyAgentSessionStatus(statusMsg);
}

void WfdSinkSession::UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    if (interrupting_ && status_ == SESSION_INTERRUPT) {
        SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
        NotifySessionInterrupted();
        return;
    }

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
            SendIDRRequest();
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    if (interrupting_ && status_ == SESSION_INTERRUPT) {
        SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
        NotifySessionInterrupted();
    }
}

void WfdSinkSession::NotifyProsumerInit(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);

    auto eventMsg = std::make_shared<WfdConsumerEventMsg>();
    eventMsg->type = EventType::EVENT_WFD_MEDIA_INIT;
    eventMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
    eventMsg->port = localRtpPort_;

    Common::SetVideoTrack(eventMsg->videoTrack, videoFormat_);
    Common::SetAudioTrack(eventMsg->audioTrack, audioFormat_);

    statusMsg->msg = std::move(eventMsg);
    statusMsg->status = NOTIFY_SESSION_PRIVATE_EVENT;

    NotifyAgentSessionStatus(statusMsg);
}

void WfdSinkSession::NotifyAgentSessionStatus(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    if (statusMsg->status == NOTIFY_PROSUMER_CREATE) {
        statusMsg->className = "WfdRtpConsumer";
    }

    Notify(statusMsg);
}

bool WfdSinkSession::StartWfdSession()
{
    SHARING_LOGD("trace.");
    if (NetworkFactory::CreateTcpClient(remoteRtspIp_, remoteRtspPort_, shared_from_this(), rtspClient_)) {
        SHARING_LOGI("sessionId: %{public}u, wfds session connected ip: %{public}s.", GetId(),
                     GetAnonyString(remoteRtspIp_).c_str());
    } else {
        // try connect again
        for (int32_t i = 0; i < 5; i++) { // 5: retry time
            if (interrupting_) {
                if (status_ == SESSION_INTERRUPT) {
                    SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
                    NotifySessionInterrupted();
                }
                SHARING_LOGW("session: %{public}u has been interrupted.", GetId());
                return false;
            }

            usleep(1000 * 200); // 1000 * 200: sleep 200ms
            if (rtspClient_) {
                if (rtspClient_->Connect(remoteRtspIp_, remoteRtspPort_, "::", 0)) {
                    SHARING_LOGW("sessionId: %{public}u, reconnected successfully, ip: %{public}s, times: %{public}d.",
                                 GetId(), GetAnonyString(remoteRtspIp_).c_str(), i);
                    break;
                } else {
                    SHARING_LOGE("sessionId: %{public}u, Failed to connect wfd rtsp server %{public}s:%{public}d.",
                                 GetId(), GetAnonyString(remoteRtspIp_).c_str(), remoteRtspPort_);
                    if (i == 4) { // 4: stop try
                        return false;
                    }
                }
            }
        }
    }

    timeoutTimer_ = std::make_unique<TimeoutTimer>();
    timeoutTimer_->SetTimeoutCallback([this]() {
        if (interrupting_) {
            if (status_ == SESSION_INTERRUPT) {
                NotifySessionInterrupted();
            }
        } else {
            NotifyServiceError(ERR_PROTOCOL_INTERACTION_TIMEOUT);
        }
    });

    timeoutTimer_->StartTimer(WFD_TIMEOUT_6_SECOND, "Waiting for WFD source M1/OPTIONS request");
    return true;
}

bool WfdSinkSession::StopWfdSession()
{
    SHARING_LOGI("sessionId: %{public}u.", GetId());
    SendM8Request();
    connected_ = false;
    return true;
}

void WfdSinkSession::OnClientReadData(int32_t fd, DataBuffer::Ptr buf)
{
    SHARING_LOGD("trace.");
    if (interrupting_) {
        if (status_ == SESSION_INTERRUPT) {
            SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
            NotifySessionInterrupted();
        }
        SHARING_LOGW("session: %{public}u has been interrupted.", GetId());
        return;
    }

    std::string message(buf->Peek(), buf->Size());
    RtspResponse response;
    RtspRequest request;

    SHARING_LOGD("sessionId: %{public}u, Recv WFD source message:\n%{public}s.", GetId(), message.c_str());
    auto ret = response.Parse(message);
    if (ret.code == RtspErrorType::OK) {
        SHARING_LOGD("Recv RTSP Response message.");

        int32_t incommingCSeq = response.GetCSeq();
        auto funcIndex = responseHandlers_.find(incommingCSeq);
        if (funcIndex != responseHandlers_.end() && funcIndex->second) {
            if (!lastMessage_.empty()) {
                lastMessage_.clear();
            }

            funcIndex->second(response, message);
            responseHandlers_.erase(funcIndex);
        } else {
            SHARING_LOGE("Can't find response handler for cseq(%{public}d)", incommingCSeq);
            return;
        }

        if (ret.info.size() > 2 && ret.info.back() == '$') { // 2: size of int
            ret.info.pop_back();
            SHARING_LOGW("*packet splicing need parse again\n%{public}s.", ret.info.c_str());
            ret = request.Parse(ret.info);
            if (ret.code == RtspErrorType::OK) {
                SHARING_LOGD("Recv RTSP Request [method:%{public}s] message.", request.GetMethod().c_str());
                HandleRequest(request, message);
                return;
            }
        }

        return;
    }

    ret = request.Parse(message);
    if (ret.code == RtspErrorType::OK) {
        SHARING_LOGD("Recv RTSP Request [method:%{public}s] message.", request.GetMethod().c_str());
        HandleRequest(request, message);
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
            HandleRequest(request, message);
            return;
        } else {
            lastMessage_.clear();
            return;
        }
    }
}

void WfdSinkSession::OnClientClose(int32_t fd)
{
    SHARING_LOGD("sessionId: %{public}u, RTSP TCP Client socket close %{public}d.", GetId(), fd);
    rtspClient_.reset();

    if (interrupting_) {
        if (status_ == SESSION_INTERRUPT) {
            SHARING_LOGE("session: %{public}u, to notify be interrupted.", GetId());
            NotifySessionInterrupted();
        }
        SHARING_LOGW("session: %{public}u has been interrupted.", GetId());
        return;
    }

    if (wfdState_ < WfdSessionState::STOPPING) {
        NotifyServiceError(ERR_NETWORK_ERROR);
    }
}

void WfdSinkSession::HandleRequest(const RtspRequest &request, const std::string &message)
{
    SHARING_LOGD("trace.");
    if (!lastMessage_.empty()) {
        lastMessage_.clear();
    }

    int32_t incomingCSeq = request.GetCSeq();
    std::string rtspMethod = request.GetMethod();
    if (rtspMethod == RTSP_METHOD_OPTIONS) {
        // this is M1 request
        SHARING_LOGD("Handle M1 request.");
        timeoutTimer_->StopTimer();
        SendM1Response(incomingCSeq);
        SendM2Request();
        return;
    }

    if (rtspMethod == RTSP_METHOD_GET_PARAMETER) {
        std::list<std::string> params = request.GetBody();
        if (!params.empty()) {
            // this is M3 request
            SHARING_LOGD("Handle M3 request.");
            timeoutTimer_->StopTimer();
            SendM3Response(incomingCSeq, params);
        } else {
            // M16: Keep-Alive message
            SHARING_LOGD("Handle M16/Keep-Alive request.");
            keepAliveTimer_->StopTimer();
            SendCommonResponse(incomingCSeq);
            keepAliveTimer_->StartTimer(keepAliveTimeout_,
                                        "Waiting for WFD source M16/GET_PARAMETER KeepAlive request");
        }

        return;
    }

    if (rtspMethod == RTSP_METHOD_SET_PARAMETER) {
        // this is M4 or M5 request
        HandleSetParamRequest(request, message);
        return;
    }
    SHARING_LOGE("RTSP Request [method:%{public}s] message shouldn't be here.", request.GetMethod().c_str());
}

void WfdSinkSession::HandleSetParamRequest(const RtspRequest &request, const std::string &message)
{
    SHARING_LOGD("trace.");
    int32_t incomingCSeq = request.GetCSeq();

    std::list<std::string> paramSet = request.GetBody();
    std::list<std::pair<std::string, std::string>> paramMap;
    RtspCommon::SplitParameter(paramSet, paramMap);
    if (paramMap.empty()) {
        SHARING_LOGE("invalid SET_PARAMETER request without body.");
        return;
    }

    // Triger request
    auto it = std::find_if(paramMap.begin(), paramMap.end(),
                           [](std::pair<std::string, std::string> value) { return value.first == WFD_PARAM_TRIGGER; });
    if (it != paramMap.end()) {
        HandleTriggerMethod(incomingCSeq, it->second);
        return;
    }

    // M4 request
    timeoutTimer_->StopTimer();
    bool ret = HandleM4Request(message);
    if (ret) {
        SessionStatusMsg::Ptr statusMsg = std::make_shared<SessionStatusMsg>();
        statusMsg->msg = std::make_shared<EventMsg>();
        statusMsg->msg->requestId = 0;
        statusMsg->msg->errorCode = ERR_OK;
        statusMsg->status = NOTIFY_PROSUMER_CREATE;

        NotifyAgentSessionStatus(statusMsg);
    }

    return;
}

void WfdSinkSession::HandleM2Response(const RtspResponse &response, const std::string &message)
{
    SHARING_LOGD("trace.");

    timeoutTimer_->StopTimer();
    timeoutTimer_->StartTimer(WFD_TIMEOUT_6_SECOND, "Waiting for M3/GET_PARAMETER request");

    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'OPTIONS' method error.");
        NotifyServiceError();
        return;
    }

    std::string publics = response.GetToken(RTSP_TOKEN_PUBLIC);
    if (publics.empty() || publics.find(RTSP_METHOD_WFD) == std::string::npos ||
        publics.find(RTSP_METHOD_SET_PARAMETER) == std::string::npos ||
        publics.find(RTSP_METHOD_GET_PARAMETER) == std::string::npos ||
        publics.find(RTSP_METHOD_SETUP) == std::string::npos || publics.find(RTSP_METHOD_PLAY) == std::string::npos ||
        publics.find(RTSP_METHOD_TEARDOWN) == std::string::npos) {
        SHARING_LOGE("WFD source peer do not support all methods.");
        NotifyServiceError();
    }
}

void WfdSinkSession::HandleM6Response(const RtspResponse &response, const std::string &message)
{
    SHARING_LOGD("trace.");
    timeoutTimer_->StopTimer();

    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'SETUP' method error.");
        NotifyServiceError();
        return;
    }

    rtspSession_ = response.GetSession();
    keepAliveTimeout_ = response.GetTimeout();
    if (keepAliveTimeout_ < WFD_KEEP_ALIVE_TIMEOUT_MIN) {
        keepAliveTimeout_ = WFD_KEEP_ALIVE_TIMEOUT_DEFAULT;
    }

    SendM7Request();
}

void WfdSinkSession::HandleM7Response(const RtspResponse &response, const std::string &message)
{
    SHARING_LOGD("trace.");

    timeoutTimer_->StopTimer();

    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'PLAY' method error.");
        NotifyServiceError();
        return;
    }

    wfdState_ = WfdSessionState::PLAYING;
    SHARING_LOGI("WFD RTSP PLAY ok, start receiver to recv the stream.");

    // Cause the interaction has already been completed,
    // then set a new callback for the next operations.
    timeoutTimer_->SetTimeoutCallback([this]() { NotifyServiceError(ERR_NETWORK_ERROR); });

    keepAliveTimer_ = std::make_unique<TimeoutTimer>();
    keepAliveTimer_->SetTimeoutCallback([this]() { NotifyServiceError(ERR_NETWORK_ERROR); });
    keepAliveTimer_->StartTimer(keepAliveTimeout_, "Waiting for WFD source M16/GET_PARAMETER KeepAlive request");
}

void WfdSinkSession::HandleM8Response(const RtspResponse &response, const std::string &message)
{
    SHARING_LOGD("trace.");
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGE("WFD source peer handle 'TEARDOWN' method error.");
        NotifyServiceError();
        return;
    }

    // notify wfd scene rtsp teardown
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    auto eventMsg = std::make_shared<WfdSceneEventMsg>();
    eventMsg->type = EventType::EVENT_WFD_NOTIFY_RTSP_TEARDOWN;
    eventMsg->toMgr = ModuleType::MODULE_INTERACTION;
    eventMsg->mac = remoteMac_;
    statusMsg->msg = std::move(eventMsg);
    statusMsg->status = NOTIFY_SESSION_PRIVATE_EVENT;

    NotifyAgentSessionStatus(statusMsg);
    SHARING_LOGI("WFD RTSP TEARDOWN ok, stop recv the stream, disconnect socket.");
}

void WfdSinkSession::HandleCommonResponse(const RtspResponse &response, const std::string &message)
{
    SHARING_LOGD("trace.");

    timeoutTimer_->StopTimer();
    if (response.GetStatus() != RTSP_STATUS_OK) {
        SHARING_LOGI("WFD source peer handle method error.");
        NotifyServiceError();
        return;
    }
}

bool WfdSinkSession::SendM1Response(int32_t cseq)
{
    SHARING_LOGD("trace.");
    if (!rtspClient_) {
        return false;
    }

    WfdRtspM1Response m1Response(cseq, RTSP_STATUS_OK);
    std::string m1Res(m1Response.Stringify());

    SHARING_LOGI("sessionId: %{public}d send M1 response, cseq: %{public}d.", GetId(), cseq);
    bool ret = rtspClient_->Send(m1Res.data(), m1Res.length());
    if (!ret) {
        SHARING_LOGE("Failed to send M1 response, cseq: %{public}d.", cseq);
    }

    cseq_ = cseq;
    return ret;
}

bool WfdSinkSession::SendM2Request()
{
    SHARING_LOGD("trace.");
    if (!rtspClient_) {
        return false;
    }

    WfdRtspM2Request m2Request(++cseq_);

    responseHandlers_[cseq_] = [this](auto &&PH1, auto &&PH2) {
        HandleM2Response(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };
    timeoutTimer_->StartTimer(WFD_TIMEOUT_5_SECOND, "Waiting for M2/OPTIONS response");

    SHARING_LOGI("sessionId: %{public}d send M2 request, cseq: %{public}d.", GetId(), cseq_);
    std::string m2Req(m2Request.Stringify());
    bool ret = rtspClient_->Send(m2Req.data(), m2Req.length());
    if (!ret) {
        SHARING_LOGE("Failed to send M2 request, cseq: %{public}d", cseq_);
        responseHandlers_.erase(cseq_);
        timeoutTimer_->StopTimer();
        NotifyServiceError();
        return false;
    }

    return true;
}

bool WfdSinkSession::SendM3Response(int32_t cseq, std::list<std::string> &params)
{
    SHARING_LOGD("trace.");
    if (!rtspClient_) {
        return false;
    }

    if (params.empty()) {
        return false;
    }

    WfdRtspM3Response m3Response(cseq, RTSP_STATUS_OK);
    for (auto &param : params) {
        if (param == WFD_PARAM_VIDEO_FORMATS) {
            m3Response.SetVideoFormats(videoFormat_);
        } else if (param == WFD_PARAM_AUDIO_CODECS) {
            m3Response.SetAudioCodecs(AUDIO_48000_16_2);
        } else if (param == WFD_PARAM_RTP_PORTS) {
            m3Response.SetClientRtpPorts(localRtpPort_);
        } else if (param == WFD_PARAM_CONTENT_PROTECTION) {
            m3Response.SetContentProtection();
        } else if (param == WFD_PARAM_COUPLED_SINK) {
            m3Response.SetCoupledSink();
        } else if (param == WFD_PARAM_UIBC_CAPABILITY) {
            m3Response.SetUibcCapability();
        } else if (param == WFD_PARAM_STANDBY_RESUME) {
            m3Response.SetStandbyResumeCapability();
        } else {
            m3Response.SetCustomParam(param);
        }
    }

    m3Response.SetCustomParam("wfd_connector_type", "5");

    timeoutTimer_->StartTimer(WFD_TIMEOUT_6_SECOND, "Waiting for M4/SET_PARAMETER request");

    SHARING_LOGI("sessionId: %{public}d send M3 response, cseq: %{public}d.", GetId(), cseq);
    std::string m3Req(m3Response.Stringify());
    bool ret = rtspClient_->Send(m3Req.data(), m3Req.length());
    if (!ret) {
        SHARING_LOGE("Failed to send M3 response, cseq: %{public}d", cseq);
        timeoutTimer_->StopTimer();
        NotifyServiceError();
        return ret;
    }

    return 0;
}

bool WfdSinkSession::HandleM4Request(const std::string &message)
{
    SHARING_LOGD("trace.");

    WfdRtspM4Request m4Request;
    m4Request.Parse(message);
    rtspUrl_ = m4Request.GetPresentationUrl();

    int incomingCSeq = m4Request.GetCSeq();

    timeoutTimer_->StartTimer(WFD_TIMEOUT_6_SECOND, "Waiting for M5/SET_PARAMETER Triger request");

    // Send M4 response
    if (!SendCommonResponse(incomingCSeq)) {
        timeoutTimer_->StopTimer();
        NotifyServiceError();
        return false;
    }

    return true;
}

bool WfdSinkSession::SendCommonResponse(int32_t cseq)
{
    SHARING_LOGD("trace.");
    if (!rtspClient_) {
        return false;
    }

    RtspResponse response(cseq, RTSP_STATUS_OK);
    std::string res(response.Stringify());

    SHARING_LOGI("sessionId: %{public}d send common response, cseq: %{public}d.", GetId(), cseq);
    bool ret = rtspClient_->Send(res.data(), res.length());
    if (!ret) {
        SHARING_LOGE("Failed to send common response.");
    }

    return ret;
}

bool WfdSinkSession::HandleTriggerMethod(int32_t cseq, const std::string &method)
{
    SHARING_LOGD("trace.");
    if (method == RTSP_METHOD_SETUP) {
        // this is M5 request
        timeoutTimer_->StopTimer();
        SendCommonResponse(cseq);
        SendM6Request();
    } else if (method == RTSP_METHOD_TEARDOWN) {
        SHARING_LOGW("sessionId: %{public}u, receive Tearndown msg: %{public}s.", GetId(), method.c_str());
        SendCommonResponse(cseq);
        SendM8Request();
    } else {
        SHARING_LOGE("ignore UNSUPPORTED triggered method '%{public}s'.", method.c_str());
    }

    return true;
}

bool WfdSinkSession::SendM6Request()
{
    SHARING_LOGD("trace.");
    if (!rtspClient_) {
        return false;
    }

    WfdRtspM6Request m6Request(++cseq_, rtspUrl_);
    m6Request.SetClientPort(localRtpPort_);

    responseHandlers_[cseq_] = [this](auto &&PH1, auto &&PH2) {
        HandleM6Response(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };
    timeoutTimer_->StartTimer(WFD_TIMEOUT_5_SECOND, "Waiting for M6/SETUP response");

    SHARING_LOGI("sessionId: %{public}d send M6 request, cseq: %{public}d.", GetId(), cseq_);
    std::string m6Req(m6Request.Stringify());
    bool ret = rtspClient_->Send(m6Req.data(), m6Req.length());
    if (!ret) {
        SHARING_LOGE("Failed to send M6 request, cseq: %{public}d.", cseq_);
        responseHandlers_.erase(cseq_);
        timeoutTimer_->StopTimer();
        NotifyServiceError();
        return false;
    }

    wfdState_ = WfdSessionState::READY;
    return ret;
}

bool WfdSinkSession::SendM7Request()
{
    SHARING_LOGD("trace.");
    if (!rtspClient_) {
        return false;
    }

    WfdRtspM7Request m7Request(++cseq_, rtspUrl_);
    if (!rtspSession_.empty()) {
        m7Request.SetSession(rtspSession_);
    }

    responseHandlers_[cseq_] = [this](auto &&PH1, auto &&PH2) {
        HandleM7Response(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };
    timeoutTimer_->StartTimer(WFD_TIMEOUT_5_SECOND, "Waiting for M7/PLAY response");

    SHARING_LOGI("sessionId: %{public}d send M7 request, cseq: %{public}d.", GetId(), cseq_);
    std::string m7Req(m7Request.Stringify());
    bool ret = rtspClient_->Send(m7Req.data(), m7Req.length());
    if (!ret) {
        SHARING_LOGE("Failed to send M7 request cseq: %{public}d.", cseq_);
        responseHandlers_.erase(cseq_);
        timeoutTimer_->StopTimer();
        NotifyServiceError();
        return false;
    }

    return ret;
}

bool WfdSinkSession::SendM8Request()
{
    SHARING_LOGD("trace.");
    if (wfdState_ == WfdSessionState::STOPPING) {
        SHARING_LOGI("already send m8 reqeust.");
        return true;
    }

    if (!rtspClient_ || !connected_) {
        SHARING_LOGW("client null or disconnecting.");
        return false;
    }

    WfdRtspM8Request m8Request(++cseq_, rtspUrl_);
    if (!rtspSession_.empty()) {
        m8Request.SetSession(rtspSession_);
    }

    responseHandlers_[cseq_] = [this](auto &&PH1, auto &&PH2) {
        HandleM8Response(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };

    SHARING_LOGI("sessionId: %{public}d send M8 request, cseq: %{public}d.", GetId(), cseq_);
    std::string m8Req(m8Request.Stringify());
    bool ret = rtspClient_->Send(m8Req.data(), m8Req.length());
    if (!ret) {
        SHARING_LOGE("sessionId: %{public}u, failed to send M8 request, cseq: %{public}d.", GetId(), cseq_);
        responseHandlers_.erase(cseq_);
    }

    SHARING_LOGI("sessionId: %{public}u,Send TEARDOWN ok.", GetId());
    wfdState_ = WfdSessionState::STOPPING;
    return ret;
}

bool WfdSinkSession::SendIDRRequest()
{
    SHARING_LOGD("trace.");
    if (wfdState_ != WfdSessionState::PLAYING) {
        return false;
    }

    if (!rtspClient_) {
        return false;
    }

    WfdRtspIDRRequest idrRequest(++cseq_, rtspUrl_);
    if (!rtspSession_.empty()) {
        idrRequest.SetSession(rtspSession_);
    }

    responseHandlers_[cseq_] = [this](auto &&PH1, auto &&PH2) {
        HandleCommonResponse(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };
    timeoutTimer_->StartTimer(WFD_TIMEOUT_6_SECOND, "Waiting for WFD SET_PARAMETER/wfd_idr_request response");

    std::string idrReq(idrRequest.Stringify());
    bool ret = rtspClient_->Send(idrReq.data(), idrReq.length());
    if (!ret) {
        SHARING_LOGE("Failed to send IDR request.");
    }

    return ret;
}

void WfdSinkSession::NotifyServiceError(SharingErrorCode errorCode)
{
    SHARING_LOGD("trace.");
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    statusMsg->status = STATE_SESSION_ERROR;
    statusMsg->msg->requestId = 0;
    statusMsg->msg->errorCode = errorCode;

    NotifyAgentSessionStatus(statusMsg);
}

void WfdSinkSession::NotifySessionInterrupted()
{
    SHARING_LOGD("trace.");
    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->status = STATE_SESSION_INTERRUPTED;

    NotifyAgentSessionStatus(statusMsg);
}

REGISTER_CLASS_REFLECTOR(WfdSinkSession);
} // namespace Sharing
} // namespace OHOS
