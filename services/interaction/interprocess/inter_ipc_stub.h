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

#ifndef OHOS_SHARING_INTER_IPC_INTERFACE_STUB_H
#define OHOS_SHARING_INTER_IPC_INTERFACE_STUB_H

#include <unordered_map>
#include <vector>
#include "i_inter_ipc.h"
#include "inter_ipc_death_recipient.h"

namespace OHOS {
namespace Sharing {

class IInterIpcStubListener {
public:
    virtual ~IInterIpcStubListener() = default;

    virtual void OnRemoteDied() = 0;
    virtual void OnIpcRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) = 0;
};

class InterIpcStubDeathListener;
class InterIpcStub : public IRemoteStub<IInterIpc>,
                     public NoCopyable {
public:
    using Ptr = std::shared_ptr<InterIpcStub>;
    InterIpcStub();
    virtual ~InterIpcStub();

    virtual void OnRemoteDied();
    virtual void SetStubListener(std::weak_ptr<IInterIpcStubListener> listener);
    virtual int32_t SendIpcRequest(std::string tokenId, std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply);

    int32_t SetListenerObject(std::string key, const sptr<IRemoteObject> &object) override;
    int32_t DoIpcCommand(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &replyMsg) override;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    sptr<IRemoteObject> GetSubSystemAbility(std::string key, std::string className) override;

protected:
    virtual void DelPeerProxy(std::string key);
    virtual void CreateDeathListener(std::string key);

    int32_t DoIpcCommand(MessageParcel &data, MessageParcel &reply);
    int32_t GetSystemAbility(MessageParcel &data, MessageParcel &reply);
    int32_t SetListenerObject(MessageParcel &data, MessageParcel &reply);

protected:
    std::mutex mutex_;
    Ptr sharedFromThis_ = nullptr;
    std::weak_ptr<IInterIpcStubListener> stubListener_;
    std::unordered_map<std::string, sptr<IInterIpc>> peerProxys_;
    std::unordered_map<std::string, sptr<InterIpcDeathRecipient>> deathRecipients_;
};

} // namespace Sharing
} // namespace OHOS
#endif