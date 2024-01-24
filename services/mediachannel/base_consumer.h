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

#ifndef OHOS_SHARING_BASE_CONSUMER_H
#define OHOS_SHARING_BASE_CONSUMER_H

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

class DataBuffer;

class IConsumerListener {
public:
    using Ptr = std::shared_ptr<IConsumerListener>;

    virtual ~IConsumerListener() = default;

    virtual void OnConsumerNotify(ProsumerStatusMsg::Ptr &statusMsg) = 0;
    virtual BufferDispatcher::Ptr GetDispatcher() = 0;
};

class BaseConsumer : public Object,
                     public HandleEventBase,
                     public IdentifierInfo {
public:
    using Ptr = std::shared_ptr<BaseConsumer>;

    BaseConsumer();
    virtual ~BaseConsumer();

    virtual void SetSinkAgentId(uint32_t agentId);
    virtual void UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg) = 0;
    void SetConsumerListener(std::weak_ptr<IConsumerListener> listener);

    virtual bool IsPasued();
    virtual bool IsRunning();
    virtual bool IsCapture();

    virtual int32_t Release() = 0;
    virtual uint32_t GetSinkAgentId();

    virtual const AudioTrack &GetAudioTrack() const;
    virtual const VideoTrack &GetVideoTrack() const;

protected:
    void Notify(ProsumerStatusMsg::Ptr &statusMsg);
    void NotifyPrivateEvent(EventMsg::Ptr eventMsg);

protected:
    bool isInit_ = false;
    bool isPaused_ = false;
    bool isRunning_ = false;
    uint32_t sinkAgentId_ = INVALID_ID;
    std::weak_ptr<IConsumerListener> listener_;

    AudioTrack audioTrack_;
    VideoTrack videoTrack_;
};

} // namespace Sharing
} // namespace OHOS
#endif