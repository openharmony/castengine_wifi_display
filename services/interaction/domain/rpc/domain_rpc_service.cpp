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

#include "domain_rpc_service.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "domain_rpc_service_death_listener.h"
#include "hap_token_info.h"
#include "interaction/domain/domain_manager.h"
#include "interaction/domain/rpc/domain_rpc_manager.h"
#include "ipc_skeleton.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace Sharing {
REGISTER_SYSTEM_ABILITY_BY_ID(DomainRpcService, SHARING_SERVICE_DOMAIN_TEMP_SA_ID, true)

DomainRpcService::DomainRpcService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate), IDomainPeer(REMOTE_CALLER_SERVER)
{
    SHARING_LOGD("trace.");
}

DomainRpcService::~DomainRpcService()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex_);
    DomainRpcStubs_.clear();
    shared_from_this_.reset();
}

void DomainRpcService::OnDump()
{
    SHARING_LOGD("trace.");
}

void DomainRpcService::OnStart()
{
    SHARING_LOGD("trace.");
    if (Publish(this)) {
        SHARING_LOGD("success.");
        DomainRpcManager::GetInstance()->Init();
        DomainRpcManager::GetInstance()->AddDomainRpcService(this);
    } else {
        SHARING_LOGD("start failed.");
    }
}

void DomainRpcService::OnStop()
{
    SHARING_LOGD("trace.");
}

sptr<IRemoteObject> DomainRpcService::GetSubSystemAbility(int32_t type)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(mutex_);
    sptr<IRemoteObject> peerObject = IPCSkeleton::GetContextObject();
    sptr<DomainRpcServiceStub> stub = new (std::nothrow) DomainRpcServiceStub();
    if (stub == nullptr) {
        SHARING_LOGE("stub null");
        return nullptr;
    }

    sptr<IRemoteObject> object = stub->AsObject();
    std::string remoteId = IPCSkeleton::GetCallingDeviceID();
    DomainRpcStubs_.insert(std::make_pair(remoteId, object));

    return object;
}

int32_t DomainRpcService::DoRpcCommand(std::shared_ptr<BaseDomainMsg> msg, std::shared_ptr<BaseDomainMsg> replyMsg)
{
    SHARING_LOGD("msg from %{public}s -> to %{public}s.", GetAnonyString(msg->fromDevId).c_str(),
                 GetAnonyString(msg->toDevId).c_str());
    auto listener = peerListener_.lock();
    if (listener) {
        listener->OnDomainRequest(msg->fromDevId, msg);
    } else {
        SHARING_LOGE("peer listener is null!");
    }

    return 0;
}

void DomainRpcService::CreateDeathListener(std::string deviceId)
{
    SHARING_LOGD("deviceId: %{public}s.", GetAnonyString(deviceId).c_str());
    if (deathRecipients_.find(deviceId) != deathRecipients_.end()) {
        auto listener = std::make_shared<DomainRpcServiceDeathListener>();
        if (shared_from_this_ == nullptr) {
            shared_from_this_ = Ptr(this, [](DomainRpcService *) { SHARING_LOGD("trace."); });
        }
        listener->SetService(shared_from_this_);
        deathRecipients_[deviceId]->SetDeathListener(listener);
    } else {
        SHARING_LOGE("deviceId not find: %{public}s.", GetAnonyString(deviceId).c_str());
    }
}

void DomainRpcService::SetPeerListener(std::weak_ptr<IDomainPeerListener> listener)
{
    SHARING_LOGD("trace.");
    peerListener_ = listener;
}

int32_t DomainRpcService::SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> msg)
{
    SHARING_LOGD("msg from %{public}s -> to %{public}s.", GetAnonyString(msg->fromDevId).c_str(),
                 GetAnonyString(msg->toDevId).c_str());
    return 0;
}

void DomainRpcService::DelPeerProxy(std::string remoteId)
{
    SHARING_LOGD("remoteId: %{public}s.", remoteId.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (peerProxys_.find(remoteId) != peerProxys_.end()) {
        peerProxys_[remoteId]->AsObject()->RemoveDeathRecipient(deathRecipients_[remoteId]);
        peerProxys_.erase(remoteId);
        deathRecipients_.erase(remoteId);
        auto listener = peerListener_.lock();
        if (listener) {
            listener->OnPeerDisconnected(remoteId);
        }
    } else {
        SHARING_LOGE("remoteId: %{public}s not find.", remoteId.c_str());
    }
}

} // namespace Sharing
} // namespace OHOS
