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

#ifndef OHOS_SHARING_MOCK_CLIENT_H
#define OHOS_SHARING_MOCK_CLIENT_H

#include <gmock/gmock.h>
#include "iclient.h"

namespace OHOS {
namespace Sharing {

class MockClient : public IClient {
public:
    bool Send(const DataBuffer::Ptr &buf, int32_t nSize)
    {
        (void)buf;
        (void)nSize;
        return false;
    }

public:
    MOCK_METHOD(void, Disconnect, ());
    MOCK_METHOD(bool, Connect,
                (const std::string &peerHost, uint16_t peerPort, const std::string &localIp, uint16_t localPort));
    MOCK_METHOD(bool, Send, (const std::string &msg));
    MOCK_METHOD(bool, Send, (const char *buf, int32_t nSize));
    MOCK_METHOD(SocketInfo::Ptr, GetSocketInfo, ());
    MOCK_METHOD(void, SetRecvOption, (int32_t flags));
    MOCK_METHOD(void, RegisterCallback, (std::weak_ptr<IClientCallback> callback));
};

} // namespace Sharing
} // namespace OHOS
#endif
