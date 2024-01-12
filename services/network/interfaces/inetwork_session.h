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

#ifndef OHOS_SHARING_I_NETWORK_SESSION_H
#define OHOS_SHARING_I_NETWORK_SESSION_H

#include "inetwork_session_callback.h"
#include "network/data/socket_info.h"

namespace OHOS {
namespace Sharing {
class INetworkSession {
public:
    using Ptr = std::shared_ptr<INetworkSession>;

    INetworkSession() = default;
    virtual ~INetworkSession() = default;

    virtual bool Start() = 0;
    virtual void Shutdown() = 0;
    virtual SocketInfo::Ptr GetSocketInfo() = 0;
    virtual bool Send(const char *buf, int32_t nSize) = 0;
    virtual bool Send(const DataBuffer::Ptr &buf, int32_t nSize) = 0;
    virtual std::shared_ptr<INetworkSessionCallback> &GetCallback() = 0;
    virtual void RegisterCallback(std::shared_ptr<INetworkSessionCallback> callback) = 0;
};
} // namespace Sharing
} // namespace OHOS
#endif