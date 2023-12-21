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

#include "inter_ipc_client_stub.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Sharing {

InterIpcClientStub::InterIpcClientStub()
{
    SHARING_LOGD("trace.");
}

InterIpcClientStub::~InterIpcClientStub()
{
    SHARING_LOGD("trace.");
}

int32_t InterIpcClientStub::SetListenerObject(std::string key, const sptr<IRemoteObject> &object)
{
    SHARING_LOGD("trace.");
    if (object == nullptr) {
        SHARING_LOGE("domain rpc null listener object.");
        return -1;
    }

    sptr<IInterIpc> peerProxy = iface_cast<IInterIpc>(object);
    if (peerProxy == nullptr) {
        SHARING_LOGE("convert null.");
        return -1;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    peerProxys_[key] = peerProxy;
    lock.unlock();
    SHARING_LOGD("peer key: %{public}s, listener num: %{public}zu.", key.c_str(), peerProxys_.size());

    auto deathRecipient = new (std::nothrow) InterIpcDeathRecipient(key);
    deathRecipients_[key] = deathRecipient;
    CreateDeathListener(key);

    if (peerProxy->AsObject() != nullptr) {
        if (!peerProxy->AsObject()->AddDeathRecipient(deathRecipient)) {
            SHARING_LOGE("add death recipient failed.");
            return -1;
        }
    }

    return ERR_NONE;
}

int32_t InterIpcClientStub::DoIpcCommand(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &replyMsg)
{
    SHARING_LOGD("traces.");
    auto listener = stubListener_.lock();
    if (listener) {
        listener->OnIpcRequest(msg, replyMsg);
    } else {
        SHARING_LOGE("stub listener is null!");
    }

    return 0;
}

void InterIpcClientStub::OnRemoteDied()
{
    SHARING_LOGD("trace.");
    auto listener = stubListener_.lock();
    if (listener) {
        listener->OnRemoteDied();
    }
}

} // namespace Sharing
} // namespace OHOS