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

#ifndef OHOS_SHARING_IPC_MSG_ADAPTER_DEF_H
#define OHOS_SHARING_IPC_MSG_ADAPTER_DEF_H

#include "inter_ipc_stub.h"

namespace OHOS {
namespace Sharing {
    
class MsgAdapterListener {
public:
    using Ptr = std::shared_ptr<MsgAdapterListener>;

    MsgAdapterListener() = default;
    virtual ~MsgAdapterListener() = default;

    virtual void OnRemoteDied() = 0;
    virtual void OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) = 0;
};

class MsgAdapter {
public:
    using Ptr = std::shared_ptr<MsgAdapter>;

    MsgAdapter() = default;
    virtual ~MsgAdapter() = default;

    int32_t SetListener(const std::weak_ptr<MsgAdapterListener> &listener)
    {
        SHARING_LOGD("trace.");
        listener_ = listener;
        return 0;
    }

public:
    virtual int32_t OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) = 0;
    virtual int32_t SendRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) = 0;

protected:
    std::weak_ptr<MsgAdapterListener> listener_;
};

class IpcMsgAdapter : public MsgAdapter,
                      public IInterIpcStubListener,
                      public std::enable_shared_from_this<IpcMsgAdapter> {
public:
    using Ptr = std::shared_ptr<IpcMsgAdapter>;

    IpcMsgAdapter() = default;
    ~IpcMsgAdapter() override;

    int32_t OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) override;
    int32_t SendRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) override;

    void OnRemoteDied() override;
    void OnIpcRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply) override;

    void SetPeerProxy(sptr<IInterIpc> proxy);
    void SetLocalStub(sptr<InterIpcStub> stub);

    std::string GetCallingKey();
    void SetCallingKey(std::string key);

    int32_t GetPeerPid();
    void SetPeerPid(int32_t pid);

protected:
    std::string key_;
    int32_t peerPid_ = 0;
    sptr<IInterIpc> peerProxy_ = nullptr;
    sptr<InterIpcStub> localStub_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS

#endif