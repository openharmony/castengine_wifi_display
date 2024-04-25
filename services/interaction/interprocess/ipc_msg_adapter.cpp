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

#include "ipc_msg_adapter.h"

namespace OHOS {
namespace Sharing {

IpcMsgAdapter::~IpcMsgAdapter()
{
    SHARING_LOGD("trace.");
    peerProxy_ = nullptr;
    localStub_ = nullptr;
}

int32_t IpcMsgAdapter::SendRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");
    if (peerProxy_ != nullptr) {
        peerProxy_->DoIpcCommand(msg, reply);
    } else {
        SHARING_LOGE("peerProxy_ is nullptr.");
    }

    return 0;
}

int32_t IpcMsgAdapter::OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");

    auto listener = listener_.lock();
    if (listener) {
        listener->OnRequest(msg, reply);
    } else {
        SHARING_LOGE("listener_ is nullptr.");
    }

    return 0;
}

void IpcMsgAdapter::SetLocalStub(sptr<InterIpcStub> stub)
{
    SHARING_LOGD("trace.");
    localStub_ = stub;
    if (localStub_) {
        localStub_->SetStubListener(shared_from_this());
    }
}

void IpcMsgAdapter::SetPeerProxy(sptr<IInterIpc> proxy)
{
    SHARING_LOGD("trace.");
    peerProxy_ = proxy;
}

void IpcMsgAdapter::OnIpcRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
{
    SHARING_LOGD("trace.");
    OnRequest(msg, reply);
}

void IpcMsgAdapter::OnRemoteDied()
{
    SHARING_LOGD("on Adapter IpcStub remote died!");
    auto listener = listener_.lock();
    if (listener) {
        listener->OnRemoteDied();
    }
}

void IpcMsgAdapter::SetCallingKey(std::string key)
{
    SHARING_LOGD("trace.");
    key_ = key;
}

std::string IpcMsgAdapter::GetCallingKey()
{
    SHARING_LOGD("trace.");
    return key_;
}

void IpcMsgAdapter::SetPeerPid(int32_t pid)
{
    SHARING_LOGD("trace.");
    peerPid_ = pid;
}

int32_t IpcMsgAdapter::GetPeerPid()
{
    SHARING_LOGD("trace.");
    return peerPid_;
}

} // namespace Sharing
} // namespace OHOS