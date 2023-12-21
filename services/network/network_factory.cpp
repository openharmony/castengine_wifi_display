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

#include "network_factory.h"
#include "client/tcp_client.h"
#include "client/udp_client.h"
#include "common/media_log.h"
#include "server/tcp_server.h"
#include "server/udp_server.h"

namespace OHOS {
namespace Sharing {
NetworkFactory::NetworkFactory()
{
    SHARING_LOGD("trace.");
}

NetworkFactory ::~NetworkFactory()
{
    SHARING_LOGD("trace.");
}

bool NetworkFactory::CreateTcpServer(uint16_t port, const IServerCallback::Ptr &callback, ServerPtr &serverPtr,
                                     const std::string &host)
{
    SHARING_LOGD("trace.");
    serverPtr = std::make_shared<TcpServer>();
    if (serverPtr) {
        serverPtr->RegisterCallback(callback);
        if (serverPtr->Start(port, host)) {
            return true;
        }
    }
    SHARING_LOGE("createTcpServer failed!");
    return false;
}

bool NetworkFactory::CreateTcpClient(const std::string &peerIp, uint16_t peerPort, const IClientCallback::Ptr &callback,
                                     ClientPtr &clientPtr, const std::string &localIp, uint16_t localPort)
{
    SHARING_LOGD("trace.");
    clientPtr = std::make_shared<TcpClient>();
    if (clientPtr) {
        clientPtr->RegisterCallback(callback);
        if (clientPtr->Connect(peerIp, peerPort, localIp, localPort)) {
            return true;
        }
    }
    SHARING_LOGE("createTcpClient failed!");
    return false;
}

bool NetworkFactory::CreateUdpServer(const uint16_t localPort, const std::string &localIp,
                                     const IServerCallback::Ptr &callback, ServerPtr &serverPtr)
{
    SHARING_LOGD("trace.");
    serverPtr = std::make_shared<UdpServer>();
    if (serverPtr) {
        serverPtr->RegisterCallback(callback);
        if (serverPtr->Start(localPort, localIp)) {
            return true;
        }
    }
    SHARING_LOGE("createUdpServer failed!");
    return false;
}

bool NetworkFactory::CreateUdpClient(const std::string &peerIp, uint16_t peerPort, const std::string &localIp,
                                     uint16_t localPort, const IClientCallback::Ptr &callback, ClientPtr &clientPtr)
{
    SHARING_LOGD("trace.");
    clientPtr = std::make_shared<UdpClient>();
    if (clientPtr) {
        clientPtr->RegisterCallback(callback);
        if (clientPtr->Connect(peerIp, peerPort, localIp, localPort)) {
            return true;
        }
    }
    SHARING_LOGE("createUdpClient failed!");
    return false;
}
} // namespace Sharing
} // namespace OHOS