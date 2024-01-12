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

#include "base_client.h"
#include "common/media_log.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {
BaseClient::~BaseClient()
{
    SHARING_LOGD("trace.");
}

BaseClient::BaseClient()
{
    SHARING_LOGD("trace.");
}

void BaseClient::RegisterCallback(std::weak_ptr<IClientCallback> callback)
{
    SHARING_LOGD("trace.");
    callback_ = callback;
}

std::weak_ptr<IClientCallback> &BaseClient::GetCallback()
{
    MEDIA_LOGD("trace.");
    return callback_;
}

void BaseClient::SetRecvOption(int32_t flags)
{
    SHARING_LOGD("trace.");
    flags_ = flags;
}

void BaseClientEventListener::OnReadable(int32_t fd)
{
    MEDIA_LOGD("trace thread_id: %{public}llu.", GetThreadId());
    auto client = client_.lock();
    if (client) {
        client->OnClientReadable(fd);
    }
}

void BaseClientEventListener::OnWritable(int32_t fd)
{
    MEDIA_LOGD("trace thread_id: %{public}llu.", GetThreadId());
    auto client = client_.lock();
    if (client) {
        auto callback = client->GetCallback().lock();
        if (callback) {
            callback->OnClientWriteable(fd);
        }
    }
}

void BaseClientEventListener::OnShutdown(int32_t fd)
{
    SHARING_LOGD("trace thread_id: %{public}llu.", GetThreadId());
    auto client = client_.lock();
    if (client) {
        auto callback = client->GetCallback().lock();
        if (callback) {
            callback->OnClientClose(fd);
        }
    }
}

void BaseClientEventListener::OnException(int32_t fd)
{
    SHARING_LOGD("trace thread_id: %{public}llu.", GetThreadId());
    auto client = client_.lock();
    if (client) {
        auto callback = client->GetCallback().lock();
        if (callback) {
            callback->OnClientException(fd);
        }
    }
}
} // namespace Sharing
} // namespace OHOS