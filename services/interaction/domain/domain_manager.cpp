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

#include "domain_manager.h"
#include "common/common_macro.h"
#include "common/sharing_log.h"
#include "interaction/domain/rpc/domain_rpc_manager.h"
#include "interaction/interaction_manager.h"
#include "interaction/ipc_codec/ipc_msg.h"

namespace OHOS {
namespace Sharing {

DomainManager::DomainManager()
{
    SHARING_LOGD("trace.");
    DmKit::InitDeviceManager();
    localDeviceInfo_ = DmKit::GetLocalDevicesInfo();
    trustedDeviceInfos_ = DmKit::GetTrustedDevicesInfo();
}

DomainManager::~DomainManager()
{
    SHARING_LOGD("trace.");
    std::unique_lock lock(mutex_);
    transmitMgrs_.clear();
    peerTypeMap_.clear();
}

int32_t DomainManager::SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(BaseMsg);
    auto mgr = FindMgrByRemoteId(BaseMsg->toDevId);
    if (mgr != nullptr) {
        SHARING_LOGD("mgr exist.");
        mgr->SendDomainRequest(BaseMsg->toDevId, BaseMsg);
    } else {
        if (!BaseMsg->toDevId.empty()) {
            SHARING_LOGD("AddRpcClient.");
            // first send  need  to create  RPC client
            DomainRpcManager::GetInstance()->AddRpcClient(BaseMsg->toDevId);
            DomainRpcManager::GetInstance()->SendDomainRequest(BaseMsg->toDevId, BaseMsg);
        } else {
            SHARING_LOGE("remote device is not exist.");
        }
    }

    return 0;
}

void DomainManager::OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg)
{
    SHARING_LOGD("redirect to interactionMgr.");
    RETURN_IF_NULL(BaseMsg);
    auto listener = listener_.lock();
    if (listener != nullptr) {
        listener->OnDomainMsg(BaseMsg);
    } else {
        SHARING_LOGE("listener is null.");
    }
}

int32_t DomainManager::AddServiceManager(std::shared_ptr<ITransmitMgr> mgr)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(mgr);
    std::unique_lock lock(mutex_);
    mgr->SetListener(shared_from_this());
    transmitMgrs_.insert(std::make_pair(mgr->GetDomainType(), mgr));

    return 0;
}

int32_t DomainManager::AddPeer(std::shared_ptr<ITransmitMgr> mgr, std::shared_ptr<IDomainPeer> caller)
{
    RETURN_INVALID_IF_NULL(mgr);
    RETURN_INVALID_IF_NULL(caller);
    SHARING_LOGD("trace, remoteId: %{public}s.", caller->GetRemoteId().c_str());
    std::unique_lock lock(mutex_);
    peerTypeMap_.insert(std::make_pair(caller->GetRemoteId(), caller->GetDomainType()));

    return 0;
}

int32_t DomainManager::DelPeer(std::string remoteId)
{
    SHARING_LOGD("trace, remoteId: %{public}s.", remoteId.c_str());
    std::unique_lock lock(mutex_);
    if (peerTypeMap_.find(remoteId) != peerTypeMap_.end()) {
        peerTypeMap_.erase(remoteId);
    } else {
        SHARING_LOGE("remoteId is not exist.");
        return -1;
    }

    return 0;
}

std::shared_ptr<ITransmitMgr> DomainManager::FindMgrByRemoteId(std::string remoteId)
{
    SHARING_LOGD("trace, peerTypeMap size: %{public}zu.", peerTypeMap_.size());
    std::unique_lock lock(mutex_);
    auto iter = peerTypeMap_.find(remoteId);
    if (iter != peerTypeMap_.end()) {
        auto mgrIter = transmitMgrs_.find(iter->second);
        if (mgrIter != transmitMgrs_.end()) {
            return mgrIter->second;
        } else {
            SHARING_LOGE("mgr is null.");
        }
    } else {
        SHARING_LOGE("remoteId is not exist.");
    }

    return nullptr;
}

void DomainManager::SetListener(DomainManagerListener::Ptr listener)
{
    listener_ = listener;
}

} // namespace Sharing
} // namespace OHOS