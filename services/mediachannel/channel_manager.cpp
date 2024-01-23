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

#include "channel_manager.h"
#include "common/common_macro.h"
#include "magic_enum.hpp"
#include "mediachannel/media_channel.h"

namespace OHOS {
namespace Sharing {

ChannelManager::ChannelManager()
{
    SHARING_LOGD("id: %{public}s.", std::string(magic_enum::enum_name(ModuleType::MODULE_MEDIACHANNEL)).c_str());
}

void ChannelManager::Init()
{
    SHARING_LOGD("trace.");
}

ChannelManager::~ChannelManager()
{
    SHARING_LOGD("id: %{public}s.", std::string(magic_enum::enum_name(ModuleType::MODULE_MEDIACHANNEL)).c_str());
}

SharingErrorCode ChannelManager::HandleMediaChannelCreate(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto eventMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (eventMsg == nullptr) {
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    auto channel = std::make_shared<MediaChannel>();
    channel->SetContextId(eventMsg->srcId);
    eventMsg->srcId = channel->GetId();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        mediaChannels_.emplace(channel->GetId(), channel);
    }

    return SharingErrorCode::ERR_OK;
}

SharingErrorCode ChannelManager::HandleMediaChannelDestroy(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto eventMsg = ConvertEventMsg<ChannelEventMsg>(event);
    if (eventMsg == nullptr) {
        return SharingErrorCode::ERR_GENERAL_ERROR;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto channel = mediaChannels_.find(eventMsg->dstId);
        if (channel != mediaChannels_.end()) {
            mediaChannels_.erase(channel);
        }
    }

    auto contextMsg = std::make_shared<ContextEventMsg>();
    contextMsg->type = EventType::EVENT_CONTEXTMGR_STATE_CHANNEL_DESTROY;
    contextMsg->toMgr = ModuleType::MODULE_CONTEXT;
    contextMsg->fromMgr = ModuleType::MODULE_MEDIACHANNEL;
    contextMsg->srcId = eventMsg->dstId;
    contextMsg->dstId = eventMsg->srcId;
    contextMsg->agentId = eventMsg->agentId;
    SharingEvent contextEvent;
    contextEvent.eventMsg = std::move(contextMsg);
    SHARING_LOGI("mediachannelId: %{public}d, SendEvent event: %{public}s.", eventMsg->dstId,
                 std::string(magic_enum::enum_name(contextEvent.eventMsg->type)).c_str());
    SendEvent(contextEvent);

    return SharingErrorCode::ERR_OK;
}

int32_t ChannelManager::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("fromMgr: %{public}u, srcId: %{public}u, toMgr: %{public}u, dstId: %{public}u, event: %{public}s.",
                 event.eventMsg->fromMgr, event.eventMsg->srcId, event.eventMsg->toMgr, event.eventMsg->dstId,
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());

    switch (event.eventMsg->type) {
        case EventType::EVENT_CONFIGURE_MEDIACHANNEL:
            SHARING_LOGD("EVENT_CONFIGURE_MEDIACHANNEL.");
            break;
        case EventType::EVENT_MEDIA_CHANNEL_CREATE:
            SHARING_LOGD("trace.");
            HandleMediaChannelCreate(event);
            break;
        case EventType::EVENT_MEDIA_CHANNEL_DESTROY:
            SHARING_LOGD("trace.");
            HandleMediaChannelDestroy(event);
            break;
        default: {
            SHARING_LOGD("default.");
            uint32_t mediaChannelId = event.eventMsg->dstId;
            auto mediaChannel = GetMediaChannel(mediaChannelId);
            if (mediaChannel != nullptr) {
                mediaChannel->HandleEvent(event);
            } else {
                SHARING_LOGE("distribute event channel not exist channelId: %{public}d.", mediaChannelId);
            }
            break;
        }
    }

    SHARING_LOGD("done eventType: %{public}s.", std::string(magic_enum::enum_name(event.eventMsg->type)).c_str());
    return 0;
}

MediaChannel::Ptr ChannelManager::GetMediaChannel(uint32_t mediaChannelId)
{
    SHARING_LOGD("trace, mediachannelId: %{public}u.", mediaChannelId);
    std::lock_guard<std::mutex> lock(mutex_);
    auto channel = mediaChannels_.find(mediaChannelId);
    if (channel != mediaChannels_.end()) {
        return (channel->second);
    } else {
        return nullptr;
    }
}

} // namespace Sharing
} // namespace OHOS