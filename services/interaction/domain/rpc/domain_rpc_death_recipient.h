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

#ifndef OHOS_SHARING_DOMAIN_RPC_DEATH_RECIPIENT_H
#define OHOS_SHARING_DOMAIN_RPC_DEATH_RECIPIENT_H

#include "common/sharing_log.h"
#include "iremote_object.h"
#include "nocopyable.h"

namespace OHOS {
namespace Sharing {
    
class DomainRpcDeathListener {
public:
    using Ptr = std::shared_ptr<DomainRpcDeathListener>;

    virtual ~DomainRpcDeathListener() = default;

    virtual void OnRemoteDied(std::string deviceId) = 0;
};

class DomainRpcDeathRecipient final : public IRemoteObject::DeathRecipient,
                                      public NoCopyable {
public:
    explicit DomainRpcDeathRecipient(std::string deviceId) : deviceId_(deviceId) {}
    virtual ~DomainRpcDeathRecipient() = default;

    void OnRemoteDied(const wptr<IRemoteObject> &object) final
    {
        SHARING_LOGD("on remote died.");
        if (listener_) {
            listener_->OnRemoteDied(deviceId_);
        }
    }

    void SetDeathListener(DomainRpcDeathListener::Ptr listener)
    {
        SHARING_LOGD("trace.");
        listener_ = listener;
    }

private:
    std::string deviceId_;
    DomainRpcDeathListener::Ptr listener_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
