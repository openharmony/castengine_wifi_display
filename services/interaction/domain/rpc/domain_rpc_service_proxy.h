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

#ifndef OHOS_SHARING_DOMAIN_RPC_SERVICE_PROXY_H
#define OHOS_SHARING_DOMAIN_RPC_SERVICE_PROXY_H

#include "i_domain_rpc_service.h"
#include "ipc_msg.h"

namespace OHOS {
namespace Sharing {
    
class DomainRpcServiceProxy : public IRemoteProxy<IDomainRpcService> {
public:
    explicit DomainRpcServiceProxy(const sptr<IRemoteObject> &impl);
    virtual ~DomainRpcServiceProxy() = default;

    int32_t SetListenerObject(const sptr<IRemoteObject> &object) override;
    int32_t DoRpcCommand(std::shared_ptr<BaseDomainMsg> msg, std::shared_ptr<BaseDomainMsg> replyMsg) override;

    sptr<IRemoteObject> GetSubSystemAbility(int32_t type) override;

private:
    static inline BrokerDelegator<DomainRpcServiceProxy> delegator_;
};

} // namespace Sharing
} // namespace OHOS
#endif