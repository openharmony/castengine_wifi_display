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

#include "base_consumer.h"
#include "common/common_macro.h"

namespace OHOS {
namespace Sharing {
    
BaseConsumer::BaseConsumer()
{
    SHARING_LOGD("consumerId: %{public}u.", GetId());
}

BaseConsumer::~BaseConsumer()
{
    SHARING_LOGD("consumerId: %{public}u.", GetId());
}

void BaseConsumer::SetConsumerListener(std::weak_ptr<IConsumerListener> listener)
{
    SHARING_LOGD("trace.");
    listener_ = listener;
}

bool BaseConsumer::IsRunning()
{
    SHARING_LOGD("trace.");
    return isRunning_;
}

bool BaseConsumer::IsPasued()
{
    SHARING_LOGD("trace.");
    return isPaused_;
}

bool BaseConsumer::IsCapture()
{
    SHARING_LOGD("trace.");
    return false;
}

uint32_t BaseConsumer::GetSinkAgentId()
{
    SHARING_LOGD("trace.");
    return sinkAgentId_;
}

void BaseConsumer::SetSinkAgentId(uint32_t agentId)
{
    SHARING_LOGD("trace.");
    sinkAgentId_ = agentId;
}

const AudioTrack &BaseConsumer::GetAudioTrack() const
{
    SHARING_LOGD("trace.");
    return audioTrack_;
}

const VideoTrack &BaseConsumer::GetVideoTrack() const
{
    SHARING_LOGD("trace.");
    return videoTrack_;
}

void BaseConsumer::Notify(ProsumerStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    statusMsg->prosumerId = GetId();
    statusMsg->agentId = GetSinkAgentId();

    auto listener = listener_.lock();
    if (listener) {
        listener->OnConsumerNotify(statusMsg);
    }
}

void BaseConsumer::NotifyPrivateEvent(EventMsg::Ptr eventMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(eventMsg);
    auto pMsg = std::make_shared<ProsumerStatusMsg>();
    pMsg->prosumerId = GetId();
    pMsg->agentId = GetSinkAgentId();
    pMsg->status = PROSUMER_NOTIFY_PRIVATE_EVENT;
    pMsg->eventMsg = std::move(eventMsg);

    auto listener = listener_.lock();
    if (listener) {
        listener->OnConsumerNotify(pMsg);
    }
}

} // namespace Sharing
} // namespace OHOS