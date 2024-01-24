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

#ifndef OHOS_SHARING_TCP_SERVER_H
#define OHOS_SHARING_TCP_SERVER_H

#include <shared_mutex>
#include <unordered_map>
#include "base_server.h"

namespace OHOS {
namespace Sharing {
class BaseNetworkSession;
class TcpSocket;

class TcpServerEventListener : public BaseServerEventListener {};

class TcpServerEventHandler : public BaseServerEventHandler {};

class TcpServer final : public BaseServer {
public:
    TcpServer();
    ~TcpServer() override;

    void Stop() override;
    bool Start(uint16_t port, const std::string &host, bool enableReuse = true,
               uint32_t backlog = MAX_BACKLOG_NUM) override;

    SocketInfo::Ptr GetSocketInfo() override;
    void CloseClientSocket(int32_t fd) override;

    void OnServerReadable(int32_t fd) override;

private:
    std::shared_mutex mutex_;
    std::shared_ptr<TcpSocket> socket_ = nullptr;
    std::unordered_map<int32_t, std::shared_ptr<BaseNetworkSession>> sessions_;
};
} // namespace Sharing
} // namespace OHOS
#endif