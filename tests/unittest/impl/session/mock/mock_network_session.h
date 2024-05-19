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

#ifndef OHOS_SHARING_MOCK_NETWORK_SESSION_H
#define OHOS_SHARING_MOCK_NETWORK_SESSION_H

#include <gmock/gmock.h>
#include "inetwork_session.h"

namespace OHOS {
namespace Sharing {

class MockNetworkSession : public INetworkSession {
public:
    bool Send(const DataBuffer::Ptr &buf, int32_t nSize)
    {
        (void)buf;
        (void)nSize;
        return false;
    }

    void RegisterCallback(std::shared_ptr<INetworkSessionCallback> callback)
    {
        callback_ = std::move(callback);
    }

    std::shared_ptr<INetworkSessionCallback> &GetCallback()
    {
        return callback_;
    }

private:
    std::shared_ptr<INetworkSessionCallback> callback_ = nullptr;

public:
    MOCK_METHOD(bool, Start, ());
    MOCK_METHOD(void, Shutdown, ());
    MOCK_METHOD(SocketInfo::Ptr, GetSocketInfo, ());
    MOCK_METHOD(bool, Send, (const char *buf, int32_t nSize));
};

} // namespace Sharing
} // namespace OHOS
#endif
