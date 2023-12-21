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

#ifndef OHOS_SHARING_DOMAIN_RPC_MANAGER_H
#define OHOS_SHARING_DOMAIN_RPC_MANAGER_H

#include <mutex>
#include "domain_def.h"
#include "domain_rpc_client.h"
#include "domain_rpc_service.h"
#include "singleton.h"

namespace OHOS {
namespace Sharing {
    
class DomainRpcManager final : public ITransmitMgr,
                               public DelayedSingleton<DomainRpcManager>,
                               public std::enable_shared_from_this<DomainRpcManager>{
    DECLARE_DELAYED_SINGLETON(DomainRpcManager);
public:
    //impl ITransmitMgr
    bool IsPeerExist(std::string peerId) override;
    int32_t SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) override;

    int32_t DelRpcClient(std::string remoteId);
    int32_t AddRpcClient(std::string remoteId);
    int32_t AddDomainRpcService(DomainRpcService* service);
    int32_t AddRpcClient(std::string remoteId, sptr<IDomainRpcService> peerProxy);

    void OnPeerConnected(std::string remoteId) final;
    void OnPeerDisconnected(std::string remoteId) final;
    void OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) final;
    
    int32_t Init();
    int32_t DeInit();

private:
    int32_t ClearRpcClient();

private:
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<DomainRpcClient>> rpcClients_;
    
    DomainRpcService *localService_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif