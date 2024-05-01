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

#ifndef OHOS_SHARING_DOMAIN_MANAGER_H
#define OHOS_SHARING_DOMAIN_MANAGER_H

#include <mutex>
#include "dm_kit.h"
#include "domain_def.h"
#include "ipc_msg.h"
#include "nocopyable.h"
#include "singleton.h"

namespace OHOS {
namespace Sharing {

class DomainManagerListener {
public:
    using Ptr = std::shared_ptr<DomainManagerListener>;
    virtual ~DomainManagerListener() = default;

    virtual int32_t OnDomainMsg(std::shared_ptr<BaseDomainMsg> msg) = 0;
};

class DomainManager : public DelayedSingleton<DomainManager>,
                      public std::enable_shared_from_this<DomainManager>,
                      public ITransmitMgrListener {
    DECLARE_DELAYED_SINGLETON(DomainManager);

public:
    // impl ITransmitMgrListener
    int32_t DelPeer(std::string remoteId) override;
    int32_t AddPeer(std::shared_ptr<ITransmitMgr> mgr, std::shared_ptr<IDomainPeer> caller) override;
    void OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) override;

    void SetListener(DomainManagerListener::Ptr listener);
    virtual int32_t AddServiceManager(std::shared_ptr<ITransmitMgr> mgr);
    int32_t SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg);

private:
    std::shared_ptr<ITransmitMgr> FindMgrByRemoteId(std::string remoteId);

private:
    std::mutex mutex_;
    std::map<std::string, DomainType> peerTypeMap_;
    std::map<DomainType, std::shared_ptr<ITransmitMgr>> transmitMgrs_;
    std::vector<OHOS::DistributedHardware::DmDeviceInfo> trustedDeviceInfos_;
    std::weak_ptr<DomainManagerListener> listener_;
    OHOS::DistributedHardware::DmDeviceInfo localDeviceInfo_;
};

} // namespace Sharing
} // namespace OHOS

#endif