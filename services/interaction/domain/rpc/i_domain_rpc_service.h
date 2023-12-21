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

#ifndef OHOS_SHARING_IDOMAIN_RPC_SHARING_SERVICE_H
#define OHOS_SHARING_IDOMAIN_RPC_SHARING_SERVICE_H

#include "ipc_msg.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Sharing {
    
class IDomainRpcService : public IRemoteBroker {
public:
    enum DomainServiceMsg { DOMAIN_MSG, SET_LISTENER_OBJ, GET_SUBSYSTEM };

    virtual ~IDomainRpcService() = default;

    virtual int32_t SetListenerObject(const sptr<IRemoteObject> &object) = 0;
    virtual int32_t DoRpcCommand(std::shared_ptr<BaseDomainMsg> msg, std::shared_ptr<BaseDomainMsg> replyMsg) = 0;

    virtual sptr<IRemoteObject> GetSubSystemAbility(int32_t type) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.multimedia.IDomainRpcService");
};

} // namespace Sharing
} // namespace OHOS
#endif