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

#ifndef OHOS_SHARING_ICLIENT_H
#define OHOS_SHARING_ICLIENT_H

#include <cstdint>
#include "iclient_callback.h"
#include "network/data/socket_info.h"

namespace OHOS {
namespace Sharing {
class IClient {
public:
    using Ptr = std::shared_ptr<IClient>;

    IClient() = default;
    virtual ~IClient() = default;

    virtual void Disconnect() = 0;
    virtual bool Connect(const std::string &peerHost, uint16_t peerPort, const std::string &localIp,
                         uint16_t localPort) = 0;

    virtual bool Send(const std::string &msg) = 0;
    virtual bool Send(const char *buf, int32_t nSize) = 0;
    virtual bool Send(const DataBuffer::Ptr &buf, int32_t nSize) = 0;

    virtual SocketInfo::Ptr GetSocketInfo() = 0;
    virtual void SetRecvOption(int32_t flags) = 0;
    virtual void RegisterCallback(std::weak_ptr<IClientCallback> callback) = 0;
};
} // namespace Sharing
} // namespace OHOS
#endif