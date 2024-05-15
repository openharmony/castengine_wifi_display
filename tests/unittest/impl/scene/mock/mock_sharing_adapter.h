/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_MOCK_SHARING_ADAPTER_H
#define OHOS_SHARING_MOCK_SHARING_ADAPTER_H

#include <gmock/gmock.h>
#include "sharing_adapter.h"

namespace OHOS {
namespace Sharing {

class MockSharingAdapter : public ISharingAdapter {
public:
    ~MockSharingAdapter()
    {
    }

public:
    MOCK_METHOD(void, ReleaseScene, (uint32_t sceneId));
    MOCK_METHOD(void, OnSceneNotifyDestroyed, (uint32_t sceneId));
    MOCK_METHOD(int32_t, ForwardEvent, (uint32_t contextId, uint32_t agentId, SharingEvent &event, bool isSync));
    MOCK_METHOD(void, ForwardDomainMsg, (std::shared_ptr<BaseDomainMsg> &msg));
    MOCK_METHOD(int32_t, CreateContext, (uint32_t &contextId));
    MOCK_METHOD(int32_t, CreateAgent, (uint32_t &contextId, uint32_t &agentId, AgentType agentType,
                std::string sessionName));
    MOCK_METHOD(int32_t, DestroyContext, (uint32_t contextId));
    MOCK_METHOD(int32_t, DestroyAgent, (uint32_t contextId, uint32_t agentId));
    MOCK_METHOD(int32_t, Stop, (uint32_t contextId, uint32_t agentId));
    MOCK_METHOD(int32_t, Start, (uint32_t contextId, uint32_t agentId));
    MOCK_METHOD(int32_t, Pause, (uint32_t contextId, uint32_t agentId, MediaType mediaType));
    MOCK_METHOD(int32_t, Resume, (uint32_t contextId, uint32_t agentId, MediaType mediaType));
    MOCK_METHOD(int32_t, Play, (uint32_t contextId, uint32_t agentId));
    MOCK_METHOD(int32_t, Close, (uint32_t contextId, uint32_t agentId));
    MOCK_METHOD(int32_t, SetKeyPlay, (uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyFrame));
    MOCK_METHOD(int32_t, SetKeyRedirect, (uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyRedirect));
    MOCK_METHOD(int32_t, SetVolume, (uint32_t contextId, uint32_t agentId, float volume));
    MOCK_METHOD(int32_t, AppendSurface, (uint32_t contextId, uint32_t agentId, sptr<Surface> surface,
                SceneType sceneType));
    MOCK_METHOD(int32_t, RemoveSurface, (uint32_t contextId, uint32_t agentId, uint64_t surfaceId));
    MOCK_METHOD(int32_t, DestroyWindow, (int32_t windowId));
    MOCK_METHOD(int32_t, CreateWindow, (int32_t &windowId, WindowProperty &windowProperty));
    MOCK_METHOD(int32_t, Hide, (int32_t windowId));
    MOCK_METHOD(int32_t, Show, (int32_t windowId));
    MOCK_METHOD(int32_t, SetFullScreen, (int32_t windowId, bool isFull));
    MOCK_METHOD(int32_t, MoveTo, (int32_t windowId, int32_t x, int32_t y));
    MOCK_METHOD(int32_t, GetSurface, (int32_t windowId, sptr<Surface> &surface));
    MOCK_METHOD(int32_t, ReSize, (int32_t windowId, int32_t width, int32_t height));
};

} // namespace Sharing
} // namespace OHOS
#endif
