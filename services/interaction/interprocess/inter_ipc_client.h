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

#ifndef OHOS_SHARING_SHARING_DOMAIN_RPC_CLIENT_H
#define OHOS_SHARING_SHARING_DOMAIN_RPC_CLIENT_H

#include <cstdint>
#include "common/identifier.h"
#include "inter_ipc_client_stub.h"
#include "inter_ipc_death_recipient.h"
#include "inter_ipc_proxy.h"
#include "interaction/interprocess/ipc_msg_adapter.h"

namespace OHOS {
namespace Sharing {

class InterIpcClient final : public IdentifierInfo,
                             public InterIpcDeathListener,
                             public std::enable_shared_from_this<InterIpcClient> {
public:
    explicit InterIpcClient();
    ~InterIpcClient() override {};

    // rpc client
    int32_t CreateListenerObject();
    void OnRemoteDied(std::string key) final;
    void Initialize(sptr<IInterIpc> standardProxy = nullptr);

    sptr<IInterIpc> GetSharingProxy();
    sptr<IInterIpc> GetSubProxy(std::string key, std::string className);
    std::shared_ptr<InterIpcClient> CreateSubService(std::string key, std::string clientClassName,
                                                     std::string serverClassName);

    void SetKey(std::string key);
    std::shared_ptr<IpcMsgAdapter> GetMsgAdapter();
    int32_t Send(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply);

private:
    void Reset();

private:
    std::string key_;
    std::mutex mutex_;
    std::shared_ptr<IpcMsgAdapter> msgAdapter_ = nullptr;

    sptr<IInterIpc> domainProxy_ = nullptr;
    sptr<InterIpcClientStub> listenerStub_ = nullptr;
    sptr<InterIpcDeathRecipient> deathRecipient_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif