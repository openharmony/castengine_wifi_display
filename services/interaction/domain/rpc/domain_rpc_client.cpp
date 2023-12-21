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

#include "domain_rpc_client.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Sharing {

DomainRpcClient::DomainRpcClient() : IDomainPeer(REMOTE_CALLER_CLIENT)
{
    SHARING_LOGD("trace.");
    domainType_ = DOMAIN_TYPE_RPC;
}

void DomainRpcClient::OnRemoteDied(std::string deviceId)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (domainProxy_ != nullptr) {
        (void)domainProxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        domainProxy_ = nullptr;
    }

    auto listener = peerListener_.lock();
    if (listener != nullptr) {
        listener->OnPeerDisconnected(remoteId_);
    }

    if (deathRecipient_ != nullptr) {
        deathRecipient_ = nullptr;
    }

    listenerStub_ = nullptr;
    remoteId_ = "";
}

sptr<IDomainRpcService> DomainRpcClient::GetDomainProxy(std::string deviceId)
{
    SHARING_LOGD("trace.");
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        SHARING_LOGE("get system ability manager failed.");
        return nullptr;
    }

    sptr<IRemoteObject> object = saMgr->GetSystemAbility(SHARING_SERVICE_DOMAIN_TEMP_SA_ID, deviceId);
    if (object == nullptr) {
        SHARING_LOGE("get system ability failed id: %{public}d.", SHARING_SERVICE_DOMAIN_TEMP_SA_ID);
        return nullptr;
    } else {
        SHARING_LOGD("GetSystemAbility success.");
        remoteId_ = deviceId;
    }

    sptr<IDomainRpcService> proxy = iface_cast<IDomainRpcService>(object);
    if (proxy == nullptr) {
        SHARING_LOGE("iface_cast IDomainRpcService failed id: %{public}d.", SHARING_SERVICE_DOMAIN_TEMP_SA_ID);
        return nullptr;
    } else {
        SHARING_LOGD("get domain proxy iface_cast success.");
    }

    deathRecipient_ = new (std::nothrow) DomainRpcDeathRecipient(deviceId);
    deathRecipient_->SetDeathListener(shared_from_this());
    if (!object->AddDeathRecipient(deathRecipient_)) {
        SHARING_LOGE("add IDomainRpcService death recipient failed.");
        return nullptr;
    } else {
        SHARING_LOGD("get domain AddDeathRecipient success.");
    }

    auto listener = peerListener_.lock();
    if (listener) {
        listener->OnPeerConnected(deviceId);
    }

    domainProxy_ = proxy;
    SHARING_LOGD("get domain proxy success.");
    return proxy;
}

void DomainRpcClient::SetDomainProxy(sptr<IDomainRpcService> peerProxy)
{
    SHARING_LOGD("trace.");
    domainProxy_ = peerProxy;
}

sptr<IDomainRpcService> DomainRpcClient::GetSubProxy(int32_t type)
{
    SHARING_LOGD("trace.");
    if (domainProxy_ == nullptr) {
        SHARING_LOGE("get subsystem ability '%{public}d' proxy null.", type);
        return nullptr;
    }

    sptr<IRemoteObject> object = domainProxy_->GetSubSystemAbility(type);
    if (!object) {
        SHARING_LOGE("get subsystem ability type '%{public}d' not exist.", type);
        return nullptr;
    }

    sptr<IDomainRpcService> subProxy = iface_cast<IDomainRpcService>(object);
    if (!subProxy) {
        SHARING_LOGE("get subsystem ability '%{public}d' iface_cast null.", type);
        return nullptr;
    }

    return subProxy;
}

int32_t DomainRpcClient::CreateListenerObject()
{
    SHARING_LOGD("trace.");
    if (domainProxy_ == nullptr) {
        SHARING_LOGE("proxy null.");
        return -1;
    }

    listenerStub_ = new (std::nothrow) DomainRpcServiceStub();
    listenerStub_->SetStubListener(shared_from_this());
    sptr<IRemoteObject> object = listenerStub_->AsObject();

    return domainProxy_->SetListenerObject(object);
}

void DomainRpcClient::Initialize()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (domainProxy_) {
        CreateListenerObject();
    } else {
        SHARING_LOGE("new StandardClient proxy null.");
    }
}

void DomainRpcClient::SetPeerListener(std::weak_ptr<IDomainPeerListener> listener)
{
    SHARING_LOGD("trace.");
    peerListener_ = listener;
}

int32_t DomainRpcClient::SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg)
{
    SHARING_LOGD("trace.");
    std::shared_ptr<BaseDomainMsg> replyMsg = std::make_shared<BaseDomainMsg>();
    if (domainProxy_ != nullptr) {
        domainProxy_->DoRpcCommand(BaseMsg, replyMsg);
    } else {
        SHARING_LOGE("domain proxy is null.");
    }

    return 0;
}

void DomainRpcClient::OnDomainRequest(std::shared_ptr<BaseDomainMsg> msg)
{
    SHARING_LOGD("trace.");
    auto listener = peerListener_.lock();
    if (listener != nullptr) {
        listener->OnDomainRequest(remoteId_, msg);
    } else {
        SHARING_LOGE("peer listener is null.");
    }
}

} // namespace Sharing
} // namespace OHOS
