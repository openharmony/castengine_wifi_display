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

#ifndef OHOS_SHARING_INTERACTION_H
#define OHOS_SHARING_INTERACTION_H

#include "common/identifier.h"
#include "event/event_base.h"
#include "event/handle_event_base.h"
#include "interaction/ipc_codec/ipc_msg.h"
#include "interaction/interprocess/ipc_msg_adapter.h"
#include "interaction/scene/base_scene.h"

namespace OHOS {
namespace Sharing {

class Interaction final : public HandleEventBase,
                          public IdentifierInfo,
                          public ISharingAdapter,
                          public EventEmitter,
                          public std::enable_shared_from_this<Interaction> {
public:
    using Ptr = std::shared_ptr<Interaction>;

    Interaction() = default;
    virtual ~Interaction();

    IpcMsgAdapter::Ptr GetIpcAdapter()
    {
        SHARING_LOGD("trace.");
        return ipcAdapter_;
    }

    void SetIpcAdapter(IpcMsgAdapter::Ptr ipcAdapter)
    {
        SHARING_LOGD("trace.");
        ipcAdapter_ = ipcAdapter;
    }

    BaseScene::Ptr GetScene()
    {
        SHARING_LOGD("trace.");
        return scene_;
    }

    int32_t GetRequestId()
    {
        SHARING_LOGD("trace.");
        return ++requestId_;
    }

    void SetRpcKey(const std::string &key)
    {
        SHARING_LOGD("trace.");
        rpcKey_ = key;
    }

    const std::string &GetRpcKey() const
    {
        SHARING_LOGD("trace.");
        return rpcKey_;
    }

public:
    void Destroy();
    bool CreateScene(const std::string &className);
    int32_t HandleEvent(SharingEvent &event) final;

    // the two funcs are used to interact with the domain
    // to determine if they need to be moved to the ISharingAdapter
    void OnDomainMsg(std::shared_ptr<BaseDomainMsg> &msg);
    void ForwardDomainMsg(std::shared_ptr<BaseDomainMsg> &msg) override;

    // impl ISharing about Protocol
    void ReleaseScene(uint32_t sceneId) override;
    void OnSceneNotifyDestroyed(uint32_t sceneId) override;
    int32_t ForwardEvent(uint32_t contextId, uint32_t agentId, SharingEvent &event, bool isSync) override;

    int32_t CreateContext(uint32_t &contextId) override;
    int32_t DestroyContext(uint32_t contextId) override;
    int32_t DestroyAgent(uint32_t contextId, uint32_t agentId) override;
    int32_t CreateAgent(uint32_t &contextId, uint32_t &agentId, AgentType agentType, std::string sessionName) override;

    int32_t Stop(uint32_t contextId, uint32_t agentId) override;
    int32_t Start(uint32_t contextId, uint32_t agentId) override;
    int32_t Pause(uint32_t contextId, uint32_t agentId, MediaType mediaType) override;
    int32_t Resume(uint32_t contextId, uint32_t agentId, MediaType mediaType) override;

    // impl ISharing about play
    int32_t Play(uint32_t contextId, uint32_t agentId) override;
    int32_t Close(uint32_t contextId, uint32_t agentId) override;
    int32_t SetVolume(uint32_t contextId, uint32_t agentId, float volume) override;
    int32_t SetKeyPlay(uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyFrame) override;
    int32_t SetKeyRedirect(uint32_t contextId, uint32_t agentId, uint64_t surfaceId, bool keyRedirect) override;

    int32_t RemoveSurface(uint32_t contextId, uint32_t agentId, uint64_t surfaceId) override;
    int32_t AppendSurface(uint32_t contextId, uint32_t agentId, sptr<Surface> surface,
                          SceneType sceneType = FOREGROUND) override;

    // impl ISharing about window
    int32_t DestroyWindow(int32_t windowId) override;
    int32_t CreateWindow(int32_t &windowId, WindowProperty &windowProperty) override;

    int32_t Hide(int32_t windowId) override;
    int32_t Show(int32_t windowId) override;
    int32_t SetFullScreen(int32_t windowId, bool isFull) override;
    int32_t MoveTo(int32_t windowId, int32_t x, int32_t y) override;
    int32_t GetSurface(int32_t windowId, sptr<Surface> &surface) override;
    int32_t ReSize(int32_t windowId, int32_t width, int32_t height) override;

private:
    int32_t NotifyEvent(EventMsg::Ptr eventMsg);

private:
    std::string rpcKey_;
    std::atomic<int32_t> requestId_ = 0;
	
    BaseScene::Ptr scene_ = nullptr;
    IpcMsgAdapter::Ptr ipcAdapter_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif