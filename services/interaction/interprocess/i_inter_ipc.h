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

#ifndef OHOS_SHARING_IINTER_IPC_INTERFACE_SERVICE_H
#define OHOS_SHARING_IINTER_IPC_INTERFACE_SERVICE_H

#include "ipc_msg.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Sharing {
    
class IInterIpc : public IRemoteBroker {
public:
    enum InterIpcMsg { INTER_IPC_MSG, SET_LISTENER_OBJ, GET_SUBSYSTEM };

    virtual ~IInterIpc() = default;
    
    virtual int32_t SetListenerObject(std::string key, const sptr<IRemoteObject> &object) = 0;
    virtual int32_t DoIpcCommand(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &replyMsg) = 0;

    virtual sptr<IRemoteObject> GetSubSystemAbility(std::string key, std::string className) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.multimedia.IInterIpc");
};

} // namespace Sharing
} // namespace OHOS
#endif