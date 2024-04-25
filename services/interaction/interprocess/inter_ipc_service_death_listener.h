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

#ifndef OHOS_SHARING_SHARING_SHARING_DEATH_LISTENER_H
#define OHOS_SHARING_SHARING_SHARING_DEATH_LISTENER_H

#include "common/sharing_log.h"
#include "inter_ipc_death_recipient.h"
#include "inter_ipc_service.h"

namespace OHOS {
namespace Sharing {

class InterIpcServiceDeathListener : public InterIpcDeathListener {
public:
    explicit InterIpcServiceDeathListener() = default;

    ~InterIpcServiceDeathListener() override = default;

    void OnRemoteDied(std::string key) final
    {
        SHARING_LOGD("on IpcService remote died.");
        auto service = service_.lock();
        if (service != nullptr) {
            service->DelPeerProxy(key);
        }
    }

    void SetService(InterIpcServiceStub::Ptr service)
    {
        SHARING_LOGD("trace.");
        service_ = service;
    }

private:
    std::weak_ptr<InterIpcServiceStub> service_;
};

} // namespace Sharing
} // namespace OHOS
#endif