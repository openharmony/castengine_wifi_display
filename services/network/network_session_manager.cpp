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

#include "network_session_manager.h"
#include "network/session/base_network_session.h"

namespace OHOS {
namespace Sharing {
NetworkSessionManager::NetworkSessionManager()
{
    SHARING_LOGD("trace.");
}

NetworkSessionManager::~NetworkSessionManager()
{
    SHARING_LOGD("trace.");
}

void NetworkSessionManager::Add(int32_t fd, BaseNetworkSession::Ptr &session)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(sessionMux_);
    sessions_.emplace(fd, session);
}

void NetworkSessionManager::Remove(int32_t fd)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(sessionMux_);
    auto itemItr = sessions_.find(fd);
    if (itemItr != sessions_.end()) {
        if (itemItr->second) {
            itemItr->second->Shutdown();
        }
        sessions_.erase(itemItr);
    }
}

void NetworkSessionManager::RemoveAll()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(sessionMux_);
    for (auto &item : sessions_) {
        if (item.second) {
            item.second->Shutdown();
        }
    }
    sessions_.clear();
}

int8_t NetworkSessionManager::GetLogFlag(void)
{
    SHARING_LOGD("trace.");
    return logFlag_;
}

void NetworkSessionManager::SetLogFlag(int8_t flag)
{
    SHARING_LOGD("trace.");
    logFlag_ = flag;
}
} // namespace Sharing
} // namespace OHOS