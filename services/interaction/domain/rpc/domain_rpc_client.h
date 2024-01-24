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

#ifndef OHOS_SHARING_DOMAIN_RPC_CLIENT_H
#define OHOS_SHARING_DOMAIN_RPC_CLIENT_H

#include <cstdint>
#include "domain_def.h"
#include "domain_rpc_death_recipient.h"
#include "common/identifier.h"
#include "domain_rpc_service_stub.h"
#include "domain_rpc_service_proxy.h"

namespace OHOS {
namespace Sharing {
    
class DomainRpcClient final : public IDomainPeer,
                              public IdentifierInfo,
                              public DomainRpcDeathListener,
                              public IDomainStubListener,
                              public std::enable_shared_from_this<DomainRpcClient> {
public:
    explicit DomainRpcClient();
    ~DomainRpcClient() override {};

    //rpc client
    void Initialize();
    void OnRemoteDied(std::string deviceId) final;
    void OnDomainRequest(std::shared_ptr<BaseDomainMsg> msg) final;
    int32_t CreateListenerObject();

    void SetDomainProxy(sptr<IDomainRpcService> peerProxy);
    sptr<IDomainRpcService> GetSubProxy(int32_t type = 0);
    sptr<IDomainRpcService> GetDomainProxy(std::string deviceId);

    //peer client
    void SetPeerListener(std::weak_ptr<IDomainPeerListener> listener) final;
    int32_t SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) final;

private:
    void Reset();

private:
    std::mutex mutex_;
    sptr<IDomainRpcService> domainProxy_ = nullptr;
    sptr<DomainRpcServiceStub> listenerStub_ = nullptr;
    sptr<DomainRpcDeathRecipient> deathRecipient_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
