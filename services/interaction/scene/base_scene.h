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

#ifndef OHOS_SHARING_BASE_SCENE_H
#define OHOS_SHARING_BASE_SCENE_H

#include "common/identifier.h"
#include "common/object.h"
#include "common/sharing_log.h"
#include "interaction/interprocess/ipc_msg_adapter.h"
#include "interaction/ipc_codec/ipc_msg.h"
#include "interaction/sharing_adapter.h"

namespace OHOS {
namespace Sharing {

class BaseScene : public Object,
                  public MsgAdapterListener,
                  public IdentifierInfo {
public:
    using Ptr = std::shared_ptr<BaseScene>;

    virtual ~BaseScene()
    {
        SHARING_LOGD("~BaseScene.");
        auto sharingAdapter = sharingAdapter_.lock();
        if (sharingAdapter) {
            sharingAdapter->OnSceneNotifyDestroyed(GetId());
        }
    }

    uint32_t GetInteractionId()
    {
        SHARING_LOGD("trace.");
        return interactionId_;
    }

    void SetInteractionId(uint32_t interactionId)
    {
        SHARING_LOGD("trace.");
        interactionId_ = interactionId;
    }

    void SetSharingAdapter(std::weak_ptr<ISharingAdapter> sharingAdapter)
    {
        SHARING_LOGD("trace.");
        sharingAdapter_ = sharingAdapter;
    }

public:
    virtual void Initialize() = 0;
    virtual void OnInnerEvent(SharingEvent &event) = 0;
    virtual void OnDomainMsg(std::shared_ptr<BaseDomainMsg> &msg) = 0;
    virtual void SetIpcAdapter(const std::weak_ptr<IpcMsgAdapter> &adapter) = 0;
    virtual void OnInnerDestroy(uint32_t contextId, uint32_t agentId, AgentType agentType) = 0;
    virtual void OnInnerError(uint32_t contextId, uint32_t agentId, SharingErrorCode errorCode,
                              std::string message = "inner error") = 0;

protected:
    uint32_t interactionId_ = -1;

    std::weak_ptr<IpcMsgAdapter> ipcAdapter_;
    std::weak_ptr<ISharingAdapter> sharingAdapter_;
};

} // namespace Sharing
} // namespace OHOS
#endif