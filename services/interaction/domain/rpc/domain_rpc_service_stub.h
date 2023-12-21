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

#ifndef OHOS_SHARING_DOMAIN_RPC_SERVICE_STUB_H
#define OHOS_SHARING_DOMAIN_RPC_SERVICE_STUB_H

#include <unordered_map>
#include <vector>
#include "domain_rpc_death_recipient.h"
#include "i_domain_rpc_service.h"

namespace OHOS {
namespace Sharing {
    
class IDomainStubListener {
public:
    virtual ~IDomainStubListener() = default;
    virtual void OnDomainRequest(std::shared_ptr<BaseDomainMsg> msg) = 0;
};

class DomainRpcServiceStub : public IRemoteStub<IDomainRpcService>,
                             public NoCopyable {
public:
    virtual ~DomainRpcServiceStub();

    sptr<IRemoteObject> GetSubSystemAbility(int32_t type) override;
    void SetStubListener(std::weak_ptr<IDomainStubListener> listener);
    int32_t SetListenerObject(const sptr<IRemoteObject> &object) override;
    int32_t DoRpcCommand(std::shared_ptr<BaseDomainMsg> msg, std::shared_ptr<BaseDomainMsg> replyMsg) override;
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    
protected:
    virtual void CreateDeathListener(std::string deviceId);
    int32_t DoRpcCommand(MessageParcel &data, MessageParcel &reply);
    int32_t GetSystemAbility(MessageParcel &data, MessageParcel &reply);
    int32_t SetListenerObject(MessageParcel &data, MessageParcel &reply);

protected:
    std::mutex mutex_;
    std::weak_ptr<IDomainStubListener> stubListener_;
    std::unordered_map<std::string, sptr<IDomainRpcService>> peerProxys_;
    std::unordered_map<std::string, sptr<DomainRpcDeathRecipient>> deathRecipients_;
};

} // namespace Sharing
} // namespace OHOS
#endif
