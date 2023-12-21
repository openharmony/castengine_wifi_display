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

#ifndef OHOS_SHARING_INTER_IPC_DEATH_RECIPIENT_H
#define OHOS_SHARING_INTER_IPC_DEATH_RECIPIENT_H

#include <string>
#include "common/sharing_log.h"
#include "iremote_object.h"
#include "nocopyable.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Sharing {

class InterIpcDeathListener {
public:
    using Ptr = std::shared_ptr<InterIpcDeathListener>;

    virtual ~InterIpcDeathListener() = default;

    virtual void OnRemoteDied(std::string key) = 0;
};

class InterIpcDeathRecipient final : public IRemoteObject::DeathRecipient,
                                      public NoCopyable {
public:
    virtual ~InterIpcDeathRecipient() = default;
    
    explicit InterIpcDeathRecipient(std::string key) : key_(key)
    {
        SHARING_LOGD("trace.");
    }

    void OnRemoteDied(const wptr<IRemoteObject> &object) final
    {
        SHARING_LOGD("trace.");
        if (listener_) {
            listener_->OnRemoteDied(key_);
        }
    }

    void SetDeathListener(InterIpcDeathListener::Ptr listener)
    {
        SHARING_LOGD("trace.");
        listener_ = listener;
    }

private:
    std::string key_;
    InterIpcDeathListener::Ptr listener_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif