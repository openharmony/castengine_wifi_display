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

#ifndef OHOS_SHARING_MEDIA_CHANNEL_H
#define OHOS_SHARING_MEDIA_CHANNEL_H

#include <memory>
#include <unordered_map>
#include "base_channel.h"
#include "base_consumer.h"
#include "common/event_comm.h"
#include "media_controller.h"

namespace OHOS {
namespace Sharing {

enum MediaChannelNotifyStatus {
    NOTIFY_VIDEO_STOP,
};

class IMediaChannelListener {
public:
    virtual ~IMediaChannelListener() = default;

    virtual void OnMediaChannelNotify(MediaChannelNotifyStatus status) = 0;
};

class MediaChannel : public IProducerListener,
                     public IConsumerListener,
                     public BufferDispatcherListener,
                     public BaseChannel,
                     public std::enable_shared_from_this<MediaChannel> {
public:
    using Ptr = std::shared_ptr<MediaChannel>;

    MediaChannel();
    ~MediaChannel() override;

    void OnWriteTimeout() override;
    void SetContextId(uint32_t contextId)
    {
        SHARING_LOGD("trace.");
        srcContextId_ = contextId;
    }

    BufferDispatcher::Ptr GetDispatcher() override
    {
        MEDIA_LOGD("trace.");
        return dispatcher_;
    }

public:
    uint32_t GetSinkAgentId();
    uint32_t CreateProducer(std::string &className) override;

    int32_t Release();
    int32_t HandleEvent(SharingEvent &event) override;

    void OnMediaControllerNotify(ProsumerStatusMsg::Ptr &statusMsg);
    void OnProducerNotify(ProsumerStatusMsg::Ptr &statusMsg) override;
    void OnConsumerNotify(ProsumerStatusMsg::Ptr &statusMsg) override;
    void SendAgentEvent(ProsumerStatusMsg::Ptr &msg, EventType eventType);
    void SetMediaChannelListener(std::weak_ptr<IMediaChannelListener> listener);

    SharingErrorCode CreateConsumer(std::string &className) override;

private:
    SharingErrorCode HandleStopConsumer(SharingEvent &event);
    SharingErrorCode HandleStartConsumer(SharingEvent &event);
    SharingErrorCode HandlePauseConsumer(SharingEvent &event);
    SharingErrorCode HandleCreateConsumer(SharingEvent &event);
    SharingErrorCode HandleResumeConsumer(SharingEvent &event);
    SharingErrorCode HandleDestroyConsumer(SharingEvent &event);

    SharingErrorCode HandleStopProducer(SharingEvent &event);
    SharingErrorCode HandlePauseProducer(SharingEvent &event);
    SharingErrorCode HandleStartProducer(SharingEvent &event);
    SharingErrorCode HandleCreateProducer(SharingEvent &event);
    SharingErrorCode HandleResumeProducer(SharingEvent &event);
    SharingErrorCode HandleDestroyProducer(SharingEvent &event);

    SharingErrorCode InitPlayController();
    SharingErrorCode HandleStopPlay(SharingEvent &event);
    SharingErrorCode HandleSetVolume(SharingEvent &event);
    SharingErrorCode HandleStartPlay(SharingEvent &event);
    SharingErrorCode HandleSetSceneType(SharingEvent &event);
    SharingErrorCode HandleAppendSurface(SharingEvent &event);
    SharingErrorCode HandleRemoveSurface(SharingEvent &event);
    SharingErrorCode HandleSetKeyRedirect(SharingEvent &event);

private:
    uint32_t srcContextId_ = INVALID_ID;

    std::mutex mutex_;
    std::weak_ptr<IMediaChannelListener> listener_;

    BufferDispatcher::Ptr dispatcher_ = nullptr;
    MediaController::Ptr playController_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif