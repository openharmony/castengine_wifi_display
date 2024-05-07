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

#include "domain_rpc_service_stub.h"
#include "common/sharing_log.h"
#include "interaction/domain/domain_def.h"
#include "interaction/domain/domain_manager.h"
#include "interaction/domain/rpc/domain_rpc_manager.h"
#include "ipc_msg_decoder.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Sharing {

DomainRpcServiceStub::~DomainRpcServiceStub()
{
    SHARING_LOGD("trace.");
    for (auto deathRecipient : deathRecipients_) {
        if (deathRecipient.second != nullptr) {
            deathRecipient.second->SetDeathListener(nullptr);
            deathRecipient.second = nullptr;
        }
    }
}

void DomainRpcServiceStub::SetStubListener(std::weak_ptr<IDomainStubListener> listener)
{
    SHARING_LOGD("trace.");
    stubListener_ = listener;
}

int DomainRpcServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                          MessageOption &option)
{
    SHARING_LOGD("trace.");
    switch (code) {
        case DomainServiceMsg::DOMAIN_MSG: {
            SHARING_LOGD("DOMAIN_MSG.");
            DoRpcCommand(data, reply);
            break;
        }
        case DomainServiceMsg::SET_LISTENER_OBJ:
            SHARING_LOGD("SET_LISTENER_OBJ.");
            SetListenerObject(data, reply);
            break;
        case DomainServiceMsg::GET_SUBSYSTEM:
            SHARING_LOGD("GET_SUBSYSTEM.");
            GetSystemAbility(data, reply);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return 0;
}

int32_t DomainRpcServiceStub::SetListenerObject(MessageParcel &data, MessageParcel &reply)
{
    SHARING_LOGD("trace.");
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    (void)reply.WriteInt32(SetListenerObject(object));
    return ERR_NONE;
}

int32_t DomainRpcServiceStub::SetListenerObject(const sptr<IRemoteObject> &object)
{
    SHARING_LOGD("trace.");
    if (object == nullptr) {
        SHARING_LOGE("domain rpc null listener object.");
        return -1;
    }

    sptr<IDomainRpcService> peerProxy = iface_cast<IDomainRpcService>(object);
    if (peerProxy == nullptr) {
        SHARING_LOGE("convert null.");
        return -1;
    }

    std::string peerDevId = IPCSkeleton::GetCallingDeviceID();
    std::unique_lock<std::mutex> lock(mutex_);
    peerProxys_[peerDevId] = peerProxy;
    lock.unlock();
    SHARING_LOGD("peer deviceId: %{public}s, listener num: %{public}zu.", GetAnonyString(peerDevId).c_str(),
                 peerProxys_.size());

    auto deathRecipient = new (std::nothrow) DomainRpcDeathRecipient(peerDevId);
    deathRecipients_[peerDevId] = deathRecipient;
    CreateDeathListener(peerDevId);

    if (peerProxy->AsObject() != nullptr) {
        if (!peerProxy->AsObject()->AddDeathRecipient(deathRecipient)) {
            SHARING_LOGE("add death recipient failed.");
            return -1;
        }
    }

    DomainRpcManager::GetInstance()->AddRpcClient(peerDevId, peerProxy);
    return ERR_NONE;
}

int32_t DomainRpcServiceStub::DoRpcCommand(MessageParcel &data, MessageParcel &reply)
{
    SHARING_LOGD("trace.");
    std::shared_ptr<BaseDomainMsg> msg = nullptr;
    IpcMsgDecoder::GetInstance().DomainMsgDecode(msg, data);
    if (msg == nullptr) {
        SHARING_LOGE("msg null.");
        return -1;
    }

    std::shared_ptr<BaseDomainMsg> replyMsg = std::make_shared<BaseDomainMsg>();
    DoRpcCommand(msg, replyMsg);
    return 0;
}

int32_t DomainRpcServiceStub::GetSystemAbility(MessageParcel &data, MessageParcel &reply)
{
    SHARING_LOGD("trace.");
    int32_t subtype = data.ReadInt32();
    SHARING_LOGD("subtype: %{public}d.", subtype);
    (void)reply.WriteRemoteObject(GetSubSystemAbility(subtype));

    return ERR_NONE;
}

int32_t DomainRpcServiceStub::DoRpcCommand(std::shared_ptr<BaseDomainMsg> msg, std::shared_ptr<BaseDomainMsg> replyMsg)
{
    SHARING_LOGD("traces.");
    auto listener = stubListener_.lock();
    if (listener) {
        listener->OnDomainRequest(msg);
    } else {
        SHARING_LOGE("stub listener is null.");
    }

    return 0;
}

sptr<IRemoteObject> DomainRpcServiceStub::GetSubSystemAbility(int32_t type)
{
    SHARING_LOGD("trace.");
    return nullptr;
}

void DomainRpcServiceStub::CreateDeathListener(std::string key)
{
    SHARING_LOGD("trace.");
}

} // namespace Sharing
} // namespace OHOS
