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

#include "interaction/interprocess/inter_ipc_sub_stub.h"
#include "common/sharing_log.h"
#include "interaction/interaction_manager.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Sharing {

InterIpcSubStub::InterIpcSubStub()
{
    SHARING_LOGD("trace.");
}

InterIpcSubStub::~InterIpcSubStub()
{
    SHARING_LOGD("trace.");
}

int32_t InterIpcSubStub::DoIpcCommand(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &replyMsg)
{
    SHARING_LOGD("traces.");
    auto listener = stubListener_.lock();
    if (listener) {
        listener->OnIpcRequest(msg, replyMsg);
    } else {
        SHARING_LOGE("stub listener is null.");
    }

    return 0;
}

int32_t InterIpcSubStub::SetListenerObject(std::string key, const sptr<IRemoteObject> &object)
{
    SHARING_LOGD("trace.");
    if (object == nullptr) {
        SHARING_LOGE("Inter ipc null listener object.");
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

    auto interaction = InteractionManager::GetInstance().GetInteraction(key);
    if (interaction == nullptr) {
        SHARING_LOGE("interaction is null.");
        return -1;
    }

    auto adapter = interaction->GetIpcAdapter();
    if (adapter != nullptr) {
        SHARING_LOGI("adpater finded key: %{public}s.", key.c_str());
        adapter->SetPeerProxy(peerProxy);
    } else {
        SHARING_LOGE("adpater is null key: %{public}s.", key.c_str());
    }

    SHARING_LOGD("peer key: %{public}s, listener num: %{public}zu.", key.c_str(), peerProxys_.size());
    auto deathRecipient = new (std::nothrow) InterIpcDeathRecipient(key);
    deathRecipients_[key] = deathRecipient;
    CreateDeathListener(key);

    if (peerProxy->AsObject() != nullptr) {
        if (!peerProxy->AsObject()->AddDeathRecipient(deathRecipient)) {
            SHARING_LOGE("add death recipient failed!");
            return -1;
        }
    }

    return ERR_NONE;
}

void InterIpcSubStub::OnRemoteDied()
{
    SHARING_LOGD("sub Stub OnRemoteDied trace.");
    peerProxys_.clear();
    auto listener = stubListener_.lock();
    if (listener) {
        listener->OnRemoteDied();
    } else {
        SHARING_LOGE("stub listener is null.");
    }
}

} // namespace Sharing
} // namespace OHOS
