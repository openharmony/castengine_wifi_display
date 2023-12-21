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

#ifndef OHOS_SHARING_NETWORK_SESSION_MANAGER_H
#define OHOS_SHARING_NETWORK_SESSION_MANAGER_H

#include <mutex>
#include <unordered_map>
#include "singleton.h"

namespace OHOS {
namespace Sharing {
class BaseNetworkSession;
class NetworkSessionManager : public Singleton<NetworkSessionManager> {
public:
    NetworkSessionManager();
    ~NetworkSessionManager();

    void RemoveAll();
    void Remove(int32_t fd);
    void Add(int32_t fd, std::shared_ptr<BaseNetworkSession> &session);

    int8_t GetLogFlag(void);
    void SetLogFlag(int8_t flag);

protected:
    std::mutex sessionMux_;
    std::unordered_map<int32_t, std::shared_ptr<BaseNetworkSession>> sessions_;

private:
    int8_t logFlag_ = 0;
};
} // namespace Sharing
} // namespace OHOS
#endif