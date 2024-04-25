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

#include "inter_ipc_service_stub.h"
#include "inter_ipc_service_death_listener.h"
#include "interaction/interaction.h"
#include "interaction/interaction_manager.h"
#include "interaction/interprocess/inter_ipc_sub_stub.h"
#include "interaction/interprocess/ipc_msg_adapter.h"
#include "interaction/scene/base_scene.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Sharing {

InterIpcServiceStub::InterIpcServiceStub()
{
    SHARING_LOGD("trace.");
}

InterIpcServiceStub::~InterIpcServiceStub()
{
    SHARING_LOGD("trace.");
}

sptr<IRemoteObject> InterIpcServiceStub::GetSubSystemAbility(std::string key, std::string className)
{
    SHARING_LOGD("trace.");
    auto interaction = InteractionManager::GetInstance().CreateInteraction(className);
    if (interaction == nullptr) {
        SHARING_LOGE("create interaction error.");
        return nullptr;
    }

    auto scene = interaction->GetScene();
    if (scene == nullptr) {
        SHARING_LOGE("create scene error.");
        return nullptr;
    }

    sptr<InterIpcStub> adapterStub = new (std::nothrow) InterIpcSubStub();
    sptr<IRemoteObject> object = adapterStub->AsObject();

    auto adapter = std::make_shared<IpcMsgAdapter>();
    adapter->SetCallingKey(key);
    adapter->SetLocalStub(adapterStub);
    scene->SetIpcAdapter(adapter);
    interaction->SetIpcAdapter(adapter);
    interaction->SetRpcKey(key);

    SHARING_LOGI("AddInteractionKey key: %{public}s  interactionId: %{public}d.", key.c_str(), interaction->GetId());
    InteractionManager::GetInstance().AddInteractionKey(key, interaction->GetId());

    return object;
}

int32_t InterIpcServiceStub::DoIpcCommand(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &replyMsg)
{
    SHARING_LOGD("trace.");
    return 0;
}

void InterIpcServiceStub::CreateDeathListener(std::string key)
{
    SHARING_LOGD("trace.");
    if (deathRecipients_.find(key) != deathRecipients_.end()) {
        SHARING_LOGI("key: %{public}s.", key.c_str());
        auto listener = std::make_shared<InterIpcServiceDeathListener>();
        if (sharedFromThis_ == nullptr) {
            sharedFromThis_ = InterIpcStub::Ptr(this, [](InterIpcStub *) { SHARING_LOGD("trace."); });
        }
        auto service = std::static_pointer_cast<InterIpcServiceStub>(sharedFromThis_);
        listener->SetService(service);
        deathRecipients_[key]->SetDeathListener(listener);
    } else {
        SHARING_LOGE("key not find %{public}s.", key.c_str());
    }
}

void InterIpcServiceStub::DelPeerProxy(std::string key)
{
    SHARING_LOGD("Delete SA peer proxy, key: %{public}s.", key.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (peerProxys_.find(key) != peerProxys_.end()) {
        peerProxys_[key]->AsObject()->RemoveDeathRecipient(deathRecipients_[key]);
        peerProxys_.erase(key);
        deathRecipients_.erase(key);
    } else {
        SHARING_LOGE("Delete SA peer proxy, key: %{public}s not find.", key.c_str());
    }
}

int32_t InterIpcServiceStub::SetListenerObject(std::string key, const sptr<IRemoteObject> &object)
{
    SHARING_LOGD("trace.");
    if (object == nullptr) {
        SHARING_LOGE("domain rpc null listener object.");
        return -1;
    }

    sptr<IInterIpc> peerProxy = iface_cast<IInterIpc>(object);
    if (peerProxy == nullptr) {
        SHARING_LOGE("convert null.");
        return -1;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    peerProxys_[key] = peerProxy;
    lock.unlock();
    SHARING_LOGD("peer key: %{public}s, listener num: %{public}zu.", key.c_str(), peerProxys_.size());

    auto deathRecipient = new (std::nothrow) InterIpcDeathRecipient(key);
    deathRecipients_[key] = deathRecipient;
    CreateDeathListener(key);

    if (peerProxy->AsObject() != nullptr) {
        if (!peerProxy->AsObject()->AddDeathRecipient(deathRecipient)) {
            SHARING_LOGE("add death recipient failed.");
            return -1;
        }
    }

    return ERR_NONE;
}

} // namespace Sharing
} // namespace OHOS