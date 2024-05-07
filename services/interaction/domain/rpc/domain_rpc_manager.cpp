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

#include "domain_rpc_manager.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "hap_token_info.h"
#include "interaction/device_kit/dm_kit.h"
#include "interaction/domain/domain_manager.h"
#include "interaction/ipc_codec/ipc_msg.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace Sharing {

DomainRpcManager::DomainRpcManager()
{
    SHARING_LOGD("trace.");
    domainType_ = DOMAIN_TYPE_RPC;
}

DomainRpcManager::~DomainRpcManager()
{
    SHARING_LOGD("trace.");
    DeInit();
}

int32_t DomainRpcManager::SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg)
{
    SHARING_LOGD("trace.");
    BaseMsg->fromDevId = DmKit::GetLocalDevicesInfo().deviceId;
    SHARING_LOGD("msg from %{public}s -> to %{public}s.", GetAnonyString(BaseMsg->fromDevId).c_str(),
                 GetAnonyString(BaseMsg->toDevId).c_str());

    if (rpcClients_.find(BaseMsg->toDevId) != rpcClients_.end()) {
        rpcClients_[BaseMsg->toDevId]->SendDomainRequest(remoteId, BaseMsg);
    } else {
        SHARING_LOGE("rpc client is null.");
    }

    return 0;
}

bool DomainRpcManager::IsPeerExist(std::string peerId)
{
    SHARING_LOGD("trace.");
    if (rpcClients_.find(peerId) != rpcClients_.end()) {
        return true;
    }

    return false;
}

int32_t DomainRpcManager::AddDomainRpcService(DomainRpcService *service)
{
    SHARING_LOGD("trace.");
    localService_ = service;
    localService_->SetPeerListener(shared_from_this());
    DomainManager::GetInstance()->AddServiceManager(shared_from_this());
    return 0;
}

int32_t DomainRpcManager::Init()
{
    SHARING_LOGD("trace.");
    return 0;
}

int32_t DomainRpcManager::DeInit()
{
    SHARING_LOGD("trace.");
    ClearRpcClient();
    std::unique_lock lock(mutex_);
    localService_ = nullptr;
    return 0;
}

int32_t DomainRpcManager::AddRpcClient(std::string remoteId)
{
    SHARING_LOGD("remoteId: %{public}s.", remoteId.c_str());
    std::unique_lock lock(mutex_);
    std::shared_ptr<DomainRpcClient> client = std::make_shared<DomainRpcClient>();
    if (client->GetDomainProxy(remoteId) == nullptr) {
        SHARING_LOGE("failed.");
        return -1;
    }

    client->Initialize();
    client->SetPeerListener(shared_from_this());
    rpcClients_.insert(std::make_pair(remoteId, client));

    auto listener = transMgrListener_.lock();
    if (listener != nullptr) {
        listener->AddPeer(shared_from_this(), client);
    }

    return 0;
}

int32_t DomainRpcManager::AddRpcClient(std::string remoteId, sptr<IDomainRpcService> peerProxy)
{
    SHARING_LOGD("remoteId: %{public}s.", remoteId.c_str());
    std::unique_lock lock(mutex_);
    std::shared_ptr<DomainRpcClient> client = std::make_shared<DomainRpcClient>();
    client->SetDomainProxy(peerProxy);
    client->SetPeerListener(shared_from_this());
    client->SetRemoteId(remoteId);
    rpcClients_.insert(std::make_pair(remoteId, client));

    auto listener = transMgrListener_.lock();
    if (listener != nullptr) {
        listener->AddPeer(shared_from_this(), client);
    }

    return 0;
}

int32_t DomainRpcManager::DelRpcClient(std::string remoteId)
{
    SHARING_LOGD("remoteId: %{public}s.", remoteId.c_str());
    std::unique_lock lock(mutex_);
    if (rpcClients_.find(remoteId) != rpcClients_.end()) {
        rpcClients_.erase(remoteId);
    }

    auto listener = transMgrListener_.lock();
    if (listener != nullptr) {
        listener->DelPeer(remoteId);
    }

    return 0;
}

int32_t DomainRpcManager::ClearRpcClient()
{
    SHARING_LOGD("trace.");
    std::unique_lock lock(mutex_);
    rpcClients_.clear();
    return 0;
}

void DomainRpcManager::OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg)
{
    SHARING_LOGD("trace.");
    auto listener = transMgrListener_.lock();
    if (listener) {
        listener->OnDomainRequest(remoteId, BaseMsg);
    } else {
        SHARING_LOGE("peer listener is null.");
    }
}

void DomainRpcManager::OnPeerConnected(std::string remoteId)
{
    SHARING_LOGI("remoteId: %{public}s.", remoteId.c_str());
}

void DomainRpcManager::OnPeerDisconnected(std::string remoteId)
{
    SHARING_LOGI("remoteId: %{public}s.", remoteId.c_str());
    DelRpcClient(remoteId);
}

} // namespace Sharing
} // namespace OHOS