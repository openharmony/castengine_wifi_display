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

#ifndef OHOS_SHARING_UDP_CLIENT_H
#define OHOS_SHARING_UDP_CLIENT_H

#include <shared_mutex>
#include "base_client.h"

namespace OHOS {
namespace Sharing {
class UdpSocket;

class UdpClientEventListener : public BaseClientEventListener {};

class UdpClientEventHandler : public BaseClientEventHandler {};

class UdpClient final : public BaseClient {
public:
    UdpClient();
    ~UdpClient() override;

    bool Send(const std::string &msg) override;
    bool Send(const char *buf, int32_t nSize) override;
    bool Send(const DataBuffer::Ptr &buf, int32_t nSize) override;

    void Disconnect() override;
    bool Connect(const std::string &peerHost, uint16_t peerPort, const std::string &localIp,
                 uint16_t localPort) override;

    SocketInfo::Ptr GetSocketInfo() override;

    void OnClientReadable(int32_t fd) override;

private:
    std::shared_mutex mutex_;
    std::shared_ptr<UdpSocket> socket_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif