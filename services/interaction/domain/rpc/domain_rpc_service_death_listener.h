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

#ifndef OHOS_SHARING_DOMAIN_RPC_SERVICE_DEATH_LISTENER_H
#define OHOS_SHARING_DOMAIN_RPC_SERVICE_DEATH_LISTENER_H

#include "common/sharing_log.h"
#include "domain_rpc_death_recipient.h"
#include "interaction/domain/rpc/domain_rpc_service.h"

namespace OHOS {
namespace Sharing {

class DomainRpcServiceDeathListener : public DomainRpcDeathListener {
public:
    explicit DomainRpcServiceDeathListener() = default;

    void OnRemoteDied(std::string deviceId) final
    {
        SHARING_LOGD("on RPC remote died deviceId: %{public}s.", deviceId.c_str());
        auto service = service_.lock();
        if (service != nullptr) {
            service->DelPeerProxy(deviceId);
        }
    }

    void SetService(DomainRpcService::Ptr service)
    {
        SHARING_LOGD("trace.");
        service_ = service;
    }

private:
    std::weak_ptr<DomainRpcService> service_;
};

} // namespace Sharing
} // namespace OHOS
#endif
