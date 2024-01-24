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

#ifndef OHOS_SHARING_ISERVER_H
#define OHOS_SHARING_ISERVER_H

#include <cstdint>
#include <string>
#include "iserver_callback.h"
#include "network/data/socket_info.h"

namespace OHOS {
namespace Sharing {
class IServer {
public:
    using Ptr = std::shared_ptr<IServer>;

    IServer() = default;
    virtual ~IServer() = default;

    virtual void Stop() = 0;
    virtual bool Start(uint16_t port, const std::string &host, bool enableReuse = true, uint32_t backlog = 1000) = 0;

    virtual SocketInfo::Ptr GetSocketInfo() = 0;
    virtual void CloseClientSocket(int32_t fd) = 0;

    virtual std::weak_ptr<IServerCallback> &GetCallback() = 0;
    virtual void RegisterCallback(std::weak_ptr<IServerCallback> callback) = 0;
};
} // namespace Sharing
} // namespace OHOS
#endif