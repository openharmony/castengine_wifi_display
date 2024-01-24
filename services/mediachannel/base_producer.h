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

#ifndef OHOS_SHARING_BASE_PRODUCER_H
#define OHOS_SHARING_BASE_PRODUCER_H

#include <functional>
#include <memory>
#include <cstdint>
#include <string>
#include "buffer_dispatcher.h"
#include "common/event_channel.h"
#include "common/identifier.h"
#include "common/object.h"
#include "event/handle_event_base.h"
#include "mediachannel/media_channel_def.h"
#include "utils/data_buffer.h"

namespace OHOS {
namespace Sharing {

class IProducerListener {
public:
    using Ptr = std::shared_ptr<IProducerListener>;

    virtual ~IProducerListener() = default;

    virtual void OnProducerNotify(ProsumerStatusMsg::Ptr &statusMsg) = 0;
};

class BaseProducer : public Object,
                     public HandleEventBase,
                     public BufferReceiver {
public:
    using Ptr = std::shared_ptr<BaseProducer>;

    BaseProducer();
    ~BaseProducer() override;

    virtual void SetSrcAgentId(uint32_t agentId);
    void SetProducerListener(std::weak_ptr<IProducerListener> listener);

    virtual bool IsRunning();
    virtual void StartDispatchThread(){};
    virtual void PublishOneFrame(const MediaData::Ptr &buff) = 0;
    virtual void UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg) = 0;

    virtual int32_t Release() = 0;
    virtual uint32_t GetSrcAgentId();

protected:
    void Notify(ProsumerStatusMsg::Ptr &statusMsg);
    void NotifyPrivateEvent(EventMsg::Ptr eventMsg);

protected:
    bool isInit_ = false;
    bool isPaused_ = false;
    bool isRunning_ = false;
    uint32_t srcAgentId_ = INVALID_ID;
    std::weak_ptr<IProducerListener> listener_;
};

} // namespace Sharing
} // namespace OHOS
#endif