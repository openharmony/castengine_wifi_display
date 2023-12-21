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

#include "base_socket.h"
#include "common/media_log.h"
#include "network/socket/socket_utils.h"

namespace OHOS {
namespace Sharing {
BaseSocket::~BaseSocket()
{
    SHARING_LOGD("~BaseSocket.");
}

BaseSocket::BaseSocket()
{
    SHARING_LOGD("BaseSocket.");
}

void BaseSocket::Close()
{
    SHARING_LOGD("close.");
    if (socketLocalFd_ >= 0) {
        SocketUtils::ShutDownSocket(socketLocalFd_);
        socketLocalFd_ = -1;
    }
}
} // namespace Sharing
} // namespace OHOS