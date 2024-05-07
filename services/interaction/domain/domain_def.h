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

#ifndef OHOS_SHARING_DOMAIN_DEF_H
#define OHOS_SHARING_DOMAIN_DEF_H

#include "common/identifier.h"
#include "common/sharing_log.h"
#include "interaction/scene/scene_format.h"
#include "ipc_msg.h"
#include "iremote_object.h"
#include "singleton.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {

enum DomainType { DOMAIN_TYPE_RPC = 0, DOMAIN_TYPE_DBUS, DOMAIN_TYPE_SOCKET, DOMAIN_TYPE_MAX };

class IDomainPeerListener {
public:
    IDomainPeerListener() = default;
    virtual ~IDomainPeerListener() = default;

    virtual void OnPeerConnected(std::string remoteId) = 0;
    virtual void OnPeerDisconnected(std::string remoteId) = 0;
    virtual void OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) = 0;
};

class IDomainPeer {
public:
    enum DomainPeerType { REMOTE_CALLER_SERVER = 0, REMOTE_CALLER_CLIENT };

    virtual ~IDomainPeer() = default;

    explicit IDomainPeer(DomainPeerType type)
    {
        SHARING_LOGD("trace.");
        peerType_ = type;
    }

    DomainPeerType GetPeerType()
    {
        SHARING_LOGD("trace.");
        return peerType_;
    }

    std::string GetRemoteId()
    {
        SHARING_LOGD("trace.");
        return remoteId_;
    }

    void SetRemoteId(std::string remoteId)
    {
        SHARING_LOGD("trace.");
        remoteId_ = remoteId;
    }

    DomainType GetDomainType()
    {
        SHARING_LOGD("trace.");
        return domainType_;
    }

public:
    virtual void SetPeerListener(std::weak_ptr<IDomainPeerListener> listener) = 0;
    virtual int32_t SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) = 0;

protected:
    std::string remoteId_;
    std::weak_ptr<IDomainPeerListener> peerListener_;

    DomainType domainType_;
    DomainPeerType peerType_;
};

class ITransmitMgr;

class ITransmitMgrListener {
public:
    ITransmitMgrListener() = default;
    virtual ~ITransmitMgrListener() = default;

    virtual int32_t DelPeer(std::string remoteId) = 0;
    virtual int32_t AddPeer(std::shared_ptr<ITransmitMgr> mgr, std::shared_ptr<IDomainPeer> caller) = 0;
    virtual void OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) = 0;
};

class ITransmitMgr : public IDomainPeerListener,
                     public IdentifierInfo {
public:
    DomainType GetDomainType()
    {
        SHARING_LOGD("trace.");
        return domainType_;
    }

    void SetListener(std::weak_ptr<ITransmitMgrListener> listener)
    {
        SHARING_LOGD("trace.");
        transMgrListener_ = listener;
    }

public:
    virtual bool IsPeerExist(std::string remoteId) = 0;
    virtual int32_t SendDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> BaseMsg) = 0;

protected:
    DomainType domainType_;
    std::weak_ptr<ITransmitMgrListener> transMgrListener_;
};

} // namespace Sharing
} // namespace OHOS
#endif