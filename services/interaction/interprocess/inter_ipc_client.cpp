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

#include "inter_ipc_client.h"
#include <string>
#include "common/reflect_registration.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Sharing {

InterIpcClient::InterIpcClient()
{
    SHARING_LOGD("trace.");
}

void InterIpcClient::OnRemoteDied(std::string key)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);

    if (domainProxy_ != nullptr) {
        (void)domainProxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        domainProxy_ = nullptr;
    }

    deathRecipient_ = nullptr;
    listenerStub_ = nullptr;

    if (msgAdapter_ != nullptr) {
        msgAdapter_->OnRemoteDied();
    }
}

void InterIpcClient::SetKey(std::string key)
{
    SHARING_LOGD("trace.");
    key_ = key;
}

sptr<IInterIpc> InterIpcClient::GetSharingProxy()
{
    SHARING_LOGD("trace.");
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        SHARING_LOGE("get system ability manager failed.");
        return nullptr;
    }

    sptr<IRemoteObject> object = saMgr->GetSystemAbility(SHARING_SERVICE_TEMP_SA_ID);
    if (object == nullptr) {
        SHARING_LOGE("get system ability failed id: %{public}d.", SHARING_SERVICE_TEMP_SA_ID);
        return nullptr;
    }

    sptr<IInterIpc> proxy = iface_cast<IInterIpc>(object);
    if (proxy == nullptr) {
        SHARING_LOGE("iface_cast IInterIpc failed id: %{public}d.", SHARING_SERVICE_TEMP_SA_ID);
        return nullptr;
    }

    deathRecipient_ = new (std::nothrow) InterIpcDeathRecipient(key_);
    deathRecipient_->SetDeathListener(shared_from_this());

    if (!object->AddDeathRecipient(deathRecipient_)) {
        SHARING_LOGE("add IInterIpc death recipient failed.");
        return nullptr;
    }
    SHARING_LOGD("get domain proxy success.");

    return proxy;
}

sptr<IInterIpc> InterIpcClient::GetSubProxy(std::string key, std::string className)
{
    SHARING_LOGD("trace.");
    if (domainProxy_ == nullptr) {
        SHARING_LOGE("get subsystem ability '%{public}s' proxy null.", className.c_str());
        return nullptr;
    }

    sptr<IRemoteObject> object = domainProxy_->GetSubSystemAbility(key, className);
    if (!object) {
        SHARING_LOGE("get subsystem ability type '%{public}s' not exist.", className.c_str());
        return nullptr;
    }

    sptr<IInterIpc> subProxy = iface_cast<IInterIpc>(object);
    if (!subProxy) {
        SHARING_LOGE("get subsystem ability '%{public}s' iface_cast null.", className.c_str());
        return nullptr;
    }

    return subProxy;
}

int32_t InterIpcClient::CreateListenerObject()
{
    SHARING_LOGD("trace.");
    if (domainProxy_ == nullptr) {
        SHARING_LOGE("proxy null.");
        return -1;
    }

    if (key_.empty()) {
        key_ = std::to_string(IPCSkeleton::GetCallingTokenID());
        SHARING_LOGI("key is empty, using token As key: %{public}s.", key_.c_str());
    }

    listenerStub_ = new (std::nothrow) InterIpcClientStub();
    sptr<IRemoteObject> object = listenerStub_->AsObject();
    msgAdapter_ = std::make_shared<IpcMsgAdapter>();
    msgAdapter_->SetLocalStub(listenerStub_);
    msgAdapter_->SetPeerProxy(domainProxy_);

    return domainProxy_->SetListenerObject(key_, object);
}

void InterIpcClient::Initialize(sptr<IInterIpc> standardProxy)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);

    if (standardProxy != nullptr) {
        domainProxy_ = standardProxy;
    }

    if (domainProxy_) {
        CreateListenerObject();
    } else {
        SHARING_LOGE("new StandardClient proxy null.");
    }
}

std::shared_ptr<InterIpcClient> InterIpcClient::CreateSubService(std::string key, std::string clientClassName,
                                                                 std::string serverClassName)
{
    SHARING_LOGD("trace.");
    auto client = std::make_shared<InterIpcClient>();
    client->SetKey(key);
    auto subProxy = GetSubProxy(key, serverClassName);
    if (subProxy == nullptr) {
        return nullptr;
    }

    client->Initialize(subProxy);

    return client;
}

std::shared_ptr<IpcMsgAdapter> InterIpcClient::GetMsgAdapter()
{
    SHARING_LOGD("trace.");
    return msgAdapter_;
}

} // namespace Sharing
} // namespace OHOS