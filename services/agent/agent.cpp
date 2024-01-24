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

#include "agent/agent.h"
#include "agent.h"
#include "common/common_macro.h"
#include "common/event_channel.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "magic_enum.hpp"

namespace OHOS {
namespace Sharing {

Agent::Agent(AgentType agentType) : agentType_(agentType)
{
    SHARING_LOGD("trace.");
}

SharingErrorCode Agent::CreateSession(const std::string &className)
{
    SHARING_LOGD("trace.");
    session_ = std::static_pointer_cast<BaseSession>(ReflectRegistration::GetInstance().CreateObject(className));
    if (session_) {
        session_->SetSessionListener(shared_from_this());
        SHARING_LOGI("create session classname: %{public}s, agentId: %{public}u, sessionId: %{public}u.",
                     className.c_str(), GetId(), session_->GetId());
        SetRunningStatus(AGENT_STEP_CREATE, AGENT_STATUS_DONE);
        return SharingErrorCode::ERR_OK;
    }

    SetRunningStatus(AGENT_STEP_CREATE, AGENT_STATUS_ERROR);
    return SharingErrorCode::ERR_SESSION_CREATE;
}

int32_t Agent::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI(
        "contextId: %{public}u, agentId: %{public}u, mediaChannelId: %{public}u, "
        "fromMgr: %{public}u, srcId: %{public}u channel event: %{public}s.",
        event.eventMsg->dstId, GetId(), mediaChannelId_, event.eventMsg->fromMgr, event.eventMsg->srcId,
        std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());

    if (EventType::EVENT_AGENT_PROSUMER_ERROR == event.eventMsg->type) {
        std::lock_guard<std::mutex> lock(runStepMutex_);
        SetRunningStatus(runStep_, AGENT_STATUS_ERROR);
    }

    bool isCached = false;
    SharingErrorCode retCode = CheckRunStep(event, isCached);
    if (((retCode != ERR_OK) || isCached) && runningStatus_ != AGENT_STATUS_INTERRUPT) {
        SHARING_LOGW("check run step return, agentId: %{public}u.", GetId());
        return retCode;
    }

    auto statusMsg = std::make_shared<SessionStatusMsg>();
    statusMsg->msg = std::make_shared<EventMsg>();
    statusMsg->msg->type = event.eventMsg->type;
    statusMsg->msg->requestId = event.eventMsg->requestId;
    statusMsg->msg->errorCode = event.eventMsg->errorCode;

    if (runningStatus_ == AGENT_STATUS_INTERRUPT) {
        if (session_) {
            SHARING_LOGW("agentId: %{public}u, interrupt.", GetId());
            statusMsg->status = SESSION_INTERRUPT;
            session_->UpdateOperation(statusMsg);
        }
        return retCode;
    }

    auto inputMsg = ConvertEventMsg<AgentEventMsg>(event);
    if (inputMsg == nullptr) {
        SHARING_LOGE("unknow msg.");
        return -1;
    }

    switch (event.eventMsg->type) {
        case EventType::EVENT_AGENT_START:
            statusMsg->status = SESSION_START;
            UpdateSessionStatus(statusMsg);
            break;
        case EventType::EVENT_AGENT_DESTROY:
            statusMsg->status = SESSION_STOP;
            UpdateSessionStatus(statusMsg);
            break;
        case EventType::EVENT_AGENT_PAUSE:
            statusMsg->status = SESSION_PAUSE;
            statusMsg->mediaType = inputMsg->mediaType;
            UpdateSessionStatus(statusMsg);
            break;
        case EventType::EVENT_AGENT_RESUME:
            statusMsg->status = SESSION_RESUME;
            statusMsg->mediaType = inputMsg->mediaType;
            UpdateSessionStatus(statusMsg);
            break;
        case EventType::EVENT_AGENT_STATE_PROSUMER_CREATE: {
            prosumerId_ = inputMsg->prosumerId;
            statusMsg->prosumerId = prosumerId_;
            HandleProsumerState(statusMsg);
            break;
        }
        case EventType::EVENT_AGENT_STATE_PROSUMER_START:  // fall-through
        case EventType::EVENT_AGENT_STATE_PROSUMER_PAUSE:  // fall-through
        case EventType::EVENT_AGENT_STATE_PROSUMER_RESUME: // fall-through
        case EventType::EVENT_AGENT_STATE_PROSUMER_STOP:   // fall-through
        case EventType::EVENT_AGENT_STATE_PROSUMER_DESTROY:
            statusMsg->prosumerId = inputMsg->prosumerId;
            HandleProsumerState(statusMsg);
            break;
        case EventType::EVENT_AGENT_PROSUMER_ERROR:
            statusMsg->prosumerId = inputMsg->prosumerId;
            HandleProsumerError(statusMsg);
            break;
        case EventType::EVENT_AGENT_PLAY_START:
            SendChannelEvent(statusMsg, EVENT_MEDIA_PLAY_START);
            break;
        case EventType::EVENT_AGENT_PLAY_STOP:
            SendChannelEvent(statusMsg, EVENT_MEDIA_PLAY_STOP);
            break;
        case EventType::EVENT_AGENT_STATE_PLAY_START:
            if (statusMsg->msg->errorCode == ERR_OK) {
                PushNextStep(statusMsg);
            } else {
                std::lock_guard<std::mutex> lock(runStepMutex_);
                SetRunningStatus(runStep_, AGENT_STATUS_ERROR);
            }
            break;
        case EventType::EVENT_AGENT_STATE_PLAY_STOP:
            if (statusMsg->msg->errorCode == ERR_OK) {
                PushNextStep(statusMsg);
            } else {
                std::lock_guard<std::mutex> lock(runStepMutex_);
                SetRunningStatus(runStep_, AGENT_STATUS_ERROR);
            }
            break;
        case EventType::EVENT_AGENT_CHANNEL_APPENDSURFACE:
            statusMsg->surface = inputMsg->surface;
            statusMsg->sceneType = inputMsg->sceneType;
            SendChannelAppendSurfaceEvent(statusMsg, EVENT_MEDIA_CHANNEL_APPENDSURFACE);
            break;
        case EventType::EVENT_AGENT_CHANNEL_REMOVESURFACE:
            statusMsg->surfaceId = inputMsg->surfaceId;
            SendChannelRemoveSurfaceEvent(statusMsg, EVENT_MEDIA_CHANNEL_REMOVESURFACE);
            break;
        case EventType::EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE:
            if (statusMsg->msg->errorCode == ERR_OK) {
                PushNextStep(statusMsg);
            } else {
                std::lock_guard<std::mutex> lock(runStepMutex_);
                SetRunningStatus(runStep_, AGENT_STATUS_ERROR);
            }
            break;
        case EventType::EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE:
            PushNextStep(statusMsg);
            statusMsg->surfaceId = inputMsg->surfaceId;
            SendInteractionEvent(statusMsg, EVENT_INTERACTION_STATE_REMOVE_SURFACE);
            break;
        case EventType::EVENT_AGENT_CHANNEL_SETSCENETYPE:
            statusMsg->surfaceId = inputMsg->surfaceId;
            statusMsg->sceneType = inputMsg->sceneType;
            SendChannelSceneTypeEvent(statusMsg, EVENT_MEDIA_CHANNEL_SETSCENETYPE);
            break;
        case EventType::EVENT_AGENT_CHANNEL_SETVOLUME:
            statusMsg->volume = inputMsg->volume;
            SendChannelSetVolumeEvent(statusMsg, EVENT_MEDIA_CHANNEL_SETVOLUME);
            break;
        case EventType::EVENT_AGENT_CHANNEL_SETKEYREDIRECT:
            statusMsg->surfaceId = inputMsg->surfaceId;
            statusMsg->keyRedirect = inputMsg->keyRedirect;
            SendChannelKeyRedirectEvent(statusMsg, EVENT_MEDIA_CHANNEL_KEY_REDIRECT);
            break;
        case EventType::EVENT_AGEINT_ACCELERATION_DONE:
            statusMsg->surfaceId = inputMsg->surfaceId;
            SendInteractionEvent(statusMsg, EVENT_INTERACTION_ACCELERATION_DONE);
            break;
        case EventType::EVENT_AGENT_DECODER_DIED:
            statusMsg->surfaceId = inputMsg->surfaceId;
            SendInteractionEvent(statusMsg, EVENT_INTERACTION_DECODER_DIED);
            break;
        default: {
            if (session_) {
                session_->HandleEvent(event);
            }
        }
    }

    return 0;
}

void Agent::HandleProsumerError(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    SHARING_LOGI("agentId: %{public}u, handle prosumerId: %{public}u, errorCode: %{public}d.", GetId(),
                 statusMsg->prosumerId, statusMsg->msg->errorCode);
    SendInteractionEvent(statusMsg, EVENT_INTERACTION_MSG_ERROR);
    switch (statusMsg->msg->errorCode) {
        case ERR_PROSUMER_INIT:          // fall-through
        case ERR_PROSUMER_CREATE:        // fall-through
        case ERR_PROSUMER_START:         // fall-through
        case ERR_PROSUMER_VIDEO_CAPTURE: // fall-through
        case ERR_PROSUMER_TIMEOUT:
            statusMsg->status = SESSION_STOP;
            UpdateSessionStatus(statusMsg);
            break;
        case ERR_PROSUMER_STOP:
            statusMsg->status = SESSION_DESTROY;
            UpdateSessionStatus(statusMsg);
            break;
        case ERR_PROSUMER_DESTROY: {
            if (agentType_ == SINK_AGENT) {
                SendChannelEvent(statusMsg, EVENT_MEDIA_CHANNEL_DESTROY);
            } else if (agentType_ == SRC_AGENT) {
                SendContextEvent(statusMsg, EVENT_CONTEXT_STATE_AGENT_DESTROY);
            }
            break;
        }
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void Agent::HandleProsumerState(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    SHARING_LOGI("agentId: %{public}u, handle prosumerId: %{public}u, eventType: %{public}s.", GetId(),
                 statusMsg->prosumerId,
                 std::string(magic_enum::enum_name(static_cast<EventType>(statusMsg->msg->type))).c_str());
    if (session_ == nullptr) {
        SHARING_LOGE("session_ is null, agentId: %{public}u.", GetId());
        return;
    }

    MediaNotifyStatus mediaStatus = STATE_PROSUMER_NONE;
    switch (statusMsg->msg->type) {
        case EVENT_AGENT_STATE_PROSUMER_CREATE:
            mediaStatus = STATE_PROSUMER_CREATE_SUCCESS;
            break;
        case EVENT_AGENT_STATE_PROSUMER_START:
            mediaStatus = STATE_PROSUMER_START_SUCCESS;
            break;
        case EVENT_AGENT_STATE_PROSUMER_STOP:
            mediaStatus = STATE_PROSUMER_STOP_SUCCESS;
            break;
        case EVENT_AGENT_STATE_PROSUMER_PAUSE:
            mediaStatus = STATE_PROSUMER_PAUSE_SUCCESS;
            break;
        case EVENT_AGENT_STATE_PROSUMER_RESUME:
            mediaStatus = STATE_PROSUMER_RESUME_SUCCESS;
            break;
        case EVENT_AGENT_STATE_PROSUMER_DESTROY:
            mediaStatus = STATE_PROSUMER_DESTROY_SUCCESS;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    statusMsg->status = mediaStatus;
    session_->UpdateMediaStatus(statusMsg);
    PushNextStep(statusMsg);
}

void Agent::PushNextStep(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    switch (statusMsg->msg->type) {
        case EVENT_AGENT_STATE_PROSUMER_START:
            PopNextStep(AGENT_STEP_START, AGENT_STATUS_DONE);
            break;
        case EVENT_AGENT_STATE_PROSUMER_STOP:
            statusMsg->status = SESSION_DESTROY;
            UpdateSessionStatus(statusMsg);
            break;
        case EVENT_AGENT_STATE_PROSUMER_DESTROY: {
            PopNextStep(AGENT_STEP_DESTROY, AGENT_STATUS_DONE);
            if (agentType_ == SINK_AGENT) {
                SendChannelEvent(statusMsg, EVENT_MEDIA_CHANNEL_DESTROY);
            } else if (agentType_ == SRC_AGENT) {
                SendContextEvent(statusMsg, EVENT_CONTEXT_STATE_AGENT_DESTROY);
            }
            break;
        }
        case EVENT_AGENT_STATE_PLAY_START:
            PopNextStep(AGENT_STEP_PLAY, AGENT_STATUS_DONE);
            break;
        case EVENT_AGENT_STATE_PLAY_STOP:
            PopNextStep(AGENT_STEP_PLAYSTOP, AGENT_STATUS_DONE);
            break;
        case EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE:
            PopNextStep(AGENT_STEP_APPENDSURFACE, AGENT_STATUS_DONE);
            break;
        case EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE:
            PopNextStep(AGENT_STEP_REMOVESURFACE, AGENT_STATUS_DONE);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void Agent::OnSessionNotify(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    SHARING_LOGI("agentId: %{public}u OnSessionNotify status: %{public}s.", GetId(),
                 std::string(magic_enum::enum_name(static_cast<SessionNotifyStatus>(statusMsg->status))).c_str());
    statusMsg->prosumerId = prosumerId_;
    switch (statusMsg->status) {
        case SessionNotifyStatus::NOTIFY_SESSION_PRIVATE_EVENT:
            NotifyPrivateEvent(statusMsg);
            break;
        case SessionNotifyStatus::STATE_SESSION_ERROR:
            HandleSessionError(statusMsg);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void Agent::HandleSessionError(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    SHARING_LOGI("agentId: %{public}u, error: %{public}d.", GetId(), statusMsg->msg->errorCode);
    SendInteractionEvent(statusMsg, EVENT_INTERACTION_MSG_ERROR);
    switch (statusMsg->msg->errorCode) {
        case ERR_SESSION_START:                // fall-through
        case ERR_CONNECTION_FAILURE:           // fall-through
        case ERR_INVALID_URL:                  // fall-through
        case ERR_DECODE_FORMAT:                // fall-through
        case ERR_UNAUTHORIZED:                 // fall-through
        case ERR_INTERACTION_FAILURE:          // fall-through
        case ERR_PROTOCOL_INTERACTION_TIMEOUT: // fall-through
        case ERR_NETWORK_ERROR:                // fall-through
        case ERR_INTAKE_TIMEOUT:
            PopNextStep(runStep_, AGENT_STATUS_ERROR);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void Agent::UpdateSessionStatus(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGI("handle session: %{public}s agentId: %{public}u.",
                 std::string(magic_enum::enum_name(static_cast<SessionRunningStatus>(statusMsg->status))).c_str(),
                 GetId());
    if (session_) {
        session_->UpdateOperation(statusMsg);
    }
}

uint32_t Agent::NotifyPrivateEvent(SessionStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(statusMsg);
    RETURN_INVALID_IF_NULL(statusMsg->msg);

    SHARING_LOGI("agentId: %{public}u, send session event, type: %{public}s.", GetId(),
                 std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());
    auto toMgr = statusMsg->msg->toMgr;
    if (toMgr != ModuleType::MODULE_MEDIACHANNEL && toMgr != ModuleType::MODULE_INTERACTION) {
        SHARING_LOGE("agentId: %{public}u, toMgr: %{public}d, eventType: %{public}s, error!", GetId(), toMgr,
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());
        return ERR_GENERAL_ERROR;
    }

    if (toMgr == ModuleType::MODULE_MEDIACHANNEL) {
        SHARING_LOGI("agentId: %{public}u, send session event to mediachannel mediaChannelId: %{public}u.", GetId(),
                     mediaChannelId_);
        statusMsg->msg->dstId = mediaChannelId_;
        auto channelMsg = std::static_pointer_cast<ChannelEventMsg>(statusMsg->msg);
        channelMsg->prosumerId = prosumerId_;
        channelMsg->agentId = GetId();
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;
    } else if (toMgr == ModuleType::MODULE_INTERACTION) {
        SHARING_LOGI("agentId: %{public}u, send session event to interaction.", GetId());
    }

    auto listener = agentListener_.lock();
    if (listener) {
        auto agentMsg = std::static_pointer_cast<AgentStatusMsg>(statusMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }

    return ERR_OK;
}

void Agent::SetRunningStatus(AgentRunStep step, AgentRunningStatus status)
{
    SHARING_LOGD("agentId: %{public}u, set agent running step: %{public}s, status: %{public}s.", GetId(),
                 std::string(magic_enum::enum_name(step)).c_str(), std::string(magic_enum::enum_name(status)).c_str());
    runStep_ = step;
    runningStatus_ = status;
}

void Agent::SendInteractionEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify interaction: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());
        auto interactionMsg = std::make_shared<InteractionEventMsg>();
        interactionMsg->agentId = GetId();
        interactionMsg->agentType = GetAgentType();
        interactionMsg->toMgr = ModuleType::MODULE_INTERACTION;
        interactionMsg->type = eventType;
        interactionMsg->errorCode = statusMsg->msg->errorCode;
        interactionMsg->requestId = statusMsg->msg->requestId;
        interactionMsg->surfaceId = statusMsg->surfaceId;

        auto agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->agentId = GetId();
        agentMsg->msg = std::move(interactionMsg);
        listener->OnAgentNotify(agentMsg);
    }
}

void Agent::SendChannelEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify to channelmgr, eventType: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());

        auto channelMsg = std::make_shared<ChannelEventMsg>();
        channelMsg->agentId = GetId();
        channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
        channelMsg->dstId = mediaChannelId_;
        channelMsg->prosumerId = prosumerId_;
        channelMsg->type = eventType;
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;

        AgentStatusMsg::Ptr agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->msg = std::move(channelMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }
}

void Agent::SendChannelAppendSurfaceEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify to channelmgr, eventType: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());

        auto channelMsg = std::make_shared<ChannelAppendSurfaceEventMsg>();
        channelMsg->agentId = GetId();
        channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
        channelMsg->dstId = mediaChannelId_;
        channelMsg->prosumerId = prosumerId_;
        channelMsg->type = eventType;
        channelMsg->surface = statusMsg->surface;
        channelMsg->sceneType = statusMsg->sceneType;
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;

        AgentStatusMsg::Ptr agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->msg = std::move(channelMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }
}

void Agent::SendChannelRemoveSurfaceEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify to channelmgr, eventType: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());

        auto channelMsg = std::make_shared<ChannelRemoveSurfaceEventMsg>();
        channelMsg->agentId = GetId();
        channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
        channelMsg->dstId = mediaChannelId_;
        channelMsg->prosumerId = prosumerId_;
        channelMsg->type = eventType;
        channelMsg->surfaceId = statusMsg->surfaceId;
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;

        AgentStatusMsg::Ptr agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->msg = std::move(channelMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }
}

void Agent::SendChannelSceneTypeEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify to channelmgr, eventType: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());

        auto channelMsg = std::make_shared<ChannelSetSceneTypeEventMsg>();
        channelMsg->agentId = GetId();
        channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
        channelMsg->dstId = mediaChannelId_;
        channelMsg->prosumerId = prosumerId_;
        channelMsg->type = eventType;
        channelMsg->surfaceId = statusMsg->surfaceId;
        channelMsg->sceneType = statusMsg->sceneType;
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;

        AgentStatusMsg::Ptr agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->msg = std::move(channelMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }
}

void Agent::SendChannelKeyRedirectEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify to channelmgr, eventType: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());

        auto channelMsg = std::make_shared<ChannelSetKeyRedirectEventMsg>();
        channelMsg->agentId = GetId();
        channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
        channelMsg->dstId = mediaChannelId_;
        channelMsg->prosumerId = prosumerId_;
        channelMsg->type = eventType;
        channelMsg->surfaceId = statusMsg->surfaceId;
        channelMsg->keyRedirect = statusMsg->keyRedirect;
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;

        AgentStatusMsg::Ptr agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->msg = std::move(channelMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }
}

void Agent::SendChannelSetVolumeEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify to channelmgr, eventType: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());

        auto channelMsg = std::make_shared<ChannelSetVolumeEventMsg>();
        channelMsg->agentId = GetId();
        channelMsg->toMgr = ModuleType::MODULE_MEDIACHANNEL;
        channelMsg->dstId = mediaChannelId_;
        channelMsg->prosumerId = prosumerId_;
        channelMsg->type = eventType;
        channelMsg->volume = statusMsg->volume;
        channelMsg->errorCode = statusMsg->msg->errorCode;
        channelMsg->requestId = statusMsg->msg->requestId;

        AgentStatusMsg::Ptr agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->msg = std::move(channelMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }
}

void Agent::SendContextEvent(SessionStatusMsg::Ptr &statusMsg, EventType eventType)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    RETURN_IF_NULL(statusMsg->msg);
    auto listener = agentListener_.lock();
    if (listener) {
        SHARING_LOGI("agentId: %{public}u, notify to context, eventType: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(statusMsg->msg->type)).c_str());
        auto contextMsg = std::make_shared<ContextEventMsg>();
        contextMsg->type = eventType;
        contextMsg->toMgr = ModuleType::MODULE_CONTEXT;
        contextMsg->fromMgr = ModuleType::MODULE_AGENT;
        contextMsg->srcId = GetId();
        contextMsg->agentId = GetId();
        contextMsg->agentType = agentType_;
        contextMsg->errorCode = statusMsg->msg->errorCode;
        contextMsg->requestId = statusMsg->msg->requestId;

        AgentStatusMsg::Ptr agentMsg = std::make_shared<AgentStatusMsg>();
        agentMsg->msg = std::move(contextMsg);
        agentMsg->agentId = GetId();
        listener->OnAgentNotify(agentMsg);
    }
}

AgentRunStep Agent::GetRunStep(EventType eventType)
{
    SHARING_LOGD("trace.");
    AgentRunStep step = AGENT_STEP_IDLE;
    switch (eventType) {
        case EVENT_AGENT_START:
            step = AGENT_STEP_START;
            break;
        case EVENT_AGENT_CHANNEL_APPENDSURFACE:
            step = AGENT_STEP_APPENDSURFACE;
            break;
        case EVENT_AGENT_CHANNEL_REMOVESURFACE:
            step = AGENT_STEP_REMOVESURFACE;
            break;
        case EVENT_AGENT_PLAY_START:
            step = AGENT_STEP_PLAY;
            break;
        case EVENT_AGENT_PLAY_STOP:
            step = AGENT_STEP_PLAYSTOP;
            break;
        case EVENT_AGENT_PROSUMER_ERROR:
            step = AGENT_STEP_DESTROY;
            break;
        case EVENT_AGENT_DESTROY:
            step = AGENT_STEP_DESTROY;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return step;
}

AgentRunStepWeight Agent::GetRunStepWeight(AgentRunStep runStep)
{
    SHARING_LOGD("trace.");
    AgentRunStepWeight weight = AGENT_STEP_WEIGHT_IDLE;
    switch (runStep) {
        case AGENT_STEP_START:
            weight = AGENT_STEP_WEIGHT_START;
            break;
        // case AGENT_STEP_PAUSE:
        // case AGENT_STEP_RESUME:
        case AGENT_STEP_APPENDSURFACE: // fall-through
        case AGENT_STEP_REMOVESURFACE: // fall-through
        case AGENT_STEP_PLAY:          // fall-through
        case AGENT_STEP_PLAYSTOP:
            weight = AGENT_STEP_WEIGHT_REUSABLE;
            break;
        case AGENT_STEP_DESTROY:
            weight = AGENT_STEP_WEIGHT_DESTROY;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return weight;
}

SharingErrorCode Agent::CheckRunStep(SharingEvent &event, bool &isCached)
{
    SHARING_LOGD("check run step, now step: %{public}s, now status: %{public}s, agentId: %{public}u.",
                 std::string(magic_enum::enum_name(runStep_)).c_str(),
                 std::string(magic_enum::enum_name(runningStatus_)).c_str(), GetId());
    SharingErrorCode retCode = ERR_OK;
    AgentRunStep step = GetRunStep(event.eventMsg->type);
    if (step == AGENT_STEP_IDLE) {
        return retCode;
    }

    AgentRunStepWeight weight = GetRunStepWeight(step);
    AgentRunStepKey nextKey = {event.eventMsg->requestId, step, weight};

    std::lock_guard<std::mutex> lock(runStepMutex_);
    switch (runningStatus_) {
        case AGENT_STATUS_DONE:
            if (runStep_ == AGENT_STEP_DESTROY) {
                return ERR_GENERAL_ERROR;
            }
            if ((runStep_ < AGENT_STEP_START) && (step > AGENT_STEP_START) && (step < AGENT_STEP_DESTROY)) {
                runEvents_.emplace(nextKey, event);
                isCached = true;
                SHARING_LOGD("cache event: %{public}s agentId: %{public}u.",
                             std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), GetId());
            } else {
                SetRunningStatus(step, AGENT_STATUS_RUNNING);
            }
            break;
        case AGENT_STATUS_RUNNING: {
            if ((step == runStep_) || (runStep_ >= AGENT_STEP_DESTROY) || (runEvents_.count(nextKey))) {
                SHARING_LOGW("already has event: %{public}s, agentId: %{public}u.",
                             std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), GetId());
                retCode = ERR_GENERAL_ERROR;
            } else {
                if (step == AGENT_STEP_DESTROY && runStep_ < step) {
                    SHARING_LOGW("agentId: %{public}u, interrupt the step: %{public}d.", GetId(), runStep_);
                    SetRunningStatus(runStep_, AGENT_STATUS_INTERRUPT);
                }
                runEvents_.emplace(nextKey, event);
                isCached = true;
                SHARING_LOGW("cache event: %{public}s, agentId: %{public}u.",
                             std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), GetId());
            }
            break;
        }
        case AGENT_STATUS_ERROR: {
            if (step == AGENT_STEP_DESTROY) {
                SetRunningStatus(step, AGENT_STATUS_RUNNING);
            } else {
                retCode = ERR_GENERAL_ERROR;
            }
            break;
        }
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    SHARING_LOGD("check run step, set step: %{public}s, set status: %{public}s, agentId: %{public}u.",
                 std::string(magic_enum::enum_name(runStep_)).c_str(),
                 std::string(magic_enum::enum_name(runningStatus_)).c_str(), GetId());
    return retCode;
}

void Agent::PopNextStep(AgentRunStep step, AgentRunningStatus status)
{
    SHARING_LOGD("pop run step, now step: %{public}s, now status: %{public}s, agentId: %{public}u.",
                 std::string(magic_enum::enum_name(runStep_)).c_str(),
                 std::string(magic_enum::enum_name(runningStatus_)).c_str(), GetId());
    SharingEvent event;
    bool ret = false;
    {
        std::lock_guard<std::mutex> lock(runStepMutex_);
        if (step != runStep_) {
            SHARING_LOGW(
                "pop run step, set step: %{public}s is not equal to now step: %{public}s, agentId: %{public}u.",
                std::string(magic_enum::enum_name(step)).c_str(), std::string(magic_enum::enum_name(runStep_)).c_str(),
                GetId());
        } else {
            SetRunningStatus(step, status);
            SHARING_LOGD("pop run step, set step: %{public}s, set status: %{public}s, agentId: %{public}u.",
                         std::string(magic_enum::enum_name(runStep_)).c_str(),
                         std::string(magic_enum::enum_name(runningStatus_)).c_str(), GetId());
            if (runEvents_.size() > 0) {
                auto it = runEvents_.begin();
                event = std::move(it->second);
                SHARING_LOGD("agentId: %{public}u, next eventType: %{public}s.", GetId(),
                             std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
                runEvents_.erase(it);
                ret = true;
            }
        }
    }

    if (ret) {
        SHARING_LOGI("agentId: %{public}u, pop to handle next event: %{public}s.", GetId(),
                     std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
        HandleEvent(event);
    }
}

} // namespace Sharing
} // namespace OHOS