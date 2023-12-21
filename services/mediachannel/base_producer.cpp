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

#include "base_producer.h"
#include "common/common_macro.h"

namespace OHOS {
namespace Sharing {

BaseProducer::BaseProducer()
{
    SHARING_LOGD("producerId: %{public}u.", GetId());
}

BaseProducer::~BaseProducer()
{
    SHARING_LOGD("producerId: %{public}u.", GetId());
}

void BaseProducer::SetProducerListener(std::weak_ptr<IProducerListener> listener)
{
    SHARING_LOGD("trace.");
    listener_ = listener;
}

bool BaseProducer::IsRunning()
{
    SHARING_LOGD("trace.");
    return isRunning_;
}

void BaseProducer::SetSrcAgentId(uint32_t agentId)
{
    SHARING_LOGD("trace.");
    srcAgentId_ = agentId;
}

uint32_t BaseProducer::GetSrcAgentId()
{
    SHARING_LOGD("trace.");
    return srcAgentId_;
}

void BaseProducer::Notify(ProsumerStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    statusMsg->prosumerId = GetId();
    statusMsg->agentId = GetSrcAgentId();

    auto listener = listener_.lock();
    if (listener) {
        listener->OnProducerNotify(statusMsg);
    }
}

void BaseProducer::NotifyPrivateEvent(EventMsg::Ptr eventMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(eventMsg);
    auto pMsg = std::make_shared<ProsumerStatusMsg>();
    pMsg->prosumerId = GetId();
    pMsg->agentId = GetSrcAgentId();
    pMsg->status = PROSUMER_NOTIFY_PRIVATE_EVENT;
    pMsg->eventMsg = std::move(eventMsg);

    auto listener = listener_.lock();
    if (listener) {
        listener->OnProducerNotify(pMsg);
    }
}

} // namespace Sharing
} // namespace OHOS