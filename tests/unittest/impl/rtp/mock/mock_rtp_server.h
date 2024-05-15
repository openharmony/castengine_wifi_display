/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_MOCK_RTP_SERVER_H
#define OHOS_SHARING_MOCK_RTP_SERVER_H

#include <gmock/gmock.h>
#include "iserver.h"

namespace OHOS {
namespace Sharing {

class MockRtpServer : public IServer {
public:
    MOCK_METHOD(void, Stop, ());
    MOCK_METHOD(bool, Start, (uint16_t port, const std::string &host, bool enableReuse, uint32_t backlog));

    MOCK_METHOD(SocketInfo::Ptr, GetSocketInfo, ());
    MOCK_METHOD(void, CloseClientSocket, (int32_t fd));

    MOCK_METHOD(std::weak_ptr<IServerCallback>&, GetCallback, ());
    MOCK_METHOD(void, RegisterCallback, (std::weak_ptr<IServerCallback> callback));
};

} // namespace Sharing
} // namespace OHOS
#endif
