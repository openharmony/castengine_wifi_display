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

#include "inter_ipc_stub.h"
#include "common/sharing_log.h"
#include "inter_ipc_stub_death_listener.h"
#include "ipc_msg_decoder.h"
#include "ipc_msg_encoder.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Sharing {

InterIpcStub::InterIpcStub()
{
    SHARING_LOGD("trace.");
}

InterIpcStub::~InterIpcStub()
{
    SHARING_LOGD("trace.");
    sharedFromThis_.reset();
    for (auto deathRecipient : deathRecipients_) {
        if (deathRecipient.second != nullptr) {
            deathRecipient.second->SetDeathListener(nullptr);
            deathRecipient.second = nullptr;
        }
    }
}

void InterIpcStub::SetStubListener(std::weak_ptr<IInterIpcStubListener> listener)
{
    SHARING_LOGD("trace.");
    stubListener_ = listener;
}

int InterIpcStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    SHARING_LOGD("trace.");
    switch (code) {
        case InterIpcMsg::INTER_IPC_MSG: {
            SHARING_LOGD("INTER_IPC_MSG.");
            DoIpcCommand(data, reply);
            break;
        }
        case InterIpcMsg::SET_LISTENER_OBJ:
            SHARING_LOGD("SET_LISTENER_OBJ.");
            SetListenerObject(data, reply);
            break;
        case InterIpcMsg::GET_SUBSYSTEM:
            SHARING_LOGD("GET_SUBSYSTEM.");
            GetSystemAbility(data, reply);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return 0;
}

int32_t InterIpcStub::SetListenerObject(MessageParcel &data, MessageParcel &reply)
{
    SHARING_LOGD("trace.");
    std::string key = data.ReadString();
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    (void)reply.WriteInt32(SetListenerObject(key, object));

    return ERR_NONE;
}

int32_t InterIpcStub::DoIpcCommand(MessageParcel &data, MessageParcel &reply)
{
    SHARING_LOGD("trace.");
    std::shared_ptr<BaseMsg> msg = nullptr;
    IpcMsgDecoder::GetInstance().MsgDecode(msg, data);
    if (msg == nullptr) {
        SHARING_LOGE("msg null!");
        return -1;
    }

    std::shared_ptr<BaseMsg> replyMsg = std::make_shared<BaseMsg>();
    DoIpcCommand(msg, replyMsg);
    IpcMsgEncoder::GetInstance().MsgEncode(reply, replyMsg);

    return 0;
}

int32_t InterIpcStub::GetSystemAbility(MessageParcel &data, MessageParcel &reply)
{
    SHARING_LOGD("trace.");
    std::string key = data.ReadString();
    std::string className = data.ReadString();
    SHARING_LOGD("subtype: %{public}s.", className.c_str());
    (void)reply.WriteRemoteObject(GetSubSystemAbility(key, className));

    return ERR_NONE;
}

int32_t InterIpcStub::SetListenerObject(std::string key, const sptr<IRemoteObject> &object)
{
    SHARING_LOGD("trace.");
    return ERR_NONE;
}

int32_t InterIpcStub::DoIpcCommand(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &replyMsg)
{
    SHARING_LOGD("traces.");
    auto listener = stubListener_.lock();
    if (listener) {
        listener->OnIpcRequest(msg, replyMsg);
    } else {
        SHARING_LOGE("stub listener is null!");
    }

    return 0;
}

sptr<IRemoteObject> InterIpcStub::GetSubSystemAbility(std::string key, std::string className)
{
    SHARING_LOGD("traces.");
    return nullptr;
}

void InterIpcStub::CreateDeathListener(std::string key)
{
    if (deathRecipients_.find(key) != deathRecipients_.end()) {
        SHARING_LOGI("key: %{public}s.", key.c_str());
        if (sharedFromThis_ == nullptr) {
            sharedFromThis_ = Ptr(this, [](InterIpcStub *) { SHARING_LOGD("traces."); });
        }
        deathRecipients_[key]->SetDeathListener(std::make_shared<InterIpcStubDeathListener>(sharedFromThis_));
    } else {
        SHARING_LOGE("key not find %{public}s.", key.c_str());
    }
}

void InterIpcStub::OnRemoteDied()
{
    SHARING_LOGD("base On remote died trace.");
}

int32_t InterIpcStub::SendIpcRequest(std::string key, std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = peerProxys_.find(key);
    if (iter == peerProxys_.end()) {
        SHARING_LOGE("peer proxy not found.");
        return -1;
    }
    lock.unlock();

    if (iter->second == nullptr) {
        SHARING_LOGE("peer proxy null!");
        return -1;
    }

    int32_t error = iter->second->DoIpcCommand(msg, reply);
    if (error != ERR_NONE) {
        SHARING_LOGE("do ipc command failed: %{public}d.", error);
        return error;
    }

    return 0;
}

void InterIpcStub::DelPeerProxy(std::string key)
{
    SHARING_LOGD("Delete sub peer proxy, key: %{public}s.", key.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (peerProxys_.find(key) != peerProxys_.end()) {
        peerProxys_[key]->AsObject()->RemoveDeathRecipient(deathRecipients_[key]);
        peerProxys_.erase(key);
        deathRecipients_.erase(key);
    } else {
        SHARING_LOGE("Delete sub peer proxy, key: %{public}s not find.", key.c_str());
    }
}

} // namespace Sharing
} // namespace OHOS