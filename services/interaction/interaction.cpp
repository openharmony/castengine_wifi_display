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

#include "interaction.h"
#include "common/common_macro.h"
#include "common/event_comm.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "interaction/interaction_manager.h"
#include "magic_enum.hpp"
#include "scene/base_scene.h"

namespace OHOS {
namespace Sharing {

Interaction::~Interaction()
{
    SHARING_LOGD("id: %{public}d.", GetId());
}

bool Interaction::CreateScene(const std::string &className)
{
    SHARING_LOGD("trace.");
    scene_ = ClassReflector<BaseScene>::Class2Instance(className);
    if (scene_ == nullptr) {
        SHARING_LOGE("create scene error.");
        return false;
    }

    scene_->SetInteractionId(GetId());
    scene_->Initialize();
    scene_->SetSharingAdapter(shared_from_this());

    return true;
}

void Interaction::OnDomainMsg(std::shared_ptr<BaseDomainMsg> &msg)
{
    SHARING_LOGD("trace.");
    if (scene_) {
        scene_->OnDomainMsg(msg);
    }
}

void Interaction::ForwardDomainMsg(std::shared_ptr<BaseDomainMsg> &msg)
{
    SHARING_LOGD("trace.");
    InteractionManager::GetInstance().SendDomainMsg(msg);
}

void Interaction::ReleaseScene(uint32_t sceneId)
{
    SHARING_LOGD("trace.");
    auto interactionMsg = std::make_shared<InteractionEventMsg>();
    interactionMsg->toMgr = ModuleType::MODULE_INTERACTION;
    interactionMsg->type = EVENT_INTERACTIONMGR_DESTROY_INTERACTION;

    SharingEvent event;
    event.eventMsg = std::move(interactionMsg);
    event.eventMsg->fromMgr = ModuleType::MODULE_INTERACTION;
    event.eventMsg->dstId = GetId();
    SendEvent(event);
}

void Interaction::OnSceneNotifyDestroyed(uint32_t sceneId)
{
    SHARING_LOGD("scene destroyed will remove interactionId: %{public}u.", GetId());
    auto interactionMsg = std::make_shared<InteractionEventMsg>();
    interactionMsg->toMgr = ModuleType::MODULE_INTERACTION;
    interactionMsg->type = EVENT_INTERACTIONMGR_REMOVE_INTERACTION;

    SharingEvent event;
    event.eventMsg = std::move(interactionMsg);
    event.eventMsg->fromMgr = ModuleType::MODULE_INTERACTION;
    event.eventMsg->dstId = GetId();
    SendEvent(event);
}

void Interaction::Destroy()
{
    SHARING_LOGD("trace.");
    if (scene_) {
        scene_.reset();
    }
}

int32_t Interaction::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("fromMgr: %{public}u, srcId: %{public}u, toMgr: %{public}u, dstId: %{public}u, event: %{public}s.",
                 event.eventMsg->fromMgr, event.eventMsg->srcId, event.eventMsg->toMgr, event.eventMsg->dstId,
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    auto interactionMsg = ConvertEventMsg<InteractionEventMsg>(event);
    auto contextId = interactionMsg->contextId ? interactionMsg->contextId : interactionMsg->srcId;
    auto agentId = interactionMsg->agentId;
    auto agentType = interactionMsg->agentType;
    auto errorCode = interactionMsg->errorCode;

    switch (event.eventMsg->type) {
        case EVENT_INTERACTION_MSG_ERROR: {
            SHARING_LOGI("interaction handle error, errorCode: %{public}d.", errorCode);
            if (scene_) {
                scene_->OnInnerError(contextId, agentId, errorCode);
                if (errorCode == ERR_NETWORK_ERROR || errorCode == ERR_CONNECTION_FAILURE ||
                    errorCode == ERR_INTERACTION_FAILURE || errorCode ==  ERR_PROTOCOL_INTERACTION_TIMEOUT ||
                    errorCode == ERR_INTAKE_TIMEOUT) {
                    SHARING_LOGD("on inner destroy network error.");
                    scene_->OnInnerDestroy(contextId, agentId, agentType);
                    DestroyAgent(contextId, agentId);
                }
            }
            break;
        }
        case EVENT_INTERACTION_STATE_AGENT_DESTROYED:
        case EVENT_INTERACTION_STATE_CONTEXT_DESTROYED: {
            SHARING_LOGI("interaction handle event, destroy result.");
            if (scene_) {
                scene_->OnInnerDestroy(contextId, agentId, agentType);
            }
            break;
        }
        default:
            SHARING_LOGI("interaction forward event to scene.");
            if (scene_) {
                scene_->OnInnerEvent(event);
            }
            break;
    }

    return 0;
}

int32_t Interaction::NotifyEvent(EventMsg::Ptr eventMsg)
{
    SHARING_LOGD("trace.");
    SharingEvent event;
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->fromMgr = ModuleType::MODULE_INTERACTION;
    event.eventMsg->srcId = GetId();
    return SendEvent(event);
}

int32_t Interaction::CreateContext(uint32_t &contextId)
{
    SHARING_LOGD("trace.");
    auto contextMsg = std::make_shared<ContextEventMsg>();
    contextMsg->type = EventType::EVENT_CONTEXTMGR_CREATE;
    contextMsg->toMgr = ModuleType::MODULE_CONTEXT;

    SharingEvent event;
    event.eventMsg = contextMsg;
    event.eventMsg->fromMgr = ModuleType::MODULE_INTERACTION;
    event.eventMsg->srcId = GetId();
    int32_t ret = SendSyncEvent(event);
    if (ret != -1) {
        contextId = contextMsg->dstId;
        SHARING_LOGI("create context success contextId: %{public}u.", contextId);
    } else {
        SHARING_LOGE("create context failed.");
    }

    return 0;
}

int32_t Interaction::DestroyContext(uint32_t contextId)
{
    SHARING_LOGD("contextId: %{public}u.", contextId);
    auto contextMsg = std::make_shared<ContextEventMsg>();
    contextMsg->type = EventType::EVENT_CONTEXTMGR_DESTROY;
    contextMsg->toMgr = ModuleType::MODULE_CONTEXT;
    contextMsg->dstId = contextId;

    int32_t ret = NotifyEvent(contextMsg);
    if (ret != -1) {
        SHARING_LOGI("destroy context success contextId: %{public}u.", contextId);
    } else {
        SHARING_LOGE("destroy context failed.");
    }

    return ret;
}

int32_t Interaction::CreateAgent(uint32_t &contextId, uint32_t &agentId, AgentType agentType, std::string sessionName)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto contextMsg = std::make_shared<ContextEventMsg>();
    contextMsg->type = EventType::EVENT_CONTEXTMGR_AGENT_CREATE;
    contextMsg->toMgr = ModuleType::MODULE_CONTEXT;
    contextMsg->dstId = contextId;
    contextMsg->agentType = agentType;
    contextMsg->className = std::move(sessionName);
    contextMsg->agentId = agentId;

    SharingEvent event;
    event.eventMsg = contextMsg;
    event.eventMsg->fromMgr = ModuleType::MODULE_INTERACTION;
    event.eventMsg->srcId = GetId();
    int32_t ret = SendSyncEvent(event);

    SHARING_LOGI("notify create agent ret: %{public}d agentId: %{public}u.", ret, contextMsg->agentId);
    if (ret != -1) {
        if ((agentId == contextMsg->agentId) || contextMsg->agentId == INVALID_ID) {
            agentId = INVALID_ID;
        } else {
            agentId = contextMsg->agentId;
        }
        contextId = contextMsg->dstId;
        SHARING_LOGI("notify create agent success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("notify create agent failed!");
    }

    return ret;
}

int32_t Interaction::DestroyAgent(uint32_t contextId, uint32_t agentId)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto contextMsg = std::make_shared<AgentEventMsg>();
    contextMsg->type = EventType::EVENT_CONTEXT_AGENT_DESTROY;
    contextMsg->toMgr = ModuleType::MODULE_CONTEXT;
    contextMsg->dstId = contextId;
    contextMsg->agentId = agentId;

    int32_t ret = NotifyEvent(contextMsg);
    if (ret != -1) {
        SHARING_LOGI("destroy agent success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("destroy agent failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::Stop(uint32_t contextId, uint32_t agentId)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    return 0;
}

int32_t Interaction::Start(uint32_t contextId, uint32_t agentId)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_START;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("start agent success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("start agent failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::Pause(uint32_t contextId, uint32_t agentId, MediaType mediaType)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_PAUSE;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    agentMsg->mediaType = mediaType;
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("pause agent success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("pause agent failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::Resume(uint32_t contextId, uint32_t agentId, MediaType mediaType)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_RESUME;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    agentMsg->mediaType = mediaType;
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("resume agent success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("resume agent failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::ForwardEvent(uint32_t contextId, uint32_t agentId, SharingEvent &event, bool isSync)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    RETURN_INVALID_IF_NULL(event.eventMsg);
    event.eventMsg->fromMgr = MODULE_INTERACTION;
    event.eventMsg->srcId = GetId();
    event.listenerType = CLASS_TYPE_SCHEDULER;

    if (isSync) {
        return SendSyncEvent(event);
    } else {
        return SendEvent(event);
    }
}

int32_t Interaction::Play(uint32_t contextId, uint32_t agentId)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_PLAY_START;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("agent play success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("agent play failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::Close(uint32_t contextId, uint32_t agentId)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_PLAY_STOP;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("agent close success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("agent close failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::SetVolume(uint32_t contextId, uint32_t agentId, float volume)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_CHANNEL_SETVOLUME;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    agentMsg->volume = volume;
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("agent set volume success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("agent set volume failed, agentId: %{public}u.", agentId);
    }

    return 0;
}

int32_t Interaction::SetKeyPlay(uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyFrame)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_CHANNEL_SETSCENETYPE;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    agentMsg->surfaceId = surfaceId;
    agentMsg->sceneType = keyFrame ? SceneType::BACKGROUND : SceneType::FOREGROUND;

    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("agent set key play success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("agent set key play failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::SetKeyRedirect(uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyRedirect)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_CHANNEL_SETKEYREDIRECT;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    agentMsg->surfaceId = surfaceId;
    agentMsg->keyRedirect = keyRedirect;

    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("agent set key play success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("agent set key play failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::AppendSurface(uint32_t contextId, uint32_t agentId, sptr<Surface> surface, SceneType sceneType)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    RETURN_INVALID_IF_NULL(surface);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_CHANNEL_APPENDSURFACE;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    agentMsg->surface = surface;
    agentMsg->sceneType = sceneType;
    agentMsg->requestId = GetRequestId();
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("agent set surface success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("agent set surface failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::RemoveSurface(uint32_t contextId, uint32_t agentId, uint64_t surfaceId)
{
    SHARING_LOGD("contextId: %{public}u, agentId: %{public}u.", contextId, agentId);
    auto agentMsg = std::make_shared<AgentEventMsg>();
    agentMsg->type = EventType::EVENT_AGENT_CHANNEL_REMOVESURFACE;
    agentMsg->toMgr = ModuleType::MODULE_CONTEXT;
    agentMsg->dstId = contextId;
    agentMsg->agentId = agentId;
    agentMsg->surfaceId = surfaceId;
    agentMsg->requestId = GetRequestId();
    int32_t ret = NotifyEvent(agentMsg);
    if (ret != -1) {
        SHARING_LOGI("agent del surface success agentId: %{public}u.", agentId);
    } else {
        SHARING_LOGE("agent del surface failed, agentId: %{public}u.", agentId);
    }

    return ret;
}

int32_t Interaction::DestroyWindow(int32_t windowId)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t Interaction::CreateWindow(int32_t &windowId, WindowProperty &windowProperty)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t Interaction::Hide(int32_t windowId)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t Interaction::Show(int32_t windowId)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t Interaction::SetFullScreen(int32_t windowId, bool isFull)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t Interaction::MoveTo(int32_t windowId, int32_t x, int32_t y)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t Interaction::GetSurface(int32_t windowId, sptr<Surface> &surface)
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t Interaction::ReSize(int32_t windowId, int32_t width, int32_t height)
{
    SHARING_LOGD("trace.");
    return 0;
}

} // namespace Sharing
} // namespace OHOS