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

#ifndef OHOS_SHARING_DOMAIN_RPC_SERVICE_H
#define OHOS_SHARING_DOMAIN_RPC_SERVICE_H

#include <mutex>
#include <unordered_map>
#include "domain_def.h"
#include "domain_rpc_service_stub.h"
#include "system_ability.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Sharing {

class DomainRpcService final : public SystemAbility,
                               public DomainRpcServiceStub,
                               public IDomainPeer,
                               public std::enable_shared_from_this<DomainRpcService> {
    DECLARE_SYSTEM_ABILITY(DomainRpcService);

public:
    using Ptr = std::shared_ptr<DomainRpcService>;
    explicit DomainRpcService(int32_t systemAbilityId, bool runOnCreate = true);
    ~DomainRpcService() override;

    void DelPeerProxy(std::string remoteId);
    void CreateDeathListener(std::string deviceId) final;
    void SetPeerListener(std::weak_ptr<IDomainPeerListener> listener) final;

    int32_t SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> msg) final;
    int32_t DoRpcCommand(std::shared_ptr<BaseDomainMsg> msg, std::shared_ptr<BaseDomainMsg> replyMsg) final;

    sptr<IRemoteObject> GetSubSystemAbility(int32_t subtype) final;

protected:
    void OnDump() final;
    void OnStop() final;
    void OnStart() final;

private:
    std::mutex mutex_;
    Ptr shared_from_this_ = nullptr;
    std::unordered_map<std::string, sptr<IRemoteObject>> DomainRpcStubs_;
};

} // namespace Sharing
} // namespace OHOS
#endif
