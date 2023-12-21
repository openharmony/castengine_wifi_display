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

#ifndef OHOS_SHARING_EVENT_CHANNEL_H
#define OHOS_SHARING_EVENT_CHANNEL_H

#include "event_comm.h"

namespace OHOS {
namespace Sharing {

struct ProsumerStatusMsg {
    using Ptr = std::shared_ptr<ProsumerStatusMsg>;

    uint32_t status;
    uint32_t agentId = INVALID_ID;
    uint32_t prosumerId = INVALID_ID;
    uint64_t surfaceId = INVALID_ID;
    MediaType mediaType;
    EventMsg::Ptr eventMsg = nullptr;
    SharingErrorCode errorCode = ERR_OK;
};

struct ChannelEventMsg : public EventMsg {
    using Ptr = std::shared_ptr<ChannelEventMsg>;

    std::string className;
    uint32_t agentId = INVALID_ID;
    uint32_t prosumerId = INVALID_ID;
    MediaType mediaType;
};

struct ChannelAppendSurfaceEventMsg : public ChannelEventMsg {
    using Ptr = std::shared_ptr<ChannelAppendSurfaceEventMsg>;

    sptr<Surface> surface = nullptr;
    SceneType sceneType;
};

struct ChannelRemoveSurfaceEventMsg : public ChannelEventMsg {
    using Ptr = std::shared_ptr<ChannelRemoveSurfaceEventMsg>;

    uint64_t surfaceId = 0;
};

struct ChannelSetSceneTypeEventMsg : public ChannelEventMsg {
    using Ptr = std::shared_ptr<ChannelSetSceneTypeEventMsg>;

    uint64_t surfaceId = 0;
    SceneType sceneType;
};

struct ChannelSetVolumeEventMsg : public ChannelEventMsg {
    using Ptr = std::shared_ptr<ChannelSetVolumeEventMsg>;

    float volume;
};

struct ChannelSetKeyRedirectEventMsg : public ChannelEventMsg {
    using Ptr = std::shared_ptr<ChannelSetKeyRedirectEventMsg>;

    bool keyRedirect;
    uint64_t surfaceId = 0;
};

} // namespace Sharing
} // namespace OHOS
#endif