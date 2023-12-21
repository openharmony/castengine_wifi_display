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

#ifndef OHOS_SHARING_EVENT_COMM_H
#define OHOS_SHARING_EVENT_COMM_H

#include <memory>
#include <string>
#include "agent/agent_def.h"
#include "const_def.h"
#include "sharing_log.h"
#include "surface.h"

#define MAKE_EVENT_TYPE(type, event) (((type) << 7) | (event) << 4)

#define EVENT_COMMON EVENT_COMMON_BASE = MAKE_EVENT_TYPE(0, 0)
#define EVENT_CONTEXT                                                                                           \
    EVENT_CONTEXT_BASE = MAKE_EVENT_TYPE(1, 0), EVENT_CONTEXTMGR_CREATE, EVENT_CONTEXTMGR_DESTROY,              \
    EVENT_CONTEXTMGR_AGENT_CREATE, EVENT_CONTEXT_AGENT_CREATE, EVENT_CONTEXT_AGENT_DESTROY,                     \
    EVENT_CONTEXT_MIXPLAYER_WINDOW_CREATE, EVENT_CONTEXT_MIXPLAYER_WINDOW_SETTING,                              \
    EVENT_CONTEXT_MIXPLAYER_WINDOW_CLOSE, EVENT_CONTEXT_MIXPLAYER_CREATE, EVENT_CONTEXT_MIXPLAYER_DESTROY,      \
    EVENT_CONTEXT_MIXPLAYER_ADD, EVENT_CONTEXT_MIXPLAYER_REMOVE, EVENT_CONTEXT_MIXPLAYER_START,                 \
    EVENT_CONTEXT_MIXPLAYER_STOP, EVENT_CONTEXT_STATE_PRODUCER_DESTROY, EVENT_CONTEXTMGR_STATE_CHANNEL_DESTROY, \
    EVENT_CONTEXT_STATE_AGENT_DESTROY, EVENT_CONTEXT_STATE_MIXPLAYER_DESTROY

#define EVENT_AGENT                                                                                                    \
    EVENT_AGENT_BASE = MAKE_EVENT_TYPE(2, 0), EVENT_AGENT_START, EVENT_AGENT_PAUSE, EVENT_AGENT_RESUME,                \
    EVENT_AGENT_DESTROY, EVENT_AGENT_PLAY_START, EVENT_AGENT_PLAY_STOP, EVENT_AGENT_PLAY_PAUSE,                        \
    EVENT_AGENT_PLAY_RESUME, EVENT_AGENT_CHANNEL_SETSCENETYPE, EVENT_AGENT_CHANNEL_SETKEYREDIRECT,                     \
    EVENT_AGENT_CHANNEL_APPENDSURFACE, EVENT_AGENT_CHANNEL_REMOVESURFACE, EVENT_AGENT_STATE_CHANNEL_REMOVESURFACE,     \
    EVENT_AGENT_STATE_CHANNEL_APPENDSURFACE, EVENT_AGENT_PROSUMER_START, EVENT_AGENT_PROSUMER_STOP,                    \
    EVENT_AGENT_PROSUMER_PAUSE, EVENT_AGENT_PROSUMER_RESUME, EVENT_AGENT_STATE_PROSUMER_CREATE,                        \
    EVENT_AGENT_STATE_PROSUMER_START, EVENT_AGENT_STATE_PROSUMER_STOP, EVENT_AGENT_STATE_PROSUMER_PAUSE,               \
    EVENT_AGENT_STATE_PROSUMER_RESUME, EVENT_AGENT_STATE_PROSUMER_DESTROY, EVENT_AGENT_STATE_PLAY_START,               \
    EVENT_AGENT_STATE_PLAY_STOP, EVENT_AGENT_PROSUMER_ERROR, EVENT_AGENT_CHANNEL_SETVOLUME, EVENT_AGENT_KEYMODE_START, \
    EVENT_AGENT_KEYMODE_STOP, EVENT_AGEINT_ACCELERATION_DONE, EVENT_AGENT_DECODER_DIED,                                \
    EVENT_AGENT_STATE_PROSUMER_INIT

#define EVENT_SESSION EVENT_SESSION_BASE = MAKE_EVENT_TYPE(3, 0), EVENT_SESSION_INIT, EVENT_SESSION_TEARDOWN
#define EVENT_MEDIA                                                                                                   \
    EVENT_MEDIA_BASE = MAKE_EVENT_TYPE(4, 0), EVENT_MEDIA_CHANNEL_CREATE, EVENT_MEDIA_CHANNEL_DESTROY,                \
    EVENT_MEDIA_CONSUMER_CREATE, EVENT_MEDIA_CONSUMER_DESTROY, EVENT_MEDIA_CONSUMER_START, EVENT_MEDIA_CONSUMER_STOP, \
    EVENT_MEDIA_CONSUMER_PAUSE, EVENT_MEDIA_CONSUMER_RESUME, EVENT_MEDIA_PLAY_START, EVENT_MEDIA_PLAY_STOP,           \
    EVENT_MEDIA_PRODUCER_CREATE, EVENT_MEDIA_PRODUCER_DESTROY, EVENT_MEDIA_PRODUCER_START, EVENT_MEDIA_PRODUCER_STOP, \
    EVENT_MEDIA_PRODUCER_PAUSE, EVENT_MEDIA_PRODUCER_RESUME, EVENT_MEDIA_MIXPLAYER_CREATE,                            \
    EVENT_MEDIA_MIXPLAYER_DESTROY, EVENT_MEDIA_MIXPLAYER_START, EVENT_MEDIA_MIXPLAYER_STOP,                           \
    EVENT_MEDIA_CHANNEL_APPENDSURFACE, EVENT_MEDIA_CHANNEL_REMOVESURFACE, EVENT_MEDIA_CHANNEL_SETSCENETYPE,           \
    EVENT_MEDIA_CHANNEL_SETVOLUME, EVENT_MEDIA_CHANNEL_KEY_REDIRECT, EVENT_AGENT_STATE_WRITE_WARNING

#define EVENT_SCHEDULER EVENT_SCHEDULER_BASE = MAKE_EVENT_TYPE(5, 0)

#define EVENT_INTERACTION                                                                                            \
    EVENT_INTERACTION_BASE = MAKE_EVENT_TYPE(6, 0), EVENT_INTERACTIONMGR_REMOVE_INTERACTION,                         \
    EVENT_INTERACTION_MSG_ERROR, EVENT_INTERACTION_STATE_AGENT_DESTROYED, EVENT_INTERACTION_STATE_CONTEXT_DESTROYED, \
    EVENT_INTERACTIONMGR_DESTROY_INTERACTION, EVENT_INTERACTION_ACCELERATION_DONE,                                   \
    EVENT_INTERACTION_STATE_REMOVE_SURFACE, EVENT_INTERACTION_DECODER_DIED

#define EVENT_CONFIGURE                                                                            \
    EVENT_CONFIGURE_BASE = MAKE_EVENT_TYPE(7, 0), EVENT_CONFIGURE_READY, EVENT_CONFIGURE_INTERACT, \
    EVENT_CONFIGURE_MEDIACHANNEL, EVENT_CONFIGURE_CONTEXT, EVENT_CONFIGURE_WINDOW

#define EVENT_WFD                                                                             \
    EVENT_WFD_BASE = MAKE_EVENT_TYPE(9, 0), EVENT_WFD_MEDIA_INIT, EVENT_WFD_STATE_MEDIA_INIT, \
    EVENT_WFD_NOTIFY_RTSP_PLAYED, EVENT_WFD_NOTIFY_RTSP_TEARDOWN, EVENT_WFD_REQUEST_IDR

#define EVENT_SCREEN_CAPTURE    \
    EVENT_SCREEN_CAPTURE_BASE = MAKE_EVENT_TYPE(11, 0), EVENT_SCREEN_CAPTURE_INIT

namespace OHOS {
namespace Sharing {

enum ClassType {
    CLASS_TYPE_INTERACTION,
    CLASS_TYPE_SCHEDULER,
    CLASS_TYPE_CONTEXT,
    CLASS_TYPE_AGENT,
    CLASS_TYPE_SESSION,
    CLASS_TYPE_CONSUMER,
    CLASS_TYPE_PRODUCER,
};

enum EventType {
    EVENT_COMMON,
    EVENT_CONTEXT,
    EVENT_AGENT,
    EVENT_SESSION,
    EVENT_MEDIA,
    EVENT_SCHEDULER,
    EVENT_INTERACTION,
    EVENT_CONFIGURE,
    EVENT_VIDEOCHAT,
    EVENT_WFD,
    // player custom event
    EVENT_PLAYER,
    EVENT_SCREEN_CAPTURE,
};

enum ModuleType {
    MODULE_CONTEXT = 1000,
    MODULE_AGENT,
    MODULE_MEDIACHANNEL,
    MODULE_INTERACTION,
    MODULE_WINDOW,
    MODULE_CONFIGURE,
    MODULE_DATACENTER,
};

struct FromMsg {
    using Ptr = std::shared_ptr<FromMsg>;

    uint32_t srcId = INVALID_ID;
    uint32_t fromMgr = INVALID_ID;
};

struct ToMsg {
    using Ptr = std::shared_ptr<ToMsg>;

    uint32_t toMgr = INVALID_ID;
    uint32_t dstId = INVALID_ID;
};

struct EventMsg : public FromMsg,
                  public ToMsg {
    using Ptr = std::shared_ptr<EventMsg>;

    virtual ~EventMsg() = default;

    int32_t requestId = 0;
    EventType type;
    SharingErrorCode errorCode = ERR_OK;
};

struct ContextEventMsg : public EventMsg {
    using Ptr = std::shared_ptr<ContextEventMsg>;

    uint32_t agentId = INVALID_ID;
    uint64_t surfaceId = INVALID_ID;
    std::string className;
    sptr<Surface> surface = nullptr;
    AgentType agentType;
    MediaType mediaType;
};

struct AgentEventMsg : public ContextEventMsg {
    using Ptr = std::shared_ptr<AgentEventMsg>;

    bool keyRedirect;
    float volume;
    uint32_t prosumerId = INVALID_ID;
    SceneType sceneType;
};

struct SessionEventMsg : public AgentEventMsg {
    using Ptr = std::shared_ptr<SessionEventMsg>;
};

struct AgentStatusMsg {
    using Ptr = std::shared_ptr<AgentStatusMsg>;

    uint32_t status;
    uint32_t agentId = INVALID_ID;
    SharingErrorCode errorCode = ERR_OK;
    EventMsg::Ptr msg = nullptr;
};

struct SessionStatusMsg : public AgentStatusMsg {
    using Ptr = std::shared_ptr<SessionStatusMsg>;

    bool keyRedirect;
    float volume;
    uint32_t prosumerId = INVALID_ID;
    uint64_t surfaceId = 0;
    std::string className;
    sptr<Surface> surface = nullptr;
    MediaType mediaType;
    SceneType sceneType;
};

struct WindowProperty {
    using Ptr = std::shared_ptr<WindowProperty>;

    bool isShow = true;
    bool isFull = true;
    uint32_t width = 0;
    uint32_t height = 0;
    int32_t startX = -1;
    int32_t startY = -1;
};

struct MixPlayerEventMsg : public ContextEventMsg {
    using Ptr = std::shared_ptr<MixPlayerEventMsg>;

    uint32_t channelId = INVALID_ID;
    uint32_t mixPlayerId = INVALID_ID;
    WindowProperty::Ptr windowProperty = nullptr;
};

struct InteractionEventMsg : public EventMsg {
    using Ptr = std::shared_ptr<InteractionEventMsg>;

    uint32_t agentId = INVALID_ID;
    uint32_t contextId = INVALID_ID;
    uint64_t surfaceId = INVALID_ID;
    AgentType agentType = NONE_AGENT;
};

struct AudioTrack {
    using Ptr = std::shared_ptr<AudioTrack>;

    uint32_t channels = 0;
    uint32_t sampleBit = 0;
    uint32_t sampleRate = 0;
    CodecId codecId = CodecId::CODEC_NONE;
};

struct VideoTrack {
    using Ptr = std::shared_ptr<VideoTrack>;

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t frameRate = 0;
    CodecId codecId = CodecId::CODEC_NONE;
};

struct SharingEvent {
    std::string description;
    EventMsg::Ptr eventMsg;
    ClassType emitterType = CLASS_TYPE_SCHEDULER;
    ClassType listenerType = CLASS_TYPE_SCHEDULER;
};

template <typename T>
std::shared_ptr<T> ConvertEventMsg(SharingEvent &event)
{
    if (event.eventMsg == nullptr) {
        SHARING_LOGE("event msg null.");
        return nullptr;
    }
    auto eventMsg = std::static_pointer_cast<T>(event.eventMsg);
    if (eventMsg == nullptr) {
        SHARING_LOGE("null.");
        return nullptr;
    }

    return eventMsg;
}

} // namespace Sharing
} // namespace OHOS
#endif
