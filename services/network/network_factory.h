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

#ifndef OHOS_SHARING_NETWORK_FACTORY_H
#define OHOS_SHARING_NETWORK_FACTORY_H

#include "interfaces/iclient.h"
#include "interfaces/iserver.h"

namespace OHOS {
namespace Sharing {
class NetworkFactory final {
public:
    using ServerPtr = std::shared_ptr<IServer>;
    using ClientPtr = std::shared_ptr<IClient>;

    NetworkFactory();
    ~NetworkFactory();

    static bool CreateTcpServer(uint16_t port, const IServerCallback::Ptr &callback, ServerPtr &serverPtr,
                                const std::string &host = "::");
    static bool CreateTcpClient(const std::string &peerIp, uint16_t peerPort, const IClientCallback::Ptr &callback,
                                ClientPtr &clientPtr, const std::string &localIp = "::", uint16_t localPort = 0);

    static bool CreateUdpServer(const uint16_t localPort, const std::string &localIp,
                                const IServerCallback::Ptr &callback, ServerPtr &serverPtr);
    static bool CreateUdpClient(const std::string &peerIp, uint16_t peerPort, const std::string &localIp,
                                uint16_t localPort, const IClientCallback::Ptr &callback, ClientPtr &clientPtr);
};
} // namespace Sharing
} // namespace OHOS
#endif